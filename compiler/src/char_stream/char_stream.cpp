//
// Created by ta1 on 10/5/24.
//

#include <logger.h>
#include <iostream>

#include <char_stream/char_stream.h>

using namespace Compiler;

std::optional<char> CharStream::get_next_char() {
    try {
        char c = m_stream->get();

        if(c == std::istream::traits_type::eof()) {
            return {};
        }

        if(c == '\n') {
            m_cur_line++;
            m_cur_col = 0;
        }

        m_cur_pos++;

        return c;
    } catch (const std::exception &e) {
        LOG(LogLevel::ERROR) << e.what() << std::endl;
        return {};
    }
}

std::optional<char> CharStream::peek_next_char() const {
    try {
        char c =  m_stream->peek();

        if(c == std::istream::traits_type::eof()) {
            return {};
        }

        return c;
    } catch (const std::exception &e) {
        LOG(LogLevel::ERROR) << e.what() << std::endl;
        return {};
    }
}

void CharStream::put_back_char(const char c) {
    try {
        m_stream->putback(c);
    } catch (const std::exception &e) {
        LOG(LogLevel::ERROR) << e.what() << std::endl;
    }
}

bool CharStream::is_end_of_stream() const {
    try {
        return m_stream->eof();
    } catch (const std::exception &e) {
        LOG(LogLevel::ERROR) << e.what() << std::endl;
    }

    return true;
}