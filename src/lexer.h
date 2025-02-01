#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>

class Lexer;
class Token {
  friend class Lexer;

public:
  enum TokenKind : unsigned short {
    eoi,
    unknown,
    ident,
    number,
    comma,
    colon,
    plus,
    minus,
    star,
    slash,
    l_paren,
    r_paren,
    KW_with,
  };

  TokenKind getKind() const { return kind; }

  llvm::StringRef getText() const { return text; }

  bool is(TokenKind k) const { return kind == k; }

  bool isOneOf(TokenKind k1) const { return is(k1); }

  template <typename... Ts> bool isOneOf(TokenKind k1, Ts... ks) const {
    return is(k1) || isOneOf(ks...);
  }

  std::string toString() const;

private:
  TokenKind kind;
  llvm::StringRef text;
};

class Lexer {
public:
  explicit Lexer(const llvm::StringRef &buffer)
      : buffer_start(buffer.begin()), buffer_ptr(buffer.begin()) {}

  void next(Token &token);

private:
  void formToken(Token &result, const char *token_end, Token::TokenKind kind);

  const char *buffer_start;
  const char *buffer_ptr;
};
