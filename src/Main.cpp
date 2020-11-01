#include <Lexer.h>
#include <Visitors.h>

#include <iostream>
#include <vector>

template <class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec) {
  out << '[';
  if (!vec.empty()) {
    out << vec.front();

    auto iter = vec.begin();
    ++iter;

    while (iter != vec.end()) {
      out << ", " << *iter;
      ++iter;
    }
  }
  return out << ']';
}

int main() {
  try {
    std::ifstream fin("example.txt");
    Lexer lexer(fin);
    Parser parser(lexer);
    std::unique_ptr<Expr> ptr = parser.ReadProgram();

    // ShrinkOneChildBranchesVisitor opt;
    // ptr->Accept(&opt);
    // auto transformed_program = std::move(opt).GetResult();

    TreeLoggingVisitor logging_visitor;

    std::cout << "Before transformation:\n";
    ptr->Accept(&logging_visitor);

    ShrinkMutatingVisitor().ApplyTo(ptr);

    std::cout << "After transformation:\n";
    ptr->Accept(&logging_visitor);

    ExecuteVisitor exec;
    ptr->Accept(&exec);
    std::cout << "Results: " << exec.GetResults() << std::endl;

    // std::cout << "Program returned code " << 0 << std::endl;
    return 0;
  } catch (const CompileError& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
  }
}
