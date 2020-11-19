#pragma once

void Parser::parseParameterList(ParameterList& list) {
    do {
        Parameter param;

        if (skipOptionalToken(TokenKind::ArithmeticMul)) {
            param.stars++;
        }
        else if (skipOptionalToken(TokenKind::ArithmeticPow)) {
            param.stars += 2;
        }

        requireToken(TokenKind::Identifier);
        param.name = currentToken.value;
        fetchToken();

        if (skipOptionalToken(TokenKind::Colon)) {
            param.hint = parseExpr();
        }

        if (skipOptionalToken("=")) {
            param.value = parseExpr();
        }

        list.push_back(std::move(param));
    } while (skipOptionalToken(TokenKind::Comma));
}

void Parser::parseArgumentList(ArgumentList& list) {
    do {
        Argument argument;
        if (skipOptionalToken(TokenKind::ArithmeticPow)) {
            argument.stars = 2;
            argument.value = parseExpr();
            list.push_back(std::move(argument));
        }
        else if (
            skipOptionalToken(TokenKind::ArithmeticMul)
        ) {
            argument.stars = 1;
            argument.value = parseExpr();
            list.push_back(std::move(argument));
        }
        else {
            auto tempArgument = parseExpr();
            if (matchToken("=")) {
                if (tempArgument->kind != ExprKind::Name) {
                    ErrorReporter::reportFatalError(
                        "Unexpected '=' after non-name expression",
                        currentLocation
                    );
                }

                if (tempArgument->await) {
                    ErrorReporter::reportFatalError(
                        "Unexpected '=' after non-name expression",
                        currentLocation
                    );
                }

                argument.name = std::static_pointer_cast<NameExpr>(
                    tempArgument
                )->value;

                fetchToken();// skip "="
                argument.value = parseExpr();
            }
            else {
                argument.value = tempArgument;
            }
            list.push_back(std::move(argument));
        }
    } while (skipOptionalToken(TokenKind::Comma));
}

void Parser::parseDecoratorList(DecoratorList& list) {
    do {
        Decorator decorator;

        do {
            decorator.dottedName.push_back(parseName());
        } while (skipOptionalToken(TokenKind::Access));

        if (skipOptionalToken(TokenKind::OpeningRoundBracket)) {
            if (not skipOptionalToken(TokenKind::ClosingRoundBracket)) {
                parseArgumentList(decorator.argumentList);
                skipRequiredToken(TokenKind::ClosingRoundBracket);
            }
        }

        if (not linesChanged) {
            ErrorReporter::reportFatalError(
                "Expected a newline here",
                currentLocation
            );
        }

        list.push_back(std::move(decorator));
    } while (skipOptionalToken(TokenKind::At));
}

StmtPtr Parser::parseStmt(uint64_t indentation) {
    if (currentToken.virtualOffset > indentation) {
        ErrorReporter::reportFatalError(
            "Unexpected indentation while parsing statement",
            currentLocation
        );
    }
    else if (indentation > currentToken.virtualOffset) {
        return nullptr;
    }

    if (matchToken(TokenKind::EndOfFile)) {
        auto stmt = std::make_shared<Stmt>(
            currentLocation, 
            StmtKind::None
        );
        skipOptionalToken(TokenKind::Semicolon);
        return stmt;
    }

    if (matchToken(TokenKind::KeywordAssert)) {
        return parseAssertStmt();
    }
    else if (matchToken(TokenKind::KeywordPass)) {
        return parsePassStmt();
    }
    else if (matchToken(TokenKind::KeywordDel)) {
        return parseDelStmt();
    }
    else if (matchToken(TokenKind::KeywordReturn)) {
        return parseReturnStmt();
    }
    else if (matchToken(TokenKind::KeywordYield)) {
        return parseYieldStmt();
    }
    else if (matchToken(TokenKind::KeywordRaise)) {
        return parseRaiseStmt();
    }
    else if (matchToken(TokenKind::KeywordBreak)) {
        return parseBreakStmt();
    }
    else if (matchToken(TokenKind::KeywordContinue)) {
        return parseContinueStmt();
    }
    else if (matchToken(TokenKind::KeywordImport)) {
        return parseImportStmt();
    }
    else if (matchToken(TokenKind::KeywordFrom)) {
        return parseFromStmt();
    }
    else if (matchToken(TokenKind::KeywordGlobal)) {
        return parseGlobalStmt();
    }
    else if (matchToken(TokenKind::KeywordNonlocal)) {
        return parseNonlocalStmt();
    }
    else if (matchToken(TokenKind::KeywordIf)) {
        return parseIfStmt(indentation);
    }
    else if (matchToken(TokenKind::KeywordWhile)) {
        return parseWhileStmt(indentation);
    }
    else if (matchToken(TokenKind::KeywordFor)) {
        return parseForStmt(indentation);
    }
    else if (matchToken(TokenKind::KeywordTry)) {
        return parseTryStmt(indentation);
    }
    else if (matchToken(TokenKind::KeywordWith)) {
        return parseWithStmt(indentation);
    }
    else if (matchToken(TokenKind::KeywordDef)) {
        return parseFuncdefStmt(indentation);
    }
    else if (matchToken(TokenKind::KeywordClass)) {
        return parseClassdefStmt(indentation);
    }
    else if (skipOptionalToken(TokenKind::At)) {
        DecoratorList decorators;
        parseDecoratorList(decorators);

        switch (currentToken.kind) {
        case TokenKind::KeywordDef: {
            auto stmt = 
                std::static_pointer_cast<FuncdefStmt>(
                    parseFuncdefStmt(indentation)
                );
            stmt->decorators = std::move(decorators);
            return stmt;
        }
        case TokenKind::KeywordClass: {
            auto stmt = 
                std::static_pointer_cast<ClassdefStmt>(
                    parseClassdefStmt(indentation)
                );
            stmt->decorators = std::move(decorators);
            return stmt;
        }
        default:
            ErrorReporter::reportFatalError(
                formatAsString(
                    "expected 'def' or 'class' after decorator list. Found: ",
                    currentToken.value
                ),
                currentLocation
            );
        }
    }
    else if (skipOptionalToken(TokenKind::ConstantMultilineString)) {
        // comment
        return parseStmt(indentation);
    }
    else {
        // assignments and function calls
        ExprList exprs;
        
        do {
            exprs.push_back(parseExpr());
        } while (skipOptionalToken(TokenKind::Comma));

        if (skipOptionalToken(TokenKind::Colon)) {
            if (exprs.size() != 1) {
                ErrorReporter::reportFatalError(
                    "Unexpected expression-list before the ':' token",
                    exprs.front()->location
                );
            }

            auto stmt = std::make_shared<AnnotatedAssignmentStmt>(
                exprs.front()->location
            );

            stmt->autoTarget = exprs.front();
            stmt->annotation = parseExpr();

            skipRequiredToken(TokenKind::Assignment);
            stmt->value = parseExpr();
            skipOptionalToken(TokenKind::Semicolon);

            return stmt;
        }

        if (skipOptionalToken(TokenKind::Assignment)) {
            auto stmt = std::make_shared<AssignmentStmt>(
                exprs.front()->location
            );

            stmt->targetList = std::move(exprs);
            stmt->value = parseExpr();
            skipOptionalToken(TokenKind::Semicolon);

            return stmt;
        }

        if (isAssignment(currentToken.kind)) {
            auto stmt = std::make_shared<AugmentedAssignmentStmt>(
                exprs.front()->location
            );

            stmt->autoTarget = exprs.front();
            stmt->augOp = currentToken.kind;

            fetchToken();

            do {
                stmt->values.push_back(parseExpr());
            } while (skipOptionalToken(TokenKind::Comma));
            
            skipOptionalToken(TokenKind::Semicolon);
            return stmt;
        }
        else {
            assert(exprs.size() >= 1);

            if (exprs.size() != 1) {
                ErrorReporter::reportFatalError(
                    "Unexpected expression-list in statement context",
                    exprs.front()->location
                );
            }
            else {
                if (exprs.front()->kind != ExprKind::Call) {
                    ErrorReporter::reportFatalError(
                        "Unexpected non-call expression in statement context",
                        exprs.front()->location
                    );
                }
                else {
                    auto stmt = std::make_shared<ExprStmt>(
                        exprs.front()->location
                    );
                    
                    stmt->expr = exprs.front();
                    skipOptionalToken(TokenKind::Semicolon);

                    return stmt;
                }
            }
        }
    }
    assert(0);
    return nullptr;
}

StmtPtr Parser::parseAssertStmt() {
    auto stmt = std::make_shared<AssertStmt>(currentLocation);
    fetchToken();

    stmt->expr1 = parseExpr();

    if (skipOptionalToken(TokenKind::Comma)) {
        stmt->expr2 = parseExpr();
    }

    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parsePassStmt() {
    auto stmt = std::make_shared<Stmt>(currentLocation, StmtKind::Pass);
    fetchToken();
    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseDelStmt() {
    auto stmt = std::make_shared<DelStmt>(currentLocation);
    fetchToken();

    while (1) {
        stmt->targetList.push_back(parseTarget());

        if (linesChanged) {
            break;
        }

        if (skipOptionalToken(TokenKind::Comma)) {
            continue;
        }

        break;
    }
    
    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseReturnStmt() {
    auto stmt = std::make_shared<ReturnStmt>(currentLocation);
    fetchToken();

    if (linesChanged) {
        skipOptionalToken(TokenKind::Semicolon);
        return stmt;
    }

    while (1) {
        stmt->exprList.push_back(parseExpr());

        if (linesChanged) {
            break;
        }

        if (skipOptionalToken(TokenKind::Comma)) {
            continue;
        }

        break;
    }
        
    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseYieldStmt() {
    auto stmt = std::make_shared<YieldStmt>(currentLocation);
    stmt->expr = parseYieldExpr();
    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseRaiseStmt() {
    auto stmt = std::make_shared<RaiseStmt>(currentLocation);
    fetchToken();

    if (linesChanged) {
        skipOptionalToken(TokenKind::Semicolon);
        return stmt;
    }

    stmt->expr = parseExpr();

    if (skipOptionalToken(TokenKind::KeywordFrom)) {
        stmt->fromExpr = parseExpr();
    }

    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseBreakStmt() {
    auto stmt = std::make_shared<Stmt>(currentLocation, StmtKind::Break);
    fetchToken();
    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseContinueStmt() {
    auto stmt = std::make_shared<Stmt>(currentLocation, StmtKind::Continue);
    fetchToken();
    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseImportStmt() {
    auto stmt = std::make_shared<ImportStmt>(currentLocation);
    fetchToken();

    while (1) {
        ImportItem item;

        while (skipOptionalToken(TokenKind::Access)) {
            item.parts.push_back(".");
        }

        bool metAtLeastOneName = false;

        do {
            if (matchToken(TokenKind::Identifier)) {
                item.parts.push_back(currentToken.value);
                fetchToken();
                metAtLeastOneName = true;
            }
            else {
                break;
            }
        } while (skipOptionalToken(TokenKind::Access));

        if (not metAtLeastOneName) {
            ErrorReporter::reportFatalError(
                "Expected an import item. Invalid syntax.",
                currentLocation
            );
        }

        if (skipOptionalToken("as")) {
            requireToken(TokenKind::Identifier);
            item.alias = currentToken.value;
            fetchToken();
        }

        stmt->items.push_back(std::move(item));

        if (not skipOptionalToken(TokenKind::Comma)) {
            break;
        }
    }

    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseFromStmt() {
    auto stmt = std::make_shared<ImportStmt>(currentLocation);
    fetchToken();

    while (skipOptionalToken(TokenKind::Access)) {
        stmt->source.push_back(".");
    }

    bool metAtLeastOneName = false;

    do {
        if (matchToken(TokenKind::Identifier)) {
            stmt->source.push_back(currentToken.value);
            fetchToken();
            metAtLeastOneName = true;
        }
        else {
            break;
        }
    } while (skipOptionalToken(TokenKind::Access));

    if (not metAtLeastOneName) {
        ErrorReporter::reportFatalError(
            "Expected an import source. Invalid syntax.",
            currentLocation
        );
    }

    skipRequiredToken(TokenKind::KeywordImport);

    do {
        requireToken(TokenKind::Identifier);

        ImportItem item;
        
        item.parts.push_back(currentToken.value);
        fetchToken();

        if (skipOptionalToken(TokenKind::KeywordAs)) {
            item.alias = parseName();
        }

        stmt->items.push_back(std::move(item));

    } while (skipOptionalToken(TokenKind::Comma));

    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseGlobalStmt() {
    auto stmt = std::make_shared<GlobalStmt>(currentLocation);
    fetchToken();

    while (1) {
        requireToken(TokenKind::Identifier);
        stmt->names.push_back(currentToken.value);
        fetchToken();

        if (not skipOptionalToken(TokenKind::Comma)) {
            break;
        }
    }

    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

StmtPtr Parser::parseNonlocalStmt() {
    auto stmt = std::make_shared<NonlocalStmt>(currentLocation);
    fetchToken();

    while (1) {
        requireToken(TokenKind::Identifier);
        stmt->names.push_back(currentToken.value);
        fetchToken();

        if (not skipOptionalToken(TokenKind::Comma)) {
            break;
        }
    }

    skipOptionalToken(TokenKind::Semicolon);
    return stmt;
}

void Parser::parseSuite(Suite& suite, uint64_t indentation) {
    auto& list = suite.stmts;
    skipRequiredToken(TokenKind::Colon);

    if (not linesChanged) {
        ErrorReporter::reportFatalError(
            formatAsString(
                "Expected a newline here, but found: ",
                currentToken.value
            ),
            currentLocation
        );
    }

    const auto blockIndentation = currentToken.virtualOffset;

    if (indentation >= blockIndentation) {
        ErrorReporter::reportFatalError(
            "Expected indentation here",
            currentLocation
        );
    }

    if (indentationScheme == 0) {
        indentationScheme = blockIndentation;
    }

    if ((indentation + indentationScheme) != currentToken.virtualOffset) {
        ErrorReporter::reportFatalError(
            "Inconsistent indentation scheme",
            currentLocation
        );
    }

    while (1) {
        auto temp = parseStmt(blockIndentation);

        if (not temp) {
            break;
        }

        if (temp->kind == StmtKind::None) {
            break;
        }

        if (not linesChanged) {
            ErrorReporter::reportFatalError(
                formatAsString(
                    "Expected a newline here.. right before: ",
                    currentToken.value
                ),
                currentLocation
            );
        }

        list.push_back(temp);
    }

    if (list.size() == 0) {
        ErrorReporter::reportFatalError(
            "Expected at least one statement for the suite/block",
            currentLocation
        );
    }
}

StmtPtr Parser::parseIfStmt(uint64_t indentation) {
    auto stmt = std::make_shared<IfStmt>(currentLocation);

    fetchToken();

    while (1) {
        stmt->suites.push_back({});
        stmt->suites.back().first = parseExpr();
        parseSuite(stmt->suites.back().second, indentation);

        if (currentToken.virtualOffset != indentation) {
            break;
        }

        if (skipOptionalToken(TokenKind::KeywordElif)) {
            continue;
        }

        if (currentToken.virtualOffset != indentation) {
            break;
        }

        if (skipOptionalToken(TokenKind::KeywordElse)) {
            parseSuite(stmt->elseSuite, indentation);
        }

        break;
    }

    return stmt;
}

StmtPtr Parser::parseWhileStmt(uint64_t indentation) {
    auto stmt = std::make_shared<WhileStmt>(currentLocation);
    fetchToken();

    stmt->expr = parseExpr();

    parseSuite(stmt->suite, indentation);

    if ((not linesChanged) || (currentToken.virtualOffset != indentation)) {
        return stmt;
    }

    if (skipOptionalToken(TokenKind::KeywordElse)) {
        parseSuite(stmt->elseSuite, indentation);
    }

    return stmt;
}

StmtPtr Parser::parseForStmt(uint64_t indentation) {
    auto stmt = std::make_shared<ForStmt>(currentLocation);
    fetchToken();

    do {
        stmt->targetList.push_back(parseName());
    } while (skipOptionalToken(TokenKind::Comma));

    skipRequiredToken("in");

    do {
        stmt->exprList.push_back(parseExpr());
    } while (skipOptionalToken(TokenKind::Comma));

    parseSuite(stmt->suite, indentation);

    if ((not linesChanged) || (indentation != currentToken.virtualOffset)) {
        return stmt;
    }

    if (skipOptionalToken(TokenKind::KeywordElse)) {
        parseSuite(stmt->elseSuite, indentation);
    }

    return stmt;
}

StmtPtr Parser::parseTryStmt(uint64_t indentation) {
    auto stmt = std::make_shared<TryStmt>(currentLocation);
    fetchToken();

    parseSuite(stmt->suite, indentation);

    if ((not linesChanged) || (indentation != currentToken.virtualOffset)) {
        return stmt;
    }

    skipRequiredToken(TokenKind::KeywordExcept);

    while (1) {
        TryExcept except;

        if (not matchToken(TokenKind::Colon)) {
            except.expr = parseExpr();   

            if (not matchToken(TokenKind::Colon)) {
                skipRequiredToken(TokenKind::KeywordAs);
                except.alias = parseName();
                requireToken(TokenKind::Colon);
            }
        }

        parseSuite(except.suite, indentation);
        stmt->exceptList.push_back(std::move(except));

        if (
            (not linesChanged) 
            || (indentation != currentToken.virtualOffset)
        ) {
            break;
        }

        if (skipOptionalToken(TokenKind::KeywordExcept)) {
            continue;
        }

        if (skipOptionalToken(TokenKind::KeywordElse)) {
            parseSuite(stmt->elseSuite, indentation);
        }

        if (
            (not linesChanged) 
            || (indentation != currentToken.virtualOffset)
        ) {
            break;
        }

        if (skipOptionalToken(TokenKind::KeywordFinally)) {
            parseSuite(stmt->finallySuite, indentation);
        }

        break;
    }

    return stmt;
}

StmtPtr Parser::parseWithStmt(uint64_t indentation) {
    auto stmt = std::make_shared<WithStmt>(currentLocation);
    fetchToken();

    do {
        WithItem withItem;
        withItem.expr = parseExpr();

        if (skipOptionalToken(TokenKind::KeywordAs)) {
            withItem.alias = parseName();
        }

        stmt->items.push_back(std::move(withItem));
    } while (skipOptionalToken(TokenKind::Comma));

    parseSuite(stmt->suite, indentation);
    return stmt;
}

// decorators should have been parsed already, and will be attached later on
StmtPtr Parser::parseFuncdefStmt(uint64_t indentation) {
    auto stmt = std::make_shared<FuncdefStmt>(currentLocation);
    fetchToken();

    stmt->name = parseName();

    skipRequiredToken(TokenKind::OpeningRoundBracket);

    if (not skipOptionalToken(TokenKind::ClosingRoundBracket)) {
        parseParameterList(stmt->parameterList);
        skipRequiredToken(TokenKind::ClosingRoundBracket);
    }

    if (skipOptionalToken(TokenKind::SingleArrow)) {
        stmt->hint = parseExpr();
    }

    parseSuite(stmt->suite, indentation);
    return stmt;
}

StmtPtr Parser::parseClassdefStmt(uint64_t indentation) {
    auto stmt = std::make_shared<ClassdefStmt>(currentLocation);
    fetchToken();

    stmt->name = parseName();

    if (skipOptionalToken(TokenKind::OpeningRoundBracket)) {
        parseArgumentList(stmt->argumentList);
        skipRequiredToken(TokenKind::ClosingRoundBracket);
    }

    parseSuite(stmt->suite, indentation);
    return stmt;
}

void Parser::parseStmtList(StmtList& list) {
    while (1) {
        auto temp = parseStmt(0);
        
        if (not temp) {
            ErrorReporter::reportFatalError(
                formatAsString(
                    "Unexpected indentation before '",
                    toString(currentToken.kind),
                    "'"
                ),
                currentLocation
            );                
        }

        list.push_back(temp);

        if (matchToken(TokenKind::EndOfFile)) {
            break;
        }

        if (not linesChanged) {
            ErrorReporter::reportFatalError(
                "Expected a newline here",
                currentLocation
            );
        }
    }
}
