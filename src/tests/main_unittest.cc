#include <gtest/gtest.h>

TEST(MainTest, basic) {
    bool value = true;
    
    EXPECT_TRUE(value);
    EXPECT_EQ(true, value);
}
