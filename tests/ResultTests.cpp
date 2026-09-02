#include "Result.hpp"

#include <gtest/gtest.h>

#include <memory>

TEST(ResultTest, OkStoresAValue) {
    auto result = Result<int, int>::Ok(42);

    EXPECT_TRUE(result.IsOk());
    EXPECT_FALSE(result.IsErr());
    EXPECT_EQ(result.value(), 42);
}

TEST(ResultTest, ErrStoresAnError) {
    auto result = Result<int, int>::Err(-1);

    EXPECT_TRUE(result.IsErr());
    EXPECT_FALSE(result.IsOk());
    EXPECT_EQ(result.error(), -1);
}

TEST(ResultTest, SupportsMoveOnlyValues) {
    auto result = Result<std::unique_ptr<int>, int>::Ok(std::make_unique<int>(7));

    ASSERT_TRUE(result.IsOk());
    ASSERT_NE(result.value(), nullptr);
    EXPECT_EQ(*result.value(), 7);
}
