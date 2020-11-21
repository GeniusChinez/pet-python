#pragma once

void PythonAstTransformer::transformExpr(ExprPtr expr) {
    if (expr->await) {
        addText("await ");
    }

    switch (expr->kind) {
    case ExprKind::None: {
        addText("None");
        break;
    }
    case ExprKind::Name: {
        transformNameExpr(
            *std::static_pointer_cast<NameExpr>(expr)
        );
        break;
    }
    case ExprKind::StringLiteral: {
        transformStringLiteralExpr(
            *std::static_pointer_cast<StringLiteralExpr>(expr)
        );
        break;
    }
    case ExprKind::IntegerLiteral: {
        transformIntegerLiteralExpr(
            *std::static_pointer_cast<IntegerLiteralExpr>(expr)
        );
        break;
    }
    case ExprKind::BooleanLiteral: {
        transformBooleanLiteralExpr(
            *std::static_pointer_cast<BooleanLiteralExpr>(expr)
        );
        break;
    }
    case ExprKind::FloatLiteral: {
        transformFloatLiteralExpr(
            *std::static_pointer_cast<FloatLiteralExpr>(expr)
        );
        break;
    }
    case ExprKind::If: {
        transformIfExpr(
            *std::static_pointer_cast<IfExpr>(expr)
        );
        break;
    }
    case ExprKind::ListDisplay: {
        transformListDisplayExpr(
            *std::static_pointer_cast<ListDisplayExpr>(expr)
        );
        break;
    }
    case ExprKind::TupleDisplay: {
        transformTupleDisplayExpr(
            *std::static_pointer_cast<TupleDisplayExpr>(expr)
        );
        break;
    }
    case ExprKind::DictDisplay: {
        transformDictDisplayExpr(
            *std::static_pointer_cast<DictDisplayExpr>(expr)
        );
        break;
    }
    case ExprKind::SetDisplay: {
        transformSetDisplayExpr(
            *std::static_pointer_cast<SetDisplayExpr>(expr)
        );
        break;
    }
    case ExprKind::Generator: {
        transformGeneratorExpr(
            *std::static_pointer_cast<GeneratorExpr>(expr)
        );
        break;
    }
    case ExprKind::Yield: {
        transformYieldExpr(
            *std::static_pointer_cast<YieldExpr>(expr)
        );
        break;
    }
    case ExprKind::AttributeRef: {
        transformAttributeRefExpr(
            *std::static_pointer_cast<AttributeRefExpr>(expr)
        );
        break;
    }
    case ExprKind::Subscription: {
        transformSubscriptionExpr(
            *std::static_pointer_cast<SubscriptionExpr>(expr)
        );
        break;
    }
    case ExprKind::Slicing: {
        transformSlicingExpr(
            *std::static_pointer_cast<SlicingExpr>(expr)
        );
        break;
    }
    case ExprKind::Call: {
        transformCallExpr(
            *std::static_pointer_cast<CallExpr>(expr)
        );
        break;
    }
    case ExprKind::Unary: {
        transformUnaryExpr(
            *std::static_pointer_cast<UnaryExpr>(expr)
        );
        break;
    }
    case ExprKind::Binary: {
        transformBinaryExpr(
            *std::static_pointer_cast<BinaryExpr>(expr)
        );
        break;
    }
    case ExprKind::Lambda: {
        transformLambdaExpr(
            *std::static_pointer_cast<LambdaExpr>(expr)
        );
        break;
    }
    default:
        assert(0);
    }
}

void PythonAstTransformer::transformArgument(const Argument& argument) {
    for(int i = 0; i < argument.stars; ++i) {
        addText("*");
    }

    if (argument.name.size() > 0) {
        addText(argument.name);
        addText("=");
    }

    assert(argument.value);

    transformExpr(argument.value);
}

void PythonAstTransformer::transformNameExpr(
    const NameExpr& expr
) {
    addText(expr.value);
}

void PythonAstTransformer::transformStringLiteralExpr(
    const StringLiteralExpr& expr
) {
    addText("\"");

    auto temp = std::regex_replace(expr.value, std::regex("\""), "\\\"");
    temp = std::regex_replace(temp, std::regex("\n"), "\\n");

    addText(temp);
    addText("\"");
}

void PythonAstTransformer::transformIntegerLiteralExpr(
    const IntegerLiteralExpr& expr
) {
    addText(formatAsString(expr.value));
}

void PythonAstTransformer::transformBooleanLiteralExpr(
    const BooleanLiteralExpr& expr
) {
    addText(expr.value == true ? "True" : "False");
}

void PythonAstTransformer::transformFloatLiteralExpr(
    const FloatLiteralExpr& expr
) {
    addText(formatAsString(expr.value));
}

void PythonAstTransformer::transformComprehension(
    const Comprehension& comp
) {
    transformExpr(comp.expr);
    addText(" ");
    transformComprehensionFor(*(comp.compFor));
}

void PythonAstTransformer::transformComprehensionFor(
    const CompFor& compFor
) {
    if (compFor.isAsync) {
        addText("async ");
    }

    addText("for ");

    int i = 0;

    while (i < compFor.targetList.size()) {
        transformTarget(compFor.targetList[i]);
        if (i != compFor.targetList.size() - 1) {
            addText(", ");
        }
        i++;
    }

    addText(" in ");

    transformExpr(compFor.test);

    if (compFor.compIter) {
        transformComprehensionIter(*(compFor.compIter));
    }
}

void PythonAstTransformer::transformComprehensionIf(
    const CompIf& compIf
) {
    transformExpr(compIf.exprNoCond);
    addText(" ");

    if (compIf.compIter) {
        transformComprehensionIter(*(compIf.compIter));
    }
}

void PythonAstTransformer::transformComprehensionIter(
    const CompIter& compIter
) {
    if (compIter.compFor) {
        transformComprehensionFor(*(compIter.compFor));
    }
    else {
        transformComprehensionIf(*(compIter.compIf));
    }
}

void PythonAstTransformer::transformListDisplayExpr(
    const ListDisplayExpr& expr
) {
    addText("[");
    if (expr.comprehension) {
        transformComprehension(*(expr.comprehension));
        addText("]");
        return;
    }

    int i = 0;

    while (i < expr.starredList.size()) {
        transformExpr(expr.starredList[i]);
        if (i != expr.starredList.size() - 1) {
            addText(", ");
        }

        i++;
    }

    addText("]");

}

void PythonAstTransformer::transformTupleDisplayExpr(
    const TupleDisplayExpr& expr
) {
    addText("(");
    int i = 0;

    while (i < expr.items.size()) {
        transformExpr(expr.items[i]);

        if (i != expr.items.size() - 1) {
            addText(", ");
        }

        i++;
    }

    addText(")");
}

void PythonAstTransformer::transformDictItem(const DictItem& item) {
    transformExpr(item.expr1);
    addText(": ");
    transformExpr(item.expr2);

    if (item.compFor) {
        addText(" ");
        transformComprehensionFor(*(item.compFor));
    }
}

void PythonAstTransformer::transformIfExpr(const IfExpr& expr) {
    transformExpr(expr.cond);
    addText(" if ");
    transformExpr(expr.thenValue);
    addText(" else ");
    transformExpr(expr.elseValue);
}

void PythonAstTransformer::transformDictDisplayExpr(
    const DictDisplayExpr& expr
) {
    addText("{");
    int i = 0;

    while (i < expr.itemList.size()) {
        transformDictItem(expr.itemList[i]);

        if (i != expr.itemList.size() - 1) {
            addText(", ");
        }

        i++;
    }

    addText("}");
}

void PythonAstTransformer::transformSetDisplayExpr(
    const SetDisplayExpr& expr
) {
    addText("{");

    if (expr.comprehension) {
        transformComprehension(*(expr.comprehension));
        addText("}");
        return;
    }

    int i = 0;

    while (i < expr.items.size()) {
        transformExpr(expr.items[i]);

        if (i != expr.items.size() - 1) {
            addText(", ");
        }

        i++;
    }

    addText("}");
}

void PythonAstTransformer::transformGeneratorExpr(
    const GeneratorExpr& expr
) {
    addText("(");
    transformExpr(expr.expr);
    addText(" ");
    transformComprehensionFor(*(expr.compFor));
    addText(")");
}

void PythonAstTransformer::transformYieldExpr(
    const YieldExpr& expr
) {
    addText("yield ");

    if (expr.exprList.size() == 0) {
        return;
    }

    int i = 0;

    while (i < expr.exprList.size()) {
        transformExpr(expr.exprList[i]);
        if (i != expr.exprList.size() - 1) {
            addText(", ");
        }
        i++;
    }

    if (expr.fromExpr) {
        addText(" from ");
        transformExpr(expr.fromExpr);
    }
}

void PythonAstTransformer::transformAttributeRefExpr(
    const AttributeRefExpr& expr
) {
    transformExpr(expr.primary);
    addText(".");
    addText(expr.name);
}

void PythonAstTransformer::transformSubscriptionExpr(
    const SubscriptionExpr& expr
) {
    transformExpr(expr.primary);

    addText("[");
    int i = 0;

    while (i < expr.exprList.size()) {
        transformExpr(expr.exprList[i]);

        if (i != expr.exprList.size() - 1) {
            addText(", ");
        }

        i++;
    }

    addText("]");
}

void PythonAstTransformer::transformSlicingExpr(
    const SlicingExpr& expr
) {
    transformExpr(expr.primary);
    addText("[");

    if (expr.lowerBound) {
        transformExpr(expr.lowerBound);
    }

    addText(":");

    if (expr.upperBound) {
        transformExpr(expr.upperBound);
    }

    addText(":");

    if (expr.stride) {
        transformExpr(expr.stride);
    }

    addText("]");
}

void PythonAstTransformer::transformCallExpr(
    const CallExpr& expr
) {
    transformExpr(expr.primary);
    addText("(");

    if (expr.comprehension) {
        transformComprehension(*(expr.comprehension));
        addText(")");
        return;
    }

    int i = 0;

    while (i < expr.argumentList.size()) {
        transformArgument(expr.argumentList[i]);
        if (i != expr.argumentList.size() - 1) {
            addText(", ");
        }
        i++;
    }

    addText(")");
}

void PythonAstTransformer::transformUnaryExpr(
    const UnaryExpr& expr
) {
    addText(toString(expr.op));
    addText("(");
    transformExpr(expr.expr);
    addText(")");
}

void PythonAstTransformer::transformBinaryExpr(
    const BinaryExpr& expr
) {
    addText("(");
    transformExpr(expr.lhs);
    addText(" ");

    addText(toString(expr.op));

    addText(" ");
    transformExpr(expr.rhs);
    addText(")");
}

void PythonAstTransformer::transformLambdaExpr(
    const LambdaExpr& expr
) {
    addText("lambda");

    if (expr.parameterList.size() > 0) {
        addText(" ");
        int i = 0;

        while (i < expr.parameterList.size()) {
            transformParameter(expr.parameterList[i]);
            if (i != expr.parameterList.size() - 1) {
                addText(", ");
            }
            i++;
        }
    }

    addText(": ");
    transformExpr(expr.expr);
}
