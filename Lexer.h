#pragma once
#include <vector>
#include <string>
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
  inline size_t num_lines() const noexcept { return _lines.size(); }
  inline std::string_view line(size_t i) const { return _lines[i]; }
  inline std::string_view filename() const { return _filename; }
private:
  std::string _filename;
  std::vector<std::string> _lines;
};
