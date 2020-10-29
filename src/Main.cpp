#include <iostream>
#include <Lexer.h>
#include <Ast.h>

int main() {
  try {
    Lexer l("example.txt");
    Parser p(l);
          auto code = p.ReadProgram()->Exec();
    std::cout << "Program returned code " << code << std::endl;
    return 0;
  }
  catch (const CompileError& e) {
    std::cout << e.what() << std::endl;
  }
}
