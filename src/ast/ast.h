#pragma once

#include <vector>
#include <string>
#include <memory>

struct Expr;
struct Stmt;

using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;

using ExprList = std::vector<ExprPtr>;
using StmtList = std::vector<StmtPtr>;

struct Argument {
    int stars {0};
    std::string name;
    ExprPtr value;
};

using ArgumentList = std::vector<Argument>;
using NameList = std::vector<std::string>;

// *x, **x, x, x=2, x: int, etc
struct Parameter {
    int stars {0};
    std::string name;
    ExprPtr hint {nullptr};
    ExprPtr value {nullptr};
};

using ParameterList = std::vector<Parameter>;

struct Target;
using TargetPtr = std::shared_ptr<Target>;
using TargetList = std::vector<TargetPtr>;

enum class TargetKind {
    Bracketted,
    Expr,
};

struct Target {
    Target(TargetKind kind)
        : kind(kind) {}
    TargetKind kind;
};

struct BrackettedTarget : public Target {
    BrackettedTarget()
        : Target(TargetKind::Bracketted) {}
    std::vector<TargetPtr> targets;
    TokenKind bracketKind;
};

struct ExprTarget : public Target {
    ExprTarget()
        : Target(TargetKind::Expr) {}
    int stars {0};
    ExprPtr expr;
};

#include "expr.h"
#include "stmt.h"
