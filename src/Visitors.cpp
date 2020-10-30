#include <Visitors.h>

void ShrinkOneChildBranchesVisitor::Visit(const Constant* expr) {
  result_of_transformation_ = std::make_unique<Constant>(expr->Value());
}

void ShrinkOneChildBranchesVisitor::Visit(const UnaryExpr* expr) {
  expr->Term()->Accept(this);
  if (expr->IsNeg()) {
    result_of_transformation_ =
        std::make_unique<UnaryExpr>(std::move(result_of_transformation_), true);
  }
}

void ShrinkOneChildBranchesVisitor::Visit(const BinaryExpr* expr) {
  expr->Terms().front()->Accept(this);
  if (expr->Terms().size() == 1) {
    return;
  }

  std::vector<std::unique_ptr<Expr>> terms;
  terms.reserve(expr->Terms().size());
  terms.emplace_back(std::move(result_of_transformation_));

  for (size_t i = 1; i < expr->Terms().size(); ++i) {
    expr->Terms()[i]->Accept(this);
    terms.emplace_back(std::move(result_of_transformation_));
  }

  std::vector<Token> ops_copy = expr->Ops();

  result_of_transformation_ =
      std::make_unique<BinaryExpr>(std::move(ops_copy), std::move(terms));
}

void ShrinkOneChildBranchesVisitor::Visit(const Program* expr) {
  std::vector<std::unique_ptr<Expr>> instructions;
  instructions.reserve(expr->Instructions().size());

  for (const auto& instruction : expr->Instructions()) {
    instruction->Accept(this);
    instructions.emplace_back(std::move(result_of_transformation_));
  }

  result_of_transformation_ =
      std::make_unique<Program>(std::move(instructions));
}

const std::unique_ptr<Expr>& ShrinkOneChildBranchesVisitor::GetResult() const& {
  return result_of_transformation_;
}

std::unique_ptr<Expr>&& ShrinkOneChildBranchesVisitor::GetResult() && {
  return std::move(result_of_transformation_);
}
