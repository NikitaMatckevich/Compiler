#include <Lexer.h>
#include <ConstVisitors.h>
#include <MutatingVisitors.h>

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

    std::ifstream fin("../example.txt");
    
    if (!fin.good())
      throw std::runtime_error("file not found");

    Lexer lexer{fin};
    Parser parser{lexer};
    std::unique_ptr<Expr> ptr = parser.ReadProgram();


    TreeLoggingVisitor logger;

    std::cout << "Before transformation:\n";
    ptr->Accept(&logger);

    ShrinkMutatingVisitor shrinker;
    ptr->Accept(&shrinker);

    std::cout << "After transformation:\n";
    ptr->Accept(&logger);

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
