//
// Created by ta1 on 10/5/24.
//

#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include <istream>
#include <optional>
#include <cassert>

struct CharStreamPosition {
    unsigned int m_cur_line{0};
    unsigned int m_cur_col{0};
    unsigned int m_cur_pos{0};

    friend std::ostream& operator<<(std::ostream& os, const CharStreamPosition& char_stream_position) {
        os << "{" << std::endl;
        os << "cur_line: " << char_stream_position.m_cur_line << std::endl;
        os << "cur_col: " << char_stream_position.m_cur_col << std::endl;
        os << "m_cur_pos: " << char_stream_position.m_cur_pos << std::endl;
        os << "}";
        return os;
    }
};

namespace Compiler {
class CharStream final {
    public:
    explicit CharStream(std::istream* const stream): m_stream(stream) {
        assert(stream != nullptr);
    }

    CharStream() = delete;
    CharStream(const CharStream&) = delete;
    CharStream(CharStream&&) = delete;
    CharStream& operator=(const CharStream&) = delete;
    CharStream& operator=(CharStream&&) = delete;

    std::optional<char> get_next_char();
    std::optional<char> peek_next_char() const;
    void put_back_char(char c);
    bool is_end_of_stream() const;

    CharStreamPosition get_position() const { return m_char_stream_position; }

    private:
    CharStreamPosition m_char_stream_position{0, 0, 0};
    std::istream* const m_stream{nullptr};
};
}

#endif //CHAR_STREAM_H
