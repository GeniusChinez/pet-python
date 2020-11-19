#pragma once

enum class StmtKind {
    None,

    // simple statements
    Expression,
    Assert,
    Assignment,
    AugmentedAssignment,
    AnnotatedAssignment,
    Pass,
    Del,
    Return,
    Yield,
    Raise,
    Break,
    Continue,
    Import,
    Global,
    Nonlocal,

    // compound statements
    If,
    While,
    For,
    Try,
    With,
    Funcdef,
    Classdef,
};

struct Stmt {
    Stmt(Location location, StmtKind kind)
        : location(std::move(location)), 
        kind(kind) {}
    StmtKind kind;
    Location location;
};

struct ExprStmt : public Stmt {
    ExprStmt(const Location& location)
        : Stmt(location, StmtKind::Expression) {}
    ExprPtr expr;
};

struct AssignmentStmt : public Stmt {
    AssignmentStmt(const Location& location)
        : Stmt(location, StmtKind::Assignment) {}
    ExprList targetList;
    ExprPtr value;
};

struct AugmentedAssignmentStmt : public Stmt {
    AugmentedAssignmentStmt(const Location& location)
        : Stmt(location, StmtKind::AugmentedAssignment) {}
    ExprPtr autoTarget;
    ExprList values;
    TokenKind augOp;
};

struct AnnotatedAssignmentStmt : public Stmt {
    AnnotatedAssignmentStmt(const Location& location)
        : Stmt(location, StmtKind::AnnotatedAssignment) {}
    ExprPtr autoTarget;
    ExprPtr annotation;
    ExprPtr value;
};

struct AssertStmt : public Stmt {
    AssertStmt(const Location& location)
        : Stmt(location, StmtKind::Assert) {}
    ExprPtr expr1;
    ExprPtr expr2 {nullptr};
};

struct DelStmt : public Stmt {
    DelStmt(const Location& location)
        : Stmt(location, StmtKind::Del) {}
    TargetList targetList;
};

struct ReturnStmt : public Stmt {
    ReturnStmt(const Location& location)
        : Stmt(location, StmtKind::Return) {}
    ExprList exprList;
};

struct YieldStmt : public Stmt {
    YieldStmt(const Location& location)
        : Stmt(location, StmtKind::Yield) {}
    ExprPtr expr; // yield expr
};

struct RaiseStmt : public Stmt {
    RaiseStmt(const Location& location)
        : Stmt(location, StmtKind::Raise) {}
    ExprPtr expr {nullptr};
    ExprPtr fromExpr {nullptr};
};

struct ImportItem {
    NameList parts;
    std::string alias; // import x as y
};

using ImportItemList = std::vector<ImportItem>;

struct ImportStmt : public Stmt {
    ImportStmt(const Location& location)
        : Stmt(location, StmtKind::Import) {}
    NameList source; // from x import y
    ImportItemList items;
};

struct GlobalStmt : public Stmt {
    GlobalStmt(const Location& location)
        : Stmt(location, StmtKind::Global) {}
    NameList names;
};

struct NonlocalStmt : public Stmt {
    NonlocalStmt(const Location& location)
        : Stmt(location, StmtKind::Nonlocal) {}
    NameList names;
};

struct Suite {
    StmtList stmts;
};

using ExprSuitePair = std::pair<ExprPtr, Suite>;
using ExprSuitePairList = std::vector<ExprSuitePair>;

struct IfStmt : public Stmt {
    IfStmt(const Location& location)
        : Stmt(location, StmtKind::If) {}
    ExprSuitePairList suites;
    Suite elseSuite;
};

struct WhileStmt : public Stmt {
    WhileStmt(const Location& location)
        : Stmt(location, StmtKind::While) {}
    ExprPtr expr;
    Suite suite;
    Suite elseSuite;
};

struct ForStmt : public Stmt {
    ForStmt(const Location& location)
        : Stmt(location, StmtKind::For) {}
    bool isAsync {false};
    NameList targetList;
    ExprList exprList;
    Suite suite;
    Suite elseSuite;
};

struct TryExcept {
    ExprPtr expr {nullptr};
    std::string alias;
    Suite suite;
};

using TryExceptList = std::vector<TryExcept>;

struct TryStmt : public Stmt {
    TryStmt(const Location& location)
        : Stmt(location, StmtKind::Try) {}
    Suite suite;
    TryExceptList exceptList;
    Suite elseSuite;
    Suite finallySuite;
};

struct WithItem {
    ExprPtr expr;
    std::string alias;
};

using WithItemList = std::vector<WithItem>;

struct WithStmt : public Stmt {
    WithStmt(const Location& location)
        : Stmt(location, StmtKind::With) {}
    bool isAsync {false};
    WithItemList items;
    Suite suite;
};

using NameList = std::vector<std::string>;

struct Decorator {
    NameList dottedName;
    ArgumentList argumentList;
};

using DecoratorList = std::vector<Decorator>;

struct FuncdefStmt : public Stmt {
    FuncdefStmt(const Location& location)
        : Stmt(location, StmtKind::Funcdef) {}
    bool isAsync {false};
    DecoratorList decorators;
    std::string name;
    ParameterList parameterList;
    ExprPtr hint {nullptr}; // -> xx
    Suite suite;
};

struct ClassdefStmt : public Stmt {
    ClassdefStmt(const Location& location)
        : Stmt(location, StmtKind::Classdef) {}
    DecoratorList decorators;
    std::string name;
    ArgumentList argumentList;
    Suite suite;
};
