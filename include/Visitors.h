#pragma once
#include <Ast.h>

class BaseVisitor {
public:
  virtual ~BaseVisitor() = default;
  virtual void Visit(std::unique_ptr<UnaryExpr>& e) = 0; \
  virtual void Visit(std::unique_ptr<BinaryExpr>& e) = 0; \
  virtual void Visit(std::unique_ptr<Program>& e) = 0; 
};

class ShrinkOneChildBranches : public BaseVisitor {
public:
  void Visit(std::unique_ptr<UnaryExpr>& e) override final;
  void Visit(std::unique_ptr<BinaryExpr>& e) override final;
  void Visit(std::unique_ptr<Program>& e) override final;
};
