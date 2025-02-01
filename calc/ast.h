#pragma once

#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>

class AST;
class Expr;
class Factor;
class BinaryOp;
class WithDecl;

class ASTVisitor {
public:
  virtual void visit(AST &) {}
  virtual void visit(Expr &) {}
  virtual void visit(Factor &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(WithDecl &) = 0;
};

class AST {
public:
  virtual ~AST() = default;
  virtual void accept(ASTVisitor &v) = 0;

  virtual std::string toString(int indent) = 0;
};

class Expr : public AST {};

class Factor : public Expr {
public:
  enum ValueKind { Ident, Number };

  Factor(ValueKind kind, llvm::StringRef val) : kind(kind), val(val) {}

  ValueKind getKind() const { return kind; }
  llvm::StringRef getVal() const { return val; }

  void accept(ASTVisitor &v) override { v.visit(*this); }

  std::string toString(int indent) override;

private:
  ValueKind kind;
  llvm::StringRef val;
};

class BinaryOp : public Expr {
public:
  enum Operator { Plus, Minus, Mul, Div };

  BinaryOp(Operator op, Expr *l, Expr *r) : op(op), l(l), r(r) {}

  Operator getOperator() const { return op; }
  Expr *getLeft() const { return l; }
  Expr *getRight() const { return r; }

  void accept(ASTVisitor &v) override { v.visit(*this); }

  std::string toString(int indent) override;

private:
  Operator op;
  Expr *l;
  Expr *r;
};

class WithDecl : public Expr {
  using VarVector = llvm::SmallVector<llvm::StringRef, 8>;

public:
  WithDecl(llvm::SmallVector<llvm::StringRef, 8> vars, Expr *expr)
      : vars(vars), expr(expr) {}

  VarVector::const_iterator begin() const { return vars.begin(); }
  VarVector::const_iterator end() const { return vars.end(); }
  Expr *getExpr() const { return expr; };

  void accept(ASTVisitor &v) override { v.visit(*this); }

  std::string toString(int indent) override;

private:
  llvm::SmallVector<llvm::StringRef, 8> vars;
  Expr *expr;
};
