#include <algorithm>
#include "Clause.hpp"
#include "util/exception.hpp"

namespace sat {

    Clause::Clause(std::vector<Literal> literals)
        : mLits(std::move(literals))
    {
        // If we have at least one literal, watch literal at index 0
        // If we have 2 or more, watch literal at index 1
        // If only one literal, watchers both refer to index 0
        if (!mLits.empty()) {
            mWatchIndex[0] = 0;
            mWatchIndex[1] = (mLits.size() > 1) ? 1 : 0;
        }
    }

    short Clause::getRank(Literal l) const {
        if (!mLits.empty() && mLits[mWatchIndex[0]] == l) {
            return 0;
        }
        if (!mLits.empty() && mLits[mWatchIndex[1]] == l) {
            return 1;
        }
        return -1; // l is not a watcher
    }

    std::size_t Clause::getIndex(short rank) const {
        // Return the index in mLits for the requested watcher rank
        return (rank == 0) ? mWatchIndex[0] : mWatchIndex[1];
    }

    bool Clause::setWatcher(Literal l, short watcherNo) {
        // Find l in the clause
        auto it = std::find(mLits.begin(), mLits.end(), l);
        if (it == mLits.end()) {
            return false;  // literal not found
        }
        // Found it; set watcher
        std::size_t idx = static_cast<std::size_t>(std::distance(mLits.begin(), it));
        if (watcherNo == 0) {
            mWatchIndex[0] = idx;
        } else {
            mWatchIndex[1] = idx;
        }
        return true;
    }

    Literal Clause::getWatcherByRank(short rank) const {
        // Return the literal that the rank-th watcher points to
        return (rank == 0)
            ? mLits[mWatchIndex[0]]
            : mLits[mWatchIndex[1]];
    }

    auto Clause::begin() const -> std::vector<Literal>::const_iterator {
        return mLits.begin();
    }

    auto Clause::end() const -> std::vector<Literal>::const_iterator {
        return mLits.end();
    }

    bool Clause::isEmpty() const {
        return mLits.empty();
    }

    Literal Clause::operator[](std::size_t index) const {
        return mLits[index];
    }

    std::size_t Clause::size() const {
        return mLits.size();
    }

    bool Clause::sameLiterals(const Clause &other) const {
        // Quick check: same size => then do a sorted compare
        if (mLits.size() != other.mLits.size()) {
            return false;
        }

        // Make copies, sort them by their .get() value
        std::vector<Literal> mine = mLits;
        std::vector<Literal> theirs = other.mLits;

        auto cmp = [](const Literal &a, const Literal &b){
            return a.get() < b.get();
        };

        std::sort(mine.begin(), mine.end(), cmp);
        std::sort(theirs.begin(), theirs.end(), cmp);

        return (mine == theirs);
    }

} // namespace sat
