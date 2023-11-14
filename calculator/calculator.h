#pragma once

#include <algorithm>
#include <sstream>
#include <memory>
#include <string>

class Tokenizer {
public:
    explicit Tokenizer(std::istream* in) : in_{in} {
        Consume();
    }

    enum TokenType { kNumber, kSymbol, kEnd };

    void Consume() {
        while (true) {
            auto symbol = in_->peek();
            if (symbol == std::char_traits<char>::eof()) {
                type_ = TokenType::kEnd;
                break;
            } else if (k_operations.find(symbol) != std::string::npos) {
                type_ = TokenType::kSymbol;
                symbol_ = in_->get();
                break;
            } else if (symbol >= '0' && symbol <= '9') {
                type_ = TokenType::kNumber;
                number_ = 0;
                do {
                    number_ = number_ * 10 + in_->get() - '0';
                    symbol = in_->peek();
                } while (symbol != in_->eof() && symbol >= '0' && symbol <= '9');
                break;
            } else {
                in_->get();
            }
        }
    }

    TokenType GetType() const {
        return type_;
    }

    int64_t GetNumber() const {
        return number_;
    }

    char GetSymbol() const {
        return symbol_;
    }

private:
    std::istream* in_;

    TokenType type_;
    int64_t number_;
    char symbol_;
    inline static std::string k_operations = "()+-*/";
};

class Expression {
public:
    virtual ~Expression() = default;
    virtual int64_t Evaluate() const = 0;
};

class F : public Expression {
public:
    F(Tokenizer* tokenizer);
    int64_t Evaluate() const override;

private:
    std::unique_ptr<Expression> f_ = nullptr;
    std::unique_ptr<Expression> s_ = nullptr;
    int64_t number_;
};

class T : public Expression {
public:
    T(Tokenizer* tokenizer);
    T(T&& other);
    int64_t Evaluate() const override;

private:
    std::unique_ptr<Expression> t_ = nullptr;
    std::unique_ptr<Expression> f_ = nullptr;
    char op_;
};

class S : public Expression {
public:
    S(Tokenizer* tokenizer);
    S(S&& other);
    int64_t Evaluate() const override;

private:
    std::unique_ptr<Expression> s_ = nullptr;
    std::unique_ptr<Expression> t_ = nullptr;
    char op_;
};

F::F(Tokenizer* tokenizer) {
    if (tokenizer->GetType() == Tokenizer::TokenType::kNumber) {
        number_ = tokenizer->GetNumber();
        tokenizer->Consume();
    } else if (tokenizer->GetType() == Tokenizer::TokenType::kSymbol) {
        if (tokenizer->GetSymbol() == '-') {
            tokenizer->Consume();
            f_ = std::unique_ptr<Expression>(new F(tokenizer));
        } else if (tokenizer->GetSymbol() == '(') {
            tokenizer->Consume();
            s_ = std::unique_ptr<Expression>(new S(tokenizer));
            if (tokenizer->GetSymbol() == ')') {
                tokenizer->Consume();
            }
        }
    }
}

int64_t F::Evaluate() const {
    if (f_) {
        return -1 * f_->Evaluate();
    } else if (s_) {
        return s_->Evaluate();
    } else {
        return number_;
    }
}

T::T(Tokenizer* tokenizer) {
    t_ = std::unique_ptr<Expression>(new F(tokenizer));
    while (tokenizer->GetType() == Tokenizer::TokenType::kSymbol) {
        char symbol = tokenizer->GetSymbol();
        if (symbol == '*' || symbol == '/') {
            if (f_) {
                t_ = std::unique_ptr<Expression>(new T(std::move(*this)));
            }
            op_ = tokenizer->GetSymbol();
            tokenizer->Consume();
            f_ = std::unique_ptr<Expression>(new F(tokenizer));
        } else {
            break;
        }
    }
}

T::T(T&& other) : t_(std::move(other.t_)), f_(std::move(other.f_)), op_(other.op_) {
    other.t_ = nullptr;
    other.f_ = nullptr;
}

int64_t T::Evaluate() const {
    int64_t res = t_->Evaluate();
    if (t_) {
        if (op_ == '*') {
            res *= f_->Evaluate();
        } else if (op_ == '/') {
            res /= f_->Evaluate();
        }
    }
    return res;
}

S::S(Tokenizer* tokenizer) {
    s_ = std::unique_ptr<Expression>(new T(tokenizer));
    while (tokenizer->GetType() == Tokenizer::TokenType::kSymbol) {
        char symbol = tokenizer->GetSymbol();
        if (symbol == '+' || symbol == '-') {
            if (t_) {
                s_ = std::unique_ptr<Expression>(new S(std::move(*this)));
            }
            op_ = tokenizer->GetSymbol();
            tokenizer->Consume();
            t_ = std::unique_ptr<Expression>(new T(tokenizer));
        } else {
            break;
        }
    }
}

S::S(S&& other) : s_(std::move(other.s_)), t_(std::move(other.t_)), op_(other.op_) {
    other.s_ = nullptr;
    other.t_ = nullptr;
}

int64_t S::Evaluate() const {
    int64_t res = s_->Evaluate();
    if (s_) {
        if (op_ == '+') {
            res += t_->Evaluate();
        } else if (op_ == '-') {
            res -= t_->Evaluate();
        }
    }
    return res;
}

std::unique_ptr<Expression> ParseExpression(Tokenizer* tokenizer) {
    return std::make_unique<S>(S(tokenizer));
}
