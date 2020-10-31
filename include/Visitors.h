#pragma once

#include <Ast.h>

class AbstractVisitor {
 public:
  virtual ~AbstractVisitor()                 = default;
  virtual void Visit(const Constant* expr)   = 0;
  virtual void Visit(const UnaryExpr* expr)  = 0;
  virtual void Visit(const BinaryExpr* expr) = 0;
  virtual void Visit(const Program* expr)    = 0;
};

class ShrinkOneChildBranchesVisitor : public AbstractVisitor {
 private:
  std::unique_ptr<Expr> result_of_transformation_{nullptr};

 public:
  void Visit(const Constant* expr) override final;
  void Visit(const UnaryExpr* expr) override final;
  void Visit(const BinaryExpr* expr) override final;
  void Visit(const Program* expr) override final;
  const std::unique_ptr<Expr>& GetResult() const&;
  std::unique_ptr<Expr>&& GetResult() &&;
};

class ExecuteVisitor : public AbstractVisitor {
 private:
  std::vector<int> stack_;

  void DispatchBinOp(const Token& token);

 public:
  void Visit(const Constant* expr) override final;
  void Visit(const UnaryExpr* expr) override final;
  void Visit(const BinaryExpr* expr) override final;
  void Visit(const Program* expr) override final;

  const std::vector<int>& GetResults() const;
};