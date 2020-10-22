#pragma once
#include "Lexer.h"
#include <memory>
#include <stdexcept>

template <class T>
using expr_ptr = std::unique_ptr<T>;

struct SyntaxError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct Expression {
  virtual int exec() const = 0;
  virtual ~Expression() = default;
};

struct Int : Expression {
  int value;
  Int(int);
  virtual int exec() const override final;
};

struct UnaryExpression : Expression {
  expr_ptr<Expression> term;
  bool is_neg;
  UnaryExpression(expr_ptr<Expression>&&, bool);
  virtual int exec() const override final;
};

struct BinaryExpression : Expression {
  std::vector<Token> ops;
  std::vector<expr_ptr<Expression>> terms;
  BinaryExpression(std::vector<Token>&&, std::vector<expr_ptr<Expression>>&&);
  virtual int exec() const override final;
};

struct Program : Expression {
  std::vector<expr_ptr<Expression>> instructions;
  Program(std::vector<expr_ptr<Expression>>&&);
  virtual int exec() const override final;
};

expr_ptr<Program> readProgram(const Lexer&, size_t&);
