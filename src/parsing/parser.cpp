#pragma once
#include "parser.h"

bool Parser::matchToken(TokenKind kind) const {
    return currentToken.kind == kind;
}

bool Parser::matchToken(const std::string& value) const {
    return currentToken.value == value;
}

bool Parser::skipOptionalToken(TokenKind kind) {
    if (matchToken(kind)) {
        fetchToken();
        return true;
    }
    return false;
}

bool Parser::skipOptionalToken(const std::string& value) {
    if (matchToken(value)) {
        fetchToken();
        return true;
    }
    return false;
}

void Parser::requireToken(TokenKind kind) const {
    if (matchToken(kind)) {
        return;
    }

    ErrorReporter::reportFatalError(
        formatAsString(
            "required '",
            toString(kind),
            "', but found: ",
            currentToken.value
        ),
        currentLocation
    );
}

void Parser::requireToken(const std::string& value) const {
    if (matchToken(value)) {
        return;
    }

    ErrorReporter::reportFatalError(
        formatAsString(
            "required '",
            value,
            "', but found: ",
            currentToken.value
        ),
        currentLocation
    );
}

void Parser::skipRequiredToken(TokenKind kind) {
    requireToken(kind);
    fetchToken();
}

void Parser::skipRequiredToken(const std::string& value) {
    requireToken(value);
    fetchToken();
}

std::string Parser::parseName() {
    requireToken(TokenKind::Identifier);
    std::string value = currentToken.value;
    fetchToken();
    return value;
}

Token& Parser::fetchToken() {
    if (not tokenBuffer.empty()) {
        currentToken = tokenBuffer.back();
        tokenBuffer.pop_back();
    }
    else {
        lexer->readToken(currentToken);
    }

    if (matchToken(TokenKind::ConditionalNot)) {
        Token temp = currentToken;
        fetchToken();

        if (matchToken(TokenKind::RelationalIsContainedIn)) {
            currentToken.kind = TokenKind::RelationalIsNotContainedIn;
            currentToken.value = "not in";
        }
        else {
            tokenBuffer.push_back(currentToken);
            currentToken = temp;
        }
    }
    else if (matchToken(TokenKind::RelationalIdentical)) {
        Token temp = currentToken;
        fetchToken();

        if (matchToken(TokenKind::ConditionalNot)) {
            currentToken.kind = TokenKind::RelationalNotIdentical;
            currentToken.value = "is not";
        }
        else {
            tokenBuffer.push_back(currentToken);
            currentToken = temp;
        }
    }

    if (currentToken.lineNumber != currentLocation.line) {
        linesChanged = true;
    }
    else {
        linesChanged = false;
    }

    currentLocation.line = currentToken.lineNumber;
    currentLocation.column = currentToken.columnNumber;
    currentLocation.file = lexer->getFileName();

    return currentToken;
}

#include "expr_parsing.h"
#include "stmt_parsing.h"
