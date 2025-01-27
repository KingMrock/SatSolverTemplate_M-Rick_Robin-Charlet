/**
* @author Tim Luchterhand
* @date 27.11.24
* @file Solver.hpp
* @brief Contains the main solver class
*/

#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <memory>
#include <deque>
#include <vector>
#include "basic_structures.hpp"
#include "Clause.hpp"

namespace sat {

    using ClausePointer = std::shared_ptr<Clause>;
    using ConstClausePointer = std::shared_ptr<const Clause>;

    /**
     * @brief Main solver class
     */
    class Solver {
    private:
        // For each variable x, we store the current assignment:
        // True, False, or Undefined (unassigned).
        std::vector<TruthValue> mModel;

        // We store all clauses in shared pointers.
        std::vector<ClausePointer> mClauses;

        // For watch-literal propagation:
        // watchers[literal_id] is a list of clauses currently watching that literal.
        // If lit has ID = l.get(), watchers[lit.get()] returns all clauses that have lit as a watcher.
        std::vector<std::vector<ClausePointer>> mWatchers;

        // A queue (FIFO) of newly assigned literals for unit propagation.
        std::deque<Literal> mUnitQueue;

        // Helper function to map a Literal to its "index" for watchers.
        // Since we store the ID in the literal, we can just use that directly.
        inline std::size_t indexOf(Literal l) const {
            return static_cast<std::size_t>(l.get());
        }

    public:
        /**
         * Ctor. Allocates enough space for the variables.
         * @param numVariables Number of variables in the problem
         */
        explicit Solver(unsigned numVariables);

        /**
         * Adds a clause to the solver.
         * @param clause The clause to add
         * @return bool true if clause was successfully added,
         *              false if clause is empty or conflicts immediately with the current model
         */
        bool addClause(Clause clause);

        /**
         * Returns a reduced set of clauses. Excludes satisfied clauses
         * and removes falsified literals from clauses.
         * @return equivalent set of clauses
         */
        auto rebase() const -> std::vector<Clause>;

        /**
         * Returns the truth value of the given variable
         * @param x a variable (needs to be contained in the solver)
         * @return TruthValue of the given variable
         */
        TruthValue val(Variable x) const;

        /**
         * Checks if a literal is satisfied under the current model
         * @param l
         * @return true if literal l is satisfied, false otherwise
         */
        bool satisfied(Literal l) const;

        /**
         * Checks if a literal is falsified under the current model
         * @param l
         * @return true if literal l is falsified, false otherwise
         */
        bool falsified(Literal l) const;

        /**
         * Assigns the given literal to true in the model (if the literal is negative, the variable is set to false).
         * @param l The literal to assign
         * @return false if this literal is already falsified => conflict,
         *         true otherwise
         */
        bool assign(Literal l);

        /**
         * Performs unit propagation until a fixpoint or a conflict is found.
         * @return true if no conflict, false if conflict
         */
        bool unitPropagate();
    };

} // namespace sat

#endif //SOLVER_HPP
