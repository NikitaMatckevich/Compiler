#include <MutatingVisitors.h>

namespace {

double DispatchBinOp(double lhs, Token bin_op, double rhs) {    
  double result = lhs;
  if (bin_op.Is<types::Add>()) {
    result += rhs;
  } else if (bin_op.Is<types::Mul>()) {
    result *= rhs;
  } else if (bin_op.Is<types::Sub>()) {
    result -= rhs;
  } else if (bin_op.Is<types::Div>()) {
    result /= rhs;
  } else {
    throw SyntaxError(bin_op);
  }
  return result;
}

}

void ExecuteMutatingVisitor::Visit(Constant* expr) {
  double val = expr->Value().As<types::Number>().value;
  const_expr_.emplace(val); 
}

void ExecuteMutatingVisitor::Visit(Variable* /* expr */) {
  const_expr_.reset();
}

void ExecuteMutatingVisitor::Visit(UnaryExpr* expr) {
  ApplyMutatorTo(expr->Term());
  if (const_expr_.has_value()) {
    double val = expr->IsNeg() ? -(*const_expr_) : (*const_expr_);
    Token t{TokenContext(), types::Number{val}};
    GetMutator() = std::make_unique<Constant>(std::move(t));
  }
}

void ExecuteMutatingVisitor::Visit(BinaryExpr* expr) {
  
  std::vector<std::unique_ptr<Expr>> shrinked_terms;
  std::unique_ptr<Constant> const_term{nullptr};
  
  std::vector<Token> shrinked_ops;
  Token const_op;
  
  auto& term = expr->Terms().front();
  ApplyMutatorTo(term);
  if (!const_expr_.has_value()) {
    shrinked_terms.emplace_back(std::move(term));
  } else {
    Token t{TokenContext(), types::Number{*const_expr_}};
    const_term = std::make_unique<Constant>(std::move(t));
  }

  for (size_t i = 0; i < expr->Ops().size(); ++i) {

    auto& term = expr->Terms()[i+1]; 
    auto& op = expr->Ops()[i];

    ApplyMutatorTo(term);
    if (!const_expr_.has_value()) {
      shrinked_ops.emplace_back(std::move(op));
      shrinked_terms.emplace_back(std::move(term));
    } else {
      if (const_term) {
        auto& num = const_term->Value().As<types::Number>();
        num.value = DispatchBinOp(num.value, op, *const_expr_);
      } else {
        const_op = std::move(op);
        Token t{TokenContext(), types::Number{*const_expr_}};
        const_term = std::make_unique<Constant>(std::move(t));
      }
    }
  }
   
  if (shrinked_terms.empty()) {
    const_expr_.emplace(const_term->Value().As<types::Number>().value);
    GetMutator() = std::move(const_term);
  } else {
    const_expr_.reset();
    if (shrinked_terms.size() > shrinked_ops.size()) {
      if (!const_term) {
        return;
      } else {
        shrinked_ops.emplace_back(std::move(const_op));
        shrinked_terms.emplace_back(std::move(const_term));
      }
    } else {
      shrinked_terms.insert(shrinked_terms.begin(), std::move(const_term));
    }
    GetMutator() = std::make_unique<BinaryExpr>(std::move(shrinked_ops),
                                                std::move(shrinked_terms));
  }
}

void ExecuteMutatingVisitor::Visit(Assignment* expr) {
  ApplyMutatorTo(expr->Terms().back());
  if (expr->Terms().size() > 1) {
    const_expr_.reset();
  }
}

void ExecuteMutatingVisitor::Visit(Declaration* expr) {
  ApplyMutatorTo(expr->Rhs());
}

void ExecuteMutatingVisitor::Visit(Program* expr) {
  for (auto& instruction : expr->Instructions()) {
    ApplyMutatorTo(instruction);
  }
}
