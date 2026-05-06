#ifndef __AUTUMN_CLASS_HPP__
#define __AUTUMN_CLASS_HPP__
#include "AutumnCallable.hpp"
#include "AutumnCallableValue.hpp"
#include "AutumnType.hpp"
#include <iostream>
namespace Autumn {

class AutumnInstance;

class AutumnMethod : public AutumnCallableValue {
  std::shared_ptr<AutumnInstance> instance;

  static std::shared_ptr<AutumnCallable>
  checkNull(std::shared_ptr<AutumnCallable> method) {
    if (method == nullptr) {
      throw Error("Error Initializing AutumnMethod: method is null");
    }
    return method;
  }

public:
  AutumnMethod(int instId, std::shared_ptr<AutumnCallable> method)
      : AutumnCallableValue(instId, checkNull(method)), instance(nullptr) {
    AutumnCallableValue(instId, checkNull(method));
  }

  AutumnMethod(std::shared_ptr<AutumnCallable> method)
      : AutumnCallableValue(checkNull(method)), instance(nullptr) {}

  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;

  std::string toString() const override;

  void setInstance(std::shared_ptr<AutumnInstance> instance) {
    if (instance == nullptr) {
      std::cerr << "Error Binding Method Instance: instance is null"
                << std::endl;
      throw Error("Error Binding Method Instance: instance is null");
    }
    this->instance = instance;
  }

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnMethod>(callable);
  }
};

class AutumnClass : public AutumnType {
protected:
  std::vector<std::string> fieldnames;
  std::vector<std::shared_ptr<AutumnType>> fieldtypes;
  std::shared_ptr<AutumnCallable> initializer;

public:
  std::string name;
  std::unordered_map<std::string, std::shared_ptr<AutumnMethod>> methods;

  AutumnClass(
      std::string name, std::vector<std::string> fieldnames,
      std::vector<std::shared_ptr<AutumnType>> fieldtypes,
      std::shared_ptr<AutumnCallable> initializer,
      std::unordered_map<std::string, std::shared_ptr<AutumnMethod>> methods)
      : name(name), methods(methods), initializer(initializer),
        fieldnames(fieldnames), fieldtypes(fieldtypes) {}

  std::shared_ptr<AutumnMethod> findMethod(const std::string &name) {
    if (methods.find(name) != methods.end()) {
      return methods[name];
    }
    return nullptr;
  }

  const std::vector<std::string> &getFieldNames() { return fieldnames; }
  std::shared_ptr<AutumnCallable> getInitializer() { return initializer; }
  using AutumnType::toString;
  void toString(std::string &acc) const override {
    acc += "class ";
    acc += name;
    acc += " {";
    for (size_t i = 0; i < fieldnames.size(); i++) {
      acc += fieldnames[i];
      acc += ": ";
      fieldtypes[i]->toString(acc);
      acc += ", ";
    }
    acc += "}";
  }

  const std::vector<std::shared_ptr<AutumnType>> &getFieldTypes() {
    return fieldtypes;
  }
};

class AutumnClassValue : public AutumnValue {
  std::shared_ptr<AutumnClass> aclass;

public:
  AutumnClassValue(std::shared_ptr<AutumnClass> aclass)
      : AutumnValue(aclass), aclass(aclass) {}

  std::string toString() const override { return aclass->toString(); }

  // Note: equal is not reflexive. Figure out if that's intended
  std::shared_ptr<AutumnValue> binop_on(AutumnValue & /*rhs*/, const Token &op) override {
    switch (op.type) {
    case TokenType::EQUAL_EQUAL:
      return std::make_shared<AutumnBool>(false);
    case TokenType::BANG_EQUAL:
      return std::make_shared<AutumnBool>(true);
    default:
      throw Error("Operator not supported on class values");
    }
  }

  bool equal_to(AutumnValue & /*rhs*/) override { return false; }

  std::shared_ptr<AutumnClass> getClass() { return aclass; }

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnClassValue>(aclass);
  }
};

} // namespace Autumn

#endif // !__AUTUMN_CLASS_HPP__
