#include "CodeGen.h"
#include "lexer.h"
#include "parser.h"
#include "sema.h"

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/raw_ostream.h>

#include <string>

static llvm::cl::opt<std::string> Input(llvm::cl::Positional,
                                        llvm::cl::desc("<input expression>"),
                                        llvm::cl::init(""));

int main(int argc, const char **argv) {
  llvm::InitLLVM x(argc, argv);
  llvm::cl::ParseCommandLineOptions(argc, argv,
                                    "calc - the expression compiler\n");

  Lexer lex(Input);
  Parser parser(lex);

  AST *tree = parser.parse();
  if (!tree || parser.hasError()) {
    llvm::errs() << "Syntax errors occurred\n";
    return 1;
  }

  Sema semantic;
  if (semantic.semantic(tree)) {
    llvm::errs() << "Semantic errors occurred\n";
    return 1;
  }

  CodeGen code_generator;
  code_generator.compile(tree);

  return 0;
}
