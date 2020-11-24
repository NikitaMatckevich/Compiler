#include <ConstVisitors.h>

void LvalueRecognizeVisitor::Visit(const Constant* /*expr*/) {
}

void LvalueRecognizeVisitor::Visit(const Variable* /*expr*/) {
  is_lvalue_ = true;
}

void LvalueRecognizeVisitor::Visit(const Declaration* /*expr*/) {
}

void LvalueRecognizeVisitor::Visit(const Assignment* /*expr*/) {
}

void LvalueRecognizeVisitor::Visit(const UnaryExpr* expr) {
  if (!expr->IsNeg()) {
    expr->Term()->Accept(this);
  }
}

void LvalueRecognizeVisitor::Visit(const BinaryExpr* expr) {
  if (expr->Terms().size() == 1)
    expr->Terms().back()->Accept(this);
}

void LvalueRecognizeVisitor::Visit(const Program* /*expr*/) {
}

bool LvalueRecognizeVisitor::GetResults() const { return is_lvalue_; }
