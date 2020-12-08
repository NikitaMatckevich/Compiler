#include <MutatingVisitors.h>

void ShrinkMutatingVisitor::Visit(Constant* /* expr */) {}

void ShrinkMutatingVisitor::Visit(Variable* /* expr */) {}

void ShrinkMutatingVisitor::Visit(UnaryExpr* expr) {
  ApplyMutatorTo(expr->Term());
  if (!expr->IsNeg()) {
    GetMutator() = std::move(expr->Term()); }
}

void ShrinkMutatingVisitor::Visit(BinaryExpr* expr) {
  for (auto& term : expr->Terms()) {
    ApplyMutatorTo(term);
  }
  if (expr->Terms().size() == 1) {
    GetMutator() = std::move(expr->Terms().front()); }
}

void ShrinkMutatingVisitor::Visit(Assignment* expr) {
  for (auto& term : expr->Terms()) {
    ApplyMutatorTo(term);
  }
  if (expr->Terms().size() == 1) {
    GetMutator() = std::move(expr->Terms().front()); }
}

void ShrinkMutatingVisitor::Visit(Declaration* expr) {
  ApplyMutatorTo(expr->Rhs());
}

void ShrinkMutatingVisitor::Visit(Program* expr) {
  for (auto& instruction : expr->Instructions()) {
    ApplyMutatorTo(instruction);
  }
}
