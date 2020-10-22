#include <fstream>
#include <charconv>
#include "Lexer.h"

using namespace std;

LexerError::LexerError(const char* msg,
                       const char c,
                       size_t line_number,
                       string_view line,
                       string_view filename)
: runtime_error(msg)
, _msg("Lexer error:")
, _c(c)
, _line_number(line_number)
, _line(line)
, _filename(filename) {
  _msg += "\ncharacter ";
  _msg += _c;
  _msg += "\nat line ";
  _msg += to_string(_line_number);
  _msg += ":\n";
  _msg += _line;
  _msg += "\nin file ";
  _msg += _filename;
}

const char* LexerError::what() const noexcept {
  return _msg.data();
}

namespace {
  void trim(string& str) {
    const string spaces = " \t\r";
    auto beg = std::min(str.size(), str.find_first_not_of(spaces));
    auto end = std::min(str.size(), str.find_last_not_of(spaces) + 1);
    str = string{str.begin() + beg, str.begin() + end};
  }
}

Lexer::Lexer(string&& filename) : _filename{move(filename)} {
  ifstream fin(_filename);
  const string spaces = " \t\r";
  const string numbers = "0123456789";
  size_t line_nb = 0;
  for (string line; getline(fin, line); line_nb++) {
    trim(line);
    _lines.push_front(line);
    for (string_view sv{_lines.front()}; !sv.empty();
         sv.remove_prefix(std::min(sv.find_first_not_of(spaces), sv.size())))
    {
      size_t length = 1;
      char c = sv.front();
      if (numbers.find(c) != string::npos) {
        length = std::min(sv.find_first_not_of(numbers), sv.size());
        int value;
        from_chars(sv.data(), sv.data() + length, value);
        tokens.emplace_back(Token{sv, line_nb, Number{value}});
      }
      else {
        switch (c) {
          case '+' : tokens.emplace_back(Token(sv, line_nb, Add {})); break;
          case '-' : tokens.emplace_back(Token(sv, line_nb, Sub {})); break;
          case '*' : tokens.emplace_back(Token(sv, line_nb, Mul {})); break;
          case '/' : tokens.emplace_back(Token(sv, line_nb, Div {})); break;
          case '(' : tokens.emplace_back(Token(sv, line_nb, LPar{})); break;
          case ')' : tokens.emplace_back(Token(sv, line_nb, RPar{})); break;
          case ';' : tokens.emplace_back(Token(sv, line_nb, Eol {})); break;
          default  : throw
            LexerError("unexpected symbol", c, line_nb, sv, _filename);
        }
      }
      sv.remove_prefix(length);
    }
  }
  tokens.emplace_back(Token{string_view{}, line_nb, Eof{}});
}
