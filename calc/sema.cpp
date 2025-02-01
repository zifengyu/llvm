#include "sema.h"
#include "ast.h"

#include <llvm/ADT/StringSet.h>
#include <llvm/Support/raw_os_ostream.h>

namespace {
class DeclCheck : public ASTVisitor {
public:
  DeclCheck() : has_error(false) {}

  bool hasError() const { return has_error; }

  void visit(Factor &node) override {
    if (node.getKind() == Factor::Ident &&
        scope.find(node.getVal()) == scope.end())
      error(Not, node.getVal());
  }

  void visit(BinaryOp &node) override {
    auto *left = node.getLeft();
    if (left != nullptr)
      left->accept(*this);
    else
      has_error = true;

    auto *right = node.getRight();
    if (right != nullptr)
      right->accept(*this);
    else
      has_error = true;
  }

  void visit(WithDecl &node) override {
    for (auto iter = node.begin(), end = node.end(); iter != end; ++iter) {
      if (!scope.insert(*iter).second) {
        error(Twice, *iter);
      }
    }

    if (auto expr = node.getExpr(); expr != nullptr)
      expr->accept(*this);
    else
      has_error = true;
  }

private:
  enum ErrorType { Twice, Not };

  void error(ErrorType et, llvm::StringRef v) {
    llvm::errs() << "Variable " << v << " " << (et == Twice ? "already" : "not")
                 << " declared\n";
    has_error = true;
  }

  bool has_error;
  llvm::StringSet<> scope;
};
} // namespace

bool Sema::semantic(AST *tree) {
  if (tree == nullptr)
    return false;

  DeclCheck check;
  tree->accept(check);
  return check.hasError();
}

