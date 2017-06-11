#include <iostream>
#include <string>
#include <curry.h>
#include <apply.h>
#include <named_param.h>
#include <compose.h>
#include <typeinfo>

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
    return x + y * z;
}

SomeText test4(int, std::string txt)
{
    return SomeText(txt + " from test4");
}

std::string test3(SomeText text1, OtherText text2, CharCarrier text3)
{
    return text1 + text3[0] + text2;
}

struct Functor { bool operator()(int x, float y, char z) const { std::cout << x << " " << y << " " << z << std::endl; return (x < y) == z; } };
    

int main()
{
    using namespace imp;
    auto t0 = apply(test) << std::string("out of order");
    t0(1, 1.9);

    apply(test) << 1 << 1.4 << "apply with <<";
    apply(test) << "apply with << in different order" << 1 << 1.4;
    apply(test, 1, 1.4, "apply with function");
    apply(test, 1.5, "apply with function with different order", 2);
    curry(test, 1, 1.4, "curry with function")();

    auto t1 = apply(test2);
    t1 << CoordY(3) << CoordZ(9) << CoordX(11);

    auto t2 = curry(test2) << CoordX(1) << CoordY(2) << CoordZ(3);
    t2();

    std::cout << "compose test" << std::endl;
    auto proper4 = compose(test3, test4, test, test2);
    std::cout << proper4(CoordX(1), CoordY(2), CoordZ(3), 1.4, "compose with spline", "composition", OtherText("sth"), CharCarrier("%ba")) << std::endl;
    std::cout << apply(compose(test4, test2), CoordX(2), "compose with spline and appply", CoordZ(6), CoordY(66)) << std::endl; 
}
