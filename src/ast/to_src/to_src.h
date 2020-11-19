#pragma once
#include <regex>

class PythonAstTransformer {
public:
    void appendStmt(StmtPtr stmt) {
        switch (stmt->kind) {
        case StmtKind::If:
        case StmtKind::While:
        case StmtKind::For:
        case StmtKind::Try:
        case StmtKind::With:
        case StmtKind::Funcdef:
        case StmtKind::Classdef: {
            addNewLine();
            transformStmt(stmt, 0);
            nextGlobalStmtShouldBeOnANewLine = true;
            break;
        }
        default:
            if (nextGlobalStmtShouldBeOnANewLine) {
                addNewLine();
                nextGlobalStmtShouldBeOnANewLine = false;
            }
            transformStmt(stmt, 0);
        }
    }

    const std::string& getBuffer() {
        return buffer;
    }

    void clearBuffer() {
        buffer.clear();
    }

private:
    void transformArgument(const Argument&);
    void transformParameter(const Parameter&);
    void transformWithItem(const WithItem&);
    void transformExcept(const TryExcept&, int);
    void transformComprehension(const Comprehension&);
    void transformComprehensionFor(const CompFor&);
    void transformComprehensionIf(const CompIf&);
    void transformComprehensionIter(const CompIter&);
    void transformDictItem(const DictItem&);

    void transformExpr(ExprPtr); //remember "await"
    void transformNameExpr(const NameExpr&);
    void transformStringLiteralExpr(const StringLiteralExpr&);
    void transformIntegerLiteralExpr(const IntegerLiteralExpr&);
    void transformBooleanLiteralExpr(const BooleanLiteralExpr&);
    void transformFloatLiteralExpr(const FloatLiteralExpr&);
    void transformListDisplayExpr(const ListDisplayExpr&);
    void transformTupleDisplayExpr(const TupleDisplayExpr&);
    void transformDictDisplayExpr(const DictDisplayExpr&);
    void transformSetDisplayExpr(const SetDisplayExpr&);
    void transformGeneratorExpr(const GeneratorExpr&);
    void transformYieldExpr(const YieldExpr&);
    void transformAttributeRefExpr(const AttributeRefExpr&);
    void transformSubscriptionExpr(const SubscriptionExpr&);
    void transformSlicingExpr(const SlicingExpr&);
    void transformCallExpr(const CallExpr&);
    void transformUnaryExpr(const UnaryExpr&);
    void transformBinaryExpr(const BinaryExpr&);
    void transformLambdaExpr(const LambdaExpr&);

    void transformTarget(const TargetPtr);
    void transformImportItem(const ImportItem&);

    void transformStmt(StmtPtr, int);
    void transformAssertStmt(const AssertStmt&);
    void transformAssignmentStmt(const AssignmentStmt&);
    void transformAugmentedAssignmentStmt(const AugmentedAssignmentStmt&);
    void transformAnnotatedAssignmentStmt(const AnnotatedAssignmentStmt&);
    void transformDelStmt(const DelStmt&);
    void transformReturnStmt(const ReturnStmt&);
    void transformYieldStmt(const YieldStmt&);
    void transformRaiseStmt(const RaiseStmt&);
    void transformImportStmt(const ImportStmt&);
    void transformGlobalStmt(const GlobalStmt&);
    void transformNonlocalStmt(const NonlocalStmt&);
    void transformIfStmt(const IfStmt&, int);
    void transformWhileStmt(const WhileStmt&, int);
    void transformForStmt(const ForStmt&, int);
    void transformTryStmt(const TryStmt&, int);
    void transformWithStmt(const WithStmt&, int);
    void transformFuncdefStmt(const FuncdefStmt&, int);
    void transformClassdefStmt(const ClassdefStmt&, int);
    void transformSuite(const Suite&, int);

    void transformDecorator(const Decorator&);

    void addText(std::string_view txt) {
        buffer += txt.data();
    }

    void addNewLine(int indent=0) {
        buffer += '\n';
        if (indent > 0) {
            addIndent(indent);
        }
    }

    void addIndent(int amount) {
        addText(std::string(amount, ' '));
    }

    std::string buffer;
    bool nextGlobalStmtShouldBeOnANewLine = false;
};

#include "expr_transform.h"
#include "stmt_transform.h"
