#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "compose.h"

namespace
{
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


TEST(Compose, properComposeMultipleParams)
{
    std::string aux = "gg";
    std::string auxh = "hh";
    auto ff = [](std::string x, std::string y, std::string z) { return "ff" + x + y + z + "ff"; };
    auto gg = [&](std::string x, std::string y, std::string z) { return aux + x + y + z + aux; };
    auto hh = [&](std::string x, std::string y, std::string z) { return auxh + x + y + z + "hh"; };
    ASSERT_EQ("ffgghh123hh45gg67ff", imp::compose(ff, gg, hh)("1", "2", "3", "4", "5", "6", "7"));
}

struct A { int x; };
struct B { int x; };
struct C { int x; };
struct D { int x; };
struct E { int x; };
struct F { int x; };

C fff(A a, B b) { return C{a.x + b.x}; }
auto ggg = [](D d, C c) { return D{d.x * c.x}; };

TEST(Compose, composeByTypeMatching)
{
    ASSERT_EQ((1 + 7) * 3, imp::type_compose(ggg, fff)(D{3}, A{1}, B{7}).x);
}
} //namespace

