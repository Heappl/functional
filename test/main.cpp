#include <iostream>
#include <string>
#include <curry.h>

int test(int x, float arg, const std::string& y) {
    std::cout << x << " " << arg << " " << y << std::endl;
    return x + 11;
}
 
NAMED_PARAM(CoordX, int);
NAMED_PARAM(CoordY, int);
NAMED_PARAM(CoordZ, int);
int test2(CoordX x, CoordY y, CoordZ z) {
    std::cout << "test2: " << x + y * z << std::endl;
    return x;
}

struct Functor { bool operator()(int x, float y, char z) const { std::cout << x << " " << y << " " << z << std::endl; return x < y == z; } };
 

 struct Test {};
 
int main()
{
    auto t0 = apply(test) << "out of order";
    t0(1, 1.9);

    apply(test, 1, 1.4, "ttt");
    curry(test, 1, 1.4, "ttt")();

    auto t1 = apply(test2);
    t1 << CoordY(3) << CoordZ(9) << CoordX(11);

    auto t2 = curry(test2) << CoordX(1) << CoordY(2) << CoordZ(3);
    t2();
}
