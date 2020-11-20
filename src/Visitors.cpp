#include <Visitors.h>

void MutatingVisitor::ApplyTo(std::unique_ptr<Expr>& expr) {
  expr->AcceptMutating(this);
  if (mutator_)
    expr = std::move(mutator_);
}

void ShrinkConstVisitor::Visit(const Constant* expr) {
  shrinked_copy_ = std::make_unique<Constant>(expr->Value());
}

void ShrinkConstVisitor::Visit(const UnaryExpr* expr) {
  expr->Term()->AcceptConst(this);
  if (expr->IsNeg()) {
    shrinked_copy_ =
        std::make_unique<UnaryExpr>(std::move(shrinked_copy_), true);
  }
}

void ShrinkConstVisitor::Visit(const BinaryExpr* expr) {
  expr->Terms().front()->AcceptConst(this);
  if (expr->Terms().size() == 1) {
    return;
  }

  std::vector<std::unique_ptr<Expr>> terms;
  terms.reserve(expr->Terms().size());
  terms.emplace_back(std::move(shrinked_copy_));

  for (size_t i = 1; i < expr->Terms().size(); ++i) {
    expr->Terms()[i]->AcceptConst(this);
    terms.emplace_back(std::move(shrinked_copy_));
  }

  std::vector<Token> ops_copy = expr->Ops();

  shrinked_copy_ =
      std::make_unique<BinaryExpr>(std::move(ops_copy), std::move(terms));
}

void ShrinkConstVisitor::Visit(const Program* expr) {
  std::vector<std::unique_ptr<Expr>> instructions;
  instructions.reserve(expr->Instructions().size());

  for (const auto& instruction : expr->Instructions()) {
    instruction->AcceptConst(this);
    instructions.emplace_back(std::move(shrinked_copy_));
  }

  shrinked_copy_ =
      std::make_unique<Program>(std::move(instructions));
}

const std::unique_ptr<Expr>& ShrinkConstVisitor::GetResults() const& {
  return shrinked_copy_;
}

std::unique_ptr<Expr>&& ShrinkConstVisitor::GetResults() && {
  return std::move(shrinked_copy_);
}

void ExecuteVisitor::Visit(const Constant* expr) {
  stack_.push_back(expr->Value());
}

void ExecuteVisitor::Visit(const UnaryExpr* expr) {
  expr->Term()->AcceptConst(this);
  if (expr->IsNeg()) {
    stack_.back() = -stack_.back();
  }
}

void ExecuteVisitor::Visit(const BinaryExpr* expr) {
  expr->Terms().front()->AcceptConst(this);

  for (size_t i = 1; i < expr->Terms().size(); ++i) {
    expr->Terms()[i]->AcceptConst(this);
    DispatchBinOp(expr->Ops()[i - 1]);
  }
}

void ExecuteVisitor::Visit(const Program* expr) {
  for (const auto& instruction : expr->Instructions()) {
    instruction->AcceptConst(this);
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

void ShrinkMutatingVisitor::Visit(Constant* /* expr */) {}

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

void TreeLoggingVisitor::Visit(const Constant* expr) {
  LogName("Constant");
  LogNode("Value", expr->Value(), true);
}

void TreeLoggingVisitor::Visit(const UnaryExpr* expr) {
  LogName("UnaryExpr");
  LogNode("IsNeg", expr->IsNeg(), false);
  LogNode("Term", expr->Term(), true);
}

void TreeLoggingVisitor::Visit(const BinaryExpr* expr) {
  LogName("BinaryExpr");
  for (size_t i = 0; i < expr->Ops().size(); ++i) {
    LogNode("Term", expr->Terms()[i], false);
    LogNode("Op", expr->Ops()[i].GetContext().GetTokenString(), false);
  }
  LogNode("Term", expr->Terms().back(), true);
}

void TreeLoggingVisitor::Visit(const Program* expr) {
  LogName("Program");
  for (size_t i = 0; i < expr->Instructions().size() - 1; ++i) {
    LogNode("Instruction", expr->Instructions()[i], false);
  }
  LogNode("Instruction", expr->Instructions().back(), true);
}

void TreeLoggingVisitor::Preamble() const {
  for (bool is_last_in_subtree : preamble_description_) {
    std::cout << (is_last_in_subtree ? kVLastEdge : kVEdge);
  }
}
