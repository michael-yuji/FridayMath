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
    std::unique_ptr<Scanner::Expr> mu = math_scanner.parse("( 0.5 * 10 ^ -1 * 10 ^ -2 ) ^ 2 * pi * 7800");
    
    std::cout << mu->eval() << std::endl;
    std::unique_ptr<Scanner::Expr> fn = math_scanner.parse("( n / ( 2 * L ) ) * sqrt ( T /  ( mu + t * ( 2.5 * 10 ^ -6 ) / L ) ) ");
    
    math_scanner.setVariable("t", 0);
    math_scanner.setVariable("n", 1.0f);
    math_scanner.setVariable("T", 680);
    math_scanner.setVariable("L", 0.78/100);
    math_scanner.setVariable("mu", std::move(mu));
    
    std::cout << "f1: " << fn->eval() << std::endl;
    
    math_scanner.setVariable("n", 2.0f);
    std::cout << "f2: " << fn->eval() << std::endl;
    
    math_scanner.setVariable("n", 3.0f);
    std::cout << "f3: " << fn->eval() << std::endl;

    std::unique_ptr<Scanner::Expr> T = math_scanner.parse(" mu * ( 2 * L * ( f - df ) ) ^ 2");
    math_scanner.setVariable("n", 1.0);
    
    math_scanner.setVariable("f", fn->eval());
    math_scanner.setVariable("df", 0.15);
    
    std::cout << "T: " << T->eval() << std::endl;
    
    math_scanner.setVariable("t", 1);
    std::cout << fn->eval() << std::endl;
    
    math_scanner.setVariable("t", 2);
    std::cout << fn->eval() << std::endl;
    
    math_scanner.setVariable("t", 3);
    std::cout << fn->eval() << std::endl;

    return 0;
}
