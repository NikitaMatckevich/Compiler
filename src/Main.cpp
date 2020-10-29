#include <Ast.h>
#include <Lexer.h>
#include <iostream>

int main() {
  try {
    Lexer l("example.txt");
    Parser p(l);
    auto code = p.ReadProgram()->Exec();
    std::cout << "Program returned code " << code << std::endl;
    return 0;
  } catch (const CompileError& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
