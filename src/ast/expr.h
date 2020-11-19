#pragma once

enum class ExprKind {
    None, // "None"
    Name,
    StringLiteral,
    IntegerLiteral,
    BooleanLiteral,
    FloatLiteral,
    ListDisplay,
    SetDisplay,
    TupleDisplay,
    DictDisplay,
    Generator,
    Yield,
    AttributeRef,
    Subscription,
    Slicing,
    Call,
    Await,
    Unary,
    Binary,
    Lambda,
};

struct Expr {
    Expr(Location location, ExprKind kind)
        : location(std::move(location)),
        kind(kind) {}
    int stars {0};
    bool await {false};
    Location location;
    ExprKind kind;
};

struct NameExpr : public Expr {
    NameExpr(const Location& location)
        : Expr(std::move(location), ExprKind::Name) {}
    std::string value;
};

// bytes and string literals
struct StringLiteralExpr : public Expr {
    StringLiteralExpr(const Location& location)
        : Expr(std::move(location), ExprKind::StringLiteral) {}
    bool isBytes {false};
    std::string value;
};

struct IntegerLiteralExpr : public Expr {
    IntegerLiteralExpr(const Location& location)
        : Expr(location, ExprKind::IntegerLiteral) {}
    int64_t value;
};

struct FloatLiteralExpr : public Expr {
    FloatLiteralExpr(const Location& location)
        : Expr(location, ExprKind::FloatLiteral) {}
    bool isImaginary {false};
    long double value;
};

struct BooleanLiteralExpr : public Expr {
    BooleanLiteralExpr(const Location& location)
        : Expr(location, ExprKind::BooleanLiteral) {}
    bool value;
};

struct CompIf;
struct CompFor;
struct CompIter;

using CompIfPtr = std::shared_ptr<CompIf>;
using CompForPtr = std::shared_ptr<CompFor>;
using CompIterPtr = std::shared_ptr<CompIter>;

struct CompFor {
    bool isAsync {false};
    TargetList targetList;
    ExprPtr test;
    CompIterPtr compIter {nullptr};
};

struct CompIf {
    ExprPtr exprNoCond;
    CompIterPtr compIter {nullptr};
};

struct CompIter {
    CompForPtr compFor {nullptr};
    CompIfPtr compIf {nullptr};
};

struct Comprehension {
    ExprPtr expr;
    CompForPtr compFor;
};

using ComprehensionPtr = std::shared_ptr<Comprehension>;

struct ListDisplayExpr : public Expr {
    ListDisplayExpr(const Location& location)
        : Expr(location, ExprKind::ListDisplay) {}        
    ExprList starredList;
    ComprehensionPtr comprehension {nullptr};
};

struct SetDisplayExpr : public Expr {
    SetDisplayExpr(const Location& location)
        : Expr(location, ExprKind::SetDisplay) {}        
    ExprList items;
    ComprehensionPtr comprehension {nullptr};
};

struct TupleDisplayExpr : public Expr {
    TupleDisplayExpr(const Location& location)
        : Expr(location, ExprKind::TupleDisplay) {}        
    ExprList items;
};

struct DictItem {
    ExprPtr expr1;
    ExprPtr expr2;
    CompForPtr compFor {nullptr};
};

using DictItemList = std::vector<DictItem>;

struct DictDisplayExpr : public Expr {
    DictDisplayExpr(const Location& location)
        : Expr(location, ExprKind::DictDisplay) {}
    DictItemList itemList;
};

struct GeneratorExpr : public Expr {
    GeneratorExpr(const Location& location)
        : Expr(location, ExprKind::Generator) {}
    ExprPtr expr;
    CompForPtr compFor;
};

struct YieldExpr : public Expr {
    YieldExpr(const Location& location)
        : Expr(location, ExprKind::Yield) {}
    ExprList exprList;
    ExprPtr fromExpr {nullptr};
};

struct AttributeRefExpr : public Expr {
    AttributeRefExpr(const Location& location)
        : Expr(location, ExprKind::AttributeRef) {}
    ExprPtr primary;
    std::string name;
};

struct SubscriptionExpr : public Expr {
    SubscriptionExpr(const Location& location)
        : Expr(location, ExprKind::Subscription) {}
    ExprPtr primary;
    ExprList exprList;
};

struct SlicingExpr : public Expr {
    SlicingExpr(const Location& location)
        : Expr(location, ExprKind::Slicing) {}
    ExprPtr primary;
    ExprPtr lowerBound {nullptr};
    ExprPtr upperBound {nullptr};
    ExprPtr stride {nullptr};
};

struct CallExpr : public Expr {
    CallExpr(const Location& location)
        : Expr(location, ExprKind::Call) {}
    ExprPtr primary;
    ArgumentList argumentList;
    ComprehensionPtr comprehension {nullptr};
};

struct AwaitExpr : public Expr {
    AwaitExpr(const Location& location)
        : Expr(location, ExprKind::Await) {}
    ExprPtr primary;
};

struct UnaryExpr : public Expr {
    UnaryExpr(const Location& location)
        : Expr(location, ExprKind::Unary) {}
    ExprPtr expr;
    TokenKind op;
};

struct BinaryExpr : public Expr {
    BinaryExpr(const Location& location)
        : Expr(location, ExprKind::Binary) {}
    ExprPtr lhs;
    ExprPtr rhs;
    TokenKind op;
};

struct LambdaExpr : public Expr {
    LambdaExpr(const Location& location)
        : Expr(location, ExprKind::Lambda) {}
    ParameterList parameterList;
    ExprPtr expr;
};
