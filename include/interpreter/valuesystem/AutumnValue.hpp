#ifndef __AUTUMN_VALUE_HPP__
#define __AUTUMN_VALUE_HPP__

#include "AutumnType.hpp"
#include "Error.hpp"
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace Autumn {

class Interpreter;
class AutumnInstance;
class AutumnExprValue;
class AutumnCallableValue;
class AutumnValue;

// Non-owning view over the primitive payload of an AutumnValue.
// Used by stdlib introspection (IsOutsideBounds, isFreePos, etc.). Callers
// must not retain the view beyond the lifetime of the source value.
using AutumnValueData = std::variant<
    int,
    bool,
    std::string_view,
    std::span<const std::shared_ptr<AutumnValue>>
>;

class AutumnValue {
  static int instCount;

protected:
  int instId;
  AutumnValue(int instId, std::shared_ptr<AutumnType> type)
      : instId(instId), type(type) {}

  std::shared_ptr<AutumnType> type;

public:
  AutumnValue(std::shared_ptr<AutumnType> type) : type(type) {
    instId = instCount++;
  }

  virtual std::string toString() const = 0;
  virtual void toString(std::string &acc) const { acc += toString(); }

  // Binary operator dispatch (visitor-style double dispatch). See the
  // refactor notes in AutumnValue.cpp for the full method matrix.
  virtual std::shared_ptr<AutumnValue> binop_on(AutumnValue &rhs, const Token &op) = 0;
  virtual bool equal_to(AutumnValue &rhs) = 0;

  virtual int add_by_number(int lhs) { throw Error("Binary + requires two numbers"); }
  virtual int sub_by_number(int lhs) { throw Error("Binary - requires two numbers"); }
  virtual int mul_by_number(int lhs) { throw Error("Binary * requires two numbers"); }
  virtual int div_by_number(int lhs) { throw Error("Binary / requires two numbers"); }
  virtual int mod_by_number(int lhs) { throw Error("Binary % requires two numbers"); }

  virtual bool greater_by_number(int lhs)       { throw Error("Binary > requires two numbers"); }
  virtual bool greater_equal_by_number(int lhs) { throw Error("Binary >= requires two numbers"); }
  virtual bool less_by_number(int lhs)          { throw Error("Binary < requires two numbers"); }
  virtual bool less_equal_by_number(int lhs)    { throw Error("Binary <= requires two numbers"); }

  virtual bool equal_by_number(int /*lhs*/)                                        { return false; }
  virtual bool equal_by_bool(bool /*lhs*/)                                         { return false; }
  virtual bool equal_by_string(const std::string & /*lhs*/)                        { return false; }
  virtual bool equal_by_list(const std::vector<std::shared_ptr<AutumnValue>> & /*lhs*/) { return false; }
  virtual bool equal_by_instance(AutumnInstance & /*lhs*/)                         { return false; }
  virtual bool equal_by_expr(AutumnExprValue & /*lhs*/)                            { return false; }
  virtual bool equal_by_callable(AutumnCallableValue & /*lhs*/)                    { return false; }

  // Thin wrapper kept for legacy callers. Funnels through equal_to.
  bool isEqual(std::shared_ptr<AutumnValue> other) {
    if (!other) return false;
    return equal_to(*other);
  }

  virtual std::shared_ptr<AutumnValue> eval_unary(const Token &op);
  virtual ~AutumnValue() = default;
  virtual bool isTruthy() { return true; }
  int getInstId() { return instId; }
  void setInstId(int instId) { this->instId = instId; }

  // Primitive-value introspection for stdlib. Default throws — only the
  // four primitive subclasses override.
  virtual AutumnValueData getValue() {
    throw Error("getValue() not supported on this AutumnValue kind");
  }

  virtual std::shared_ptr<AutumnValue> copy() = 0;
  std::shared_ptr<AutumnType> getType() { return type; }
};

class AutumnNumber final : public AutumnValue {
  int value;

public:
  AutumnNumber(int instId, int value)
      : AutumnValue(instId, AutumnNumberType::getInstance()), value(value) {}
  AutumnNumber(int value)
      : AutumnValue(AutumnNumberType::getInstance()), value(value) {}

  std::string toString() const override {
    return "(" + std::to_string(value) + ": N)";
  }

  AutumnValueData getValue() override { return value; }
  int getNumber() const { return value; }

  std::shared_ptr<AutumnValue> eval_unary(const Token &op) override;
  std::shared_ptr<AutumnValue> binop_on(AutumnValue &rhs, const Token &op) override;
  bool equal_to(AutumnValue &rhs) override;

  int add_by_number(int lhs) override;
  int sub_by_number(int lhs) override;
  int mul_by_number(int lhs) override;
  int div_by_number(int lhs) override;
  int mod_by_number(int lhs) override;

  bool greater_by_number(int lhs) override;
  bool greater_equal_by_number(int lhs) override;
  bool less_by_number(int lhs) override;
  bool less_equal_by_number(int lhs) override;

  bool equal_by_number(int lhs) override;

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnNumber>(value);
  }
};

class AutumnString final : public AutumnValue {
  std::string value;

public:
  AutumnString(int instId, std::string value)
      : AutumnValue(instId, AutumnStringType::getInstance()), value(std::move(value)) {}
  AutumnString(std::string value)
      : AutumnValue(AutumnStringType::getInstance()), value(std::move(value)) {}

  std::string toString() const override {
    return "(" + value + ": S)";
  }

  AutumnValueData getValue() override { return std::string_view(value); }
  const std::string &getString() const { return value; }

  std::shared_ptr<AutumnValue> binop_on(AutumnValue &rhs, const Token &op) override;
  bool equal_to(AutumnValue &rhs) override;
  bool equal_by_string(const std::string &lhs) override;

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnString>(value);
  }
};

class AutumnBool final : public AutumnValue {
  bool value;

public:
  AutumnBool(int instId, bool value)
      : AutumnValue(instId, AutumnBoolType::getInstance()), value(value) {}
  AutumnBool(bool value)
      : AutumnValue(AutumnBoolType::getInstance()), value(value) {}

  std::string toString() const override {
    return std::string("(") + (value ? "true" : "false") + ": " + type->toString() + ")";
  }
  bool isTruthy() override { return value; }

  AutumnValueData getValue() override { return value; }
  bool getBool() const { return value; }

  std::shared_ptr<AutumnValue> eval_unary(const Token &op) override;

  std::shared_ptr<AutumnValue> binop_on(AutumnValue &rhs, const Token &op) override;
  bool equal_to(AutumnValue &rhs) override;
  bool equal_by_bool(bool lhs) override;
  // Legacy coercion: number lhs equals bool rhs iff lhs == (value ? 1 : 0).
  // Reverse (bool == number) is not coerced.
  bool equal_by_number(int lhs) override;

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnBool>(value);
  }
};

class AutumnList final : public AutumnValue {
  std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> values;

public:
  static std::shared_ptr<AutumnType>
  inferType(const std::vector<std::shared_ptr<AutumnValue>> &values) {
    std::shared_ptr<AutumnType> type = AutumnUnknownType::getInstance();
    for (size_t i = 0; i < values.size(); i++) {
      if (values[i]->getType() == AutumnUnknownType::getInstance()) {
        continue;
      }
      if (type == AutumnUnknownType::getInstance()) {
        type = values[i]->getType();
      } else if (type != values[i]->getType()) {
        return AutumnUnknownType::getInstance();
      }
    }
    return AutumnListType::getInstance(type);
  }

  AutumnList(int instId,
             std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> values)
      : AutumnValue(instId, inferType(*values)), values(std::move(values)) {}

  AutumnList(std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> values)
      : AutumnValue(inferType(*values)), values(std::move(values)) {}

  AutumnList()
      : AutumnValue(AutumnListType::getInstance()),
        values(std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>()) {}

  AutumnValueData getValue() override {
    return std::span<const std::shared_ptr<AutumnValue>>(*values);
  }

  std::shared_ptr<AutumnValue> binop_on(AutumnValue &rhs, const Token &op) override;
  bool equal_to(AutumnValue &rhs) override;
  bool equal_by_list(const std::vector<std::shared_ptr<AutumnValue>> &lhs) override;

  std::string toString() const override {
    std::string s;
    toString(s);
    return s;
  }

  void toString(std::string &acc) const override {
    if (!values) {
      acc += "([Null List])";
      return;
    }
    acc += "([";
    for (size_t i = 0; i < values->size(); i++) {
      auto &element = (*values)[i];
      if (!element) {
        acc += "null";
      } else {
        try {
          element->toString(acc);
        } catch (const std::exception &e) {
          acc += "error";
        }
      }
      if (i != values->size() - 1) acc += ", ";
    }
    acc += "] :";
    if (type) type->toString(acc);
    else acc += "Unknown Type";
    acc += ")";
  }

  std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> getValues() {
    return values;
  }

  void add(std::shared_ptr<AutumnValue> elem);

  bool isTruthy() override {
    return values && !values->empty();
  }

  std::shared_ptr<AutumnValue> copy() override {
    std::vector<std::shared_ptr<AutumnValue>> newlist;
    newlist.reserve(values->size());
    for (const auto &v : *values) {
      newlist.push_back(v->copy());
    }
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(std::move(newlist)));
  }
};

} // namespace Autumn
#endif
