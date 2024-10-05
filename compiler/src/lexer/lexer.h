#ifndef LEXER_H
#define LEXER_H

#include <char_stream/char_stream.h>
#include <istream>
#include <optional>

namespace Compiler {
class Lexer final {
  public:
  using Token = std::string;

  explicit Lexer(std::istream* const stream): m_char_stream(stream) {}

  Lexer() = delete;
  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;
  Lexer(Lexer&&) = delete;
  Lexer& operator=(Lexer&&) = delete;

  bool has_next_token();
  std::optional<Token> get_next_token();
  void skip_whitespace();

  private:
  static bool char_is_whitespace(char c) ;
  CharStream m_char_stream;
};
}

#endif // LEXER_H