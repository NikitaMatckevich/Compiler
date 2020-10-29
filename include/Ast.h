#pragma once
#include "Lexer.h"
#include <memory>
#include <stdexcept>

class Expr {
public:
  virtual int Exec() const = 0;
  virtual ~Expr() = default;
};

class Constant : public Expr {
  int value_;
public:
  Constant(int);
  virtual int Exec() const override final;
};

class UnaryExpr : public Expr {
  std::unique_ptr<Expr> term_;
  bool is_neg_;
public:
  UnaryExpr(std::unique_ptr<Expr>&&, bool);
  virtual int Exec() const override final;
};

class BinaryExpr : public Expr {
  std::vector<Token> ops_;
  std::vector<std::unique_ptr<Expr>> terms_;
public:
  BinaryExpr(std::vector<Token>&&, std::vector<std::unique_ptr<Expr>>&&);
  virtual int Exec() const override final;
};

class Program : public Expr {
  std::vector<std::unique_ptr<Expr>> instructions_;
public:
  Program(std::vector<std::unique_ptr<Expr>>&&);
  virtual int Exec() const override final;
};

class Parser {
  Lexer& lex_;
  std::unique_ptr<UnaryExpr> ReadUnaryOp();
  std::unique_ptr<BinaryExpr> ReadMulDiv();
  std::unique_ptr<BinaryExpr> ReadAddSub();
public:
  explicit Parser(Lexer& lex);
  std::unique_ptr<Program> ReadProgram();
};

template <class ...Types>
constexpr const Token& Expect(const Token& t) {
	if (!(... || t.Is<Types>())) {
		throw SyntaxError(t);
}
	return t;
}
