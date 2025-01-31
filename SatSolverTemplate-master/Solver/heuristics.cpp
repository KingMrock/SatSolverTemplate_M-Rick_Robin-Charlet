/**
* @author Tim Luchterhand
* @date 29.11.24
* @brief
*/

#include <Iterators.hpp>

#include "heuristics.hpp"
#include "util/exception.hpp"
#include "util/random.hpp"

namespace sat {

    Variable FirstVariable::operator()(const std::vector<TruthValue> &model, std::size_t) const {
        for (unsigned varId = 0; varId < model.size(); ++varId) {
            if (model[varId] == TruthValue::Undefined) {
                return Variable(varId);
            }
        }
        throw std::runtime_error("No unassigned variable");
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

    Variable RandomVariable::operator()(const std::vector<TruthValue>& model, std::size_t seed) const {
        std::vector<Variable> unassigned;
        for (auto [varId, val]: iterators::enumerate(model, 0u)) {
            if (val == TruthValue::Undefined) {
                unassigned.push_back(Variable(varId));
            }
        }
        
        if (unassigned.empty()) {
            throw std::runtime_error("Found no open variable");
        }
        
        // Set seed and get random variable from unassigned list
        RNG::get().setSeed(seed);
        size_t idx = RNG::get().random_int<size_t>(0, unassigned.size() - 1);
        return unassigned[idx];
    }
}
