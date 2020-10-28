#include <iostream>
#include <Lexer.h>
#include <Ast.h>
using namespace std;

int main() {
try {
  lexer L("example.txt");
  parser P(L);
	auto code = P.read_program()->exec();
  cout << "Program returned code " << code << endl;
  return 0;
}
catch (const compile_error& e) {
  cout << e.what() << endl;
}
}
