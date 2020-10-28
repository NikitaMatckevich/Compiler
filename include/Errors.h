#pragma once
#include <stdexcept>
#include <Token.h>

class compile_error : public std::runtime_error {
protected:
  std::string _msg;
  size_t _count;
  compile_error(std::string_view, std::string_view, size_t);
public:
  virtual const char* what() const noexcept final override;
};

class lexer_error : public compile_error {
public:
  lexer_error(std::string_view, std::string_view, size_t);
};

class syntax_error : public compile_error {
public:
  syntax_error(const token&);
};
