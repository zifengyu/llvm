#include "lexer.h"

#include <sstream>

namespace charinfo {

LLVM_READNONE inline bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\f' || c == '\v';
}

LLVM_READNONE inline bool isDigit(char c) { return c >= '0' && c <= '9'; }

LLVM_READNONE inline bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

} // namespace charinfo

std::string Token::toString() const {
  std::stringstream ss;
  switch (kind) {
  case TokenKind::eoi:
    ss << "EOI";
    break;
  case TokenKind::unknown:
    ss << "UNKNOWN";
    break;
  case TokenKind::ident:
    ss << "IDENT(" << std::string_view(text) << ")";
    break;
  case TokenKind::number:
    ss << "NUMBER(" << std::string_view(text) << ")";
    break;
  case TokenKind::comma:
    ss << "COMMA";
    break;
  case TokenKind::colon:
    ss << "COLON";
    break;
  case TokenKind::plus:
    ss << "PLUS";
    break;
  case TokenKind::minus:
    ss << "MINUS";
    break;
  case TokenKind::star:
    ss << "STAR";
    break;
  case TokenKind::slash:
    ss << "SLASH";
    break;
  case TokenKind::l_paren:
    ss << "L_PAREN";
    break;
  case TokenKind::r_paren:
    ss << "R_PAREN";
    break;
  case TokenKind::KW_with:
    ss << "WITH";
    break;
  default:
    ss << "invalid";
  }

  return ss.str();
}

void Lexer::formToken(Token &result, const char *token_end,
                      Token::TokenKind kind) {
  result.kind = kind;
  result.text = llvm::StringRef(buffer_ptr, token_end - buffer_ptr);
  buffer_ptr = token_end;
}

void Lexer::next(Token &token) {
  while (*buffer_ptr && charinfo::isWhitespace(*buffer_ptr))
    ++buffer_ptr;

  if (!*buffer_ptr) {
    token.kind = Token::TokenKind::eoi;
    return;
  }

  if (charinfo::isLetter(*buffer_ptr)) {
    auto *end = buffer_ptr + 1;
    while (*end && charinfo::isLetter(*end))
      ++end;
    llvm::StringRef name(buffer_ptr, end - buffer_ptr);
    auto kind =
        name == "with" ? Token::TokenKind::KW_with : Token::TokenKind::ident;
    formToken(token, end, kind);
    return;
  }

  if (charinfo::isDigit(*buffer_ptr)) {
    auto *end = buffer_ptr + 1;
    while (*end && charinfo::isDigit(*end))
      ++end;
    llvm::StringRef number(buffer_ptr, end - buffer_ptr);
    formToken(token, end, Token::TokenKind::number);
    return;
  }

  switch (*buffer_ptr) {
#define CASE(ch, tok)                                                          \
  case ch:                                                                     \
    formToken(token, buffer_ptr + 1, tok);                                     \
    return;

    CASE('+', Token::TokenKind::plus);
    CASE('-', Token::TokenKind::minus);
    CASE('*', Token::TokenKind::star);
    CASE('/', Token::TokenKind::slash);
    CASE('(', Token::TokenKind::l_paren);
    CASE(')', Token::TokenKind::r_paren);
    CASE(':', Token::TokenKind::colon);
    CASE(',', Token::TokenKind::comma);

#undef CASE

  default:
    formToken(token, buffer_ptr + 1, Token::unknown);
  }
}
