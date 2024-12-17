/**
* @author Tim Luchterhand
* @date 26.11.24
* @brief
*/

#include <cassert>
#include <algorithm>
#include <unordered_set>

#include "Clause.hpp"
#include "util/exception.hpp"


namespace sat {

    Clause::Clause() 
        : literals(), 
          watchers{Literal(0), Literal(0)} {}

    Clause::Clause(std::vector<Literal> literals) 
        : literals(std::move(literals)), 
          watchers{Literal(0), Literal(0)} {
        if (this->literals.size() >= 2) {
            watchers[0] = this->literals[0];
            watchers[1] = this->literals[1];
        } else if (this->literals.size() == 1) {
            watchers[0] = this->literals[0];
            watchers[1] = Literal(0); // Assuming default constructor for Literal
        }
    }

    short Clause::getRank(Literal l) const {
        if (watchers[0] == l) return 0;
        if (watchers[1] == l) return 1;
        return -1;
    }

    std::size_t Clause::getIndex(short rank) const {
        if (rank < 0 || rank >= 2) {
            throw std::out_of_range("Rank out of range");
        }
        auto it = std::find(literals.begin(), literals.end(), watchers[rank]);
        if (it != literals.end()) {
            return std::distance(literals.begin(), it);
        }
        throw std::out_of_range("Watcher not found in literals");
    }

    bool Clause::setWatcher(Literal l, short watcherNo) {
        if (watcherNo < 0 || watcherNo >= 2) {
            throw std::out_of_range("Watcher number out of range");
        }
        auto it = std::find(literals.begin(), literals.end(), l);
        if (it != literals.end()) {
            watchers[watcherNo] = l;
            return true;
        }
        return false;
    }

    auto Clause::begin() const -> std::vector<Literal>::const_iterator {
        return literals.begin();
    }

    auto Clause::end() const -> std::vector<Literal>::const_iterator {
        return literals.end();
    }

    bool Clause::isEmpty() const {
        return literals.empty();
    }

    Literal Clause::operator[](std::size_t index) const {
        if (index < literals.size()) {
            return literals[index];
        }
        throw std::out_of_range("Index out of range");
    }

    std::size_t Clause::size() const {
        return literals.size();
    }

    Literal Clause::getWatcherByRank(short rank) const {
        if (rank >= 0 && rank < 2) {
            return watchers[rank];
        }
        throw std::out_of_range("Rank out of range");
    }

    bool Clause::sameLiterals(const Clause &other) const {
        if (literals.size() != other.literals.size()) {
            return false;
        }
        std::unordered_set<unsigned> literalSet;
        for (const auto& literal : literals) {
            literalSet.insert(literal.get());
        }
        for (const auto& literal : other.literals) {
            if (literalSet.find(literal.get()) == literalSet.end()) {
                return false;
            }
        }
        return true;
    }

}