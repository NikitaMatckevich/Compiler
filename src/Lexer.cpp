#include <Lexer.h>
#include <algorithm>
#include <charconv>
#include <fstream>
//#include <iostream>

Lexer::Lexer(std::istream &in) : in_(in) {}

/*
Lexer::Lexer(std::string&& file)
    : file_{move(file)}
    , fin_{}
    , lines_{}
    , sv_{}
    , line_nb_{0}
    , curr_{} {
  if (fin_.open(file_); !fin_.is_open()) {
    std::cout << "cannot open file \n";
    throw LexerError(file_, sv_, line_nb_);
  }
  NextToken();
}


void Lexer::Trim(std::string& s) {
  auto finder = [](auto first, auto last) {
    return std::find_if(first, last,
                        [](unsigned char c) { return !std::isspace(c); });
  };
  s.erase(s.begin(), finder(s.begin(), s.end()));
  s.erase(finder(s.rbegin(), s.rend()).base(), s.end());
}


bool Lexer::ReachedEOF() const noexcept { return curr_.Is<types::Eof>(); }


bool Lexer::SkipEmptyLines() {
  if (sv_.empty()) {
    std::string line;
    for (; line.empty(); Trim(line)) {
      line_nb_++;
      if (!getline(fin_, line)) {
        CutToken<types::Eof>();
        return false;
      }
    }
    lines_.push_front(line);
    sv_ = std::string_view{lines_.front()};
  }
  return true;
}
*/

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
  } else {
    switch (c) {
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
