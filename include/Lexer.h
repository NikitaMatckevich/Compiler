#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <forward_list>
#include <stdexcept>
#include <Token.h>
#include <Errors.h>

class Lexer {
  const std::string spaces_ = " \t\r";
  const std::string numbers_ = "0123456789";
  void NextToken();
  std::string file_;
  std::ifstream fin_;
  std::forward_list<std::string> lines_;
  std::string_view sv_;
  size_t line_nb_;
  Token curr_;
public:
  explicit Lexer(std::string&& file);
	~Lexer();
  inline const std::string& File() const noexcept { return file_; }
  inline const Token& Peek() const noexcept { return curr_; }
  inline Token Get() { Token out = curr_; NextToken(); return out; }
};


