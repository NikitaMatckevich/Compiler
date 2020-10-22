#include <iostream>
#include <vector>
#include "Ast.h"
using namespace std;

Constant::Constant(int value)
  : value(value) {}
int Constant::exec() const {
  return value;
}

UnaryExpression::UnaryExpression(expr_ptr<Expression>&& term, bool is_neg)
  : term{move(term)}, is_neg(is_neg) {}
int UnaryExpression::exec() const {
  return is_neg ? -term->exec() : term->exec();
}

BinaryExpression::BinaryExpression(vector<Token>&& ops, vector<expr_ptr<Expression>>&& terms)
  : ops{move(ops)}, terms{move(terms)} {}
int BinaryExpression::exec() const {
  int value = terms.front()->exec();
  for (size_t i = 0; i < ops.size();) {
    Token op = ops[i];
    if (op.is<Add>())
      value += terms[++i]->exec();
    else if (op.is<Sub>())
      value -= terms[++i]->exec();
    else if (op.is<Mul>())
      value *= terms[++i]->exec();
    else if (op.is<Div>())
      value /= terms[++i]->exec();
    else throw
      SyntaxError("error");
  }
  return value;
}

Program::Program(vector<expr_ptr<Expression>>&& instructions)
  : instructions{move(instructions)} {}
int Program::exec() const {
  for (size_t i = 0; i < instructions.size(); ++i) {
    cout << "result of instruction " << i << " is ";
    cout << instructions[i]->exec() << endl;
  }
  return 0;
}

namespace {

expr_ptr<BinaryExpression> readAddSub(const Lexer& lex, size_t& pos);
 
expr_ptr<UnaryExpression> readUnaryOp(const Lexer& lex, size_t& pos) {
  Token t = lex.tokens[pos++];
  bool is_neg = false;
  for (; t.is<Sub>(); t = lex.tokens[pos++]) is_neg = !is_neg;
  if (t.is<LPar>()) {
    auto p = make_unique<UnaryExpression>(readAddSub(lex, pos), is_neg);
    if (lex.tokens[pos].is<RPar>())
      pos++;
    else
      throw SyntaxError("error");
    return p;
  }
  else if (t.is<Number>())
    return make_unique<UnaryExpression>(make_unique<Constant>(t.as<Number>().value), is_neg);
  else
    throw SyntaxError("error");
}

expr_ptr<BinaryExpression> readMulDiv(const Lexer& lex, size_t& pos) {
  vector<Token> ops;
  vector<expr_ptr<Expression>> terms;
  terms.emplace_back(readUnaryOp(lex, pos));
  Token t = lex.tokens[pos];
  for (; t.is<Mul>() || t.is<Div>(); t = lex.tokens[pos]) {
    ops.push_back(t);
    terms.emplace_back(readUnaryOp(lex, ++pos));
  }
  if (t.is<Eol>() || t.is<RPar>() || t.is<Add>() || t.is<Sub>())
    return make_unique<BinaryExpression>(move(ops), move(terms));
  else
    throw SyntaxError("error");
}

expr_ptr<BinaryExpression> readAddSub(const Lexer& lex, size_t& pos) {
  vector<Token> ops;
  vector<expr_ptr<Expression>> terms;
  terms.emplace_back(readMulDiv(lex, pos));
  Token t = lex.tokens[pos];
  for (; t.is<Add>() || t.is<Sub>(); t = lex.tokens[pos]) {
    ops.push_back(t);
    terms.emplace_back(readMulDiv(lex, ++pos));
  }
  if (t.is<Eol>() || t.is<RPar>())
    return make_unique<BinaryExpression>(move(ops), move(terms));
  else
    throw SyntaxError("error");
}

}

expr_ptr<Program> readProgram(const Lexer& lex, size_t& pos) {
  vector<expr_ptr<Expression>> instructions;
  for (; !lex.tokens[pos].is<Eof>(); ++pos)
    instructions.emplace_back(readAddSub(lex, pos));
  return make_unique<Program>(move(instructions));
}
