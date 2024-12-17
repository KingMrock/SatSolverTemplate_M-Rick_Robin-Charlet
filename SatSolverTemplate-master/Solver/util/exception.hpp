/**
* @author Tim Luchterhand
* @date 28.11.24
* @file exception.hpp
* @brief Not implemented exception and BadHeuristicCall exception
*/

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <functional>

struct NotImplementedException : std::logic_error {
    NotImplementedException(const std::string &methodName = {});
};

class BadHeuristicCall : public std::bad_function_call {
    std::string message;
public:
    BadHeuristicCall(std::string message = {});
    const char *what() const noexcept override;
};

#define NOT_IMPLEMENTED NotImplementedException(__PRETTY_FUNCTION__)


#endif //EXCEPTION_HPP
