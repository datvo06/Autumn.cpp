#ifndef AUTUMN_EXPR_VALUE_HPP
#define AUTUMN_EXPR_VALUE_HPP

#include "AutumnType.hpp"
#include "AutumnValue.hpp"
#include "Expr.hpp"
#include "Environment.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Autumn {

class AutumnExprType : public AutumnType {
public:
  explicit AutumnExprType(SingletonKey) {}
  using AutumnType::toString;

  void toString(std::string &acc) const override { acc += "Expr"; }

  static std::shared_ptr<AutumnExprType> getInstance() {
    static auto instance = std::make_shared<AutumnExprType>(SingletonKey{});
    return instance;
  }
};

class AutumnExprValue : public AutumnValue {
private:
  std::shared_ptr<Expr> cexpr;
  std::shared_ptr<Environment> cenv;

public:
  AutumnExprValue(int instId, std::shared_ptr<Expr> cexpr, std::shared_ptr<Environment> cenv)
      : AutumnValue(instId, AutumnExprType::getInstance()),
        cexpr(cexpr), cenv(cenv) {}

  AutumnExprValue(std::shared_ptr<Expr> cexpr, std::shared_ptr<Environment> cenv)
      : AutumnValue(AutumnExprType::getInstance()), cexpr(cexpr), cenv(cenv) {}

  std::string toString() const override;

  std::shared_ptr<AutumnValue> binop_on(AutumnValue &rhs, const Token &op) override;
  bool equal_to(AutumnValue &rhs) override;
  bool equal_by_expr(AutumnExprValue &lhs) override;

  std::shared_ptr<Expr> getExpr() { return cexpr; }
  std::shared_ptr<Environment> getCenv() { return cenv; }

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnExprValue>(cexpr, cenv);
  }
};

} // namespace Autumn

#endif
