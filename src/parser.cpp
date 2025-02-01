#include "parser.h"

AST *Parser::parse() {
  auto *res = parseCalc();
  expect(Token::eoi);
  return res;
}

AST *Parser::parseCalc() {
  Expr *expr;
  llvm::SmallVector<llvm::StringRef, 8> vars;

  if (tok.is(Token::TokenKind::KW_with)) {
    advance();

    if (expect(Token::TokenKind::ident)) {
      goto _error;
    }

    vars.push_back(tok.getText());
    advance();

    while (tok.is(Token::TokenKind::comma)) {
      advance();
      if (expect(Token::TokenKind::ident)) {
        goto _error;
      }

      vars.push_back(tok.getText());
      advance();
    }

    if (consume(Token::TokenKind::colon)) {
      goto _error;
    }
  }

  expr = parseExpr();

  if (vars.empty())
    return expr;

  return new WithDecl(vars, expr);

_error:
  while (!tok.is(Token::TokenKind::eoi))
    advance();

  return nullptr;
}

Expr *Parser::parseExpr() {
  auto *left = parseTerm();
  while (tok.isOneOf(Token::TokenKind::plus, Token::TokenKind::minus)) {
    auto op = tok.is(Token::TokenKind::plus) ? BinaryOp::Operator::Plus
                                             : BinaryOp::Operator::Minus;
    advance();
    auto *right = parseTerm();
    left = new BinaryOp(op, left, right);
  }
  return left;
}

Expr *Parser::parseTerm() {
  auto *left = parseFactor();
  while (tok.isOneOf(Token::TokenKind::star, Token::TokenKind::slash)) {
    auto op = tok.is(Token::TokenKind::star) ? BinaryOp::Operator::Mul
                                             : BinaryOp::Operator::Div;
    advance();
    auto *right = parseFactor();
    left = new BinaryOp(op, left, right);
  }
  return left;
}

Expr *Parser::parseFactor() {
  Expr *res = nullptr;
  switch (tok.getKind()) {
  case Token::TokenKind::ident: {
    res = new Factor(Factor::ValueKind::Ident, tok.getText());
    advance();
    break;
  }
  case Token::TokenKind::number: {
    res = new Factor(Factor::ValueKind::Number, tok.getText());
    advance();
    break;
  }
  case Token::TokenKind::l_paren: {
    advance();
    res = parseExpr();
    if (!consume(Token::TokenKind::r_paren))
      break;
  }
  default:
    if (!res)
      error();

    while (!tok.isOneOf(Token::TokenKind::r_paren, Token::star, Token::plus,
                        Token::minus, Token::slash, Token::eoi)) {
      advance();
    }
  }
  return res;
}
