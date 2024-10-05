#include <lexer/lexer.h>
#include <logger.h>

int main() {
    Logger::init({&std::cerr});

    LOG(LogLevel::INFO) << "start main" << std::endl;

    std::string input{"test this message"};
    std::istringstream iss(input);
    Compiler::Lexer lexer(&iss);

    while(lexer.has_next_token()) {
        LOG(LogLevel::INFO) << lexer.get_next_token().value();
    }

    LOG(LogLevel::INFO) << "end main" << std::endl;

    return 0;
}
