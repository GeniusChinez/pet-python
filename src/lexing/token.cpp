#pragma once

#include <cassert>
#include "token.h"

const char* toString(TokenKind kind) {
    switch (kind) {
    case TokenKind::None: return "<None>";
    case TokenKind::EndOfFile: return "<EndOfFile>";

    case TokenKind::ConstantBooleanTrue: return "true";
    case TokenKind::ConstantBooleanFalse: return "false";
    case TokenKind::ConstantInteger: return "<IntegerConstant>";
    case TokenKind::ConstantFloat: return "<FloatConstant>";
    case TokenKind::ConstantImaginary: return "<ImaginaryConstant>";
    case TokenKind::ConstantString: return "<StringConstant>";
    case TokenKind::ConstantMultilineString: return "<MultilineStringConstant>";

    case TokenKind::KeywordAs: return "as";
    case TokenKind::KeywordAssert: return "assert";
    case TokenKind::KeywordAwait: return "await";
    case TokenKind::KeywordBreak: return "break";
    case TokenKind::KeywordClass: return "class";
    case TokenKind::KeywordContinue: return "continue";
    case TokenKind::KeywordDef: return "def";
    case TokenKind::KeywordDel: return "del";
    case TokenKind::KeywordElif: return "elif";
    case TokenKind::KeywordElse: return "else";
    case TokenKind::KeywordExcept: return "except";
    case TokenKind::KeywordFinally: return "finally";
    case TokenKind::KeywordFor: return "for";
    case TokenKind::KeywordFrom: return "from";
    case TokenKind::KeywordGlobal: return "global";
    case TokenKind::KeywordIf: return "if";
    case TokenKind::KeywordImport: return "import";
    case TokenKind::KeywordIn: return "in";
    case TokenKind::KeywordIs: return "is";
    case TokenKind::KeywordLambda: return "lambda";
    case TokenKind::KeywordLen: return "len";
    case TokenKind::KeywordNone: return "None";
    case TokenKind::KeywordNonlocal: return "nonlocal";
    case TokenKind::KeywordPass: return "pass";
    case TokenKind::KeywordRaise: return "raise";
    case TokenKind::KeywordReturn: return "return";
    case TokenKind::KeywordTry: return "try";
    case TokenKind::KeywordWhile: return "while";
    case TokenKind::KeywordWith: return "with";
    case TokenKind::KeywordYield: return "yield";

    case TokenKind::ConditionalNot: return "not";
    case TokenKind::ConditionalAnd: return "and";
    case TokenKind::ConditionalOr: return "or";
    case TokenKind::ConditionalXor: return "xor";

    case TokenKind::Identifier: return "<identifier>";

    case TokenKind::Semicolon: return ";";
    case TokenKind::Colon: return ":";
    case TokenKind::Comma: return ",";
    case TokenKind::Access: return ".";
    case TokenKind::Pound: return "#";
    case TokenKind::SingleArrow: return "->";
    case TokenKind::QuestionMark: return "?";
    case TokenKind::At: return "@";

    case TokenKind::OpeningRoundBracket: return "(";
    case TokenKind::ClosingRoundBracket: return ")";
    case TokenKind::OpeningSquareBracket: return "[";
    case TokenKind::ClosingSquareBracket: return "]";
    case TokenKind::OpeningCurlyBracket: return "{";
    case TokenKind::ClosingCurlyBracket: return "}";

    case TokenKind::ArithmeticAdd: return "+";
    case TokenKind::ArithmeticSub: return "-";
    case TokenKind::ArithmeticMul: return "*";
    case TokenKind::ArithmeticDiv: return "/";
    case TokenKind::ArithmeticFloorDiv: return "//";
    case TokenKind::ArithmeticMod: return "%";
    case TokenKind::ArithmeticPow: return "**";

    case TokenKind::RelationalEquals: return "==";
    case TokenKind::RelationalNotEqual: return "!=";
    case TokenKind::RelationalIdentical: return "is";
    case TokenKind::RelationalNotIdentical: return "is not";
    case TokenKind::RelationalGreaterThan: return ">";
    case TokenKind::RelationalGreaterThanOrEquals: return ">=";
    case TokenKind::RelationalLesserThan: return "<";
    case TokenKind::RelationalLesserThanOrEquals: return "<=";
    case TokenKind::RelationalIsContainedIn: return "in";
    case TokenKind::RelationalIsNotContainedIn: return "not in";

    case TokenKind::LogicalAnd: return "&";
    case TokenKind::LogicalOr: return "|";
    case TokenKind::LogicalNot: return "~";
    case TokenKind::LogicalXor: return "^";
    case TokenKind::LogicalLeftShift: return "<<";
    case TokenKind::LogicalRightShift: return ">>";

    case TokenKind::Assignment: return "=";
    case TokenKind::AssignmentLogicalAnd: return "&=";
    case TokenKind::AssignmentLogicalOr: return "|=";
    case TokenKind::AssignmentLogicalXor: return "^=";
    case TokenKind::AssignmentLogicalLeftShift: return "<<=";
    case TokenKind::AssignmentLogicalRightShift: return ">>=";
    case TokenKind::AssignmentArithmeticAdd: return "+=";
    case TokenKind::AssignmentArithmeticSub: return "-=";
    case TokenKind::AssignmentArithmeticMul: return "*=";
    case TokenKind::AssignmentArithmeticDiv: return "/=";
    case TokenKind::AssignmentArithmeticFloorDiv: return "//=";
    case TokenKind::AssignmentArithmeticMod: return "%=";
    case TokenKind::AssignmentArithmeticPow: return "**=";
    case TokenKind::AssignmentArithmeticAt: return "@=";
    default:
        assert(0);
    }
    return "<token>";
}

std::string toString(const Token& token) {
    std::string value = "Token(value='" + token.value + "')";
    return value;
}
