#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "curry.h"

namespace
{
TEST(Curry, curryFunctionOnSimpleTypes)
{
    struct Test{ std::string val; };
    auto func = [](int i, const char* c, float f, const Test& t){ 
        return std::to_string(i) + "," + c + "," + std::to_string(f) + "," + t.val; };
    
    ASSERT_EQ("121,cstr,4.130000,test", imp::curry(func, 121, "cstr", 4.13, Test{"test"})());
    ASSERT_EQ("121,cstr,4.130000,test", (imp::curry(func, 121, "cstr", 4.13) << Test{"test"})());
    ASSERT_EQ("121,cstr,4.130000,test", (imp::curry(func) << 121 << "cstr" << 4.13 << Test{"test"})());
}

TEST(Curry, curryFunctorOnStructs)
{
    struct First{ int x; };
    struct Second{ int x; };
    struct Third{ int x; };

    struct Functor
    {
        int operator()(First f, Second s, Third t) const { return f.x + s.x * 10 + t.x * 100; }
    };

    ASSERT_EQ(351, imp::curry(Functor(), First{1}, Second{5}, Third{3})());
    ASSERT_EQ(351, (imp::curry(Functor(), First{1}, Second{5}) << Third{3})());
    ASSERT_EQ(925, imp::curry(imp::curry(Functor()) << First{5}, Second{2}, Third{9})());
}
} //namespace

