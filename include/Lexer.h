#pragma once

#include <Errors.h>
#include <Token.h>

#include <forward_list>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

class Lexer {
 private:
  std::istream& in_;
  std::forward_list<std::string> reversed_line_list_;
  std::string_view current_line_;
  size_t current_line_number_{0};
  std::optional<Token> last_token_{std::nullopt};

  bool NextLine() {
    std::string str;

    if (std::getline(in_, str)) {
      reversed_line_list_.push_front(std::move(str));
      ++current_line_number_;
      current_line_ = reversed_line_list_.front();
      return true;
    }

    return false;
  }

  void SkipWhitespaces() {

    auto space_counter = [&]() {
      return std::find_if_not(current_line_.begin(), current_line_.end(),
                              [](unsigned char c) {
                                return std::isspace(c);
                              }) - current_line_.begin();
    };

    do
      current_line_.remove_prefix(space_counter());
    while
      (current_line_.empty() && NextLine());

  }

  TokenContext GetTokenContext(size_t token_length) const;

  size_t GetCurrentLineNumber() const { return current_line_number_; }

  size_t GetCurrentLineOffset() const {
    if (reversed_line_list_.empty() || !current_line_.data()) {
      return 0;
    }
    return current_line_.data() - reversed_line_list_.front().data();
  }

  template <class T, class... Args>
  void CutToken(size_t token_length, Args&&... args) {
    last_token_.emplace(GetTokenContext(token_length), T{std::forward<Args>(args)...});
    current_line_.remove_prefix(token_length);
  }

  void NextToken();

 public:
  explicit Lexer(std::istream& in);
  ~Lexer() = default;

  inline const Token& Peek() {
    if (!last_token_) {
      NextToken();
    }
    return *last_token_;
  }

  inline Token Get() {
    Token result = Peek();
    last_token_.reset();
    return result;
  }
};
