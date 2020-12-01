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

    std::string path = "../example.txt";
    Lexer lexer{path};
    
    Parser parser{lexer};
    std::unique_ptr<Expr> ptr = parser.ReadProgram();

    LvalueCheckVisitor checker;
    ptr->Accept(&checker);

    std::cout << "Before transformation:\n";
    TreeLoggingVisitor logger;
    ptr->Accept(&logger);

    ShrinkMutatingVisitor shrinker;
    ptr->Accept(&shrinker);

    ExecuteMutatingVisitor executor;
    ptr->Accept(&executor);

    std::cout << "After transformation:\n";
    ptr->Accept(&logger);

    return 0;

  } catch (const CompileError& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
  }
}
