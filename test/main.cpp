#include <iostream>
#include <string>
#include <curry.h>
#include <apply.h>
#include <named_param.h>

int test(int x, float arg, const std::string& y) {
    std::cout << x << " " << arg << " " << y << std::endl;
    return x + 11;
}
 
SIMPLE_TYPE_NAMED_PARAM(CoordX, int);
SIMPLE_TYPE_NAMED_PARAM(CoordY, int);
SIMPLE_TYPE_NAMED_PARAM(CoordZ, int);
NAMED_PARAM(SomeText, std::string);
NAMED_PARAM(OtherText, std::string);
NAMED_PARAM(CharCarrier, std::string);
int test2(CoordX x, CoordY y, CoordZ z) {
    std::cout << "test2: " << (x + y * z) << std::endl;
    return x;
}

std::string test3(SomeText text1, OtherText text2, CharCarrier text3)
{
    return text1 + text3[0] + text2;
}

struct Functor { bool operator()(int x, float y, char z) const { std::cout << x << " " << y << " " << z << std::endl; return (x < y) == z; } };
 
struct Test { int operator[](int){ return 0; } };
 
int main()
{
    using namespace imp;
    auto t0 = apply(test) << "out of order";
    t0(1, 1.9);

    apply(test, 1, 1.4, "ttt");
    curry(test, 1, 1.4, "ttt")();

    auto t1 = apply(test2);
    t1 << CoordY(3) << CoordZ(9) << CoordX(11);

    auto t2 = curry(test2) << CoordX(1) << CoordY(2) << CoordZ(3);
    t2();

    test3(SomeText("testing"), OtherText("blah"), CharCarrier("what"));
}
