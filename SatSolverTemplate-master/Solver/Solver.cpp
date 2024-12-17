/**
* @author Tim Luchterhand
* @date 27.11.24
* @brief

**/
#include "Solver.hpp"
#include "util/exception.hpp"

#include "Solver.hpp"
#include "util/exception.hpp"

namespace sat {
    
    Solver::Solver(unsigned numVariables) 
        : model(numVariables, TruthValue::Undefined), 
          literalClauses(numVariables * 2) {}

    bool Solver::addClause(Clause clause) {
        if (clause.isEmpty()) {
            return false;
        }

        auto clausePtr = std::make_shared<Clause>(std::move(clause));
        clauses.push_back(clausePtr);

        for (short rank = 0; rank < 2; ++rank) {
            Literal watcher = clausePtr->getWatcherByRank(rank);
            literalClauses[watcher.get()].push_back(clausePtr);
        }

        if (clausePtr->size() == 1) {
            unitLiterals.push_back(clausePtr->getWatcherByRank(0));
        }

        return true;
    }

    std::vector<Clause> Solver::rebase() const {
    std::vector<Clause> rebased;

    // Include unit clauses for assigned variables
    for (unsigned varId = 0; varId < model.size(); ++varId) {
        if (model[varId] != TruthValue::Undefined) {
            Literal l = (model[varId] == TruthValue::True) ? pos(Variable(varId)) : neg(Variable(varId));
            rebased.push_back(Clause({l}));
        }
    }

    // Process clauses
    for (const auto &clausePtr : clauses) {
        const Clause &clause = *clausePtr;
        bool clauseSatisfied = false;
        std::vector<Literal> newLiterals;

        for (const Literal &l : clause) {
            if (satisfied(l)) {
                clauseSatisfied = true;
                break;
            } else if (!falsified(l)) {
                newLiterals.push_back(l);
            }
        }

        if (!clauseSatisfied && !newLiterals.empty()) {
            rebased.push_back(Clause(std::move(newLiterals)));
        }
    }

    return rebased;
}

    TruthValue Solver::val(Variable x) const {
        return model[x.get()];
    }

    bool Solver::satisfied(Literal l) const {
        return val(var(l)) == (l.sign() == 1 ? TruthValue::True : TruthValue::False);
    }

    bool Solver::falsified(Literal l) const {
        return val(var(l)) == (l.sign() == 1 ? TruthValue::False : TruthValue::True);
    }

    bool Solver::assign(Literal l) {
    Variable variable = var(l);
    TruthValue currentVal = model[variable.get()];
    TruthValue newVal = (l.sign() == 1) ? TruthValue::True : TruthValue::False;

    if (currentVal == TruthValue::Undefined) {
        model[variable.get()] = newVal;
        return true;
    } else {
        return currentVal == newVal;
    }
}


bool Solver::unitPropagate() {
    
}
    
} // namespace sat