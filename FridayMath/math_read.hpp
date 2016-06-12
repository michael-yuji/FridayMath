//
//  math_read.h
//  FridayMath
//
//  Created by Yuji on 6/10/16.
//  Copyright © 2016 YS. All rights reserved.
//

#ifndef math_read_hpp
#define math_read_hpp

#include <iostream>
#include <string>
#include <cmath>
#include <memory>
#include <stack>
#include <map>
#include <exception>

namespace std {
    template <class T, class ...Args>
    static typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
    make_unique(Args &&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

namespace tok {
    enum {
        eof = -1,   add = -2,   sub = -3,
        mul = -4,   div = -5,   pow = -6,
        frac = -7,  sqrt = -8,  exp = -9,
        ln = -10,   log = -11,
        var = -15,  num = -20,
    };
}


class Scanner {
    std::string s;
    std::string varName;
    int position;
    int curtok;
    double curVal;
    bool hasError;
public:
    class Expr {
    public:
        virtual double eval() = 0;
    };
private:
    std::map<std::string, std::unique_ptr<Expr>> varMap;
    
    char getc() {
        if (position == s.size()) return EOF;
        return s[position++];
    }
    
    
    int getTokPrec(int op) {
        switch (op) {
            case '(':
                return 10;
            case tok::add:
            case tok::sub:
                return 20;
            case tok::mul:
            case tok::div:
                return 30;
            case tok::pow:
                return 35;
            case tok::frac:
                return 40;
        }
        return -1;
    }
    
    char gettok() {
        char last = ' ';
        
        while (isspace(last))
            last = getc();
        
        if (isdigit(last) || last == '.' || last == '-') {
            std::string numStr;
            
            do {
                numStr += last;
                last = getc();
                
            } while (isdigit(last) || last == '.');
            
            if (numStr != "") {
                curVal = strtod(numStr.c_str(), 0);
                return tok::num;
            } else {
                return tok::sub;
            }
        }
        
        switch (last) {
            case '+': return tok::add;
            case '-': return tok::sub;
            case '*': return tok::mul;
            case '/': return tok::div;
            case '^': return tok::pow;
            case '!': return tok::frac;
                
            case '(': return '(';
                
            case ')': return ')';
        }
        
        if (isalpha(last)) {
            std::string t;
            while (isalpha(last)) {
                t += last;
                last = getc();
            }
            
#define nlk(name) if (t == #name) return tok::name;
            nlk(sqrt);
            nlk(ln);
            nlk(log);
            nlk(exp);
#undef nlk
            varName = t;
            return tok::var;
        }
        
        return last;
    }
    
    char getNextTok() {
        
        curtok = gettok();
        return curtok;
    }
    
    class NumberExpr : public Expr {
        double val;
    public:
        double eval() override {
            
            return val;
        }
        NumberExpr(double val) : val(val) {};
    };
    
    class InfixExpr : public Expr {
        std::unique_ptr<Expr> LHS, RHS;
        int op;
    public:
        double eval() override {
            
            if (!LHS || !RHS)
                throw CannotParseException("Infix: LHS | RHS is NULL");
                
                switch (op) {
                    case tok::add:
                        return LHS->eval() + RHS->eval();
                    case tok::sub:
                        return LHS->eval() - RHS->eval();
                    case tok::mul:
                        return LHS->eval() * RHS->eval();
                    case tok::div:
                        return LHS->eval() / RHS->eval();
                    case tok::pow:
                        return pow(LHS->eval(), RHS->eval());
                }
            return 0;
        }
        
        InfixExpr(int op, std::unique_ptr<Expr> lhs,std::unique_ptr<Expr> rhs)
        : op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {
        }
    };
    
    class UExpr : public Expr {
        int op;
        std::unique_ptr<Expr> e;
        public :
        
        double eval() override {
            
            if (!e) throw CannotParseException("Unary Operator: expression is NULL");
                
                switch (op) {
                    case tok::frac:
                        return tgamma(e->eval() + 1);
                    case tok::exp:
                        return exp(e->eval());
                    case tok::ln:
                        return log(e->eval());
                    case tok::log:
                        return log10(e->eval());
                    case tok::sqrt:
                        return sqrt(e->eval());
                }
            throw CannotParseException("Unary Operator: unknown operator");
        };
        
        UExpr(int op, std::unique_ptr<Expr>e) : op(op), e(std::move(e)) {}
    };
    
    class VarExpr : public Expr {
        std::string name;
        Scanner * scanner;
    public:
        double eval() override {
            if (!scanner->varMap[name]) throw CannotParseException("undined variable");
                return scanner->varMap[name]->eval();
                }
        
        VarExpr(Scanner * scanner, const std::string name) : scanner(scanner), name(name) {}
        
    };
    
    std::unique_ptr<Expr> LogError(std::string msg) {
        throw CannotParseException(msg);
        return nullptr;
    }
    
    std::unique_ptr<NumberExpr> parseNum() {
        auto result = std::make_unique<NumberExpr>(curVal);
        getNextTok();
        return std::move(result);
    }
    
    std::unique_ptr<VarExpr> parseVar() {
        auto result = std::make_unique<VarExpr>(this, varName);
        getNextTok();
        return std::move(result);
    }
    
    std::unique_ptr<Expr> parseInfixOp(int prec, std::unique_ptr<Expr> LHS) {
        
        while (true) {
            
            int curPrec = getTokPrec(curtok);
            
            if (curtok == tok::frac) {
                LHS = std::make_unique<UExpr>(curtok, std::move(LHS));
                getNextTok();
                continue;
            }
            
            if (curPrec < prec) {
                return std::move(LHS);
            }
            
            int op = curtok; // reserver
            
            getNextTok(); // eat the operator
            
            auto RHS = parse();
            if (!RHS)
                return nullptr;
            
            int nextPrec = getTokPrec(curtok);
            if (curPrec < nextPrec) {
                RHS = parseInfixOp(curPrec + 1, std::move(RHS));
                if (!RHS)
                    return nullptr;
            }
            
            if (op == '(') {
                LHS = std::make_unique<InfixExpr>(tok::mul, std::move(LHS), std::move(RHS));
                getNextTok();// eat the ')'
            } else
                LHS = std::make_unique<InfixExpr>(op, std::move(LHS), std::move(RHS));
        }
        
    }
    
    
    std::unique_ptr<Expr> parseExpr() {
        auto LHS = parse();
        if (!LHS)
            return LogError("Invalid LHS");
        
        switch (curtok) {
                // infix operators
            case tok::add:
            case tok::sub:
            case tok::mul:
            case tok::div:
            case tok::pow:
            case tok::frac:
            case '(':
                return parseInfixOp(0, std::move(LHS));
            case ')':
            case tok::eof:
                return std::move(LHS);
        }
        
        return LogError("unknown operator");
    }
    
    std::unique_ptr<Expr> parsePre(int op) {
        getNextTok(); // eat op
        auto e = parse();
        auto a = std::make_unique<UExpr>(op, std::move(e));
        return std::move(a);
    }
    
    std::unique_ptr<Expr> parseP() {
        getNextTok(); // eat '(';
        auto expr = parseExpr();
        if (curtok != ')') // eat ')';
            return LogError("Expected ')'");
        getNextTok();
        return expr;
    }
    
    std::unique_ptr<Expr> parse() {
        switch (curtok) {
            case tok::num:
                return parseNum();
            case tok::var:
                return parseVar();
            case tok::log:
            case tok::ln:
            case tok::sqrt:
            case tok::exp:
                
                return parsePre(curtok);
            case '(':
                return parseP();
        }
        
        return LogError("Unknown token");
    }
    
    std::unique_ptr<Expr> run() {
        getNextTok();
        return parseExpr();
    }
    
public:
    
    class CannotParseException : public std::exception {
        std::string msg;
    public:
        CannotParseException(const std::string msg) : msg(std::string("Cannot Evaluate Expression: " + msg)) {}
        const char * what() {
            return msg.c_str();
        }
    };
    
    void setVariable(std::string var, double val) {
        varMap[var] = std::make_unique<NumberExpr>(val);
    }
    
    void setVariable(std::string var, std::unique_ptr<Expr> val) {
        varMap[var] = std::move(val);
    }
    
    Scanner() : s(""), position(0) { setVariable("pi", M_PI); }
    
    std::unique_ptr<Expr> parse(std::string str) {
        s = str;
        position = 0;
        try {
            return std::move(run());
        } catch (CannotParseException &e) {
            std::cout << e.what() << std::endl;
            throw e;
        }
    }
};

#endif /* math_read_hpp */
