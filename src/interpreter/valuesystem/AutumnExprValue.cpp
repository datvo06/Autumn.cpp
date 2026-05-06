#include "AutumnExprValue.hpp"

namespace Autumn {
std::string AutumnExprValue::toString() const {
  try {
    return std::string("(" + cexpr->prettyPrint() + ": Expr)");
  } catch (Error &e) {
    throw Error(std::string("Error Printing Expr value: ") + e.what());
  }
}

std::shared_ptr<AutumnValue>
AutumnExprValue::binop_on(AutumnValue &rhs, const Token &op) {
  switch (op.type) {
  case TokenType::EQUAL_EQUAL:
    return std::make_shared<AutumnBool>(equal_to(rhs));
  case TokenType::BANG_EQUAL:
    return std::make_shared<AutumnBool>(!equal_to(rhs));
  default:
    throw Error("Operator not supported on expr values");
  }
}

bool AutumnExprValue::equal_to(AutumnValue &rhs) { return rhs.equal_by_expr(*this); }

bool AutumnExprValue::equal_by_expr(AutumnExprValue &lhs) {
  return lhs.cexpr == cexpr;
}
} // namespace Autumn
