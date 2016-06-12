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
    std::unique_ptr<Scanner::Expr> mu = math_scanner.parse("( 0.5 * 10 ^ -1 * 10 ^ -2 ) ^ 2 * pi ");
    std::cout << mu->eval() << std::endl;
    return 0;
}
