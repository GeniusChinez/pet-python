#pragma once

void PythonAstTransformer::transformStmt(StmtPtr stmt, int indent) {
    if (indent != 0) {
        addText(std::string(indent, ' '));
    }

    switch (stmt->kind) {
    case StmtKind::None: return;
    case StmtKind::Expression: {
        transformExpr(std::static_pointer_cast<ExprStmt>(stmt)->expr);
        break;
    }
    case StmtKind::Assert: {
        transformAssertStmt(
            *std::static_pointer_cast<AssertStmt>(stmt)
        );
        break;
    }
    case StmtKind::Assignment: {
        transformAssignmentStmt(
            *std::static_pointer_cast<AssignmentStmt>(stmt)
        );
        break;
    }
    case StmtKind::AugmentedAssignment: {
        transformAugmentedAssignmentStmt(
            *std::static_pointer_cast<AugmentedAssignmentStmt>(stmt)
        );
        break;
    }
    case StmtKind::AnnotatedAssignment: {
        transformAnnotatedAssignmentStmt(
            *std::static_pointer_cast<AnnotatedAssignmentStmt>(stmt)
        );
        break;
    }
    case StmtKind::Pass: {
        addText("pass");
        break;
    }
    case StmtKind::Del: {
        transformDelStmt(
            *std::static_pointer_cast<DelStmt>(stmt)
        );
        break;
    }
    case StmtKind::Return: {
        transformReturnStmt(
            *std::static_pointer_cast<ReturnStmt>(stmt)
        );
        break;
    }
    case StmtKind::Yield: {
        transformYieldStmt(
            *std::static_pointer_cast<YieldStmt>(stmt)
        );
        break;
    }
    case StmtKind::Raise: {
        transformRaiseStmt(
            *std::static_pointer_cast<RaiseStmt>(stmt)
        );
        break;
    }
    case StmtKind::Break: {
        addText("break");
        break;
    }
    case StmtKind::Continue: {
        addText("continue");
        break;
    }
    case StmtKind::Import: {
        transformImportStmt(
            *std::static_pointer_cast<ImportStmt>(stmt)
        );
        break;
    }
    case StmtKind::Global: {
        transformGlobalStmt(
            *std::static_pointer_cast<GlobalStmt>(stmt)
        );
        break;
    }
    case StmtKind::Nonlocal: {
        transformNonlocalStmt(
            *std::static_pointer_cast<NonlocalStmt>(stmt)
        );
        break;
    }
    case StmtKind::If: {
        transformIfStmt(
            *std::static_pointer_cast<IfStmt>(stmt),
            indent
        );
        return;
    }
    case StmtKind::While: {
        transformWhileStmt(
            *std::static_pointer_cast<WhileStmt>(stmt),
            indent
        );
        return;
    }
    case StmtKind::For: {
        transformForStmt(
            *std::static_pointer_cast<ForStmt>(stmt),
            indent
        );
        return;
    }
    case StmtKind::Try: {
        transformTryStmt(
            *std::static_pointer_cast<TryStmt>(stmt),
            indent
        );
        return;
    }
    case StmtKind::With: {
        transformWithStmt(
            *std::static_pointer_cast<WithStmt>(stmt),
            indent
        );
        return;
    }
    case StmtKind::Funcdef: {
        transformFuncdefStmt(
            *std::static_pointer_cast<FuncdefStmt>(stmt),
            indent
        );
        return;
    }
    case StmtKind::Classdef: {
        transformClassdefStmt(
            *std::static_pointer_cast<ClassdefStmt>(stmt),
            indent
        );
        return;
    }
    default:
        assert(0);
    }
    addNewLine();
}

void PythonAstTransformer::transformAssertStmt(const AssertStmt& stmt) {
    addText("assert ");
    transformExpr(stmt.expr1);

    if (stmt.expr2) {
        addText(", ");
        transformExpr(stmt.expr2);
    }
}

void PythonAstTransformer::transformAssignmentStmt(
    const AssignmentStmt& stmt
) {
    int i = 0;

    while (i < stmt.targetList.size()) {
        transformExpr(stmt.targetList[i]);
        if (i != stmt.targetList.size() - 1) {
            addText(", ");
        }
        i++;
    }

    addText(" = ");

    transformExpr(stmt.value);
}

void PythonAstTransformer::transformAugmentedAssignmentStmt(
    const AugmentedAssignmentStmt& stmt
) {
    transformExpr(stmt.autoTarget);

    addText(" ");
    addText(toString(stmt.augOp));
    addText(" ");

    int i = 0;

    while (i < stmt.values.size()) {
        transformExpr(stmt.values[i]);
        if (i != stmt.values.size() - 1) {
            addText(", ");
        }
        i++;
    }
}

void PythonAstTransformer::transformAnnotatedAssignmentStmt(
    const AnnotatedAssignmentStmt& stmt
) {
    transformExpr(stmt.autoTarget);
    addText(": ");
    transformExpr(stmt.annotation);
    addText(" = ");
    transformExpr(stmt.value);
}

void PythonAstTransformer::transformTarget(const TargetPtr target) {
    if (target->kind == TargetKind::Expr) {
        auto exprTarget = std::static_pointer_cast<ExprTarget>(target);
        for(int i = 0; i < exprTarget->stars; ++i) {
            addText("*");
        }
        transformExpr(exprTarget->expr);
    }
    else {
        auto newTarget = std::static_pointer_cast<BrackettedTarget>(target);
        if (newTarget->bracketKind == TokenKind::OpeningRoundBracket) {
            addText("(");
        }
        else {
            addText("[");
        }

        int i = 0;

        while (i < newTarget->targets.size()) {
            transformTarget(newTarget->targets[i]);
            if (i != newTarget->targets.size() - 1) {
                addText(", ");
            }
            i++;
        }

        if (newTarget->bracketKind == TokenKind::OpeningRoundBracket) {
            addText(")");
        }
        else {
            addText("]");
        }
    }
}

void PythonAstTransformer::transformDelStmt(const DelStmt& stmt) {
    addText("del ");
    int i = 0;

    while (i < stmt.targetList.size()) {
        transformTarget(stmt.targetList[i]);
        if (i != stmt.targetList.size() - 1) {
            addText(", ");
        }
        i++;
    }
}

void PythonAstTransformer::transformReturnStmt(const ReturnStmt& stmt) {
    addText("return ");

    if (stmt.exprList.size() == 0) {
        return;
    }

    int i = 0;

    while (i < stmt.exprList.size()) {
        transformExpr(stmt.exprList[i]);
        if (i != stmt.exprList.size() - 1) {
            addText(", ");
        }
        i++;
    }
}

void PythonAstTransformer::transformYieldStmt(const YieldStmt& stmt) {
    transformExpr(stmt.expr);
}

void PythonAstTransformer::transformRaiseStmt(const RaiseStmt& stmt) {
    addText("raise ");

    if (not stmt.expr) {
        return;
    }

    transformExpr(stmt.expr);

    if (stmt.fromExpr) {
        addText(" from ");
        transformExpr(stmt.fromExpr);
    }
}

void PythonAstTransformer::transformImportItem(const ImportItem& item) {
    int i = 0;

    while (i < item.parts.size()) {
        addText(item.parts[i]);
        
        if (item.parts[i] == ".") {
            i++;
            continue;
        }
        
        if (i != item.parts.size() - 1) {
            addText(".");
        }

        i++;
    }

    if (item.alias.size() != 0) {
        addText(" as ");
        addText(item.alias);
    }
}

void PythonAstTransformer::transformImportStmt(const ImportStmt& stmt) {
    if (stmt.source.size() > 0) {
        addText("from ");

        int i = 0;

        while (i < stmt.source.size()) {
            addText(stmt.source[i]);
            if (i != stmt.source.size() - 1) {
                addText(".");
            }
            i++;
        }

        addText(" import ");

        i = 0;

        while (i < stmt.items.size()) {
            transformImportItem(stmt.items[i]);
            if (i != stmt.items.size() - 1) {
                addText(", ");
            }
            i++;
        }
    }
    else {
        addText("import ");

        int i = 0;

        while (i < stmt.items.size()) {
            transformImportItem(stmt.items[i]);
            if (i != stmt.items.size() - 1) {
                addText(", ");
            }
            i++;
        }
    }
}

void PythonAstTransformer::transformGlobalStmt(const GlobalStmt& stmt) {
    addText("global ");

    int i = 0;

    while (i < stmt.names.size()) {
        addText(stmt.names[i]);
        if (i != stmt.names.size() - 1) {
            addText(", ");
        }
        i++;
    }
}

void PythonAstTransformer::transformNonlocalStmt(const NonlocalStmt& stmt) {
    addText("nonlocal ");

    int i = 0;

    while (i < stmt.names.size()) {
        addText(stmt.names[i]);
        if (i != stmt.names.size() - 1) {
            addText(", ");
        }
        i++;
    }
}

void PythonAstTransformer::transformIfStmt(
    const IfStmt& stmt, 
    int indent
) {
    addText("if ");
    transformExpr(stmt.suites.front().first);
    transformSuite(stmt.suites.front().second, indent);

    auto it = std::begin(stmt.suites) + 1;
    while (it != std::end(stmt.suites)) {
        addIndent(indent);
        addText("elif ");
        transformExpr(it->first);
        transformSuite(it->second, indent);                
        it++;
    }

    if (stmt.elseSuite.stmts.size() > 0) {
        addIndent(indent);
        addText("else");
        transformSuite(stmt.elseSuite, indent);
    }
}

void PythonAstTransformer::transformWhileStmt(
    const WhileStmt& stmt, 
    int indent
) {
    addText("while ");
    transformExpr(stmt.expr);
    transformSuite(stmt.suite, indent);

    if (stmt.elseSuite.stmts.size() > 0) {
        addIndent(indent);
        addText("else");
        transformSuite(stmt.elseSuite, indent);
    }
}

void PythonAstTransformer::transformForStmt(
    const ForStmt& stmt, 
    int indent
) {
    if (stmt.isAsync) {
        addText("async ");
        addNewLine(indent);
    }

    addText("for ");

    int i = 0;

    while (i < stmt.targetList.size()) {
        addText(stmt.targetList[i]);
        if (i != stmt.targetList.size() - 1) {
            addText(", ");
        }
        i++;
    }

    addText(" in ");

    i = 0;

    while (i < stmt.exprList.size()) {
        transformExpr(stmt.exprList[i]);
        if (i != stmt.exprList.size() - 1) {
            addText(", ");
        }
        i++;
    }

    transformSuite(stmt.suite, indent);

    if (stmt.elseSuite.stmts.size() > 0) {
        addIndent(indent);
        addText("else");
        transformSuite(stmt.elseSuite, indent);
    }
}

void PythonAstTransformer::transformExcept(
    const TryExcept& except, 
    int indent
) {
    addIndent(indent);
    addText("except");

    if (except.expr) {
        addText(" ");
        transformExpr(except.expr);

        if (except.alias.size() > 0) {
            addText(" as ");
            addText(except.alias);
        }
    }

    transformSuite(except.suite, indent);
}

void PythonAstTransformer::transformTryStmt(
    const TryStmt& stmt, 
    int indent
) {
    addText("try");
    transformSuite(stmt.suite, indent);

    for (auto& e : stmt.exceptList) {
        transformExcept(e, indent);
    }

    if (stmt.elseSuite.stmts.size() > 0) {
        addIndent(indent);
        addText("else");
        transformSuite(stmt.elseSuite, indent);
    }

    if (stmt.finallySuite.stmts.size() > 0) {
        addIndent(indent);
        addText("finally");
        transformSuite(stmt.finallySuite, indent);
    }
}

void PythonAstTransformer::transformWithItem(const WithItem& item) {
    transformExpr(item.expr);

    if (item.alias.size() > 0) {
        addText(" as ");
        addText(item.alias);
    }
}

void PythonAstTransformer::transformWithStmt(
    const WithStmt& stmt, 
    int indent
) {
    if (stmt.isAsync) {
        addText("aync");
    }

    addText("with ");

    int i = 0;

    while (i < stmt.items.size()) {
        transformWithItem(stmt.items[i]);
        if (i != stmt.items.size() - 1) {
            addText(", ");
        }
        i++;
    }

    transformSuite(stmt.suite, indent);
}

void PythonAstTransformer::transformDecorator(const Decorator& decorator) {
    addText("@");
    int i = 0;

    while (i < decorator.dottedName.size()) {
        addText(decorator.dottedName[i]);
        if (i != decorator.dottedName.size() - 1) {
            addText(".");
        }
        i++;
    }

    if (decorator.argumentList.size() == 0) {
        return;
    }

    addText("(");

    i = 0;

    while (i < decorator.argumentList.size()) {
        transformArgument(decorator.argumentList[i]);
        if (i != decorator.argumentList.size() - 1) {
            addText(", ");
        }
        i++;
    }

    addText(")");
}

void PythonAstTransformer::transformParameter(const Parameter& parameter) {
    for(int i = 0; i < parameter.stars; ++i) {
        addText("*");
    }
    
    addText(parameter.name);
    
    if (parameter.hint) {
        addText(": ");
        transformExpr(parameter.hint);
    }

    if (parameter.value) {
        addText("=");
        transformExpr(parameter.value);
    }
}

void PythonAstTransformer::transformFuncdefStmt(
    const FuncdefStmt& stmt, 
    int indent
) {
    for (auto& decorator : stmt.decorators) {
        transformDecorator(decorator);
        addNewLine(indent);
    }

    if (stmt.isAsync) {
        addText("async ");
    }

    addText("def ");
    addText(stmt.name + "(");

    int i = 0;

    while (i < stmt.parameterList.size()) {
        transformParameter(stmt.parameterList[i]);
        if (i != stmt.parameterList.size() - 1) {
            addText(", ");
        }
        i++;
    }

    addText(")");

    if (stmt.hint) {
        addText(" -> ");
        transformExpr(stmt.hint);
    }

    transformSuite(stmt.suite, indent);
}

void PythonAstTransformer::transformClassdefStmt(
    const ClassdefStmt& stmt, 
    int indent
) {
    for (auto& decorator : stmt.decorators) {
        transformDecorator(decorator);
        addNewLine(indent);
    }

    addText("class ");
    addText(stmt.name + "(");

    if (stmt.argumentList.size() == 0) {
        addText("object");
    }
    else {
        int i = 0;

        while (i < stmt.argumentList.size()) {
            transformArgument(stmt.argumentList[i]);
            if (i != stmt.argumentList.size() - 1) {
                addText(", ");
            }
            i++;
        }
    }

    addText(")");
    transformSuite(stmt.suite, indent);
}

void PythonAstTransformer::transformSuite(const Suite& suite, int indent) {
    addText(":");
    addNewLine();

    if (suite.stmts.size() > 0) {
        int i = 0;

        while (i < suite.stmts.size()) {
            transformStmt(suite.stmts[i], indent + 4);

            if (i != suite.stmts.size() - 1) {
                switch (suite.stmts[i]->kind) {
                case StmtKind::If:
                case StmtKind::While:
                case StmtKind::For:
                case StmtKind::Try:
                case StmtKind::With:
                case StmtKind::Funcdef:
                case StmtKind::Classdef: {
                    addNewLine();
                    break;
                }
                default:
                    assert(1);
                }
            }
            i++;
        }
    }
    else {
        Location location;
        auto tempStmt = std::make_shared<Stmt>(location, StmtKind::Pass);
        transformStmt(tempStmt, indent + 4);
    }
}
