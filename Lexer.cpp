#include <iostream>
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

Lexer::Lexer(string&& filename) : _filename{move(filename)} {
  ifstream fin(_filename);
  const string spaces = " \t\r";
  const string numbers = "0123456789";
  for (string line; getline(fin, line);) {
    auto beg = std::min(line.find_first_not_of(spaces), line.size());
    auto end = std::min(line.find_last_not_of(spaces), line.size());
    _lines.emplace_back(string{line.begin() + beg, line.begin() + end});
    for (string_view sv{_lines.back()}; !sv.empty();
      sv.remove_prefix(std::min(sv.find_first_not_of(spaces), sv.size())))
    {
      size_t length = 1;
      char c = sv.front();
      if (numbers.find(c) != string::npos) {
        length = std::min(sv.find_first_not_of(numbers), sv.size());
        int value;
        from_chars(sv.data(), sv.data() + length, value);
        tokens.emplace_back(Token{sv, _lines.size(), Number{value}});
      }
      else {
        switch (c) {
          case '+' : tokens.emplace_back(Token(sv, _lines.size(), Add {})); break;
          case '-' : tokens.emplace_back(Token(sv, _lines.size(), Sub {})); break;
          case '*' : tokens.emplace_back(Token(sv, _lines.size(), Mul {})); break;
          case '/' : tokens.emplace_back(Token(sv, _lines.size(), Div {})); break;
          case '(' : tokens.emplace_back(Token(sv, _lines.size(), LPar{})); break;
          case ')' : tokens.emplace_back(Token(sv, _lines.size(), RPar{})); break;
          case ';' : tokens.emplace_back(Token(sv, _lines.size(), Eol {})); break;
          default  : throw
            LexerError("unexpected symbol", c, _lines.size(), sv, _filename);
        }
      }
      sv.remove_prefix(length);
    }
  }
  tokens.emplace_back(Token{string_view{}, _lines.size(), Eof{}});
}

int main () {
try {
  Lexer L("program.txt");
  for (auto token : L.tokens) {
    cout << "line " << token.line_number << " " << token.context;
    if (token.is<Number>())
      cout << " {" << token.as<Number>().value << "}";
    cout << endl;
  }
  for (size_t i = 0; i < L.num_lines(); ++i)
    cout << L.line(i) << endl;
  return 0;
}
catch (const exception& e) {
  cerr << e.what() << endl;
}
}
