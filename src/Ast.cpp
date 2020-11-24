#include <Ast.h>
#include <ConstVisitors.h>
#include <MutatingVisitors.h>

#include <vector>

Constant::Constant(Token&& number)
    : number_{std::move(number)} {}

Variable::Variable(Token&& name)
    : name_{std::move(name)} {}

Declaration::Declaration(Token&& name, std::unique_ptr<Expr>&& rhs)
    : name_{std::move(name)}
    , rhs_{std::move(rhs)} {}

Assignment::Assignment(std::vector<std::unique_ptr<Expr>>&& terms)
    : terms_{std::move(terms)} {}

UnaryExpr::UnaryExpr(std::unique_ptr<Expr>&& term, bool is_neg)
    : term_{std::move(term)}
    , is_neg_(is_neg) {}

BinaryExpr::BinaryExpr(std::vector<Token>&& ops,
                       std::vector<std::unique_ptr<Expr>>&& terms)
    : ops_{std::move(ops)}
    , terms_{std::move(terms)} {}

Program::Program(std::vector<std::unique_ptr<Expr>>&& instructions)
    : instructions_{move(instructions)} {}

Parser::Parser(Lexer& lex)
    : lex_(lex) {}

std::unique_ptr<UnaryExpr> Parser::ReadUnaryOp() {
  Token t     = lex_.Get();
  bool is_neg = false;
  for (; t.Is<types::Sub>(); t = lex_.Get()) {
    is_neg = !is_neg;
  }
  if (t.Is<types::LPar>()) {
    auto p = std::make_unique<UnaryExpr>(ReadAddSub(), is_neg);
    Expect<types::RPar>(lex_.Get());
    return p;
  } else {
    if (t.Is<types::Number>()) {
      return
      std::make_unique<UnaryExpr>(std::make_unique<Constant>(std::move(t)), is_neg);
    } else {
      Expect<types::Identifier>(t);
      return
      std::make_unique<UnaryExpr>(std::make_unique<Variable>(std::move(t)), is_neg);
    }
  }
}

std::unique_ptr<BinaryExpr> Parser::ReadMulDiv() {
  std::vector<Token> ops;
  std::vector<std::unique_ptr<Expr>> terms;
  terms.emplace_back(ReadUnaryOp());
  Token t = lex_.Peek();
  for (; t.Is<types::Mul>() || t.Is<types::Div>(); t = lex_.Peek()) {
    ops.push_back(lex_.Get());
    terms.emplace_back(ReadUnaryOp());
  }
  return std::make_unique<BinaryExpr>(std::move(ops), std::move(terms));
}

std::unique_ptr<BinaryExpr> Parser::ReadAddSub() {
  std::vector<Token> ops;
  std::vector<std::unique_ptr<Expr>> terms;
  terms.emplace_back(ReadMulDiv());
  Token t = lex_.Peek();
  for (; t.Is<types::Add>() || t.Is<types::Sub>(); t = lex_.Peek()) {
    ops.push_back(lex_.Get());
    terms.emplace_back(ReadMulDiv());
  }
  return std::make_unique<BinaryExpr>(std::move(ops), std::move(terms));
}

std::unique_ptr<Declaration> Parser::ReadDeclaration() {
  lex_.Get();
  Token name = lex_.Get();
  Expect<types::Identifier>(name);
  std::unique_ptr<Expr> rhs{nullptr};
  if (lex_.Peek().Is<types::Assignment>()) {
    lex_.Get();
    rhs = ReadAddSub();
  }
  return std::make_unique<Declaration>(std::move(name), std::move(rhs));
}

std::unique_ptr<Expr> Parser::ReadAssignment() {
  std::vector<std::unique_ptr<Expr>> terms;
  terms.emplace_back(ReadAddSub());
  while (lex_.Peek().Is<types::Assignment>()) {
    LvalueRecognizeVisitor lrv;
    terms.back()->Accept(&lrv);
    if (lrv.GetResults()) {
      lex_.Get();
      terms.emplace_back(ReadAddSub());
    }
    else {
      throw SyntaxError(lex_.Peek());
    }
  }
  if (terms.size() == 1)
    return std::move(terms.back());
  else
    return std::make_unique<Assignment>(std::move(terms));
}

std::unique_ptr<Program> Parser::ReadProgram() {
  std::vector<std::unique_ptr<Expr>> instructions;
  Token t;
  while (!(t = lex_.Peek()).Is<types::Eof>()) {
    if (t.Is<types::Double>())
      instructions.emplace_back(ReadDeclaration());
    else
      instructions.emplace_back(ReadAssignment());
    lex_.Get();
  }
  return std::make_unique<Program>(std::move(instructions));
}
