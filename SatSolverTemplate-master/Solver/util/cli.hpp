/**
* @author Tim Luchterhand
* @date 02.12.24
* @file cli.hpp
* @brief Contains Command Line Interface parsing utility
*/

#ifndef CLI_HPP
#define CLI_HPP

#include <span>
#include <stdexcept>
#include <string>
#include <concepts>
#include <iostream>

#include "concepts.hpp"

namespace cli {
    /**
     * @brief Argument specification for switch arguments
     */
    struct Switch {
        Switch(std::string name, bool &value): name(std::move(name)), value(value) {
        }

        std::string name;
        bool &value;
    };

    /**
     * @brief Argument specification for value arguments
     * @tparam T argument type
     */
    template<typename T>
    struct ValueArg {
        using type = T;

        ValueArg(std::string name, T &value, bool required = false): name(std::move(name)), value(value),
                                                                     required(required) {
        }

        std::string name;
        T &value;
        bool required;
    };

    namespace detail {
        /**
         * Argument concept
         */
        template<typename T>
        concept arg = std::same_as<T, Switch> or sat::concepts::same_template<T, ValueArg>;

        template<typename T>
        struct TypeParse {
            static_assert(sat::traits::always_false_v<T>, "Unsupported value type");
        };

        template<>
        struct TypeParse<float> {
            float operator()(const std::string &s) const {
                return std::stof(s);
            }
        };

        template<>
        struct TypeParse<double> {
            double operator()(const std::string &s) const {
                return std::stod(s);
            }
        };

        template<std::integral T>
        struct TypeParse<T> {
            T operator()(const std::string &s) const {
                return std::stoi(s);
            }
        };

        template<sat::concepts::enum_type T>
        struct TypeParse<T> {
            T operator()(const std::string &s) const {
                return static_cast<T>(std::stoi(s));
            }
        };
    }


    /**
     * Parser function.
     * @note this function stops the template recursion
     * @param argc
     * @param argv
     * @return instance file
     */
    inline std::string parse(int argc, char *argv[]) {
        if (argc < 2) {
            std::cerr << "Specify the input file" << std::endl;
            std::exit(1);
        }

        return argv[1];
    }

    /**
     * Compiletime recursive parse method
     * @tparam Option
     * @tparam Options
     * @param argc
     * @param argv
     * @param option
     * @param rest
     * @return instance file
     */
    template<detail::arg Option, detail::arg ... Options>
    std::string parse(int argc, char *argv[], const Option &option, const Options &... rest) {
        using namespace std::string_literals;
        if (argc < 2) {
            std::cerr << "Specify the input file" << std::endl;
            std::exit(1);
        }

        const std::span options(argv + 2, argv + argc);
        auto res = std::ranges::find(options, option.name);
        if (res != options.end()) {
            if constexpr (std::same_as<Option, Switch>) {
                option.value = not option.value;
            } else {
                if (++res == options.end()) {
                    throw std::runtime_error("Could not find argument for option "s + option.name);
                }

                option.value = detail::TypeParse<typename Option::type>()(*res);
                std::cout << "c -- using value " << option.value << " for option " << option.name << std::endl;
            }
        } else {
            if constexpr (sat::concepts::same_template<Option, ValueArg>) {
                if (option.required) {
                    throw std::runtime_error("Required argument "s + option.name + " not specified");
                }

                std::cout << "c -- using default value " << option.value << " for option " << option.name << std::endl;
            }
        }

        return parse(argc, argv, rest...);
    }
}

#endif //CLI_HPP
