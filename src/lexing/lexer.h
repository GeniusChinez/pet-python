#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include "reader.h"
#include "token.cpp"

class Lexer {
public:
    Lexer() = default;
    ~Lexer() {}

    /**
     * @brief      Prepares the lexer to use the given file.
     *
     * @param[in]  fileName  The file name.
     *
     * @return     An indication of whether the file was read or not.
     */
    [[nodiscard]]
    bool useFile(std::string fileName) {
        if (not reader.openFile(fileName)) {
            return false;
        }

        currentLineNumber = 1;
        currentColumnNumber = 0;

        fetchNextCharacter();
        return true;
    }

    /**
     * @brief      Gets the file name curretly being used by the lexer.
     *
     * @return     The file name.
     */
    inline const std::string& getFileName() const {
        return reader.getFileName();
    }

    /**
     * @brief      Reads a token.
     *
     * @param      token  where to put details of the token.
     */
    void readToken(Token& token) {
        token.clear();

        while (iswspace(currentCharacter)) {
            if (matchCharacter('\n')) {
                token.virtualOffset = 0;
                currentLineNumber++;
                currentColumnNumber = 0;
            }
            else if (matchCharacter('\t')) {
                token.virtualOffset += 4;
            }
            else {
                token.virtualOffset += 1;
            }
            fetchNextCharacter();
        }

        token.lineNumber = currentLineNumber;
        token.columnNumber = currentColumnNumber;
        token.kind = TokenKind::None;

        if (fileEnded()) {
            token.kind = TokenKind::EndOfFile;
            token.value = toString(TokenKind::EndOfFile);
            token.virtualOffset = 0;
            return;
        }

        #define LEX_CASE_1(ch, ifchar) \
            case ch: {\
                appendCharacterAndFetchNext(currentCharacter, token);\
                token.kind = ifchar;\
                return;\
            }

        #define LEX_CASE_2(char1, char2, ifchar1, ifchar12) \
            case char1: {\
                appendCharacterAndFetchNext(currentCharacter, token);\
\
                if (not matchCharacter(char2)) {\
                    token.kind = ifchar1;\
                    return;\
                }\
\
                token.kind = ifchar12;\
                appendCharacterAndFetchNext(currentCharacter, token);\
\
                return;\
            }

        switch (currentCharacter) {
        LEX_CASE_1(0, TokenKind::EndOfFile)

        LEX_CASE_1('(', TokenKind::OpeningRoundBracket)
        LEX_CASE_1(')', TokenKind::ClosingRoundBracket)
        LEX_CASE_1('[', TokenKind::OpeningSquareBracket)
        LEX_CASE_1(']', TokenKind::ClosingSquareBracket)
        LEX_CASE_1('{', TokenKind::OpeningCurlyBracket)
        LEX_CASE_1('}', TokenKind::ClosingCurlyBracket)
        
        LEX_CASE_1(';', TokenKind::Semicolon)
        LEX_CASE_1(':', TokenKind::Colon)
        LEX_CASE_1(',', TokenKind::Comma)

        LEX_CASE_1('~', TokenKind::LogicalNot)
        LEX_CASE_1('.', TokenKind::Access)
        
        LEX_CASE_2('@', '=', 
            TokenKind::At, 
            TokenKind::AssignmentArithmeticAt)

        LEX_CASE_2('!', '=', 
            TokenKind::ConditionalNot, 
            TokenKind::RelationalNotEqual)

        LEX_CASE_2('|', '=', 
            TokenKind::LogicalOr, 
            TokenKind::AssignmentLogicalOr)

        LEX_CASE_2('^', '=', 
            TokenKind::LogicalXor, 
            TokenKind::AssignmentLogicalXor)

        LEX_CASE_2('+', '=', 
            TokenKind::ArithmeticAdd, 
            TokenKind::AssignmentArithmeticAdd)

        LEX_CASE_2('%', '=', 
            TokenKind::ArithmeticMod, 
            TokenKind::AssignmentArithmeticMod)

        LEX_CASE_2('&', '=', 
            TokenKind::LogicalAnd, 
            TokenKind::AssignmentLogicalAnd)

        case '*': {
            appendCharacterAndFetchNext(currentCharacter, token);

            switch (currentCharacter) {
            case '*': {
                appendCharacterAndFetchNext(currentCharacter, token);

                if (matchCharacter('=')) {
                    appendCharacterAndFetchNext(currentCharacter, token);
                    token.kind = 
                        TokenKind::AssignmentArithmeticPow;
                    return;
                }

                token.kind = TokenKind::ArithmeticPow;
                return;
            }
            case '=': {
                appendCharacterAndFetchNext(currentCharacter, token);
                token.kind = 
                    TokenKind::AssignmentArithmeticMul;
                return;
            }
            default:
                token.kind = TokenKind::ArithmeticMul;
            }

            return;
        } 

        case '#': {
            fetchNextCharacter();
            skipInlineComment();
            readToken(token);
            return;
        }
        case '/': {
            appendCharacterAndFetchNext(currentCharacter, token);

            switch (currentCharacter) {
            case '/': {
                appendCharacterAndFetchNext(currentCharacter, token);

                if (matchCharacter('=')) {
                    appendCharacterAndFetchNext(currentCharacter, token);
                    token.kind = 
                        TokenKind::AssignmentArithmeticFloorDiv;
                    return;
                }

                token.kind = TokenKind::ArithmeticFloorDiv;
                return;
            }
            case '=': {
                appendCharacterAndFetchNext(currentCharacter, token);
                token.kind = TokenKind::AssignmentArithmeticDiv;
                return;
            }
            default: 
                token.kind = TokenKind::ArithmeticDiv;
            }

            return;
        }
        case '=': {
            appendCharacterAndFetchNext(currentCharacter, token);

            if (matchCharacter('=')) {
                appendCharacterAndFetchNext(currentCharacter, token);
                token.kind = TokenKind::RelationalEquals;
            }
            else {
                token.kind = TokenKind::Assignment;
            }

            return;
        }
        case '-': {
            appendCharacterAndFetchNext(currentCharacter, token);

            switch (currentCharacter) {
            case '=': {
                appendCharacterAndFetchNext(currentCharacter, token);
                token.kind = TokenKind::AssignmentArithmeticSub;
                return;
            }
            case '>': {
                appendCharacterAndFetchNext(currentCharacter, token);
                token.kind = TokenKind::SingleArrow;
                return;
            }
            default:
                token.kind = TokenKind::ArithmeticSub;
            }

            return;
        }
        case '<': {
            appendCharacterAndFetchNext(currentCharacter, token);

            switch (currentCharacter) {
            case '=': {
                appendCharacterAndFetchNext(currentCharacter, token);
                token.kind = TokenKind::RelationalLesserThanOrEquals;
                return;
            }
            case '<': {
                appendCharacterAndFetchNext(currentCharacter, token);

                if (matchCharacter('=')) {
                    appendCharacterAndFetchNext(currentCharacter, token);
                    token.kind = TokenKind::AssignmentLogicalLeftShift;
                    return;
                }

                token.kind = TokenKind::LogicalLeftShift;
                return;
            }
            default:
                token.kind = TokenKind::RelationalLesserThan;
            }

            return;
        }
        case '>': {
            appendCharacterAndFetchNext(currentCharacter, token);

            switch (currentCharacter) {
            case '=': {
                appendCharacterAndFetchNext(currentCharacter, token);
                token.kind = TokenKind::RelationalGreaterThanOrEquals;
                return;
            }
            case '>': {
                appendCharacterAndFetchNext(currentCharacter, token);

                if (matchCharacter('=')) {
                    token.appendCharacter(currentCharacter);
                    fetchNextCharacter();
                    token.kind = TokenKind::AssignmentLogicalRightShift;
                    return;
                }

                token.kind = TokenKind::LogicalRightShift;
                return;
            }
            default:
                token.kind = TokenKind::RelationalGreaterThan;
            }

            return;
        }
        case '\'':
        case '"': {
            readString(token);
            return;
        }
        default:
            if (iswdigit(currentCharacter)) {
                readNumber(token);
                return;
            }
            else if (
                iswalpha(currentCharacter) 
                || matchCharacter('_')
            ) {
                readName(token);
                return;
            }
            else {
                Location thisLocation(
                    getFileName(), 
                    currentLineNumber, 
                    currentColumnNumber
                );

                ErrorReporter::reportError(
                    formatAsString(
                        "unrecognized character: ",
                        convertCodepointToHex(currentCharacter)
                    ),
                    thisLocation
                );
                quit();
            }
        }
    }

private:
    inline bool matchCharacter(uint32_t val) const {
        return currentCharacter == val;
    }

    inline bool fileEnded() const {
        return matchCharacter(0);
    }

    void fetchNextCharacter() {
        if (not characterBuffer.empty()) {
            currentCharacter = characterBuffer.back();
            characterBuffer.pop_back();
            currentColumnNumber++;
        }
        else {
            currentCharacter = reader.getCharacter();
            currentColumnNumber++;
        }
    }

    inline void putbackCharacter(uint32_t val) {
        characterBuffer.push_back(val);
        currentColumnNumber--;
    }

    inline void appendCharacterAndFetchNext(uint32_t value, Token& token) {
        token.appendCharacter(value);
        fetchNextCharacter();
    }

    inline void skipSpace() {
        while (iswspace(currentCharacter)) {
            if (matchCharacter('\n')) {
                currentLineNumber++;
                currentColumnNumber = 0;
            }
            fetchNextCharacter();
        }
    }

    inline const Location getCurrentLocation() const {
        Location location(
            getFileName(), 
            currentLineNumber, 
            currentColumnNumber
        );
        return location;
    }

    inline void skipInlineComment() {
        while (not matchCharacter('\n')) {
            fetchNextCharacter();
        }
    }

    inline void readName(Token& token) {
        while (iswalnum(currentCharacter) || matchCharacter('_')) {
            appendCharacterAndFetchNext(currentCharacter, token);
        }
        token.kind = getKindOfWord(token.value);
    }

    // @note: this function is very minimalistic at the moment.
    // ... There are various number formats that it can't understand.
    inline void readNumber(Token& token) {
        while (iswdigit(currentCharacter)) {
            appendCharacterAndFetchNext(currentCharacter, token);
        }

        if (matchCharacter('j')) {
            appendCharacterAndFetchNext(currentCharacter, token);
            token.kind = TokenKind::ConstantImaginary;
            return;
        }

        if (not matchCharacter('.')) {
            token.kind = TokenKind::ConstantInteger;
            return;
        }

        fetchNextCharacter();

        if (not iswdigit(currentCharacter)) {
            putbackCharacter(currentCharacter);

            putbackCharacter('.');
            fetchNextCharacter();
            
            token.kind = TokenKind::ConstantInteger;
            return;
        }

        token.appendCharacter('.');
        appendCharacterAndFetchNext(currentCharacter, token);

        while (iswdigit(currentCharacter)) {
            appendCharacterAndFetchNext(currentCharacter, token);
        }

        if (matchCharacter('j')) {
            appendCharacterAndFetchNext(currentCharacter, token);
            token.kind = TokenKind::ConstantImaginary;
            return;
        }

        token.kind = TokenKind::ConstantFloat;
    }

    inline void readString(Token& token) {
        token.kind = TokenKind::ConstantString;

        const auto openingCharacter = currentCharacter;
        const auto startingLocation = getCurrentLocation();

        appendCharacterAndFetchNext(currentCharacter, token);

        bool isMultiline = false;

        if ((openingCharacter == '"') && matchCharacter('"')) {
            appendCharacterAndFetchNext(currentCharacter, token);
            if (matchCharacter('"')) {
                appendCharacterAndFetchNext(currentCharacter, token);
                isMultiline = true;
                token.kind = TokenKind::ConstantMultilineString;
            }
            else {
                return;
            }
        }

        while (not fileEnded()) {
            switch (currentCharacter) {
            case '\n': {
                token.appendCharacter(currentCharacter);
                currentLineNumber++;
                currentColumnNumber = 0;
                fetchNextCharacter();
                break;
            }
            case '\\': {
                fetchNextCharacter();

                switch (currentCharacter) {
                case 't':  token.appendCharacter('\t'); break;
                case '"':  token.appendCharacter('"');  break;
                case '\'': token.appendCharacter('\''); break;
                case 'n':  token.appendCharacter('\n'); break;
                default:
                    auto location = getCurrentLocation();
                    ErrorReporter::reportWarning(
                        formatAsString(
                            "unrecognized escape sequence: \\",
                            convertCodepointToHex(currentCharacter)
                        ),
                        location
                    );
                    token.appendCharacter('\\');
                    continue;
                }

                fetchNextCharacter();
                break;
            }
            default:
                token.appendCharacter(currentCharacter);
                if ((openingCharacter == '"') && matchCharacter('"')) {
                    fetchNextCharacter();

                    if (not isMultiline) {
                        return;
                    }

                    if (not matchCharacter('"')) {
                        continue;
                    }

                    appendCharacterAndFetchNext(currentCharacter, token);

                    if (matchCharacter('"')) {
                        appendCharacterAndFetchNext(
                            currentCharacter, 
                            token
                        );
                        return;
                    }

                    continue;
                } 
                else if (
                    (openingCharacter == '\'') 
                    && matchCharacter('\'')
                ) {
                    fetchNextCharacter();
                    return;
                }
                else {
                    fetchNextCharacter();
                }
            }
        }

        ErrorReporter::reportFatalError(
            "unterminated string constant",
            startingLocation
        );
    }

    uint32_t currentCharacter;
    std::vector<uint32_t> characterBuffer;

    size_t currentLineNumber;
    size_t currentColumnNumber;

    Reader reader;
};

