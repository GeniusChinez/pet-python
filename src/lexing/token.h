#pragma once

#include <string>
#include <map>

enum class TokenKind {
    None,
    EndOfFile,

    ConstantBooleanTrue,
    ConstantBooleanFalse,
    ConstantInteger,
    ConstantFloat,
    ConstantImaginary,
    ConstantString,
    ConstantMultilineString,

    KeywordAs,
    KeywordAssert,
    KeywordAwait,
    KeywordBreak,
    KeywordClass,
    KeywordContinue,
    KeywordDef,
    KeywordDel,
    KeywordElif,
    KeywordElse,
    KeywordExcept,
    KeywordFinally,
    KeywordFor,
    KeywordFrom,
    KeywordGlobal,
    KeywordIf,
    KeywordImport,
    KeywordIn,
    KeywordIs,
    KeywordLambda,
    KeywordLen,
    KeywordNone,
    KeywordNonlocal,
    KeywordPass,
    KeywordRaise,
    KeywordReturn,
    KeywordTry,
    KeywordWhile,
    KeywordWith,
    KeywordYield,

    ConditionalNot,
    ConditionalAnd,
    ConditionalOr,
    ConditionalXor,

    Identifier,

    Semicolon,
    Colon,
    Comma,
    Access,
    Pound,
    SingleArrow,
    QuestionMark,
    At,

    OpeningRoundBracket,
    ClosingRoundBracket,
    OpeningSquareBracket,
    ClosingSquareBracket,
    OpeningCurlyBracket,
    ClosingCurlyBracket,

    ArithmeticAdd,
    ArithmeticSub,
    ArithmeticMul,
    ArithmeticDiv,
    ArithmeticFloorDiv,
    ArithmeticMod,
    ArithmeticPow,

    RelationalEquals,
    RelationalNotEqual,
    RelationalIdentical,
    RelationalNotIdentical,
    RelationalGreaterThan,
    RelationalGreaterThanOrEquals,
    RelationalLesserThan,
    RelationalLesserThanOrEquals,
    RelationalIsContainedIn,
    RelationalIsNotContainedIn,

    LogicalAnd,
    LogicalOr,
    LogicalNot,
    LogicalXor,
    LogicalLeftShift,
    LogicalRightShift,

    Assignment,
    AssignmentLogicalAnd,
    AssignmentLogicalOr,
    AssignmentLogicalXor,
    AssignmentLogicalLeftShift,
    AssignmentLogicalRightShift,
    AssignmentArithmeticAdd,
    AssignmentArithmeticSub,
    AssignmentArithmeticMul,
    AssignmentArithmeticDiv,
    AssignmentArithmeticFloorDiv,
    AssignmentArithmeticMod,
    AssignmentArithmeticPow,
    AssignmentArithmeticAt,
};

const std::map<std::string, TokenKind> stringTokenMap = {
    { "True",     TokenKind::ConstantBooleanTrue },
    { "False",    TokenKind::ConstantBooleanFalse },

    { "as",       TokenKind::KeywordAs },
    { "assert",   TokenKind::KeywordAssert },
    { "await",    TokenKind::KeywordAwait },
    { "break",    TokenKind::KeywordBreak },
    { "class",    TokenKind::KeywordClass },
    { "continue", TokenKind::KeywordContinue },
    { "def",      TokenKind::KeywordDef },
    { "del",      TokenKind::KeywordDel },
    { "elif",     TokenKind::KeywordElif },
    { "else",     TokenKind::KeywordElse },
    { "except",   TokenKind::KeywordExcept },
    { "finally",  TokenKind::KeywordFinally },
    { "for",      TokenKind::KeywordFor },
    { "from",     TokenKind::KeywordFrom },
    { "global",   TokenKind::KeywordGlobal },
    { "if",       TokenKind::KeywordIf },
    { "import",   TokenKind::KeywordImport },
    { "in",       TokenKind::RelationalIsContainedIn },
    { "not in",   TokenKind::RelationalIsNotContainedIn },
    { "is",       TokenKind::RelationalIdentical },
    { "lambda",   TokenKind::KeywordLambda },
    { "None",     TokenKind::KeywordNone },
    { "nonlocal", TokenKind::KeywordNonlocal },
    { "pass",     TokenKind::KeywordPass },
    { "raise",    TokenKind::KeywordRaise },
    { "return",   TokenKind::KeywordReturn },
    { "try",      TokenKind::KeywordTry },
    { "while",    TokenKind::KeywordWhile },
    { "with",     TokenKind::KeywordWith },
    { "yield",    TokenKind::KeywordYield },

    { "not",      TokenKind::ConditionalNot },
    { "and",      TokenKind::ConditionalAnd },
    { "or",       TokenKind::ConditionalOr },
    { "xor",      TokenKind::ConditionalXor },
};

struct Token {
    TokenKind kind;
    size_t lineNumber;
    size_t columnNumber;
    std::string value;

    int virtualOffset {0}; // offset from the beginning of the line.. 

    void appendCharacter(uint32_t val) {
        GeniusC::AppendUtf8(value, val);
    }

    void clear() {
        value.clear();
        virtualOffset = 0;
    }
};
