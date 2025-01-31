#include "Solver.hpp"
#include "util/exception.hpp"
#include <algorithm>
#include <cassert>

namespace sat {

    Solver::Solver(unsigned numVariables)
        : mModel(numVariables, TruthValue::Undefined),
        mHeuristic(FirstVariable())
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
        Variable v = var(l);
        
        // Check current assignment
        if (val(v) != TruthValue::Undefined) {
            // Already assigned
            return (val(v) == TruthValue::True && l.sign() > 0) || 
                   val(v) == TruthValue::False && l.sign() < 0;
        }
        
        // Make new assignment
        mModel[v.get()] = l.sign() > 0 ? TruthValue::True : TruthValue::False;
        mUnitQueue.push_back(l);
        
        return true;
    }

    bool Solver::unitPropagate()
{
    // Process newly assigned literals until no more changes
    while (!mUnitQueue.empty()) {
        Literal assignedLit = mUnitQueue.front();
        mUnitQueue.pop_front();

        // We only need to handle watchers of the *negation* of assignedLit
        Literal notLit = assignedLit.negate();
        auto &oldWatchList = mWatchers[indexOf(notLit)];

        // We'll build a fresh watch list for notLit as we iterate
        std::vector<ClausePointer> newWatchList;
        newWatchList.reserve(oldWatchList.size());

        for (auto &cl : oldWatchList) {
            // Which rank is notLit?
            short rank = cl->getRank(notLit);
            short otherRank = rank ^ 1;  // flip 0<->1
            Literal otherWatch = cl->getWatcherByRank(otherRank);

            // If other watcher is satisfied, do nothing
            if (satisfied(otherWatch)) {
                newWatchList.push_back(cl);
                continue;
            }

            // Attempt ring search for a replacement literal
            std::size_t startIdx = cl->getIndex(rank);
            bool replaced = false;

            for (std::size_t i = 0, sz = cl->size(); i < sz; i++) {
                // Next index in ring fashion
                std::size_t idx = (startIdx + 1 + i) % sz;
                // Skip the other watch
                if (idx == cl->getIndex(otherRank)) {
                    continue;
                }
                Literal candidate = (*cl)[idx];

                if (!falsified(candidate)) {
                    // Switch watch from notLit -> candidate
                    cl->setWatcher(candidate, rank);
                    // Put the clause in watchers of candidate
                    mWatchers[indexOf(candidate)].push_back(cl);
                    replaced = true;
                    break;
                }
            }

            if (!replaced) {
                // No replacement found => otherWatch must be unit
                if (falsified(otherWatch)) {
                    return false; // Conflict
                }
                if (val(var(otherWatch)) == TruthValue::Undefined) {
                    if (!assign(otherWatch)) {
                        return false; // Conflict
                    }
                }
                // We continue to watch notLit
                newWatchList.push_back(cl);
            }
        }
        // Swap in the updated watch list for notLit
        oldWatchList.swap(newWatchList);
    }
    return true; // No conflict
}

bool Solver::solve() {
    // We'll represent three states: UNKNOWN, SAT, UNSAT
    enum class State { UNKNOWN, SAT, UNSAT };
    State satisfiability = State::UNKNOWN;

    // Clear any old decisions
    mTrail.clear();
    mTrail.reserve(mModel.size());
    
    // Reset model to undefined
    std::fill(mModel.begin(), mModel.end(), TruthValue::Undefined);
    mUnitQueue.clear();

    // Repeatedly propagate or branch until SAT/UNSAT is determined
    while (satisfiability == State::UNKNOWN) {
        // First, propagate forced literals
        if (!unitPropagate()) {
            // Conflict -> check if we can backtrack
            if (mTrail.empty()) {
                satisfiability = State::UNSAT;
            } else {
                //  Backtrack: revert to last decision level
                std::size_t checkpoint = mTrail.back();
                mTrail.pop_back();
                unassignBack(checkpoint);
                
                // Try to flip the last decision
                if (!branchFlip()) {
                    // If flipping fails and no more decisions, formula is UNSAT
                    if (mTrail.empty()) {
                        satisfiability = State::UNSAT;
                    }
                    continue;
                }
            }
        } else {
            // Check if all variables assigned -> SAT
            if (allVariablesAssigned()) {
                satisfiability = State::SAT;
            } else {
                // Save current decision level
                mTrail.push_back(mUnitQueue.size());
                
                // Make new decision
                Literal decisionLit = selectLit();
                if (!assign(decisionLit)) {
                    mTrail.pop_back();
                    if (mTrail.empty()) {
                        satisfiability = State::UNSAT;
                    }
                    continue;
                }
            }
        }
    }

    return satisfiability == State::SAT;
}

// Simple helper that picks a literal from any unassigned variable
Literal Solver::selectLit() {
    FirstVariable heuristic;
    Variable v = heuristic(mModel, mModel.size());
    // Default to positive literal, but many heuristics are possible
    return pos(v);
}

// Helper to revert assignments from the back of mUnitQueue until we reach 'checkpoint'
void Solver::unassignBack(std::size_t checkpoint) {
    while (mUnitQueue.size() > checkpoint) {
        Literal lit = mUnitQueue.back();
        mUnitQueue.pop_back();
        mModel[var(lit).get()] = TruthValue::Undefined;
    }
}

// Helper to flip the last assigned literal (if needed)
bool Solver::branchFlip() {
    if (mUnitQueue.empty()) {
        return false;
    }
    // Last assigned literal
    Literal lit = mUnitQueue.back();
    // Remove it from queue & unassign it
    mUnitQueue.pop_back();
    mModel[var(lit).get()] = TruthValue::Undefined;

    // Attempt to assign its negation
    Literal flipped = lit.negate();
    if (!assign(flipped)) {
        return false;
    }
    return true;
}


bool Solver::allVariablesAssigned() const {
    for (const auto& tv : mModel) {
        if (tv == TruthValue::Undefined) {
            return false;
        }
    }
    return true;
}

} // namespace sat