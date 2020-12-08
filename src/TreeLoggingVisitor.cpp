#include <ConstVisitors.h>

void TreeLoggingVisitor::Visit(const Constant* expr) {
  LogName("Constant");
  LogNode("Value", expr->Value().As<types::Number>().value, true);
}

void TreeLoggingVisitor::Visit(const Variable* expr) {
  LogName("Variable");
  LogNode("Name", expr->Name().GetContext().GetTokenString(), true);
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
  if (!expr->Terms().empty()) {
    LogNode("Term", expr->Terms().back(), true);
  }
}

void TreeLoggingVisitor::Visit(const Declaration* expr) {
  LogName("Declaration");
  LogNode("Left side", expr->Name().GetContext().GetTokenString(), false);
  if (expr->Rhs()) {
    LogNode("Right side", expr->Rhs(), false);
  } else {
    LogNode("Right side", "", true);
  }
}

void TreeLoggingVisitor::Visit(const Assignment* expr) {
  LogName("Assignment");
  for (size_t i = 0; i < expr->Terms().size() - 1; ++i) {
    LogNode("Term", expr->Terms()[i], false);
  }
  LogNode("Term", expr->Terms().back(), true);
}

void TreeLoggingVisitor::Visit(const Program* expr) {
  LogName("Program");
  for (size_t i = 0; i < expr->Instructions().size() - 1; ++i) {
    LogNode("Instruction", expr->Instructions()[i], false);
  }
  if (!expr->Instructions().empty()) {
    LogNode("Instruction", expr->Instructions().back(), true);
  }
}

void TreeLoggingVisitor::Preamble() const {
  for (bool is_last_in_subtree : preamble_description_) {
    std::cout << (is_last_in_subtree ? kVLastEdge : kVEdge);
  }
}
