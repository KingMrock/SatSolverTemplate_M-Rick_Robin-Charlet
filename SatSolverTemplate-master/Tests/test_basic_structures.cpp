/**
* @author Tim Luchterhand
* @date 26.11.24
* @brief
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "basic_structures.hpp"

TEST(structures, basic_ctors) {
    using namespace sat;
    Variable x = 3;
    Literal l = 7;
    EXPECT_EQ(l.get(), 7);
    EXPECT_EQ(l, 7);
    EXPECT_EQ(x.get(), 3);
    EXPECT_EQ(x, 3);
}

TEST(structures, literal) {
    using namespace sat;
    Literal l = 4;
    EXPECT_EQ(l.sign(), -1);
    EXPECT_EQ(l.negate(), 5);
    EXPECT_EQ(l.negate().sign(), 1);
}

TEST(structures, conversion_var_lit) {
    using namespace sat;
    Variable x = 2;
    EXPECT_EQ(pos(x), 5);
    EXPECT_EQ(neg(x), 4);
    EXPECT_EQ(var(pos(x)), x);
    EXPECT_EQ(var(neg(x)), x);
    EXPECT_EQ(var(7), 3);
}

#ifndef __RUN_ALL_TESTS__

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif