#pragma once
#include <string>
#include <vector>
#include <forward_list>
#include <stdexcept>
#include "Token.h"

struct LexerError : std::runtime_error {
  LexerError(const char*, const char, size_t, std::string_view, std::string_view);
  virtual const char* what() const noexcept final override;
private:
  std::string _msg;
  const char _c;
  const size_t _line_number;
  std::string_view _line;
  std::string_view _filename;
};

struct Lexer {
  std::vector<Token> tokens;
  explicit Lexer(std::string&& filename);
  inline std::string_view filename() const { return _filename; }
private:
  std::string _filename;
  std::forward_list<std::string> _lines;
};
