#include <ConstVisitors.h>

void ExecuteVisitor::Visit(const Constant* expr) {
  stack_.push_back(expr->Value());
}

void ExecuteVisitor::Visit(const UnaryExpr* expr) {
  expr->Term()->Accept(this);
  if (expr->IsNeg()) {
    stack_.back() = -stack_.back();
  }
}

void ExecuteVisitor::Visit(const BinaryExpr* expr) {
  expr->Terms().front()->Accept(this);

  for (size_t i = 1; i < expr->Terms().size(); ++i) {
    expr->Terms()[i]->Accept(this);
    DispatchBinOp(expr->Ops()[i - 1]);
  }
}

void ExecuteVisitor::Visit(const Program* expr) {
  for (const auto& instruction : expr->Instructions()) {
    instruction->Accept(this);
  }
}

const std::vector<int>& ExecuteVisitor::GetResults() const { return stack_; }
void ExecuteVisitor::DispatchBinOp(const Token& token) {
  int rhs = stack_.back();
  stack_.pop_back();

  int lhs = stack_.back();
  stack_.pop_back();

  int result = 0xDEADBEEF;

  if (token.Is<types::Add>()) {
    result = lhs + rhs;
  } else if (token.Is<types::Mul>()) {
    result = lhs * rhs;
  } else if (token.Is<types::Sub>()) {
    result = lhs - rhs;
  } else if (token.Is<types::Div>()) {
    result = lhs / rhs;
  }

  stack_.push_back(result);
}
