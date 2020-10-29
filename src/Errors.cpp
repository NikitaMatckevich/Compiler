#include <Errors.h>
#include <string>

CompileError::CompileError(std::string_view file,
                             std::string_view line,
                             size_t line_number)
: std::runtime_error::runtime_error{""}, msg_{file.data()}
{
  msg_ += ":";
  msg_ += std::to_string(line_number);
  msg_ += ":\n\t";
  msg_ += line;
  msg_ += "\n\t";
  std::string delimiter = " \t\n\r";
  count_ = line.find_first_of(delimiter);
  msg_ += std::string(count_, '^');
}

const char* CompileError::what() const noexcept { return msg_.data(); }

LexerError::LexerError(std::string_view file,
                         std::string_view line,
                         size_t line_number)
: CompileError(file, line, line_number)
{
  msg_ += "\nlexer error: unrecognized character ";
  msg_ += line.front();
}

SyntaxError::SyntaxError(const Token& t)
: CompileError(t.info.file, t.info.context, t.info.line_number)
{
  msg_ += "\nsyntax error: unexpected token ";
  msg_ += t.info.context.substr(0, count_);
}
