//
// Created by ta1 on 10/5/24.
//

#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include <istream>
#include <optional>
#include <cassert>

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

    unsigned int get_cur_line() const { return m_cur_line; }
    unsigned int get_cur_col() const { return m_cur_col; }
    unsigned int get_cur_pos() const { return m_cur_pos; }

    private:
    unsigned int m_cur_line{0};
    unsigned int m_cur_col{0};
    unsigned int m_cur_pos{0};

    std::istream* const m_stream{nullptr};
};
}

#endif //CHAR_STREAM_H
