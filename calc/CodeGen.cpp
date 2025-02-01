#include "CodeGen.h"

#include <llvm/ADT/StringMap.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {
class ToIRVisitor final : public ASTVisitor {
public:
  ToIRVisitor(Module *m) : m(m), builder(m->getContext()) {
    auto &context = m->getContext();
    voidTy = Type::getVoidTy(context);
    int32Ty = Type::getInt32Ty(context);
    ptrTy = PointerType::getUnqual(context);
    int32Zero = ConstantInt::get(int32Ty, 0, true);
  }

  void run(AST *tree) {
    FunctionType *main_fty =
        FunctionType::get(int32Ty, {int32Ty, ptrTy}, false);
    Function *main_fn =
        Function::Create(main_fty, GlobalValue::ExternalLinkage, "main", m);

    BasicBlock *bb = BasicBlock::Create(m->getContext(), "entry", main_fn);
    builder.SetInsertPoint(bb);

    tree->accept(*this);

    auto *calc_write_fn_ty = FunctionType::get(voidTy, {int32Ty}, false);
    auto *calc_write_fn = Function::Create(
        calc_write_fn_ty, GlobalValue::ExternalLinkage, "calc_write", m);
    builder.CreateCall(calc_write_fn_ty, calc_write_fn, {v});

    builder.CreateRet(int32Zero);
  }

  void visit(Factor &node) override {
    if (node.getKind() == Factor::Ident) {
      v = nameMap[node.getVal()];
    } else {
      int int_val;
      node.getVal().getAsInteger(10, int_val);
      v = ConstantInt::get(int32Ty, int_val, true);
    }
  }

  void visit(BinaryOp &node) override {
    node.getLeft()->accept(*this);
    Value *left = v;

    node.getRight()->accept(*this);
    Value *right = v;

    switch (node.getOperator()) {
    case BinaryOp::Plus:
      v = builder.CreateNSWAdd(left, right);
      break;
    case BinaryOp::Minus:
      v = builder.CreateNSWSub(left, right);
      break;
    case BinaryOp::Mul:
      v = builder.CreateNSWMul(left, right);
      break;
    case BinaryOp::Div:
      v = builder.CreateSDiv(left, right);
      break;
    }
  }

  void visit(WithDecl &node) override {
    auto *read_fty = FunctionType::get(int32Ty, {ptrTy}, false);
    auto *read_fn = Function::Create(read_fty, GlobalValue::ExternalLinkage,
                                     "calc_read", m);

    for (auto i = node.begin(), e = node.end(); i != e; ++i) {
      StringRef var = *i;
      Constant *str_text = ConstantDataArray::getString(m->getContext(), var);
      GlobalVariable *str = new GlobalVariable(
          *m, str_text->getType(), true, GlobalVariable::PrivateLinkage,
          str_text, Twine(var).concat(".str"));
      CallInst *call = builder.CreateCall(read_fty, read_fn, {str});
      nameMap[var] = call;
    }

    node.getExpr()->accept(*this);
  }

private:
  Module *m;
  IRBuilder<> builder;
  Type *voidTy;
  Type *int32Ty;
  PointerType *ptrTy;
  Constant *int32Zero;
  Value *v;
  StringMap<Value *> nameMap;
};
} // namespace

void CodeGen::compile(AST *tree) {
  LLVMContext ctx;
  Module *m = new Module("calc.expr", ctx);
  ToIRVisitor to_ir(m);
  to_ir.run(tree);
  m->print(outs(), nullptr);
}
