#pragma once
#include "Lexer.h"
#include <memory>
#include <stdexcept>

class expr {
public:
  virtual int exec() const = 0;
  virtual ~expr() = default;
};

class constant : public expr {
  int value;
public:
  constant(int);
  virtual int exec() const override final;
};

class unary_expr : public expr {
  std::unique_ptr<expr> term;
public:
  bool is_neg;
  unary_expr(std::unique_ptr<expr>&&, bool);
  virtual int exec() const override final;
};

class binary_expr : public expr {
  std::vector<token> ops;
  std::vector<std::unique_ptr<expr>> terms;
public:
  binary_expr(std::vector<token>&&, std::vector<std::unique_ptr<expr>>&&);
  virtual int exec() const override final;
};

class program : public expr {
  std::vector<std::unique_ptr<expr>> instructions;
public:
  program(std::vector<std::unique_ptr<expr>>&&);
  virtual int exec() const override final;
};

class parser {
  lexer& _lex;
  std::unique_ptr<unary_expr> read_unary_op(); 
  std::unique_ptr<binary_expr> read_mul_div(); 
  std::unique_ptr<binary_expr> read_add_sub(); 
public:
  explicit parser(lexer& lex);
  std::unique_ptr<program> read_program(); 
};

template <class ...Types>
constexpr const token& expect(const token& t) {
	if (!(... || t.is<Types>()))
		throw syntax_error(t);
	return t;
}
