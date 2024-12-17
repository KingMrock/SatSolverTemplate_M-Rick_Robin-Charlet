/**
* @author Tim Luchterhand
* @date 29.11.24
* @brief
*/

#include <Iterators.hpp>

#include "heuristics.hpp"
#include "util/exception.hpp"

namespace sat {

    Variable FirstVariable::operator()(const std::vector<TruthValue> &model, std::size_t) const {
        for (auto [varId, val]: iterators::enumerate(model, 0u)) {
            if (val == TruthValue::Undefined) {
                return Variable(varId);
            }
        }

        throw std::runtime_error("Found no open variable");
    }

    Variable Heuristic::operator()(const std::vector<TruthValue> &values, std::size_t numOpenVariables) const {
        if (nullptr == impl) {
            throw BadHeuristicCall("heuristic wrapper does not contain a heuristic");
        }

        return impl->invoke(values, numOpenVariables);
    }

    bool Heuristic::isValid() const {
        return nullptr != impl;
    }
}
