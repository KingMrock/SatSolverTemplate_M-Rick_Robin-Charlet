/**
* @author Tim Luchterhand
* @date 26.11.24
* @brief
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <unordered_set>

#include "util/concepts.hpp"
#include "Clause.hpp"
#include "testing_utils.hpp"


TEST(clause, clause_empty_ctor) {
    using namespace sat;
    Clause c;
    EXPECT_TRUE(c.isEmpty());
    EXPECT_EQ(c.begin(), c.end());
    EXPECT_FALSE(c.setWatcher(3, 0));
}

TEST(clause, clause_ctor) {
    using namespace sat;
    std::vector<Literal> lits{2, 5, 3, 1, 4};
    test::LitSet gt(lits.begin(), lits.end());
    Clause c(lits);
    EXPECT_FALSE(c.isEmpty());
    EXPECT_TRUE(setsEqual(c, gt));
}

TEST(clause, equal_literals) {
    using namespace sat;
    std::vector<Literal> lits {3, 1, 4, 2};
    std::vector<Literal> sortedLits {1, 2, 3, 4};
    Clause c1(lits);
    Clause c2(sortedLits);
    EXPECT_TRUE(c1.sameLiterals(c2));
    EXPECT_TRUE(c2.sameLiterals(c1));
    Clause c3({4, 2, 5, 1});
    EXPECT_FALSE(c3.sameLiterals(c1));
    EXPECT_FALSE(c2.sameLiterals(c3));
}

TEST(clause, watchers) {
    using namespace sat;
    Clause c({5, 2, 3, 4, 1});
    c.setWatcher(3, 0);
    c.setWatcher(5, 1);
    EXPECT_EQ(c.getRank(3), 0);
    EXPECT_EQ(c.getRank(5), 1);
    EXPECT_EQ(c.getRank(19), -1);
    EXPECT_EQ(c.getRank(2), -1);
    EXPECT_EQ(c.getWatcherByRank(0), 3);
    EXPECT_EQ(c.getWatcherByRank(1), 5);
}

TEST(clause, clause_set_watcher) {
    using namespace sat;
    Clause c({5, 2, 3, 4, 1});
    c.setWatcher(4, 1);
    c.setWatcher(5, 0);
    c.setWatcher(2, 0);
    EXPECT_EQ(c.getWatcherByRank(0), 2);
    EXPECT_EQ(c.getWatcherByRank(1), 4);
    EXPECT_EQ(c.getRank(5), -1);
    EXPECT_EQ(c.getWatcherByRank(0), 2);
    EXPECT_EQ(c.getWatcherByRank(1), 4);
}

TEST(clause, watcher_index) {
    using namespace sat;
    Clause c({5, 2, 3, 4, 1});
    c.setWatcher(4, 1);
    c.setWatcher(2, 0);
    EXPECT_EQ(c.getWatcherByRank(0), c[c.getIndex(0)]);
    EXPECT_EQ(c.getWatcherByRank(1), c[c.getIndex(1)]);
}

#ifndef __RUN_ALL_TESTS__

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
