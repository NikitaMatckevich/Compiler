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
  virtual void Visit(Constant* expr)    = 0;
  virtual void Visit(Variable* expr)    = 0;
  virtual void Visit(Declaration* expr) = 0;
  virtual void Visit(Assignment* expr)  = 0;
  virtual void Visit(UnaryExpr* expr)   = 0;
  virtual void Visit(BinaryExpr* expr)  = 0;
  virtual void Visit(Program* expr)     = 0;

  #define VISITABLE\
  void Visit(Constant* expr) override final;\
  void Visit(Variable* expr) override final;\
  void Visit(Declaration* expr) override final;\
  void Visit(Assignment* expr) override final;\
  void Visit(UnaryExpr* expr) override final;\
  void Visit(BinaryExpr* expr) override final;\
  void Visit(Program* expr) override final;
};

class ShrinkMutatingVisitor : public MutatingVisitor {
 public:
  VISITABLE
};

#undef VISITABLE

template <class ConcreteExpr>
void VisitableExpr<ConcreteExpr>::Accept(MutatingVisitor* visitor) {
  visitor->Visit(static_cast<ConcreteExpr*>(this));
}
