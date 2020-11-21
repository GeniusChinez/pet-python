#pragma once

// @todo: inline all functions that arent recursive

ExprPtr Parser::parseYieldExpr() {
    auto expr = std::make_shared<YieldExpr>(currentLocation);
    fetchToken();

    if (linesChanged) {
        return expr;
    }

    if (skipOptionalToken(TokenKind::KeywordFrom)) {
        expr->fromExpr = parseExpr();
    }
    else {
        do {
            expr->exprList.push_back(parseExpr());
        } while (skipOptionalToken(TokenKind::Comma));
    }

    return expr;
}

ExprPtr Parser::parseTopExpr() {
    switch (currentToken.kind) {
    case TokenKind::Identifier: {
        auto expr = std::make_shared<NameExpr>(currentLocation);
        expr->value = currentToken.value;
        fetchToken();
        return expr;
    }
    case TokenKind::KeywordNone: {
        auto expr = std::make_shared<Expr>(currentLocation, ExprKind::None);
        fetchToken();
        return expr;
    }
    case TokenKind::ConstantMultilineString: {
        auto expr = std::make_shared<StringLiteralExpr>(currentLocation);
        expr->value = "";

        do {
            expr->value += currentToken.value.substr(3, currentToken.value.size() - 6);
            fetchToken();
        } while (matchToken(TokenKind::ConstantString) || matchToken(TokenKind::ConstantMultilineString));

        return expr;
    }
    case TokenKind::ConstantString: {
        auto expr = std::make_shared<StringLiteralExpr>(currentLocation);
        expr->value = "";

        do {
            expr->value += currentToken.value.substr(1, currentToken.value.size() - 2);
            fetchToken();
        } while (matchToken(TokenKind::ConstantString) || matchToken(TokenKind::ConstantMultilineString));

        return expr;
    }
    case TokenKind::ConstantBooleanTrue: {
        auto expr = std::make_shared<BooleanLiteralExpr>(currentLocation);
        expr->value = true;
        fetchToken();
        return expr;
    }
    case TokenKind::ConstantBooleanFalse: {
        auto expr = std::make_shared<BooleanLiteralExpr>(currentLocation);
        expr->value = false;
        fetchToken();
        return expr;
    }
    case TokenKind::ConstantInteger: {
        auto expr = std::make_shared<IntegerLiteralExpr>(currentLocation);
        try {
            expr->value = std::stoll(currentToken.value);
        }
        catch (...) {
            ErrorReporter::reportFatalError(
                "invalid integer literal",
                currentLocation
            );
        }
        fetchToken();
        return expr;
    }
    case TokenKind::ConstantFloat: {
        auto expr = std::make_shared<FloatLiteralExpr>(currentLocation);
        try {
            expr->value = std::stold(currentToken.value);
        }
        catch (...) {
            ErrorReporter::reportFatalError(
                "invalid float literal",
                currentLocation
            );
        }
        fetchToken();
        return expr;
    }
    case TokenKind::ConstantImaginary: {
        assert(0);
    }
    case TokenKind::KeywordYield: {
        return parseYieldExpr();
    }
    default:
        ErrorReporter::reportFatalError(
            formatAsString(
                "expected an expression here, but got: ",
                currentToken.value
            ),
            currentLocation
        );
    }
    assert(0);
    return nullptr;
}

ExprPtr Parser::parseListDisplayExpr() {
    auto expr = std::make_shared<ListDisplayExpr>(currentLocation);
    fetchToken(); // skip "["

    if (skipOptionalToken(TokenKind::ClosingSquareBracket)) {
        return expr;
    }
    auto first = parseExpr();

    if (matchToken("for") || matchToken("async")) {
        auto comprehension = std::make_shared<Comprehension>();
        comprehension->expr = first;
        comprehension->compFor = parseComprehensionFor();

        expr->comprehension = comprehension;
        skipRequiredToken(TokenKind::ClosingSquareBracket);
    }
    else {
        expr->starredList.push_back(first);

        if (not skipOptionalToken(TokenKind::Comma)) {
            skipRequiredToken(TokenKind::ClosingSquareBracket);
            return expr;
        }

        while (not skipOptionalToken(TokenKind::ClosingSquareBracket)) {
            expr->starredList.push_back(parseExpr());
            if (not skipOptionalToken(TokenKind::Comma)) {
                skipRequiredToken(TokenKind::ClosingSquareBracket);
                break;
            }
        }
    }

    return expr;
}

ExprPtr Parser::parseParenthesizedExpr() {
    Location location = currentLocation;

    fetchToken();
    ExprList list;

    bool danglingComma = false;

    while (not skipOptionalToken(TokenKind::ClosingRoundBracket)) {
        danglingComma = false;
        list.push_back(parseExpr());

        if (not skipOptionalToken(TokenKind::Comma)) {
            if (list.size() != 1) {
                skipRequiredToken(TokenKind::ClosingRoundBracket);
                break;
            }

            if (not matchToken(TokenKind::KeywordFor)) {
                skipRequiredToken(TokenKind::ClosingRoundBracket);
                break;
            }

            auto expr = std::make_shared<GeneratorExpr>(currentLocation);
            expr->expr = list.front();
            expr->compFor = parseComprehensionFor();

            skipRequiredToken(TokenKind::ClosingRoundBracket);
            return expr;
        }

        danglingComma = true;
    }

    if (danglingComma) {
        auto noneExpr = std::make_shared<Expr>(
            currentLocation, 
            ExprKind::None
        );

        list.push_back(noneExpr);
    }

    if (list.size() == 0) {
        ErrorReporter::reportFatalError(
            "empty paretheses not allowed",
            location
        );
    }
    else if (list.size() > 1) {
        // tuple
        auto expr = std::make_shared<TupleDisplayExpr>(location);
        expr->items = std::move(list);
        return expr;
    }
    else {
        return list.front();
    }

    return nullptr;
}

void Parser::parseSetDisplayExpr(SetDisplayExpr& expr) {
    while (not skipOptionalToken(TokenKind::ClosingCurlyBracket)) {
        expr.items.push_back(parseExpr());
        if (not skipOptionalToken(TokenKind::Comma)) {
            skipRequiredToken(TokenKind::ClosingCurlyBracket);
            break;
        }
    }
}

void Parser::parseDictDisplayExpr(DictDisplayExpr& expr) {
    while (not skipOptionalToken(TokenKind::ClosingCurlyBracket)) {
        DictItem item;
        item.expr1 = parseExpr();

        skipRequiredToken(TokenKind::Colon);
        item.expr2 = parseExpr();

        expr.itemList.push_back(std::move(item));

        if (not skipOptionalToken(TokenKind::Comma)) {
            skipRequiredToken(TokenKind::ClosingCurlyBracket);
            break;
        }
    }
}

ExprPtr Parser::parseCurlyBracketDisplayExpr() {
    Location location = currentLocation;
    fetchToken();

    if (skipOptionalToken(TokenKind::ClosingCurlyBracket)) {
        return std::make_shared<SetDisplayExpr>(std::move(location));
    }

    auto temp = parseExpr();

    if (matchToken("async") || matchToken("for")) {
        auto expr = std::make_shared<SetDisplayExpr>(std::move(location));
        
        auto comprehension = std::make_shared<Comprehension>();
        comprehension->expr = temp;
        comprehension->compFor = parseComprehensionFor();

        expr->comprehension = comprehension;
        skipRequiredToken(TokenKind::ClosingCurlyBracket);            
        return expr;
    }

    if (skipOptionalToken(TokenKind::Colon)) {
        // this is a DictDisplay
        DictItem firstItem;

        firstItem.expr1 = temp;
        firstItem.expr2 = parseExpr();

        if (matchToken("async") || matchToken("for")) {
            firstItem.compFor = parseComprehensionFor();
            skipRequiredToken(TokenKind::ClosingCurlyBracket);

            auto expr = std::make_shared<DictDisplayExpr>(
                std::move(location)
            );

            expr->itemList.push_back(std::move(firstItem));
            return expr;
        }

        auto expr = std::make_shared<DictDisplayExpr>(std::move(location));
        expr->itemList.push_back(std::move(firstItem));

        if (skipOptionalToken(TokenKind::Comma)) {
            parseDictDisplayExpr(*expr);
        }
        else {
            skipRequiredToken(TokenKind::ClosingCurlyBracket);
        }

        return expr;
    }
    else if (skipOptionalToken(TokenKind::Comma)) {
        // this is a set display
        auto expr = std::make_shared<SetDisplayExpr>(std::move(location));

        if (matchToken("async") || matchToken("for")) {
            auto comprehension = std::make_shared<Comprehension>();
            comprehension->expr = temp;
            comprehension->compFor = parseComprehensionFor();

            expr->comprehension = comprehension;
            skipRequiredToken(TokenKind::ClosingCurlyBracket);
            return expr;
        }

        expr->items.push_back(temp);
        parseSetDisplayExpr(*expr);

        return expr;
    }
    else if (skipOptionalToken(TokenKind::ClosingCurlyBracket)) {
        auto expr = std::make_shared<SetDisplayExpr>(std::move(location));
        expr->items.push_back(temp);
        return expr;
    }
    else {
        ErrorReporter::reportFatalError(
            formatAsString(
                "expected either ':', or '}', but found: ",
                currentToken.value
            ),
            currentLocation
        );
    }

    //...
    assert(0);
    return nullptr;
}

ExprPtr Parser::parseDisplayExpr() {
    if (matchToken(TokenKind::OpeningSquareBracket)) {
        return parseListDisplayExpr();
    }
    else if (matchToken(TokenKind::OpeningRoundBracket)) {
        return parseParenthesizedExpr();
    }
    else if (matchToken(TokenKind::OpeningCurlyBracket)) {
        return parseCurlyBracketDisplayExpr();
    }
    else {
        return parseTopExpr();
    }
}

TargetPtr Parser::parseTarget() {
    if (skipOptionalToken(TokenKind::OpeningRoundBracket)) {
        auto target = std::make_shared<BrackettedTarget>();
        target->bracketKind = TokenKind::OpeningRoundBracket;

        do {
            target->targets.push_back(parseTarget());
            if (not skipOptionalToken(TokenKind::Comma)) {
                skipRequiredToken(TokenKind::ClosingRoundBracket);
                break;
            }
        } while (not skipOptionalToken(TokenKind::ClosingRoundBracket));

        return target;
    }
    else if (skipOptionalToken(TokenKind::ClosingSquareBracket)) {
        auto target = std::make_shared<BrackettedTarget>();
        target->bracketKind = TokenKind::OpeningSquareBracket;

        do {
            target->targets.push_back(parseTarget());
            if (not skipOptionalToken(TokenKind::Comma)) {
                skipRequiredToken(TokenKind::ClosingSquareBracket);
                break;
            }
        } while (not skipOptionalToken(TokenKind::ClosingSquareBracket));

        return target;
    }
    
    std::shared_ptr<ExprTarget> target;
    Location targetLocation = currentLocation;

    if (skipOptionalToken(TokenKind::ArithmeticMul)) {
        target = std::make_shared<ExprTarget>();
        target->expr = parseSliceCallAttrSubsExpr();
        target->stars = 1;
    }
    else {
        target = std::make_shared<ExprTarget>();
        target->expr = parseSliceCallAttrSubsExpr();
        target->stars = 0;
    }

    switch (target->expr->kind) {
    case ExprKind::Name:
    case ExprKind::AttributeRef:
    case ExprKind::Subscription:
    case ExprKind::Slicing: {
        break;
    }
    default:
        ErrorReporter::reportFatalError(
            "invalid target",
            targetLocation
        );
    }

    return target;
}

CompIterPtr Parser::parseComprehensionIter() {
    auto compIter = std::make_shared<CompIter>();
    compIter->compFor = parseComprehensionFor();

    if (matchToken("async") || matchToken("for")) {
        compIter->compFor = parseComprehensionFor();
    }
    else if (matchToken("if")) {
        compIter->compIf = parseComprehensionIf();
    }

    return compIter;
}

CompForPtr Parser::parseComprehensionFor() {
    auto compFor = std::make_shared<CompFor>();

    if (skipOptionalToken("async")) {
        compFor->isAsync = true;
    }

    assert(matchToken("for"));
    fetchToken(); // skip "for"

    do {
        compFor->targetList.push_back(parseTarget());
    } while (skipOptionalToken(TokenKind::Comma));

    skipRequiredToken("in");

    compFor->test = parseBooleanOrExpr();

    if (linesChanged) {
        return compFor;
    }

    if (matchToken("async") || matchToken("for")) {
        compFor->compIter = parseComprehensionIter();
    }
    else if (matchToken("if")) {
        auto compIter = std::make_shared<CompIter>();
        compIter->compIf = parseComprehensionIf();
    }

    return compFor;
}

CompIfPtr Parser::parseComprehensionIf() {
    auto compIf = std::make_shared<CompIf>();
    fetchToken(); // skip "if"

    compIf->exprNoCond = parseBooleanOrExpr();

    if (linesChanged) {
        return compIf;
    }

    if (matchToken("async") || matchToken("for")) {
        compIf->compIter = parseComprehensionIter();
    }

    return compIf;
}

ComprehensionPtr Parser::parseComprehension() {
    auto comprehension = std::make_shared<Comprehension>();
    comprehension->expr = parseExpr();
    comprehension->compFor = parseComprehensionFor();
    return comprehension;
}

ExprPtr Parser::parseCallExpr(ExprPtr left) {
    auto expr = std::make_shared<CallExpr>(left->location);
    expr->primary = left;

    // @note: This doesn't take into account the ordering of the 
    // ... the arguments as per requirements in the specification. 
    // ... This problem should be fixed soon.
    while (
        not skipOptionalToken(TokenKind::ClosingRoundBracket)
    ) {
        Argument argument;
        if (skipOptionalToken(TokenKind::ArithmeticPow)) {
            argument.stars = 2;
            argument.value = parseExpr();
            expr->argumentList.push_back(std::move(argument));
        }
        else if (
            skipOptionalToken(TokenKind::ArithmeticMul)
        ) {
            argument.stars = 1;
            argument.value = parseExpr();
            expr->argumentList.push_back(std::move(argument));
        }
        else {
            auto tempArgument = parseExpr();
            if (
                (expr->argumentList.size() == 0) 
                && (
                    (matchToken(TokenKind::KeywordFor)) 
                    || matchToken("async")
                )
            ) {
                expr->comprehension = std::make_shared<Comprehension>();
                expr->comprehension->expr = tempArgument;
                expr->comprehension->compFor = parseComprehensionFor();
                skipRequiredToken(TokenKind::ClosingRoundBracket);
                break;
            }
            else if (matchToken("=")) {
                if (tempArgument->kind != ExprKind::Name) {
                    ErrorReporter::reportFatalError(
                        "unexpected '=' after non-name expression",
                        currentLocation
                    );
                }

                if (tempArgument->await) {
                    ErrorReporter::reportFatalError(
                        "unexpected '=' after non-name expression",
                        currentLocation
                    );
                }

                argument.name = std::static_pointer_cast<NameExpr>(
                    tempArgument
                )->value;

                fetchToken();// skip "="

                argument.value = parseExpr();
                expr->argumentList.push_back(std::move(argument));
            }
            else {
                argument.value = tempArgument;
                expr->argumentList.push_back(std::move(argument));
            }
        }

        if (not skipOptionalToken(TokenKind::Comma)) {
            skipRequiredToken(TokenKind::ClosingRoundBracket);
            break;
        }
    }

    return expr;
}

void Parser::parseSliceStride(SlicingExpr& slice) {
    if (matchToken(TokenKind::ClosingSquareBracket)) {
        return;
    }
    slice.stride = parseExpr();
}

void Parser::parseSliceUpperbound(SlicingExpr& slice) {
    if (skipOptionalToken(TokenKind::Colon)) {
        parseSliceStride(slice);
        skipRequiredToken(TokenKind::ClosingSquareBracket);
        return;
    }

    if (skipOptionalToken(TokenKind::ClosingSquareBracket)) {
        return;
    }

    slice.upperBound = parseExpr();

    if (skipOptionalToken(TokenKind::Colon)) {
        parseSliceStride(slice);
    }

    skipRequiredToken(TokenKind::ClosingSquareBracket);
}

ExprPtr Parser::parseSliceOrSubscription(ExprPtr left) {
    if (skipOptionalToken(TokenKind::Colon)) {
        // it's a slice
        auto expr = std::make_shared<SlicingExpr>(
            left->location
        );

        expr->primary = left;
        expr->lowerBound = nullptr;

        parseSliceUpperbound(*expr);
        return expr;
    }
    else {
        auto firstExpr = parseExpr();
        if (skipOptionalToken(TokenKind::Colon)) {
            // slice
            auto expr = std::make_shared<SlicingExpr>(left->location);

            expr->primary = left;
            expr->lowerBound = firstExpr;

            if (not skipOptionalToken(TokenKind::ClosingSquareBracket)) {
                parseSliceUpperbound(*expr);
            }

            return expr;
        }
        else {
            // subcription
            auto expr = std::make_shared<SubscriptionExpr>(
                left->location
            );

            expr->primary = left;
            expr->exprList.push_back(firstExpr);

            while (
                not skipOptionalToken(TokenKind::ClosingSquareBracket)
            ) {
                expr->exprList.push_back(parseExpr());
                if (not skipOptionalToken(TokenKind::Comma)) {
                    skipRequiredToken(TokenKind::ClosingSquareBracket);
                    break;
                }
            }

            return expr;
        }
    }
}

// @todo: clean this thing up.. it's pathetically disgusting
ExprPtr Parser::parseSliceCallAttrSubsExpr() {
    auto expr = parseDisplayExpr();
    while (1) {
        switch (currentToken.kind) {
        case TokenKind::Access: {
            auto temp = std::make_shared<AttributeRefExpr>(expr->location);
            temp->primary = expr;

            fetchToken();

            requireToken(TokenKind::Identifier);
            temp->name = currentToken.value;
            fetchToken();

            expr = temp;
            continue;
        }
        case TokenKind::OpeningRoundBracket: {
            fetchToken();
            expr = parseCallExpr(expr);
            continue;
        }
        case TokenKind::OpeningSquareBracket: {
            fetchToken();
            expr = parseSliceOrSubscription(expr);
            break;
        }
        default:
            return expr;
        }
    }
    return expr;
}

ExprPtr Parser::parseAwaitExpr() {
        return parseSliceCallAttrSubsExpr();
    if (not matchToken(TokenKind::KeywordAwait)) {
    }

    fetchToken();

    auto expr = parseSliceCallAttrSubsExpr();
    expr->await = true;

    return expr;
}

ExprPtr Parser::parseExponentiationExpr() {
    auto expr = parseAwaitExpr();
    while (matchToken(TokenKind::ArithmeticPow)) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseAwaitExpr();
        expr = temp;
    }
    return expr;
}

ExprPtr Parser::parseUnaryExpr() {
    switch (currentToken.kind) {
    case TokenKind::LogicalNot:
    case TokenKind::ArithmeticAdd:
    case TokenKind::ArithmeticSub: {
        auto expr = std::make_shared<UnaryExpr>(currentLocation);
        expr->op = currentToken.kind;

        fetchToken();

        expr->expr = parseExponentiationExpr();
        return expr;
    }
    default:
        return parseExponentiationExpr();
    }
}

ExprPtr Parser::parseMultiplicativeExpr() {
    auto expr = parseUnaryExpr();
    while (
        matchToken(TokenKind::ArithmeticMul)
        || matchToken(TokenKind::ArithmeticDiv)
        || matchToken(TokenKind::ArithmeticFloorDiv)
        || matchToken(TokenKind::ArithmeticMod)
        || ((not linesChanged) && matchToken(TokenKind::At)) // matrix multiplication
    ) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseUnaryExpr();
        expr = temp;
    }
    return expr;
}

ExprPtr Parser::parseAdditiveExpr() {
    auto expr = parseMultiplicativeExpr();
    while (
        matchToken(TokenKind::ArithmeticAdd)
        || matchToken(TokenKind::ArithmeticSub)
    ) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseMultiplicativeExpr();
        expr = temp;
    }
    return expr;
}

ExprPtr Parser::parseShiftExpr() {
    auto expr = parseAdditiveExpr();
    while (
        matchToken(TokenKind::LogicalLeftShift)
        || matchToken(TokenKind::LogicalRightShift)
    ) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseAdditiveExpr();
        expr = temp;
    }
    return expr;
}

ExprPtr Parser::parseBitwiseAndExpr() {
    auto expr = parseShiftExpr();
    while (matchToken(TokenKind::LogicalAnd)) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseShiftExpr();
        expr = temp;
    }
    return expr;
}

ExprPtr Parser::parseBitwiseXorExpr() {
    auto expr = parseBitwiseAndExpr();
    while (matchToken(TokenKind::LogicalXor)) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseBitwiseAndExpr();
        expr = temp;
    }
    return expr;
}

ExprPtr Parser::parseBitwiseOrExpr() {
    auto expr = parseBitwiseXorExpr();
    while (matchToken(TokenKind::LogicalOr)) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseBitwiseXorExpr();
        expr = temp;
    }
    return expr;
}

// @note @todo Make the lexer recognize tokens like "is not"
ExprPtr Parser::parseComparisonExpr() {
    auto expr = parseBitwiseOrExpr();

    while (1) {
        switch (currentToken.kind) {
        case TokenKind::RelationalEquals:
        case TokenKind::RelationalNotEqual:
        case TokenKind::RelationalIdentical:
        case TokenKind::RelationalNotIdentical:
        case TokenKind::RelationalGreaterThan:
        case TokenKind::RelationalGreaterThanOrEquals:
        case TokenKind::RelationalLesserThan:
        case TokenKind::RelationalLesserThanOrEquals:
        case TokenKind::RelationalIsContainedIn:
        case TokenKind::RelationalIsNotContainedIn: {
            auto temp = std::make_shared<BinaryExpr>(expr->location);

            temp->lhs = expr;
            temp->op = currentToken.kind;

            fetchToken();

            temp->rhs = parseBitwiseOrExpr();
            expr = temp;

            continue;
        }
        default:
            return expr;
        }
    }

    return expr;
}

ExprPtr Parser::parseBooleanNotExpr() {
    if (not matchToken(TokenKind::ConditionalNot)) {
        return parseComparisonExpr();
    }

    auto expr = std::make_shared<UnaryExpr>(currentLocation);
    expr->op = currentToken.kind;

    fetchToken();

    expr->expr = parseComparisonExpr();
    return expr;
}

ExprPtr Parser::parseBooleanAndExpr() {
    auto expr = parseBooleanNotExpr();
    while (matchToken(TokenKind::ConditionalAnd)) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseBooleanNotExpr();
        expr = temp;
    }
    return expr;
}

ExprPtr Parser::parseBooleanOrExpr() {
    auto expr = parseBooleanAndExpr();
    while (matchToken(TokenKind::ConditionalOr)) {
        auto temp = std::make_shared<BinaryExpr>(expr->location);
        temp->lhs = expr;
        temp->op = currentToken.kind;

        fetchToken();

        temp->rhs = parseBooleanAndExpr();
        expr = temp;
    }
    return expr;
}

ExprPtr Parser::parseLambdaExpr() {
    if (not matchToken(TokenKind::KeywordLambda)) {
        auto expr = parseBooleanOrExpr();

        if ((not linesChanged) && skipOptionalToken(TokenKind::KeywordIf)) {
            auto temp = std::make_shared<IfExpr>(expr->location);
            temp->cond = parseBooleanOrExpr();
            temp->thenValue = expr;

            skipRequiredToken(TokenKind::KeywordElse);

            temp->elseValue = parseBooleanOrExpr();
            expr = temp;
        }

        return expr;
    }

    auto expr = std::make_shared<LambdaExpr>(currentLocation);
    fetchToken();

    if (not skipOptionalToken(TokenKind::Colon)) {
        do {
            Parameter parameter;

            if (skipOptionalToken(TokenKind::ArithmeticMul)) {
                parameter.stars++;
            }
            else if (skipOptionalToken(TokenKind::ArithmeticPow)) {
                parameter.stars += 2;
            }

            parameter.name = parseName();

            if (skipOptionalToken(TokenKind::Assignment)) {
                parameter.value = parseExpr();
            }

            expr->parameterList.push_back(std::move(parameter));
        } while (skipOptionalToken(TokenKind::Comma));
        
        skipRequiredToken(TokenKind::Colon);
    }

    expr->expr = parseExpr();
    return expr;
}

ExprPtr Parser::parseExpr() {
    return parseLambdaExpr();
}
