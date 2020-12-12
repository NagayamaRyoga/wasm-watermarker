#include "kyut/SafeUnique.hpp"

#include <string>
#include <gtest/gtest.h>

TEST(kyut, safe_unique) {
    std::string x = "01122233458889";

    const auto result = kyut::safe_unique(std::begin(x), std::end(x), [](char a, char b) { return a == b; });

    EXPECT_EQ(result, std::begin(x) + 8);
    EXPECT_EQ(x, "01234589122388");
}
