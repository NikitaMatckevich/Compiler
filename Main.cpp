#include <iostream>
#include "Lexer.h"
#include "Ast.h"
using namespace std;

int main () { try {
  Lexer L("../program.txt");
  for (auto token : L.tokens) {
    cout << "line " << token.line_number << " " << token.context;
    if (token.is<Number>())
      cout << " {" << token.as<Number>().value << "}";
    cout << endl;
  }
  
  size_t pos = 0;
  auto ptr = readProgram(L, pos);
  int code = ptr->exec();
  cout << "Program returned code " << code << endl;  

  return 0;
}

catch (const exception& e) {
  cerr << e.what() << endl;
}}
