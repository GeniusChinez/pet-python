#pragma once

#include <string>

#include "location.h"
#include "io.h"

struct ErrorReporter {
    static void reportError(const std::string& message) {
        Console::writeLine("    [error]: ", message.data());
        ErrorReporter::lastFile = "";
        ErrorReporter::numberOfErrors++;
    }

    static void reportError(
        const std::string& message, 
        const Location& location
    ) {
        if (ErrorReporter::lastFile != location.file) {
            Console::writeLine("\nin file '", location.file, "':");
            ErrorReporter::lastFile = location.file;
        }

        Console::writeLine(
            "    [error] on (", location.line, ",", location.column, "): ",
            message.data()
        );

        ErrorReporter::numberOfErrors++;
    }

    template <typename ...Args>
    static void reportFatalError(Args&&... args) {
        reportError(std::forward<Args>(args)...);
        quit();
    }

    static void reportWarning(const std::string& message) {
        Console::writeLine(
            "    [warning]: ",
            message.data()
        );
        ErrorReporter::lastFile = "";
        ErrorReporter::numberOfWarnings++;
    }

    static void reportWarning(
        const std::string& message, 
        const Location& location
    ) {
        if (ErrorReporter::lastFile != location.file) {
            Console::writeLine("\nin file '", location.file, "':");
            ErrorReporter::lastFile = location.file;
        }

        Console::writeLine(
            "    [warning] on (", location.line, ",", location.column, "): ",
            message.data()
        );

        ErrorReporter::numberOfWarnings++;
    }

    static void elaborate(const std::string& message) {
        Console::writeLine("        ...: ", message.data());
    }

    static void elaborate(
        const std::string& message, 
        const Location& location
    ) {
        if (ErrorReporter::lastFile != location.file) {
            Console::writeLine("\nin file '", location.file, "':");
            ErrorReporter::lastFile = location.file;
        }

        Console::writeLine(
            "        ... on (", location.line, ",", location.column, "): ",
            message.data()
        );
    }

    static int getNumberOfErrors() {
        return numberOfErrors;
    }

    static int getNumberOfWarnings() {
        return numberOfWarnings;
    }

private:
    static int numberOfErrors;
    static int numberOfWarnings;
    static std::string lastFile;
};

int ErrorReporter::numberOfErrors = 0;
int ErrorReporter::numberOfWarnings = 0;
std::string ErrorReporter::lastFile = "";
