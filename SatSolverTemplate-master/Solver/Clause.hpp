/**
* @author Tim Luchterhand
* @date 26.11.24
* @file Clause.hpp
* @brief Contains the class Clause that consists of one or more literals
*/

#ifndef CLAUSE_HPP
#define CLAUSE_HPP

#include <vector>
#include <ostream>
#include "basic_structures.hpp"
#include "util/concepts.hpp"

namespace sat {

    /**
     * Concept modeling a clause. A clause is some range of literals, i.e. it must have accessible members
     * begin() and end() that return an iterator to the first or past-the-end elements respectively. For example,
     * std::vector<Literal> satisfies this concept.
     */
    template<typename T>
    concept clause_like = concepts::typed_range<T, Literal>;

    /**
     * @brief Clause class with watch literals.
     * @details
     * In order for it to model the clause_like concept, we must implement
     * the begin() and end() member functions. Typically, we store the literals in an std::vector<Literal>.
     * 
     * The watch-literal approach usually tracks two specific literals inside the clause called the watchers.
     * We do that by storing two indices (each referencing a position in the vector).
     */
    class Clause {
    private:
        // The actual literals in this clause
        std::vector<Literal> mLits;
        // Indices of the two watched literals in mLits
        std::size_t mWatchIndex[2] {0, 0};

    public:

        /**
         * Default CTor. If you want, you can remove it or adapt it to your needs.
         */
        Clause() = default;

        /**
         * Constructs a clause from the given vector of literals.
         * By default, we will set the watchers to the first and (if possible) second literal.
         */
        Clause(std::vector<Literal> literals);

        /**
         * Gets the watcher rank of the given Literal.
         * @param l The literal for which we want the watcher rank.
         * @return 
         *     - 0 if l is the first watcher,
         *     - 1 if l is the second watcher,
         *     - -1 if l is not a watcher of this clause.
         */
        short getRank(Literal l) const;

        /**
         * Gets the index in mLits of the watcher with the given rank.
         * @param rank 0 => first watcher, 1 => second watcher
         */
        std::size_t getIndex(short rank) const;

        /**
         * Sets the given literal l as the watcher of rank watcherNo.
         * @param l The literal that we want to watch.
         * @param watcherNo 0 or 1 – which watcher we want to replace.
         * @return true if the literal l was found in the clause and set as the watcher,
         *         false if l is not in this clause.
         */
        bool setWatcher(Literal l, short watcherNo);

        /**
         * Gets the watch literal by rank
         * @param rank 0 => first watcher, 1 => second watcher
         * @return The literal that is currently watched with rank == watcherNo
         */
        Literal getWatcherByRank(short rank) const;

        /**
         * Returns an iterator to the first literal of this clause
         */
        auto begin() const -> std::vector<Literal>::const_iterator;

        /**
         * Returns an iterator to one-past-the-last literal of this clause
         */
        auto end() const -> std::vector<Literal>::const_iterator;

        /**
         * Subscript operator for random access
         * @param index Which literal to retrieve
         * @return The literal at position index
         */
        Literal operator[](std::size_t index) const;

        /**
         * Check if this clause is empty.
         */
        bool isEmpty() const;

        /**
         * Get the number of literals in this clause.
         */
        std::size_t size() const;

        /**
         * Checks if the other clause has exactly the same set of literals (order-independent).
         * We do this by comparing sorted copies of the literal vectors.
         */
        bool sameLiterals(const Clause &other) const;
    };

} // namespace sat

#endif // CLAUSE_HPP
