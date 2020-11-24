#include <Lexer.h>
#include <algorithm>
#include <charconv>
#include <fstream>
// #include <iostream>

Lexer::Lexer(std::istream &in) : in_(in) {}

void Lexer::NextToken() {
  SkipWhitespaces();

  if (current_line_.empty()) {
    CutToken<types::Eof>(0);
    return;
  }

  unsigned char c = current_line_.front();
  if (std::isdigit(c)) {
    int value = 0xDEADBEEF;
    auto result = std::from_chars(current_line_.data(), current_line_.data() + current_line_.length(), value, 10);
    CutToken<types::Number>(result.ptr - current_line_.data(), value);
  }
  else if (std::isalpha(c) || c == '_') {
    size_t word_length = std::find_if_not(current_line_.begin(), current_line_.end(),
                              [](unsigned char c) {
                                return std::isalnum(c) || c == '_';
                              }) - current_line_.begin();
    std::string_view word = current_line_.substr(0, word_length);
    if (word == "double")
      CutToken<types::Double>(word_length);
    else
      CutToken<types::Identifier>(word_length);
  }
  else {
    switch (c) {
    case '=':
      CutToken<types::Assignment>(1);
      break;
    case '+':
      CutToken<types::Add>(1);
      break;
    case '-':
      CutToken<types::Sub>(1);
      break;
    case '*':
      CutToken<types::Mul>(1);
      break;
    case '/':
      CutToken<types::Div>(1);
      break;
    case '(':
      CutToken<types::LPar>(1);
      break;
    case ')':
      CutToken<types::RPar>(1);
      break;
    case ';':
      CutToken<types::Eol>(1);
      break;
    default:
      throw LexerError(GetTokenContext(1));
    }
  }
}

TokenContext Lexer::GetTokenContext(size_t token_length) const {
  return TokenContext(reversed_line_list_.front(), GetCurrentLineNumber(), GetCurrentLineOffset(), token_length);
}
