#include <Lexer.h>
#include <Visitors.h>

#include <iostream>
#include <vector>

int main() {
  try {
    Lexer lexer("example.txt");
    Parser parser(lexer);
    auto ptr = parser.ReadProgram();

    ShrinkOneChildBranchesVisitor opt;
    ptr->Accept(&opt);
    auto transformed_program = std::move(opt).GetResult();

    std::cout << "Program returned code " << 0 << std::endl;
    return 0;
  } catch (const CompileError& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}
