#pragma once

#include <Ast.h>

class MutatingVisitor {
 protected:
  std::unique_ptr<Expr> mutator_{nullptr};
  inline void ApplyTo(std::unique_ptr<Expr>& expr) {
    expr->Accept(this);
    if (mutator_)
      expr = std::move(mutator_);
  }

 public:
  virtual ~MutatingVisitor() = default;
  virtual void Visit(Constant* expr)   = 0;
  virtual void Visit(UnaryExpr* expr)  = 0;
  virtual void Visit(BinaryExpr* expr) = 0;
  virtual void Visit(Program* expr)    = 0;
};

class ShrinkMutatingVisitor : public MutatingVisitor {
 public:
  void Visit(Constant* expr) override;
  void Visit(UnaryExpr* expr) override;
  void Visit(BinaryExpr* expr) override;
  void Visit(Program* expr) override;
};


template <class ConcreteExpr>
void VisitableExpr<ConcreteExpr>::Accept(MutatingVisitor* visitor) {
  visitor->Visit(static_cast<ConcreteExpr*>(this));
}
