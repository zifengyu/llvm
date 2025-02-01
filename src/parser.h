#pragma once

#include "ast.h"
#include "lexer.h"

#include <llvm/Support/raw_os_ostream.h>

class Parser {
public:
  explicit Parser(Lexer &lex) : lex(lex), has_error(false) { advance(); }

  AST *parseCalc();
  Expr *parseExpr();
  Expr *parseTerm();
  Expr *parseFactor();

  bool hasError() const { return has_error; }

  AST *parse();

  void error() {
    llvm::errs() << "Unexpected: " << tok.getText() << "\n";
    has_error = true;
  }

  void advance() { lex.next(tok); }

  bool expect(Token::TokenKind kind) {
    if (!tok.is(kind)) {
      error();
      return true;
    }

    return false;
  }

  bool consume(Token::TokenKind kind) {
    if (expect(kind))
      return true;

    advance();
    return false;
  }

private:
  Lexer &lex;
  Token tok;
  bool has_error;
};
