#include <cstdio>
#include <cassert>

void quit();
void quitIfErrorsWereMet();

#include "common/common.h"
#include "lexing/lexer.h"
#include "ast/ast.h"
#include "ast/to_src/to_src.h"
#include "parsing/parser.cpp"

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

void testLexer() {
    Lexer lexer;
    assert(lexer.useFile("sample.py"));

    Token token;
    lexer.readToken(token);

    while (token.kind != TokenKind::EndOfFile) {
        Console::writeLine(toString(token.kind));
        lexer.readToken(token);
    }
}

void testParser() {
    Lexer lexer;
    assert(lexer.useFile("sample.py"));

    Parser parser(&lexer);
    std::vector<StmtPtr> statements;

    parser.parseStmtList(statements);
}

void testAstToSourceTransformer() {
    Lexer lexer;
    assert(lexer.useFile("sample.py"));

    Parser parser(&lexer);
    std::vector<StmtPtr> statements;

    parser.parseStmtList(statements);

    std::ofstream fileStream("transformed_output.py");
    assert(fileStream.is_open());

    PythonAstTransformer transformer;

    for (auto& stmt : statements) {
        transformer.appendStmt(stmt);
    }

    printToStream(fileStream, transformer.getBuffer());
    fileStream.close();
}

void test() {
    //testLexer();
    //testParser();
    testAstToSourceTransformer();
}

int main(int argc, char const *argv[]) {
    test();
    quit();
    return 0;
}
