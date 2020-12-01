#include <Lexer.h>
#include <algorithm>
#include <charconv>
#include <stdexcept>

Lexer::Lexer(const std::string& path) {
  source_.open(path);
}

Lexer::~Lexer() {
  source_.close();
}

void Lexer::NextToken() {
  SkipWhitespaces();

  if (current_view_.empty()) {
    CutToken<types::Eof>(0);
    return;
  }

  unsigned char c = current_view_.front();
  if (std::isdigit(c)) {
    int value = 0xDEADBEEF;
    auto result = std::from_chars(current_view_.data(),
      current_view_.data() + current_view_.length(), value, 10);
    CutToken<types::Number>(result.ptr - current_view_.data(), value);
  }
  else if (std::isalpha(c) || c == '_') {
    size_t word_length = std::find_if_not(current_view_.begin(), current_view_.end(),
                              [](unsigned char c) {
                                return std::isalnum(c) || c == '_';
                              }) - current_view_.begin();
    std::string_view word = current_view_.substr(0, word_length);
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
  return TokenContext(GetCurrentLine(),
                      GetCurrentLineNumber(),
                      GetCurrentLineOffset(),
                      token_length);
}
