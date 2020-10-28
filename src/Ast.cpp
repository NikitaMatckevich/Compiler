#include <iostream>
#include <vector>
#include <Ast.h>
using namespace std;

constant::constant(int value)
  : value(value) {}
int constant::exec() const {
  return value;
}

unary_expr::unary_expr(unique_ptr<expr>&& term, bool is_neg)
  : term{move(term)}, is_neg(is_neg) {}
int unary_expr::exec() const {
  return is_neg ? -term->exec() : term->exec();
}

binary_expr::binary_expr(vector<token>&& ops, vector<unique_ptr<expr>>&& terms)
  : ops{move(ops)}, terms{move(terms)} {}
int binary_expr::exec() const {
  int value = terms.front()->exec();
  for (size_t i = 0; i < ops.size();) {
    token op = ops[i];
    if (op.is<types::add>())
      value += terms[++i]->exec();
    else if (op.is<types::sub>())
      value -= terms[++i]->exec();
    else if (op.is<types::mul>())
      value *= terms[++i]->exec();
    else if (op.is<types::div>())
      value /= terms[++i]->exec();
    else throw
      syntax_error(op);
  }
  return value;
}

program::program(vector<unique_ptr<expr>>&& instructions)
  : instructions{move(instructions)} {}
int program::exec() const {
  for (size_t i = 0; i < instructions.size(); ++i) {
    cout << "result of instruction " << i << " is ";
    cout << instructions[i]->exec() << endl;
  }
  return 0;
}

parser::parser(lexer& lex) : _lex(lex) {}
 
unique_ptr<unary_expr> parser::read_unary_op() {
  token t = _lex.get();
  bool is_neg = false;
  for (; t.is<types::sub>(); t = _lex.get()) is_neg = !is_neg;
  if (t.is<types::l_par>()) {
    auto p = make_unique<unary_expr>(read_add_sub(), is_neg);
    expect<types::r_par>(_lex.get());
    return p;
  }
  else
    return make_unique<unary_expr>(
      make_unique<constant>(expect<types::number>(t).as<types::number>().value),
      is_neg);
}

unique_ptr<binary_expr> parser::read_mul_div() {
  vector<token> ops;
  vector<unique_ptr<expr>> terms;
  terms.emplace_back(read_unary_op());
  token t = _lex.peek(); 
  for (; t.is<types::mul>() || t.is<types::div>(); t = _lex.peek()) {
    ops.push_back(_lex.get());
    terms.emplace_back(read_unary_op());
  }
  expect<types::eol, types::r_par, types::add, types::sub>(t);
  return make_unique<binary_expr>(move(ops), move(terms));
}

unique_ptr<binary_expr> parser::read_add_sub() {
  vector<token> ops;
  vector<unique_ptr<expr>> terms;
  terms.emplace_back(read_mul_div());
  token t = _lex.peek();
  for (; t.is<types::add>() || t.is<types::sub>(); t = _lex.peek()) {
    ops.push_back(_lex.get());
    terms.emplace_back(read_mul_div());
  }
  expect<types::eol, types::r_par>(t);
  return make_unique<binary_expr>(move(ops), move(terms));
}

unique_ptr<program> parser::read_program() {
  vector<unique_ptr<expr>> instructions;
  for (; !_lex.peek().is<types::eof>(); _lex.get())
    instructions.emplace_back(read_add_sub());
  return make_unique<program>(move(instructions));
}
