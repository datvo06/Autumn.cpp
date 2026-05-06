#ifndef __AUTUMN_CALLABLE_VALUE_HPP__
#define __AUTUMN_CALLABLE_VALUE_HPP__

#include "AstPrinter.hpp"
#include "AutumnCallable.hpp"
#include "AutumnValue.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
class AutumnFuncType : public AutumnType {
public:
  explicit AutumnFuncType(SingletonKey) {}
  using AutumnType::toString;
  void toString(std::string &acc) const override { acc += "AutumnFunc"; }
  static std::shared_ptr<AutumnFuncType> getInstance() {
    static auto instance = std::make_shared<AutumnFuncType>(SingletonKey{});
    return instance;
  }
};

class AutumnCallableValue : public AutumnValue {

  std::string callableString;

public:
  std::shared_ptr<AutumnCallable> callable;
  AutumnCallableValue(std::shared_ptr<AutumnCallable> callable)
      : AutumnValue(AutumnFuncType::getInstance()),
        callable(callable) {
    if (callable == nullptr) {
      throw Error("Error Initializing AutumnCallableValue: callable is null");
    }
    callableString = callable->toString();
  }

  AutumnCallableValue(int instId, std::shared_ptr<AutumnCallable> callable)
      : AutumnValue(instId, AutumnFuncType::getInstance()),
        callable(callable) {
    if (callable == nullptr) {
      throw Error(
          "Error Initializing Cloned AutumnCallableValue: callable is null");
    }
    callableString = callable->toString();
  }

  std::shared_ptr<AutumnValue> binop_on(AutumnValue &rhs, const Token &op) override {
    switch (op.type) {
    case TokenType::EQUAL_EQUAL:
      return std::make_shared<AutumnBool>(equal_to(rhs));
    case TokenType::BANG_EQUAL:
      return std::make_shared<AutumnBool>(!equal_to(rhs));
    default:
      throw Error("Operator not supported on callables");
    }
  }

  bool equal_to(AutumnValue &rhs) override { return rhs.equal_by_callable(*this); }

  bool equal_by_callable(AutumnCallableValue &lhs) override {
    return lhs.callable == callable;
  }

  virtual std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
    return callable->call(interpreter, arguments);
  }
  std::string toString() const override {
    if (callable == nullptr) {
      throw Error("Error in CallableValue::toString - callable is null");
    }
    return std::string("<native fn: ") + callable->toString() + ">";
  }

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnCallableValue>(callable);
  }
};
} // namespace Autumn

#endif // __AUTUMN_CALLABLE_VALUE_HPP__
