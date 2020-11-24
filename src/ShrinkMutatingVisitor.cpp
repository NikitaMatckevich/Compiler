#include <MutatingVisitors.h>

void ShrinkMutatingVisitor::Visit(Constant* /* expr */) {}

void ShrinkMutatingVisitor::Visit(Variable* /* expr */) {}

void ShrinkMutatingVisitor::Visit(Declaration* expr) {
  ApplyTo(expr->Rhs());
}

void ShrinkMutatingVisitor::Visit(Assignment* expr) {
  for (auto& term : expr->Terms()) {
    ApplyTo(term);
  }
}

void ShrinkMutatingVisitor::Visit(UnaryExpr* expr) {
  if (!expr->IsNeg())
    mutator_ = std::move(expr->Term());
}

void ShrinkMutatingVisitor::Visit(BinaryExpr* expr) {
  for (auto& term : expr->Terms()) {
    ApplyTo(term);
  }
  if (expr->Terms().size() == 1)
    mutator_ = std::move(expr->Terms().front());
}

void ShrinkMutatingVisitor::Visit(Program* expr) {
  for (auto& instruction : expr->Instructions()) {
    ApplyTo(instruction);
  }
}
