#include <Lexer.h>
#include <algorithm>
#include <charconv>
#include <fstream>
//#include <iostream>

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

Lexer::~Lexer() { fin_.close(); }

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
        BuildToken<types::Eof>();
        return false;
      }
    }
    lines_.push_front(line);
    sv_ = std::string_view{lines_.front()};
  }
  return true;
}

void Lexer::NextToken() {
  if (ReachedEOF() || !SkipEmptyLines()) {
    return;
  }
  size_t length   = 1;
  unsigned char c = sv_.front();
  if (std::isdigit(c)) {
    auto nan = std::find_if(sv_.begin(), sv_.end(),
                            [](unsigned char c) { return !std::isdigit(c); });
    int value;
    std::from_chars(sv_.data(), nan, value);
    length = static_cast<size_t>(nan - sv_.begin());
    BuildToken<types::Number>(value);
  } else {
    switch (c) {
    case '+':
      BuildToken<types::Add>();
      break;
    case '-':
      BuildToken<types::Sub>();
      break;
    case '*':
      BuildToken<types::Mul>();
      break;
    case '/':
      BuildToken<types::Div>();
      break;
    case '(':
      BuildToken<types::LPar>();
      break;
    case ')':
      BuildToken<types::RPar>();
      break;
    case ';':
      BuildToken<types::Eol>();
      break;
    default:
      throw LexerError(file_, sv_, line_nb_);
    }
  }
  sv_.remove_prefix(length);
  sv_.remove_prefix(static_cast<size_t>(
      std::find_if(sv_.begin(), sv_.end(),
                   [](unsigned char c) { return !std::isspace(c); }) -
      sv_.begin()));
}
