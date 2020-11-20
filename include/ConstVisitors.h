#pragma once

#include <Ast.h>

class ConstVisitor {
 public:
  virtual ~ConstVisitor() = default;
  virtual void Visit(const Constant* expr)   = 0;
  virtual void Visit(const UnaryExpr* expr)  = 0;
  virtual void Visit(const BinaryExpr* expr) = 0;
  virtual void Visit(const Program* expr)    = 0;
};

class ShrinkConstVisitor : public ConstVisitor {
 private:
  std::unique_ptr<Expr> shrinked_copy_{nullptr};
 public:
  void Visit(const Constant* expr) override final;
  void Visit(const UnaryExpr* expr) override final;
  void Visit(const BinaryExpr* expr) override final;
  void Visit(const Program* expr) override final;
  
  const std::unique_ptr<Expr>& GetResults() const&;
  std::unique_ptr<Expr>&& GetResults() &&;
};

class ExecuteVisitor : public ConstVisitor {
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

class TreeLoggingVisitor : public ConstVisitor {
 public:
  void Visit(const Constant* expr) override;
  void Visit(const UnaryExpr* expr) override;
  void Visit(const BinaryExpr* expr) override;
  void Visit(const Program* expr) override;

 private:
  static constexpr char kHEdge[] = "+-- ";
  static constexpr char kHLastEdge[] = "+-- ";
  static constexpr char kVEdge[] = "|   ";
  static constexpr char kVLastEdge[] = "    ";
  std::vector<bool> preamble_description_;

  template <class StringLike, class Node>
  void LogNode(const StringLike& name, const Node& node, bool is_last) {
    Preamble();
    auto flags = std::cout.flags();
    std::cout.setf(std::ios_base::boolalpha);
    std::cout << (is_last ? kHLastEdge : kHEdge) << name << '(' << node << ")\n";
    std::cout.flags(flags);
  }

  template <class StringLike>
  void LogNode(const StringLike& name, const std::unique_ptr<Expr>& subtree, bool is_last) {
    Preamble();
    auto flags = std::cout.flags();
    std::cout.setf(std::ios_base::boolalpha);
    std::cout << (is_last ? kHLastEdge : kHEdge) << name << ": ";
    preamble_description_.push_back(is_last);
    subtree->Accept(this);
    preamble_description_.pop_back();
    std::cout.flags(flags);
  }

  template <class StringLike>
  void LogName(const StringLike& name) const {
    std::cout << name << '\n';
  }

  void Preamble() const;
};

template <class ConcreteExpr>
void VisitableExpr<ConcreteExpr>::Accept(ConstVisitor* visitor) const {
  visitor->Visit(static_cast<const ConcreteExpr*>(this));
}
