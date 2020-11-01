#pragma once

#include <iostream>
#include <string_view>
#include <variant>

namespace types {
struct Number {
  int value;
};
struct Add {};
struct Sub {};
struct Mul {};
struct Div {};
struct LPar {};
struct RPar {};
struct Eol {};
struct Eof {};
} // namespace types

class TokenContext {
 public:
  TokenContext() = default;
  TokenContext(const TokenContext&) = default;
  TokenContext(TokenContext&&) = default;
  TokenContext& operator=(const TokenContext&) = default;
  TokenContext& operator=(TokenContext&&) = default;

  TokenContext(const std::string_view& line, size_t line_number,
               size_t begin_pos, size_t length)
      : line_(line)
      , line_number_(line_number)
      , begin_pos_(begin_pos)
      , length_(length) {}

  const std::string_view& GetLine() const { return line_; }

  size_t GetLineNumber() const { return line_number_; }

  size_t GetBeginPos() const { return begin_pos_; }

  size_t GetLength() const { return length_; }

  std::string_view GetTokenString() const {
    return line_.substr(begin_pos_, length_);
  }

 private:
  std::string_view line_;
  size_t line_number_{0};
  size_t begin_pos_{0};
  size_t length_{0};
};

class Token {
 public:
  Token() noexcept = default;
  Token(const Token&) = default;
  Token(Token&&) = default;
  Token& operator=(const Token&) = default;
  Token& operator=(Token&&) = default;

  template <class T>
  constexpr Token(const TokenContext& context, T&& token_data) noexcept
      : token_data_(token_data)
      , context_(context) {}

  template <class T>
  constexpr T& As() {
    return std::get<T>(token_data_);
  }
  template <class T>
  constexpr const T& As() const {
    return std::get<T>(token_data_);
  }
  template <class T>
  constexpr bool Is() const noexcept {
    return std::holds_alternative<T>(token_data_);
  }
  constexpr inline std::size_t Id() const noexcept { return token_data_.index(); }

  const TokenContext& GetContext() const { return context_; }

 private:
  std::variant<types::Number, types::Add, types::Sub, types::Mul, types::Div,
               types::LPar, types::RPar, types::Eol, types::Eof>
      token_data_;
  TokenContext context_;
};
