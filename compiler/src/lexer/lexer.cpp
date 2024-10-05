#include <lexer/lexer.h>

using namespace Compiler;

bool Lexer::has_next_token() {
    skip_whitespace();
    return !m_char_stream.is_end_of_stream();
}

std::optional<Lexer::Token> Lexer::get_next_token() {
    skip_whitespace();

    if(m_char_stream.is_end_of_stream()) {
        return {};
    }

    Token token;
    while(!m_char_stream.is_end_of_stream() && !char_is_whitespace(m_char_stream.peek_next_char().value())) {
        token.push_back(m_char_stream.get_next_char().value());
    }

    return token;
}

bool Lexer::char_is_whitespace(const char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void Lexer::skip_whitespace() {
    while(!m_char_stream.is_end_of_stream() && char_is_whitespace(m_char_stream.peek_next_char().value())) {
        m_char_stream.get_next_char();
    }
}