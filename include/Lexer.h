#pragma once

#include <Errors.h>
#include <Token.h>

#include <forward_list>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include <boost/iostreams/device/mapped_file.hpp>

class Lexer {
 private:
  boost::iostreams::mapped_file_source source_;
  size_t current_line_number_{0};
  size_t current_line_offset_{0};
  size_t current_line_ending_{0};
  std::string_view current_view_;
  std::optional<Token> last_token_{std::nullopt};

  inline std::string_view GetFile() const {
    return std::string_view(source_.data(), source_.size());
  }

  inline std::string_view GetCurrentLine() const {
    return std::string_view(GetFile().data() + current_line_offset_,
                            current_line_ending_ - current_line_offset_);
  }
  
  inline bool NextLine() {
    auto file_view_= GetFile();
    if (file_view_.begin() + current_line_ending_ < file_view_.end()) {
      ++current_line_number_;
      current_line_offset_ = current_line_ending_;
      current_line_ending_ = file_view_.find_first_of('\n', current_line_offset_) + 1;
      current_view_ = GetCurrentLine();
      return true;
    }
    return false;
  }
  
  inline void SkipWhitespaces() {

    auto space_counter = [&]() {
      return std::find_if_not(current_view_.begin(), current_view_.end(),
                              [](unsigned char c) {
                                return std::isspace(c);
                              }) - current_view_.begin();
    };

    do
      current_view_.remove_prefix(space_counter());
    while
      (current_view_.empty() && NextLine());

  }

  inline TokenContext GetTokenContext(size_t token_length) const;

  inline size_t GetCurrentLineNumber() const { return current_line_number_; }

  inline size_t GetCurrentLineOffset() const {
    if (!current_view_.data()) {
      return 0;
    }
    return current_view_.data() - (GetFile().data() + current_line_offset_);
  }

  template <class T, class... Args>
  void CutToken(size_t token_length, Args&&... args) {
    last_token_.emplace(GetTokenContext(token_length), T{std::forward<Args>(args)...});
    current_view_.remove_prefix(token_length);
  }

  void NextToken();

 public:
  explicit Lexer(const std::string& path);
  ~Lexer();

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
