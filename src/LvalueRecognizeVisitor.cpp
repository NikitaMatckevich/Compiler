#include <ConstVisitors.h>

void LvalueCheckVisitor::Visit(const Constant* expr) {
  if (from_assignment_) {
    throw SyntaxError(expr->Value());
  }
}

void LvalueCheckVisitor::Visit(const Variable* /*expr*/) {}

void LvalueCheckVisitor::Visit(const UnaryExpr* expr) {
  if (from_assignment_ && expr->IsNeg()) {
    throw SyntaxError(*(expr->SubstractionToken()));
  }
  expr->Term()->Accept(this);
}

void LvalueCheckVisitor::Visit(const BinaryExpr* expr) {
  if (from_assignment_ && (expr->Terms().size() > 1)) {
    throw SyntaxError(expr->Ops().front());
  }
  expr->Terms().back()->Accept(this);
}

void LvalueCheckVisitor::Visit(const Assignment* expr) {
  size_t nb_terms = expr->Terms().size();
  if (nb_terms > 1) {
    from_assignment_ = true;
    for (size_t i = 0; i < nb_terms - 1; ++i) {
      expr->Terms()[i]->Accept(this);
    }
    from_assignment_ = false;
  }
}

void LvalueCheckVisitor::Visit(const Declaration* expr) {
  if (from_assignment_) {
    throw SyntaxError(expr->Name());
  }
  expr->Rhs()->Accept(this);
}

void LvalueCheckVisitor::Visit(const Program* expr) {
  for (auto& instruction : expr->Instructions()) {
    instruction->Accept(this);
  }
}
