#pragma once
#include <Token.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

class CompileError : public std::runtime_error {
 public:
  template <class StringLike>
  explicit CompileError(StringLike&& str)
      : std::runtime_error(std::forward<StringLike>(str)) {}

  template <class... Args>
  static std::string BuildString(Args&&... args) {
    std::ostringstream ss;
    (ss << ... << std::forward<Args>(args));
    return ss.str();
  }

  static std::string BuildFromContext(const TokenContext& ctx) {
    std::ostringstream ss;
    ss << std::setw(6) << ctx.GetLineNumber() << ": ";
    size_t offset = ss.tellp();
    offset += ctx.GetBeginPos();

    ss << ctx.GetLine() << '\n';

    for (size_t i = 0; i < offset; ++i) {
      ss.put(' ');
    }

    for (size_t i = 0, n = std::max<size_t>(1, ctx.GetLength()); i < n; ++i) {
      ss.put('^');
    }

    return ss.str();
  }
};

class LexerError : public CompileError {
 public:
  LexerError(const TokenContext& ctx)
      : CompileError(BuildString("Lexer error:\n", BuildFromContext(ctx))) {}
};

class SyntaxError : public CompileError {
 public:
  SyntaxError(const Token& token)
      : CompileError(BuildString("Syntax error:\n",
                                 BuildFromContext(token.GetContext()))) {}
};
