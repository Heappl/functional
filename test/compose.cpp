#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "compose.h"

int f(int x) { return x * 2; }
int g(int x) { return x + 3; }
int h(int x) { return x * 7; }

TEST(Compose, properComposeSingleParameter)
{
    int x = 5;
    ASSERT_EQ((2 * 5 + 3) * 7, imp::compose(h, g, f)(x));
    ASSERT_EQ((7 * 5 + 3) * 2, imp::compose(f, g, h)(x));
    ASSERT_EQ((5 + 3) * 7 * 2, imp::compose(h, f, g)(x));
}

std::string ff(std::string x, std::string y, std::string z) { return "ff" + x + y + z + "ff"; }
std::string gg(std::string x, std::string y, std::string z) { return "gg" + x + y + z + "gg"; }
std::string hh(std::string x, std::string y, std::string z) { return "hh" + x + y + z + "hh"; }

TEST(Compose, properComposeMultipleParams)
{
    ASSERT_EQ("ffgghh123hh45gg67ff", imp::compose(ff, gg, hh)("1", "2", "3", "4", "5", "6", "7"));
}

