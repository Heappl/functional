#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "curry.h"

TEST(Curry, curryFunctionOnSimpleTypes)
{
    struct Test{ std::string val; };
    auto func = [](int i, const char* c, float f, const Test& t){ 
        return std::to_string(i) + "," + c + "," + std::to_string(f) + "," + t.val; };
    
    //ASSERT_EQ("121,cstr,4.130000,test", imp::curry(func, 121, "cstr", 4.13, Test{"test"})());
    //ASSERT_EQ("121,cstr,4.130000,test", imp::curry(func, "cstr", 121, 4.13, Test{"test"})());
    //ASSERT_EQ("121,cstr,4.130000,test", imp::curry(func, 121, 4.13, Test{"test"}, "cstr")());
    //ASSERT_EQ("4,cstr,121.000000,test", imp::curry(func, 4.13, 121, Test{"test"}, "cstr")());
    //ASSERT_EQ("4,cstr,121.000000,other", imp::curry(func, Test{"other"}, 4.13, 121, "cstr")());

    //ASSERT_EQ("4,cstr,121.000000,other", (imp::curry(func) << Test{"other"} << 4.13 << 121 << "cstr")());
    //ASSERT_EQ("121,cstr,4.130000,test", (imp::curry(imp::curry(func, "cstr"), 121, 4.13) << Test{"test"})());
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

    //ASSERT_EQ(351, imp::curry(Functor(), Second{5}, First{1}, Third{3})());
    //ASSERT_EQ(147, imp::curry(Functor(), Third{1}, First{7}, Second{4})());
    //ASSERT_EQ(925, imp::curry(imp::curry(Functor()) << Third{9}, First{5}, Second{2})());
}

