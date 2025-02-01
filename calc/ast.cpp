#include "ast.h"

#include <sstream>

namespace {
void addIndent(std::stringstream &ss, int indent) {
  for (int i = 0; i < indent; ++i)
    ss << " ";
}
} // namespace

std::string Factor::toString(int indent) {
  std::stringstream ss;
  addIndent(ss, indent);
  ss << "Factor(" << std::string_view(val) << ")";
  return ss.str();
}

std::string BinaryOp::toString(int indent) {
  std::stringstream ss;
  addIndent(ss, indent);
  switch (op) {
  case Plus:
    ss << "PLUS";
    break;
  case Minus:
    ss << "MINUS";
    break;
  case Mul:
    ss << "MULTIPLY";
    break;
  case Div:
    ss << "DIVIDE";
    break;
  }
  ss << "\n";
  ss << l->toString(indent + 2) << "\n";
  ss << r->toString(indent + 2);
  return ss.str();
}

std::string WithDecl::toString(int indent) {
  std::stringstream ss;
  addIndent(ss, indent);
  ss << "WITH";
  return ss.str();
}

