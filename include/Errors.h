#pragma once
#include <Token.h>
#include <stdexcept>

class CompileError : public std::runtime_error {
 protected:
  std::string msg_;
  size_t count_;
  CompileError(std::string_view, std::string_view, size_t);

 public:
  virtual const char* what() const noexcept final override; // NOLINT
};

class LexerError : public CompileError {
 public:
  LexerError(std::string_view, std::string_view, size_t);
};

class SyntaxError : public CompileError {
 public:
  SyntaxError(const Token&);
};
