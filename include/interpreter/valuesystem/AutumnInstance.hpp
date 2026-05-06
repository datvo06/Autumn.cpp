#ifndef __AUTUMN_INSTANCE_HPP__
#define __AUTUMN_INSTANCE_HPP__
#include "AutumnClass.hpp"
#include "AutumnValue.hpp"
#include <memory>

namespace Autumn {
class AutumnInstance : public AutumnValue {
private:
  std::shared_ptr<AutumnClass> aclass;
  std::unordered_map<std::string, std::shared_ptr<AutumnValue>> fields;

public:
  AutumnInstance(int instId, std::shared_ptr<AutumnClass> aclass,
                 std::vector<std::shared_ptr<AutumnValue>> fieldvalues)
      : AutumnValue(instId, aclass), aclass(aclass) {
    const std::vector<std::string> &fieldnames = aclass->getFieldNames();
    if (fieldnames.size() != fieldvalues.size()) {
      throw Error("Error initializing class " + aclass->name +
                  ": Field names and values do not match: " +
                  std::to_string(fieldnames.size()) + " vs " +
                  std::to_string(fieldvalues.size()));
    }
    for (size_t i = 0; i < fieldnames.size(); i++) {
      if (aclass->getFieldTypes()[i] != nullptr &&
          fieldvalues[i]->getType()->toString() !=
              aclass->getFieldTypes()[i]->toString() && fieldvalues[i]->getType()->toString() != "List<Unknown>") {
        std::cerr << "Field type mismatch: " << fieldnames[i] << " with type "
                  << aclass->getFieldTypes()[i]->toString() << " vs "
                  << fieldvalues[i]->toString() << std::endl;
        throw Error("Field type mismatch: " + fieldnames[i] + " with type " +
                    fieldvalues[i]->getType()->toString() + " vs " +
                    aclass->getFieldTypes()[i]->toString());
      }
      this->fields[fieldnames[i]] = fieldvalues[i];
    }
  }

  AutumnInstance(std::shared_ptr<AutumnClass> aclass,
                 std::vector<std::shared_ptr<AutumnValue>> fieldvalues)
      : AutumnValue(aclass), aclass(aclass) {
    const std::vector<std::string> &fieldnames = aclass->getFieldNames();
    if (fieldnames.size() != fieldvalues.size()) {
      throw Error("Field names and values do not match in class " +
                  aclass->name + ": " + std::to_string(fieldnames.size()) +
                  " vs " + std::to_string(fieldvalues.size()));
    }

    for (size_t i = 0; i < fieldnames.size(); i++) {
      if (aclass->getFieldTypes()[i] != nullptr &&
          fieldvalues[i]->getType()->toString() !=
              aclass->getFieldTypes()[i]->toString() && fieldvalues[i]->getType()->toString() != "List<Unknown>"
              && aclass->getFieldTypes()[i]->toString() != "List<Unknown>"
              ) {
        std::cerr << "Field type mismatch: " << fieldnames[i] << " with type "
                  << aclass->getFieldTypes()[i]->toString() << " vs "
                  << fieldvalues[i]->toString() << std::endl;
        throw Error("Field type mismatch: " + fieldnames[i] + " with type " +
                    fieldvalues[i]->getType()->toString() + " vs " +
                    aclass->getFieldTypes()[i]->toString());
      }
      this->fields[fieldnames[i]] = fieldvalues[i];
    }
  }

  std::string toString() const override {
    std::string s;
    toString(s);
    return s;
  }

  void toString(std::string &acc) const override {
    acc += aclass->name;
    acc += "{";
    int i = 0;
    for (const auto &key : aclass->getFieldNames()) {
      std::shared_ptr<AutumnValue> value = fields.at(key);
      if (value->getType()->toString() !=
              aclass->getFieldTypes()[i]->toString() &&
          aclass->getFieldTypes()[i]->toString() != "List<Unknown>" &&
          value->getType()->toString() != "List<Unknown>") {
        throw Error("Field type mismatch in toString: " + key + " with type " +
                    aclass->getFieldTypes()[i]->toString() + " vs " +
                    value->toString() + " in " + aclass->name);
      }
      if (i != 0) acc += ",";
      acc += key;
      acc += ": ";
      // Indent nested content: emit the value into a temp buffer, then
      // copy it over with a '\t' appended after every newline.
      std::string child;
      value->toString(child);
      for (char c : child) {
        acc += c;
        if (c == '\n') acc += '\t';
      }
      i++;
    }
    if (fields.size() == 0) {
      acc += "None";
    }
    if (aclass->methods.size() != 0) {
      acc += "\tMethods: ";
      for (const auto &method : aclass->methods) {
        acc += "\n\t";
        acc += method.first;
        acc += ": ";
        acc += method.second->toString();
      }
    }
    acc += "}";
  }

  static std::string normalizeName(const std::string &name) {
    // if start with \" or ', remove them
    if (name[0] == '\"' || name[0] == '\'') {
      return name.substr(1, name.size() - 2);
    }
    return name;
  }

  std::string getClassName() { return aclass->name; }

  // Field-only lookup. Throws if `name` is not a field on this instance.
  // For call sites that also need method binding, use getWithMethod(self, name).
  std::shared_ptr<AutumnValue> get(const std::string &name) {
    std::string normalizedName = normalizeName(name);
    auto it = fields.find(normalizedName);
    if (it != fields.end()) {
      return it->second;
    }
    throw Error("Undefined property '" + name + "' for class " +
                aclass->name);
  }

  // Field-or-method lookup. `self` must be a shared_ptr owning this instance;
  // it is used to bind any matching method. Takes self as a parameter so we
  // don't need `enable_shared_from_this`.
  static std::shared_ptr<AutumnValue>
  getWithMethod(const std::shared_ptr<AutumnInstance> &self,
                const std::string &name) {
    std::string normalizedName = normalizeName(name);
    auto it = self->fields.find(normalizedName);
    if (it != self->fields.end()) {
      return it->second;
    }
    std::shared_ptr<AutumnMethod> method =
        self->aclass->findMethod(normalizedName);
    if (method != nullptr) {
      method->setInstance(self);
      return std::dynamic_pointer_cast<AutumnCallableValue>(method);
    }
    throw Error("Undefined property '" + name + "' for class " +
                self->aclass->name);
  }

  void set(const std::string &name, std::shared_ptr<AutumnValue> value) {
    std::string normalizedName = normalizeName(name);
    if (this->fields.find(normalizedName) != fields.end()) {
      this->fields[normalizedName] = value;
      // std::cerr << "All fields: ";
      // for (const auto &[key, value] : fields) {
      //   std::cerr << key << ", ";
      //   std::cerr << value->toString() << std::endl;
      // }
      return;
    }
    throw Error("AutumnInstance::Set - Undefined property '" + normalizedName +
                "', all fields: " + aclass->toString());
  }

  std::shared_ptr<AutumnValue> binop_on(AutumnValue &rhs, const Token &op) override {
    switch (op.type) {
    case TokenType::EQUAL_EQUAL:
      return std::make_shared<AutumnBool>(equal_to(rhs));
    case TokenType::BANG_EQUAL:
      return std::make_shared<AutumnBool>(!equal_to(rhs));
    default:
      throw Error("Operator not supported on instances");
    }
  }

  bool equal_to(AutumnValue &rhs) override { return rhs.equal_by_instance(*this); }

  bool equal_by_instance(AutumnInstance &lhs) override {
    if (lhs.fields.size() != fields.size()) return false;
    for (const auto &[key, value] : lhs.fields) {
      auto it = fields.find(key);
      if (it == fields.end()) return false;
      if (!value) {
        if (it->second) return false;
        continue;
      }
      if (!value->isEqual(it->second)) return false;
    }
    return true;
  }

  std::shared_ptr<AutumnValue> copy() override {
    std::vector<std::shared_ptr<AutumnValue>> fieldvalues;
    fieldvalues.reserve(fields.size());
    for (auto &key : aclass->getFieldNames()) {
      fieldvalues.push_back(fields[key]);
    }

    return std::make_shared<AutumnInstance>(aclass, fieldvalues);
  }

  std::shared_ptr<AutumnClass> getClass() { return aclass; }
};

} // namespace Autumn

#endif // !__AUTUMN_INSTANCE_HPP__
