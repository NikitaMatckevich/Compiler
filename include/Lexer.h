#pragma once
#include <Errors.h>
#include <Token.h>
#include <forward_list>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

class Lexer {
  std::string file_;
  std::ifstream fin_;
  std::forward_list<std::string> lines_;
  std::string_view sv_;
  size_t line_nb_;
  Token curr_;

  static void Trim(std::string& s);

  bool ReachedEOF() const noexcept;
  bool SkipEmptyLines();

  template <class T, class ...Args>
  void BuildToken(Args&& ...args) {
    curr_ = Token{file_, sv_, line_nb_, T{std::forward<Args>(args)...}};
  }

  void NextToken();

public:
  explicit Lexer(std::string&& file);
  ~Lexer();
  inline const std::string& File() const noexcept { return file_; }
  inline const Token& Peek() const noexcept { return curr_; }
  inline Token Get() {
    Token out = curr_;
    NextToken();
    return out;
  }
};
