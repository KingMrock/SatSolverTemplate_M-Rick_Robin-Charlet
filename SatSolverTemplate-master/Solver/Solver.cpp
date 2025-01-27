#include "Solver.hpp"
#include "util/exception.hpp"
#include <algorithm>
#include <cassert>

namespace sat {

    Solver::Solver(unsigned numVariables)
        : mModel(numVariables, TruthValue::Undefined)
    {
        // 2 * numVariables possible literal IDs (positive & negative).
        mWatchers.resize(numVariables * 2);
    }

    bool Solver::addClause(Clause clause)
    {
        // Empty clause => immediate conflict
        if (clause.isEmpty()) {
            return false;
        }
        // Wrap the clause in a shared_ptr
        auto cp = std::make_shared<Clause>(std::move(clause));
        mClauses.push_back(cp);

        // If it's a unit clause => assign right away
        if (cp->size() == 1) {
            Literal theOnlyLit = (*cp)[0];
            if (!assign(theOnlyLit)) {
                // Conflict if it’s already falsified
                return false;
            }
            // Watch the single literal anyway
            mWatchers[indexOf(theOnlyLit)].push_back(cp);
        }
        else {
            // Otherwise watch its two watchers
            Literal w0 = cp->getWatcherByRank(0);
            Literal w1 = cp->getWatcherByRank(1);
            mWatchers[indexOf(w0)].push_back(cp);
            mWatchers[indexOf(w1)].push_back(cp);
        }
        return true;
    }

    /**
     * \brief Returns a reduced formula after unit propagation:
     *   - Skip clauses that are already satisfied
     *   - In remaining clauses, remove all falsified literals
     */
        
    std::vector<Clause> Solver::rebase() const {
        std::vector<Clause> rebased;
        
        // First, collect assigned variables
        std::vector<bool> isAssigned(mModel.size(), false);
        for (const auto& clausePtr : mClauses) {
            const Clause& clause = *clausePtr;
            for (const Literal& lit : clause) {
                Variable v = var(lit);
                if (val(v) != TruthValue::Undefined) {
                    isAssigned[v.get()] = true;
                }
            }
        }
        
        // Add unit clauses for assigned variables
        for (unsigned varId = 0; varId < mModel.size(); ++varId) {
            if (isAssigned[varId] && mModel[varId] != TruthValue::Undefined) {
                Literal l = (mModel[varId] == TruthValue::True) ? 
                        pos(Variable(varId)) : neg(Variable(varId));
                rebased.push_back(Clause({l}));
            }
        }
        
        // Add remaining non-satisfied clauses with non-falsified literals
        for (const auto& clausePtr : mClauses) {
            const Clause& clause = *clausePtr;
            std::vector<Literal> newLits;
            bool isClauseSatisfied = false;
            
            for (const Literal& lit : clause) {
                if (satisfied(lit)) {
                    isClauseSatisfied = true;
                    break;
                }
                if (!falsified(lit)) {
                    newLits.push_back(lit);
                }
            }
            
            if (!isClauseSatisfied && !newLits.empty()) {
                bool isDuplicate = false;
                for (const auto& existing : rebased) {
                    if (existing.sameLiterals({newLits})) {
                        isDuplicate = true;
                        break;
                    }
                }
                if (!isDuplicate) {
                    rebased.push_back(Clause(std::move(newLits)));
                }
            }
        }
        
        return rebased;
    }
    
    TruthValue Solver::val(Variable x) const {
        return mModel[x.get()];
    }

    bool Solver::satisfied(Literal l) const {
        TruthValue tv = val(var(l));
        if (tv == TruthValue::Undefined) {
            return false;
        }
        // Positive lit is satisfied if variable==True; negative if variable==False
        return ( (l.sign() > 0 && tv == TruthValue::True)
              || (l.sign() < 0 && tv == TruthValue::False));
    }

    bool Solver::falsified(Literal l) const {
        // A literal l is falsified if its negation is satisfied
        TruthValue tv = val(var(l));
        if (tv == TruthValue::Undefined) {
            return false;
        }
        // Positive lit is falsified if variable==False; negative if variable==True
        return ( (l.sign() > 0 && tv == TruthValue::False)
              || (l.sign() < 0 && tv == TruthValue::True));
    }

    bool Solver::assign(Literal l) {
        // If already falsified => conflict
        if (falsified(l)) {
            return false;
        }
        // If already satisfied => do nothing
        if (satisfied(l)) {
            return true;
        }
        // Otherwise, set variable in mModel
        unsigned varId = var(l).get();
        mModel[varId] = (l.sign() > 0) ? TruthValue::True : TruthValue::False;
        // Enqueue for unit propagation
        mUnitQueue.push_back(l);
        return true;
    }

    bool Solver::unitPropagate() {
        size_t to_propagate = 0;
        
        while (to_propagate < mUnitQueue.size()) {
            Literal unit = mUnitQueue[to_propagate];
            Literal negation = unit.negate();
            
            // Get all clauses watching the negation of our unit literal
            auto& watches = mWatchers[indexOf(negation)];
            std::vector<ClausePointer> stillWatching;
            
            for (const auto& clausePtr : watches) {
                Clause& clause = *clausePtr;
                
                // Skip if clause is satisfied
                bool clauseSatisfied = false;
                for (const Literal& lit : clause) {
                    if (satisfied(lit)) {
                        clauseSatisfied = true;
                        break;
                    }
                }
                if (clauseSatisfied) {
                    continue;
                }
                
                // Get rank of the negated literal in this clause
                short rank = clause.getRank(negation);
                if (rank == -1) {
                    stillWatching.push_back(clausePtr);
                    continue;
                }
                
                // Get the other watched literal
                Literal otherWatcher = clause.getWatcherByRank(1 - rank);
                
                // If other watcher is satisfied, keep watching
                if (satisfied(otherWatcher)) {
                    stillWatching.push_back(clausePtr);
                    continue;
                }
                
                // Try to find new non-falsified literal to watch
                bool found = false;
                for (const Literal& lit : clause) {
                    if (lit != negation && lit != otherWatcher && !falsified(lit)) {
                        clause.setWatcher(lit, rank);
                        mWatchers[indexOf(lit)].push_back(clausePtr);
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    // No new watch found - otherWatcher must become true
                    if (falsified(otherWatcher)) {
                        return false; // Conflict detected
                    }
                    if (val(var(otherWatcher)) == TruthValue::Undefined) {
                        if (!assign(otherWatcher)) {
                            return false;
                        }
                    }
                    stillWatching.push_back(clausePtr);
                }
            }
            
            watches = std::move(stillWatching);
            to_propagate++;
        }
        return true;
    }
    

} // namespace sat
