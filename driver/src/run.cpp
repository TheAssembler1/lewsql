#include <lexer/lexer.h>
#include <logger.h>

void test_lexer() {
    // Test Case 1: Basic Tokenization of Words
    {
        std::string input{"test this message"};
        std::istringstream iss(input);
        Compiler::Lexer lexer(&iss);

        std::vector<std::string> expected_tokens = {"test", "this", "message"};
        size_t index = 0;

        while (lexer.has_next_token()) {
            std::string token = lexer.get_next_token().value();
            assert(token == expected_tokens[index]);
            index++;
        }
        assert(index == expected_tokens.size());  // Ensure we got all tokens
    }

    // Test Case 2: Empty Input
    {
        std::string input{""};
        std::istringstream iss(input);
        Compiler::Lexer lexer(&iss);

        assert(!lexer.has_next_token());  // No tokens should be present
    }

    // Test Case 3: Input with Only Whitespace
    {
        std::string input{"     "};  // String with only spaces
        std::istringstream iss(input);
        Compiler::Lexer lexer(&iss);

        assert(!lexer.has_next_token());  // No tokens should be present
    }

    // Test Case 4: Input with Mixed Whitespace
    {
        std::string input{"   test   this\tmessage  "};
        std::istringstream iss(input);
        Compiler::Lexer lexer(&iss);

        std::vector<std::string> expected_tokens = {"test", "this", "message"};
        size_t index = 0;

        while (lexer.has_next_token()) {
            std::string token = lexer.get_next_token().value();
            assert(token == expected_tokens[index]);
            index++;
        }
        assert(index == expected_tokens.size());  // Ensure we got all tokens
    }

    // Test Case 5: Punctuation Handling
    {
        std::string input{"hello, world! test."};
        std::istringstream iss(input);
        Compiler::Lexer lexer(&iss);

        std::vector<std::string> expected_tokens = {"hello,", ",", "world!", "!", "test.", "."};
        size_t index = 0;

        while (lexer.has_next_token()) {
            std::string token = lexer.get_next_token().value();
            LOG(LogLevel::INFO) << "{" << std::endl;
            LOG(LogLevel::INFO) << "expected: " << expected_tokens[index] << std::endl;
            LOG(LogLevel::INFO) << "received: " << token << std::endl;
            LOG(LogLevel::INFO) << "}" << std::endl;
            assert(token == expected_tokens[index]);
            index++;
        }
        assert(index == expected_tokens.size());
    }

    // Test Case 6: Numeric Input
    {
        std::string input{"123 456 789"};
        std::istringstream iss(input);
        Compiler::Lexer lexer(&iss);

        std::vector<std::string> expected_tokens = {"123", "456", "789"};
        size_t index = 0;

        while (lexer.has_next_token()) {
            std::string token = lexer.get_next_token().value();
            assert(token == expected_tokens[index]);
            index++;
        }
        assert(index == expected_tokens.size());
    }

    // Test Case 7: Special Characters
    {
        std::string input{"@ # $ % ^ & * ()"};
        std::istringstream iss(input);
        Compiler::Lexer lexer(&iss);

        std::vector<std::string> expected_tokens = {"@", "#", "$", "%", "^", "&", "*", "(", ")"};
        size_t index = 0;

        while (lexer.has_next_token()) {
            std::string token = lexer.get_next_token().value();
            assert(token == expected_tokens[index]);
            index++;
        }
        assert(index == expected_tokens.size());
    }

    // Test Case 8: Alphanumeric Mixed Input
    {
        std::string input{"test123 abc456 def789"};
        std::istringstream iss(input);
        Compiler::Lexer lexer(&iss);

        std::vector<std::string> expected_tokens = {"test123", "abc456", "def789"};
        size_t index = 0;

        while (lexer.has_next_token()) {
            std::string token = lexer.get_next_token().value();
            assert(token == expected_tokens[index]);
            index++;
        }
        assert(index == expected_tokens.size());
    }
}

int main() {
    Logger::init({&std::cerr});

    test_lexer();

    return 0;
}
