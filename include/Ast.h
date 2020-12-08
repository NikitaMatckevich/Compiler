#pragma once

#include <Lexer.h>

#include <memory>
#include <stdexcept>

class ConstVisitor;
class MutatingVisitor;

class Expr {
 public:
  virtual ~Expr()                                  = default;
  virtual void Accept(ConstVisitor* visitor) const = 0;
  virtual void Accept(MutatingVisitor* visitor)    = 0;
};

template <class ConcreteExpr>
class VisitableExpr : public Expr {
 public:
  void Accept(ConstVisitor* visitor) const override;
  void Accept(MutatingVisitor* visitor) override;
};

class Constant : public VisitableExpr<Constant> {
  Token number_;

 public:
  Constant(Token&& number);
  inline const Token& Value() const noexcept { return number_; }
  inline Token& Value() noexcept { // Do we really need this function, huh?
    return number_;
  }
};

class Variable : public VisitableExpr<Variable> {
  Token name_;

 public:
  Variable(Token&& name);
  inline const Token& Name() const noexcept { return name_; }
  inline Token& Name() noexcept { return name_; }
};

class UnaryExpr : public VisitableExpr<UnaryExpr> {
  std::unique_ptr<Expr> term_;
  std::optional<Token> subtraction_symbol_;

 public:
  UnaryExpr(std::unique_ptr<Expr>&& term, std::optional<Token>&& sub_symbol);
  inline const std::unique_ptr<Expr>& Term() const noexcept { return term_; }
  inline std::unique_ptr<Expr>& Term() noexcept { return term_; }
  inline bool IsNeg() const noexcept { return subtraction_symbol_.has_value(); }
  inline const std::optional<Token>& SubstractionToken() const noexcept {
    return subtraction_symbol_;
  }
  inline std::optional<Token>& SubstractionToken() noexcept {
    return subtraction_symbol_;
  }
};

class BinaryExpr : public VisitableExpr<BinaryExpr> {
  std::vector<Token> ops_;
  std::vector<std::unique_ptr<Expr>> terms_;

 public:
  BinaryExpr(std::vector<Token>&& ops,
             std::vector<std::unique_ptr<Expr>>&& terms);
  inline const std::vector<Token>& Ops() const noexcept { return ops_; }
  inline std::vector<Token>& Ops() noexcept { return ops_; }
  inline const std::vector<std::unique_ptr<Expr>>& Terms() const noexcept {
    return terms_;
  }
  inline std::vector<std::unique_ptr<Expr>>& Terms() noexcept { return terms_; }
};

class Assignment : public VisitableExpr<Assignment> {
  std::vector<std::unique_ptr<Expr>> terms_;

 public:
  Assignment(std::vector<std::unique_ptr<Expr>>&& terms);
  inline const std::vector<std::unique_ptr<Expr>>& Terms() const noexcept {
    return terms_;
  }
  inline std::vector<std::unique_ptr<Expr>>& Terms() noexcept { return terms_; }
};

class Declaration : public VisitableExpr<Declaration> {
  Token name_;
  std::optional<std::unique_ptr<Expr>> rhs_;

 public:
  Declaration(Token&& name, std::optional<std::unique_ptr<Expr>>&& rhs);
  inline const Token& Name() const noexcept { return name_; }
  inline Token& Name() noexcept { return name_; }
  inline const std::unique_ptr<Expr>& Rhs() const noexcept { return *rhs_; }
  inline std::unique_ptr<Expr>& Rhs() noexcept { return *rhs_; }
};

class Program : public VisitableExpr<Program> {
  std::vector<std::unique_ptr<Expr>> instructions_;

 public:
  Program(std::vector<std::unique_ptr<Expr>>&& instructions);
  inline const std::vector<std::unique_ptr<Expr>>&
  Instructions() const noexcept {
    return instructions_;
  }
  inline std::vector<std::unique_ptr<Expr>>& Instructions() noexcept {
    return instructions_;
  }
};

class Parser {
  Lexer& lex_;
  std::unique_ptr<UnaryExpr> ReadUnaryOp();
  std::unique_ptr<BinaryExpr> ReadMulDiv();
  std::unique_ptr<BinaryExpr> ReadAddSub();
  std::unique_ptr<Assignment> ReadAssignment();
  std::unique_ptr<Declaration> ReadDeclaration();

 public:
  explicit Parser(Lexer& lex);
  std::unique_ptr<Program> ReadProgram();
};

template <class... Types>
constexpr const Token& Expect(const Token& t) {
  if (!(... || t.Is<Types>())) {
    throw SyntaxError(
        t, "expected ", types::Describe<Types...>(), ", got ",
        std::quoted(t.GetContext().GetTokenString(), /* delim= */ '\''));
  }
  return t;
}

template <class... Types>
void Rewind(Lexer& lexer) {
  Token t = lexer.Get();
  while (!(... || t.Is<Types>()) && !t.Is<types::Eof>()) {
    t = lexer.Get();
  }
}
