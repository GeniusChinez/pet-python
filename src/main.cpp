#include <cstdio>
#include <cassert>

void quit();
void quitIfErrorsWereMet();

#include "common/common.h"
#include "lexing/lexer.h"

void quit() {
    Console::write(
        "\nerrors: ", 
        ErrorReporter::getNumberOfErrors(), 
        ", "
    );

    Console::writeLine(
        "warnings: ", 
        ErrorReporter::getNumberOfWarnings()
    );

    exit(ErrorReporter::getNumberOfErrors());
}

void quitIfErrorsWereMet() {
    if (ErrorReporter::getNumberOfErrors() > 0) {
        quit();
    }
}

void test() {
    
}

int main(int argc, char const *argv[]) {
    test();
    quit();
    return 0;
}
