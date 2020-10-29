#include <Ast.h>
#include <iostream>
#include <vector>

Constant::Constant(int value)
    : value_(value) {}

int Constant::Exec() const { return value_; }

UnaryExpr::UnaryExpr(std::unique_ptr<Expr>&& term, bool is_neg)
    : term_{move(term)}
    , is_neg_(is_neg) {}

int UnaryExpr::Exec() const { return is_neg_ ? -term_->Exec() : term_->Exec(); }

BinaryExpr::BinaryExpr(std::vector<Token>&& ops,
                       std::vector<std::unique_ptr<Expr>>&& terms)
    : ops_{std::move(ops)}
    , terms_{std::move(terms)} {}

int BinaryExpr::Exec() const {
  int value = terms_.front()->Exec();
  for (size_t i = 0; i < ops_.size();) {
    Token op = ops_[i];
    if (op.Is<types::Add>()) {
      value += terms_[++i]->Exec();
    } else if (op.Is<types::Sub>()) {
      value -= terms_[++i]->Exec();
    } else if (op.Is<types::Mul>()) {
      value *= terms_[++i]->Exec();
    } else if (op.Is<types::Div>()) {
      value /= terms_[++i]->Exec();
    } else {
      throw SyntaxError(op);
    }
  }
  return value;
}

Program::Program(std::vector<std::unique_ptr<Expr>>&& instructions)
    : instructions_{move(instructions)} {}

int Program::Exec() const {
  for (size_t i = 0; i < instructions_.size(); ++i) {
    std::cout << "result of instruction " << i << " is ";
    std::cout << instructions_[i]->Exec() << std::endl;
  }
  return 0;
}

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
    return std::make_unique<UnaryExpr>(
        std::make_unique<Constant>(
            Expect<types::Number>(t).As<types::Number>().value),
        is_neg);
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
  Expect<types::Eol, types::RPar, types::Add, types::Sub>(t);
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
  Expect<types::Eol, types::RPar>(t);
  return std::make_unique<BinaryExpr>(std::move(ops), std::move(terms));
}

std::unique_ptr<Program> Parser::ReadProgram() {
  std::vector<std::unique_ptr<Expr>> instructions;
  for (; !lex_.Peek().Is<types::Eof>(); lex_.Get()) {
    instructions.emplace_back(ReadAddSub());
  }
  return std::make_unique<Program>(std::move(instructions));
}
