#pragma once
#include <string_view>
#include <variant>

struct Number {
  int value; 
};
struct Add  {};
struct Sub  {};
struct Mul  {};
struct Div  {};
struct LPar {};
struct RPar {};
struct Eol  {};
struct Eof  {};

struct Token {
  std::string_view context;
  size_t line_number;

  template <class T> constexpr Token(std::string_view cnt, size_t line_nb, T&& t) noexcept
  : context{cnt}, line_number{line_nb}, _var{std::move(t)} {}

  template <class T> constexpr T    as   ()          { return std::get<T>(_var); }
  template <class T> constexpr T*   as_if() noexcept { return std::get_if<T>(&_var); }
  template <class T> constexpr bool is   () const noexcept { return std::holds_alternative<T>(_var); }
private:
  std::variant<Number, Add, Sub, Mul, Div, LPar, RPar, Eol, Eof> _var;
};
