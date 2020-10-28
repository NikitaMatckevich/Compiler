#include <Errors.h>
#include <string>

compile_error::compile_error(std::string_view file,
                             std::string_view line,
                             size_t line_number)
: std::runtime_error::runtime_error{""}, _msg{file.data()}
{
  _msg += ":";
  _msg += std::to_string(line_number);
  _msg += ":\n\t";
  _msg += line;
  _msg += "\n\t";
  std::string delimeter = " \t\n\r";
  _count = line.find_first_of(delimeter);
  _msg += std::string(_count, '^');
}

const char* compile_error::what() const noexcept { return _msg.data(); }

lexer_error::lexer_error(std::string_view file,
                         std::string_view line,
                         size_t line_number)
: compile_error(file, line, line_number)
{
  _msg += "\nlexer error: unrecognized character ";
  _msg += line.front();
}

syntax_error::syntax_error(const token& t)
: compile_error(t.info.file, t.info.context, t.info.line_number)
{
  _msg += "\nsyntax error: unexpected token ";
  _msg += t.info.context.substr(0, _count);
}
