#include <fstream>
#include <charconv>
#include <Lexer.h>

using namespace std;

namespace {
  void trim(string& str) {
    const string spaces = " \t\r";
    auto beg = std::min(str.size(), str.find_first_not_of(spaces));
    auto end = std::min(str.size(), str.find_last_not_of(spaces) + 1);
    str = string{str.begin() + beg, str.begin() + end};
  }
}

lexer::lexer(string&& file)
: _file{move(file)}, _fin{}, _lines{}, _sv{}, _line_nb{0}, _curr{} {
	if (_fin.open(_file); !_fin.is_open()) {
		cout << "cannot open file \n";
		throw lexer_error(_file, _sv, _line_nb);
	}
  next_token();
}

lexer::~lexer() {
	_fin.close();
}

void lexer::next_token() {
  if (_curr.is<types::eof>())
    return;
  if (_sv.empty()) {
    string line;
    for (; line.size() == 0; trim(line)) {
			_line_nb++;
			if (!getline(_fin, line)) {
      	_curr = token{_file, _sv, _line_nb, types::eof{}};
      	return;
			}
    }
    _lines.push_front(line);
    _sv = string_view{_lines.front()};
  }
  size_t length = 1;
  char c = _sv.front();
  if (numbers.find(c) != string::npos) {
    length = std::min(_sv.find_first_not_of(numbers), _sv.size());
    int value;
    from_chars(_sv.data(), _sv.data() + length, value);
    _curr = token(_file, _sv, _line_nb, types::number{value});
  }
  else {
    switch (c) {
      case '+' : _curr = token(_file, _sv, _line_nb, types::add  {}); break;
      case '-' : _curr = token(_file, _sv, _line_nb, types::sub  {}); break;
      case '*' : _curr = token(_file, _sv, _line_nb, types::mul  {}); break;
      case '/' : _curr = token(_file, _sv, _line_nb, types::div  {}); break;
      case '(' : _curr = token(_file, _sv, _line_nb, types::l_par{}); break;
      case ')' : _curr = token(_file, _sv, _line_nb, types::r_par{}); break;
      case ';' : _curr = token(_file, _sv, _line_nb, types::eol  {}); break;
      default  : throw
        lexer_error(_file, _sv, _line_nb);
    }
  }
  _sv.remove_prefix(length);
  _sv.remove_prefix(std::min(_sv.find_first_not_of(spaces), _sv.size()));
}
