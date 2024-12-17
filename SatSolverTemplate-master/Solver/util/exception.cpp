/**
* @author Tim Luchterhand
* @date 28.11.24
* @brief
*/

#include "exception.hpp"

using namespace std::string_literals;
NotImplementedException::NotImplementedException(const std::string &methodName): logic_error(
    methodName.empty() ? "Method not implemented"s : "Method "s + methodName + " not implemented"s) {}

BadHeuristicCall::BadHeuristicCall(std::string message) : message(std::move(message)){}

const char * BadHeuristicCall::what() const noexcept {
    return message.c_str();
}
