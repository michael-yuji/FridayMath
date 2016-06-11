# FridayMath

Simple Nature Math Expression Parser that support variables/functions.

Expressions only need to `compile` (nope, but kinda similar) once, and reuse over and over.

For example:

```Cpp
#include <iostream>
#include "math_read.hpp"

int main(int argc, const char * argv[]) {
    /* Initalize Scanner, scanner is also the container of variables/functions */
    Scanner math_scanner = Scanner();
    
    /* let's define a function z(x, y) = x + y + 2 */
    std::unique_ptr<Scanner::Expr> z = math_scanner.parse("x + y + 2");
    
    /* define y: y(x) = 20 + x * exp ( 2 ) */
    std::unique_ptr<Scanner::Expr> y = math_scanner.parse("20 + x * exp ( 2 )");
    
    /* now tell the scanner that letter 'y' represent the function we have */
    math_scanner.setVariable("y", std::move(y));
    
    /* now set x as a number */
    math_scanner.setVariable("x", 10);
    
    /* evaluate the solution */
    printf("result is: %f\n", z->eval());
    return 0;
}
```

### Known Issue

Remeber to put space between expressions
