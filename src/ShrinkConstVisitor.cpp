#include <ConstVisitors.h>

void ShrinkConstVisitor::Visit(const Constant* expr) {
  shrinked_copy_ = std::make_unique<Constant>(expr->Value());
}

void ShrinkConstVisitor::Visit(const UnaryExpr* expr) {
  expr->Term()->Accept(this);
  if (expr->IsNeg()) {
    shrinked_copy_ =
        std::make_unique<UnaryExpr>(std::move(shrinked_copy_), true);
  }
}

void ShrinkConstVisitor::Visit(const BinaryExpr* expr) {
  expr->Terms().front()->Accept(this);
  if (expr->Terms().size() == 1) {
    return;
  }

  std::vector<std::unique_ptr<Expr>> terms;
  terms.reserve(expr->Terms().size());
  terms.emplace_back(std::move(shrinked_copy_));

  for (size_t i = 1; i < expr->Terms().size(); ++i) {
    expr->Terms()[i]->Accept(this);
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
    instruction->Accept(this);
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
