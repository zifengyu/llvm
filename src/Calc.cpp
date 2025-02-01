#include "lexer.h"

#include <iostream>

int main() {
  std::string id = "a + 123";
  Lexer lexer(id);
  Token token;
  do {
    lexer.next(token);
    std::cout << token.toString() << " ";
  } while (!token.is(Token::TokenKind::eoi));
  std::cout << std::endl;
  return 0;
}
