#pragma once

class Parser {
public:
    Parser(Lexer* lexer)
        : lexer(lexer) {
            fetchToken();
        }

    void parseStmtList(StmtList&);

private:
    bool matchToken(TokenKind kind) const;
    bool matchToken(const std::string& value) const;

    bool skipOptionalToken(TokenKind kind);
    bool skipOptionalToken(const std::string& value);

    void requireToken(TokenKind kind) const;
    void requireToken(const std::string& value) const;

    void skipRequiredToken(TokenKind kind);
    void skipRequiredToken(const std::string& value);

    Token& fetchToken();

    std::string parseName();

    // Expression parsing
    TargetPtr parseTarget();

    // @warning: the following 4 methods need thorough testing
    CompIterPtr parseComprehensionIter();
    CompForPtr parseComprehensionFor();
    CompIfPtr parseComprehensionIf();
    ComprehensionPtr parseComprehension();

    ExprPtr parseYieldExpr();
    ExprPtr parseTopExpr();

    void parseSetDisplayExpr(SetDisplayExpr&);
    void parseDictDisplayExpr(DictDisplayExpr&);

    ExprPtr parseListDisplayExpr();
    ExprPtr parseParenthesizedExpr();
    ExprPtr parseCurlyBracketDisplayExpr();
    ExprPtr parseDisplayExpr();

    ExprPtr parseCallExpr(ExprPtr);

    void parseSliceStride(SlicingExpr&);
    void parseSliceUpperbound(SlicingExpr&);
    ExprPtr parseSliceOrSubscription(ExprPtr);

    ExprPtr parseSliceCallAttrSubsExpr();

    ExprPtr parseAwaitExpr();
    ExprPtr parseExponentiationExpr();
    ExprPtr parseUnaryExpr();
    ExprPtr parseMultiplicativeExpr();
    ExprPtr parseAdditiveExpr();
    ExprPtr parseShiftExpr();
    ExprPtr parseBitwiseAndExpr();
    ExprPtr parseBitwiseXorExpr();
    ExprPtr parseBitwiseOrExpr();
    ExprPtr parseComparisonExpr();
    ExprPtr parseBooleanNotExpr();
    ExprPtr parseBooleanAndExpr();
    ExprPtr parseBooleanOrExpr();
    ExprPtr parseLambdaExpr();

    ExprPtr parseExpr();

    // Statement parsing
    StmtPtr parseAssertStmt();
    StmtPtr parsePassStmt();
    StmtPtr parseDelStmt();
    StmtPtr parseReturnStmt();
    StmtPtr parseYieldStmt();
    StmtPtr parseRaiseStmt();
    StmtPtr parseBreakStmt();
    StmtPtr parseContinueStmt();
    StmtPtr parseImportStmt();
    StmtPtr parseFromStmt();
    StmtPtr parseGlobalStmt();
    StmtPtr parseNonlocalStmt();
    StmtPtr parseIfStmt(uint64_t);
    StmtPtr parseWhileStmt(uint64_t);
    StmtPtr parseForStmt(uint64_t);
    StmtPtr parseTryStmt(uint64_t);
    StmtPtr parseWithStmt(uint64_t);
    StmtPtr parseFuncdefStmt(uint64_t);
    StmtPtr parseClassdefStmt(uint64_t);
    StmtPtr parseStmt(uint64_t);

    void parseParameterList(ParameterList&);
    void parseArgumentList(ArgumentList&);
    void parseDecoratorList(DecoratorList&);
    void parseSuite(Suite&, uint64_t);

    Token currentToken;
    std::vector<Token> tokenBuffer;

    Location currentLocation;

    bool linesChanged {false};
    int indentationScheme {0};

    Lexer* lexer;
};
