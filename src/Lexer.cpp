#include <Lexer.h>
#include <charconv>
#include <fstream>

namespace {

void Trim(std::string& str) {
  const std::string spaces = " \t\r";
  auto beg = std::min(str.size(), str.find_first_not_of(spaces));
  auto end = std::min(str.size(), str.find_last_not_of(spaces) + 1);
  str      = std::string{str.begin() + beg, str.begin() + end};
}

} // namespace

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

void Lexer::NextToken() {
  if (curr_.Is<types::Eof>()) {
    return;
  }

  if (sv_.empty()) {
    std::string line;
    for (; line.empty(); Trim(line)) {
      line_nb_++;
      if (!getline(fin_, line)) {
        curr_ = Token{file_, sv_, line_nb_, types::Eof{}};
        return;
      }
    }
    lines_.push_front(line);
    sv_ = std::string_view{lines_.front()};
  }
  size_t length = 1;
  char c        = sv_.front();
  if (numbers_.find(c) != std::string::npos) {
    length = std::min(sv_.find_first_not_of(numbers_), sv_.size());
    int value;
    std::from_chars(sv_.data(), sv_.data() + length, value);
    curr_ = Token(file_, sv_, line_nb_, types::Number{value});
  } else {
    switch (c) {
    case '+':
      curr_ = Token(file_, sv_, line_nb_, types::Add{});
      break;
    case '-':
      curr_ = Token(file_, sv_, line_nb_, types::Sub{});
      break;
    case '*':
      curr_ = Token(file_, sv_, line_nb_, types::Mul{});
      break;
    case '/':
      curr_ = Token(file_, sv_, line_nb_, types::Div{});
      break;
    case '(':
      curr_ = Token(file_, sv_, line_nb_, types::LPar{});
      break;
    case ')':
      curr_ = Token(file_, sv_, line_nb_, types::RPar{});
      break;
    case ';':
      curr_ = Token(file_, sv_, line_nb_, types::Eol{});
      break;
    default:
      throw LexerError(file_, sv_, line_nb_);
    }
  }
  sv_.remove_prefix(length);
  sv_.remove_prefix(std::min(sv_.find_first_not_of(spaces_), sv_.size()));
}
