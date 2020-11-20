#pragma once

#include <Lexer.h>

#include <memory>
#include <stdexcept>

class ConstVisitor;
class MutatingVisitor;

class Expr {
 public:
  virtual ~Expr() = default;
  virtual void Accept(ConstVisitor* visitor) const = 0;
  virtual void Accept(MutatingVisitor* visitor) = 0;
};

template <class ConcreteExpr>
class VisitableExpr : public Expr {
 public:
  void Accept(ConstVisitor* visitor) const override;
  void Accept(MutatingVisitor* visitor) override;
};

class Constant : public VisitableExpr<Constant> {
  int value_;

 public:
  Constant(int value);
  inline int Value() const { return value_; }
  inline int& Value() { // Do we really need this function, huh?
    return value_;
  }
};

class UnaryExpr : public VisitableExpr<UnaryExpr> {
  std::unique_ptr<Expr> term_;
  bool is_neg_;

 public:
  UnaryExpr(std::unique_ptr<Expr>&& term, bool is_neg);
  inline const std::unique_ptr<Expr>& Term() const { return term_; }
  inline std::unique_ptr<Expr>& Term() { return term_; }
  inline bool IsNeg() const { return is_neg_; }
  inline bool& IsNeg() { return is_neg_; }
};

class BinaryExpr : public VisitableExpr<BinaryExpr> {
  std::vector<Token> ops_;
  std::vector<std::unique_ptr<Expr>> terms_;

 public:
  BinaryExpr(std::vector<Token>&& ops,
             std::vector<std::unique_ptr<Expr>>&& terms);
  inline const std::vector<Token>& Ops() const { return ops_; }
  inline std::vector<Token>& Ops() { return ops_; }
  inline const std::vector<std::unique_ptr<Expr>>& Terms() const {
    return terms_;
  }
  inline std::vector<std::unique_ptr<Expr>>& Terms() { return terms_; }
};

class Program : public VisitableExpr<Program> {
  std::vector<std::unique_ptr<Expr>> instructions_;

 public:
  Program(std::vector<std::unique_ptr<Expr>>&& instructions);
  inline const std::vector<std::unique_ptr<Expr>>& Instructions() const {
    return instructions_;
  }

  inline std::vector<std::unique_ptr<Expr>>& Instructions() {
    return instructions_;
  }
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

template <class... Types>
constexpr const Token& Expect(const Token& t) {
  if (!(... || t.Is<Types>())) {
    throw SyntaxError(t);
  }
  return t;
}
