#pragma once
#include <iostream>
#include <string_view>
#include <variant>

namespace types {
  struct number {
    int value; 
  };
  struct add  {};
  struct sub  {};
  struct mul  {};
  struct div  {};
  struct l_par{};
  struct r_par{};
  struct eol  {};
  struct eof  {};
}

struct token_info {
  std::string_view file;
  std::string_view context;
  size_t line_number;
};

class token {
  std::variant<types::number,
               types::add,
               types::sub,
               types::mul,
               types::div,
               types::l_par,
               types::r_par,
               types::eol,
               types::eof>
              _var;

public:
  token_info info; 

  token() noexcept = default;
  template <class T> constexpr token(std::string_view file, 
                                     std::string_view context,
                                     size_t line_number,
                                     T&& t) noexcept
  : _var{std::move(t)}, info{file, context, line_number} {}

  template <class T> constexpr T& as() { return std::get<T>(_var); }
	template <class T> constexpr const T& as() const { return std::get<T>(_var); }
  template <class T> constexpr bool is() const noexcept { return std::holds_alternative<T>(_var); }
  constexpr inline std::size_t      id() const noexcept { return _var.index(); }
};
