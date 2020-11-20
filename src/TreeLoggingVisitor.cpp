#include <ConstVisitors.h>

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
