#include <Ast.h>
#include <ConstVisitors.h>
#include <MutatingVisitors.h>

#include <vector>

Constant::Constant(Token&& number)
    : number_{std::move(number)} {}

Variable::Variable(Token&& name)
    : name_{std::move(name)} {}

UnaryExpr::UnaryExpr(std::unique_ptr<Expr>&& term,
                     std::optional<Token>&& sub_symbol)
    : term_{std::move(term)}
    , subtraction_symbol_{std::move(sub_symbol)} {}

BinaryExpr::BinaryExpr(std::vector<Token>&& ops,
                       std::vector<std::unique_ptr<Expr>>&& terms)
    : ops_{std::move(ops)}
    , terms_{std::move(terms)} {}

Assignment::Assignment(std::vector<std::unique_ptr<Expr>>&& terms)
    : terms_{std::move(terms)} {}

Declaration::Declaration(Token&& name,
                         std::optional<std::unique_ptr<Expr>>&& rhs)
    : name_{std::move(name)}
    , rhs_{std::move(rhs)} {}

Program::Program(std::vector<std::unique_ptr<Expr>>&& instructions)
    : instructions_{move(instructions)} {}

Parser::Parser(Lexer& lex)
    : lex_(lex) {}

std::unique_ptr<UnaryExpr> Parser::ReadUnaryOp() {  // UNARY_OP = ('-')* ('(' ADD_SUB ')' | Number | Identifier)

  std::optional<Token> subtraction_symbol{std::nullopt};

  Token first = lex_.Peek();
  Token last;

  bool is_neg = false;
  while ((last = lex_.Get()).Is<types::Sub>()) {
    is_neg = !is_neg;
  }
  if (is_neg) {
    subtraction_symbol.emplace(first);
  }

  Expect<types::LPar, types::Number, types::Identifier>(last);

  if (last.Is<types::LPar>()) {
    auto p = std::make_unique<UnaryExpr>(ReadAddSub(),
                                         std::move(subtraction_symbol));
    Expect<types::RPar>(lex_.Get());
    return p;
  } else {
    if (last.Is<types::Number>()) {
      return std::make_unique<UnaryExpr>(
          std::make_unique<Constant>(std::move(last)),
          std::move(subtraction_symbol));
    } else {
      Expect<types::Identifier>(last);
      return std::make_unique<UnaryExpr>(
          std::make_unique<Variable>(std::move(last)),
          std::move(subtraction_symbol));
    }
  }
}

std::unique_ptr<BinaryExpr> Parser::ReadMulDiv() {  // MUL_DIV = UNARY_OP (('*'|'/') UNARY_OP)*
  std::vector<Token> ops;
  std::vector<std::unique_ptr<Expr>> terms;
  terms.emplace_back(ReadUnaryOp());
  Token t = lex_.Peek();
  while (t.Is<types::Mul>() || t.Is<types::Div>()) {
    ops.push_back(lex_.Get());
    terms.emplace_back(ReadUnaryOp());
    t = lex_.Peek();
  }
  return std::make_unique<BinaryExpr>(std::move(ops), std::move(terms));
}

std::unique_ptr<BinaryExpr> Parser::ReadAddSub() {  // ADD_SUB = MUL_DIV (('+'|'-') MUL_DIV)*
  std::vector<Token> ops;
  std::vector<std::unique_ptr<Expr>> terms;
  terms.emplace_back(ReadMulDiv());
  Token t = lex_.Peek();
  while (t.Is<types::Add>() || t.Is<types::Sub>()) {
    ops.push_back(lex_.Get());
    terms.emplace_back(ReadMulDiv());
    t = lex_.Peek();
  }
  return std::make_unique<BinaryExpr>(std::move(ops), std::move(terms));
}

std::unique_ptr<Assignment> Parser::ReadAssignment() {  // ASSIGNMENT = ADD_SUB ('=' ADD_SUB)*
  std::vector<std::unique_ptr<Expr>> terms;
  terms.emplace_back(ReadAddSub());
  while (lex_.Peek().Is<types::Assignment>()) {
    lex_.Get();
    terms.emplace_back(ReadAddSub());
  }
  return std::make_unique<Assignment>(std::move(terms));
}

std::unique_ptr<Declaration> Parser::ReadDeclaration() {  // DECLARATION = 'double' IDENTIFIER ('=' ASSIGNMENT)?
  lex_.Get();
  Token name = lex_.Get();
  Expect<types::Identifier>(name);
  std::optional<std::unique_ptr<Expr>> rhs{std::nullopt};
  if (lex_.Peek().Is<types::Assignment>()) {
    lex_.Get();
    rhs.emplace(ReadAssignment());
  }
  return std::make_unique<Declaration>(std::move(name), std::move(rhs));
}

std::unique_ptr<Program> Parser::ReadProgram() {  // PROGRAM = ((DECLARATION | ASSIGNMENT) ';')*
  std::vector<std::exception_ptr> errors;
  std::vector<std::unique_ptr<Expr>> instructions;
  Token t;

  while (!(t = lex_.Peek()).Is<types::Eof>()) {
    try {
      if (t.Is<types::Double>()) {
        instructions.emplace_back(ReadDeclaration());
      } else {
        instructions.emplace_back(ReadAssignment());
      }
      Expect<types::Semicolon>(lex_.Get());
    } catch (const CompileError& err) {
      errors.push_back(std::current_exception());
      Rewind<types::Semicolon>(lex_);
    }
  }

  if (!errors.empty()) {
    for (const auto& eptr : errors) {
      try {
        std::rethrow_exception(eptr);
      } catch (const CompileError& err) {
        std::cerr << err.what() << std::endl;
      }
    }
    return nullptr;
  }

  return std::make_unique<Program>(std::move(instructions));
}
