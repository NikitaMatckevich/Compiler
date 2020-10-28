#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <forward_list>
#include <stdexcept>
#include <Token.h>
#include <Errors.h>

class lexer {
  const std::string spaces = " \t\r";
  const std::string numbers = "0123456789";
  void next_token();
  std::string _file;
  std::ifstream _fin;
  std::forward_list<std::string> _lines;
  std::string_view _sv;
  size_t _line_nb;
  token _curr;
public:
  explicit lexer(std::string&& file);
	~lexer();
  inline const std::string& file() const noexcept { return _file; }
  inline const token& peek() const noexcept { return _curr; }
  inline token get() { token _out = _curr; next_token(); return _out; }
};


