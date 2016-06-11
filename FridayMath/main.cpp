//
//  main.cpp
//  FridayMath
//
//  Created by Yuji on 6/10/16.
//  Copyright © 2016 YS. All rights reserved.
//

#include <iostream>
#include "math_read.hpp"

int main(int argc, const char * argv[]) {
    Scanner math_scanner = Scanner();
    std::unique_ptr<Scanner::Expr> z = math_scanner.parse("x + y + 2");
    std::unique_ptr<Scanner::Expr> y = math_scanner.parse("20 + x * exp ( 2 )");
    math_scanner.setVariable("y", std::move(y));
    math_scanner.setVariable("x", 10);
    
    printf("result is: %f\n", z->eval());
    return 0;
}
