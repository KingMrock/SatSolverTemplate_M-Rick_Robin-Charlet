/**
* @author Tim Luchterhand
* @date 26.11.24
* @file random.hpp
* @brief Contains an easy-to-use random number generator
*/

#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>
#include <concepts>

namespace sat {
    /**
    * @brief Random number generator singleton class
    */
    class RNG {
        std::random_device rd;
        std::default_random_engine el;

        RNG();

    public:

        RNG(const RNG&) = delete;
        RNG& operator=(const RNG&) = delete;
        RNG(RNG&&) = delete;
        RNG& operator=(RNG&&) = delete;
        ~RNG() = default;

        /**
        * Get the instance of the random number generator
        * @return instance of the random number generator
        */
        static RNG &get();

        /**
        * Sets the random seed
        * @param seed the desired seed
        */
        void setSeed(unsigned seed);

        /**
         * Generates a random integer value in [min, max]
         * @tparam T integral type of value
         * @param min lower bound
         * @param max upper bound
         * @return random value
         */
        template<std::integral T>
        T random_int(T min, T max) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(el);
        }

        /**
         * Generates a random float value in [min, max)
         * @tparam T floating point type of value
         * @param min lower bound
         * @param max upper bound
         * @return random value
         */
        template<std::floating_point T>
        T random_float(T min, T max) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(el);
        }
    };
}

#endif //RANDOM_HPP
