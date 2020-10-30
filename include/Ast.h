#pragma once
#include <memory>
#include <stdexcept>
#include <Lexer.h>
/*
class BaseVisitor {
 public:
  virtual ~BaseVisitor() = default;
  virtual void Visit() = 0;
};
*/
class Expr {
 public:
  virtual ~Expr() = default;
  //virtual void Accept() = 0;
};

/*
template <class ConcreteExpr>
class VisitableExpr : public Expr {
 public:
  void Accept(Visitor& visitor) override final {
    visitor.Visit(static_cast<ConcreteExpr*>(this)); 
  }
};
*/

class Constant : public Expr {
  int value_;
 public:
  Constant(int value);
  inline int value() const {
    return value_;
  }
  inline int& value() {
    return value_;
  }
};

class UnaryExpr : public Expr {
  std::unique_ptr<Expr> term_;
  bool is_neg_;
 public:
  UnaryExpr(std::unique_ptr<Expr>&& term,
            bool is_neg);
  inline const std::unique_ptr<Expr>& term() const {
    return term_;
  }
  inline std::unique_ptr<Expr>& term() {
    return term_;
  }
  inline bool is_neg() const {
    return is_neg_;
  }
  inline bool& is_neg() {
    return is_neg_;
  }
};

class BinaryExpr : public Expr {
  std::vector<Token> ops_;
  std::vector<std::unique_ptr<Expr>> terms_;
 public:
  BinaryExpr(std::vector<Token>&& ops,
             std::vector<std::unique_ptr<Expr>>&& terms);
  inline const std::vector<Token>& ops() const {
    return ops_;
  }
  inline std::vector<Token>& ops() {
    return ops_;
  }
  inline const std::vector<std::unique_ptr<Expr>>& terms() const {
    return terms_;
  }
  inline std::vector<std::unique_ptr<Expr>>& terms() {
    return terms_;
  }
};

class Program : public Expr {
  std::vector<std::unique_ptr<Expr>> instructions_;
 public:
  Program(std::vector<std::unique_ptr<Expr>>&& instructions);
  inline const std::vector<std::unique_ptr<Expr>>& instructions() const {
    return instructions_;
  }
  inline std::vector<std::unique_ptr<Expr>>& instructions() {
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
