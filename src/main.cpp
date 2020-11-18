#include <cstdio>

void quit();
void quitIfErrorsWereMet();

#include "common/common.h"

void quit() {
    Console::write(
        "\nerrors: ", 
        ErrorReporter::getNumberOfErrors(), 
        ", "
    );

    Console::writeLine(
        "warnings: ", 
        ErrorReporter::getNumberOfWarnings(), 
        "\n"
    );

    exit(ErrorReporter::getNumberOfErrors());
}

void quitIfErrorsWereMet() {
    if (ErrorReporter::getNumberOfErrors() > 0) {
        quit();
    }
}

int main(int argc, char const *argv[]) {
    return 0;
}
