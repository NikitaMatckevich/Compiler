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

struct TokenInfo {
  std::string_view file;
  std::string_view context;
  size_t line_number;
};

class Token {
  std::variant<types::Number, types::Add, types::Sub, types::Mul, types::Div,
               types::LPar, types::RPar, types::Eol, types::Eof>
      var_;

 public:
  TokenInfo info;

  Token() noexcept = default;
  template <class T>
  constexpr Token(std::string_view file, std::string_view context,
                  size_t line_number, T&& t) noexcept
      : var_{std::move(t)}
      , info{file, context, line_number} {}

  template <class T>
  constexpr T& As() {
    return std::get<T>(var_);
  }
  template <class T>
  constexpr const T& As() const {
    return std::get<T>(var_);
  }
  template <class T>
  constexpr bool Is() const noexcept {
    return std::holds_alternative<T>(var_);
  }
  constexpr inline std::size_t Id() const noexcept { return var_.index(); }
};
