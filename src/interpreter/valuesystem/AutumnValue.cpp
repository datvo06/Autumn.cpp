#include "AutumnValue.hpp"
#include "AutumnInstance.hpp"

namespace Autumn {
int AutumnValue::instCount = 0;

// ---------------------------------------------------------------------------
// Unary
// ---------------------------------------------------------------------------

std::shared_ptr<AutumnValue> AutumnValue::eval_unary(const Token &op) {
  switch (op.type) {
  case TokenType::MINUS:
    throw Error("Unary - must be applied to a number");
  case TokenType::PLUS:
    throw Error("Unary + must be applied to a number");
  case TokenType::BANG:
    throw Error("Unary ! must be applied to a boolean");
  default:
    throw Error("Unknown unary operator");
  }
}

std::shared_ptr<AutumnValue>
AutumnNumber::eval_unary(const Token &op) {
  auto result = getNumber();
  switch (op.type) {
  case TokenType::PLUS:
    break;
  case TokenType::MINUS:
    result = -result;
    break;
  case TokenType::BANG:
    throw Error("Unary ! must be applied to a boolean");
  default:
    throw Error("Unknown unary operator");
  }
  return std::make_shared<AutumnNumber>(result);
}

std::shared_ptr<AutumnValue>
AutumnBool::eval_unary(const Token &op) {
  switch (op.type) {
  case TokenType::BANG:
    return std::make_shared<AutumnBool>(!getBool());
  case TokenType::MINUS:
    throw Error("Unary - must be applied to a number");
  case TokenType::PLUS:
    throw Error("Unary + must be applied to a number");
  default:
    throw Error("Unknown unary operator");
  }
}

// ---------------------------------------------------------------------------
// AutumnNumber — the only type that supports arithmetic and ordering.
// ---------------------------------------------------------------------------

std::shared_ptr<AutumnValue>
AutumnNumber::binop_on(AutumnValue &rhs, const Token &op) {
  int lhs = getNumber();
  switch (op.type) {
  case TokenType::PLUS:
    return std::make_shared<AutumnNumber>(rhs.add_by_number(lhs));
  case TokenType::MINUS:
    return std::make_shared<AutumnNumber>(rhs.sub_by_number(lhs));
  case TokenType::STAR:
    return std::make_shared<AutumnNumber>(rhs.mul_by_number(lhs));
  case TokenType::SLASH:
    return std::make_shared<AutumnNumber>(rhs.div_by_number(lhs));
  case TokenType::MODULO:
    return std::make_shared<AutumnNumber>(rhs.mod_by_number(lhs));
  case TokenType::GREATER:
    return std::make_shared<AutumnBool>(rhs.greater_by_number(lhs));
  case TokenType::GREATER_EQUAL:
    return std::make_shared<AutumnBool>(rhs.greater_equal_by_number(lhs));
  case TokenType::LESS:
    return std::make_shared<AutumnBool>(rhs.less_by_number(lhs));
  case TokenType::LESS_EQUAL:
    return std::make_shared<AutumnBool>(rhs.less_equal_by_number(lhs));
  case TokenType::EQUAL_EQUAL:
    return std::make_shared<AutumnBool>(equal_to(rhs));
  case TokenType::BANG_EQUAL:
    return std::make_shared<AutumnBool>(!equal_to(rhs));
  default:
    throw Error("Unknown binary operator for number");
  }
}

bool AutumnNumber::equal_to(AutumnValue &rhs) { return rhs.equal_by_number(getNumber()); }

int AutumnNumber::add_by_number(int lhs) { return lhs + getNumber(); }
int AutumnNumber::sub_by_number(int lhs) { return lhs - getNumber(); }
int AutumnNumber::mul_by_number(int lhs) { return lhs * getNumber(); }
int AutumnNumber::div_by_number(int lhs) { return lhs / getNumber(); }
int AutumnNumber::mod_by_number(int lhs) { return lhs % getNumber(); }

bool AutumnNumber::greater_by_number(int lhs)       { return lhs >  getNumber(); }
bool AutumnNumber::greater_equal_by_number(int lhs) { return lhs >= getNumber(); }
bool AutumnNumber::less_by_number(int lhs)          { return lhs <  getNumber(); }
bool AutumnNumber::less_equal_by_number(int lhs)    { return lhs <= getNumber(); }

bool AutumnNumber::equal_by_number(int lhs) { return lhs == getNumber(); }

// ---------------------------------------------------------------------------
// AutumnString — only equality.
// ---------------------------------------------------------------------------

std::shared_ptr<AutumnValue>
AutumnString::binop_on(AutumnValue &rhs, const Token &op) {
  switch (op.type) {
  case TokenType::EQUAL_EQUAL:
    return std::make_shared<AutumnBool>(equal_to(rhs));
  case TokenType::BANG_EQUAL:
    return std::make_shared<AutumnBool>(!equal_to(rhs));
  default:
    throw Error("Operator not supported on strings");
  }
}

bool AutumnString::equal_to(AutumnValue &rhs) { return rhs.equal_by_string(getString()); }

bool AutumnString::equal_by_string(const std::string &lhs) { return lhs == getString(); }

// ---------------------------------------------------------------------------
// AutumnBool — only equality, with the bool↔number asymmetric coercion
// preserved from the legacy isEqual.
// ---------------------------------------------------------------------------

std::shared_ptr<AutumnValue>
AutumnBool::binop_on(AutumnValue &rhs, const Token &op) {
  switch (op.type) {
  case TokenType::EQUAL_EQUAL:
    return std::make_shared<AutumnBool>(equal_to(rhs));
  case TokenType::BANG_EQUAL:
    return std::make_shared<AutumnBool>(!equal_to(rhs));
  default:
    throw Error("Operator not supported on booleans");
  }
}

bool AutumnBool::equal_to(AutumnValue &rhs) { return rhs.equal_by_bool(getBool()); }

bool AutumnBool::equal_by_bool(bool lhs) { return lhs == getBool(); }

// Called when LHS is a number and RHS is a bool: coerce bool to 1/0.
bool AutumnBool::equal_by_number(int lhs) { return lhs == (getBool() ? 1 : 0); }

// ---------------------------------------------------------------------------
// AutumnList — only equality; element-wise recursive via isEqual.
// ---------------------------------------------------------------------------

std::shared_ptr<AutumnValue>
AutumnList::binop_on(AutumnValue &rhs, const Token &op) {
  switch (op.type) {
  case TokenType::EQUAL_EQUAL:
    return std::make_shared<AutumnBool>(equal_to(rhs));
  case TokenType::BANG_EQUAL:
    return std::make_shared<AutumnBool>(!equal_to(rhs));
  default:
    throw Error("Operator not supported on lists");
  }
}

bool AutumnList::equal_to(AutumnValue &rhs) { return rhs.equal_by_list(*getValues()); }

bool AutumnList::equal_by_list(
    const std::vector<std::shared_ptr<AutumnValue>> &lhs) {
  auto &rhsVec = *getValues();
  if (lhs.size() != rhsVec.size()) return false;
  for (size_t i = 0; i < lhs.size(); i++) {
    if (!lhs[i]) {
      if (rhsVec[i]) return false;
      continue;
    }
    if (!lhs[i]->isEqual(rhsVec[i])) return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// AutumnList::add — mutates the underlying vector in place.
// ---------------------------------------------------------------------------

void AutumnList::add(std::shared_ptr<AutumnValue> elem) {
  auto plist = values;
  if (plist == nullptr) {
    throw Error("AutumnList::add Error: List is null");
  }
  if (elem == nullptr) {
    throw Error("AutumnList::add Error: Element is null");
  }
  auto instElem = std::dynamic_pointer_cast<AutumnInstance>(elem);
  if (elem->getType()->toString() !=
          std::dynamic_pointer_cast<AutumnListType>(type)
              ->getElementType()
              ->toString() &&
      std::dynamic_pointer_cast<AutumnUnknownType>(
          std::dynamic_pointer_cast<AutumnListType>(type)
              ->getElementType()) == nullptr &&
      instElem == nullptr) {
    throw Error("List element type mismatch: got " +
                elem->getType()->toString() + " expected " +
                std::dynamic_pointer_cast<AutumnListType>(type)
                    ->getElementType()
                    ->toString());
  }
  this->type = AutumnListType::getInstance(elem->getType());
  plist->push_back(elem);
}

} // namespace Autumn
