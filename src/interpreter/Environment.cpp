#include "Environment.hpp"
#include "Error.hpp"
#include <memory>

Autumn::Environment::Environment(Interner &interner)
    : environmentType(EnvironmentType::GLOBAL), interner_(&interner),
      enclosing(nullptr) {}

Autumn::Environment::Environment(EnvironmentPtr enclosingEnv,
                                 EnvironmentType environmentType)
    : environmentType(environmentType),
      interner_(enclosingEnv ? enclosingEnv->interner_ : nullptr),
      enclosing(enclosingEnv) {}

Autumn::Environment *Autumn::Environment::ancestor(int distance) {
  Environment *env = this;
  for (int i = 0; i < distance; i++) {
    env = env->enclosing.get();
  }
  return env;
}

void Autumn::Environment::define(Symbol name,
                                 std::shared_ptr<AutumnValue> value) {
  values[name] = value;
  definitionOrder.push_back(name);
  updateStates[name] = false;
}

void Autumn::Environment::defineType(Symbol name,
                                     std::shared_ptr<AutumnType> value) {
  typeValues[name] = value;
}

std::shared_ptr<Autumn::AutumnValue>
Autumn::Environment::getAt(int distance, const std::string &name) {
  return ancestor(distance)->values[interner_->intern(name)];
}

void Autumn::Environment::assignAt(
    int distance, const Token &name,
    const std::shared_ptr<Autumn::AutumnValue> &value) {
  Symbol sym = interner_->intern(name.lexeme);
  ancestor(distance)->values[sym] = value;
  updateStates[sym] = true;
}

std::shared_ptr<Autumn::AutumnValue>
Autumn::Environment::get(Symbol name) {
  if (auto it = values.find(name); it != values.end()) {
    return it->second;
  }

  if (enclosing != nullptr) {
    return enclosing->get(name);
  }

  throw Error(std::string("Undefined variable '" + *name +
                          "'. Defined arguments are: ") +
              this->printAllDefinedVariablesCrossStack());
}

std::shared_ptr<Autumn::AutumnType>
Autumn::Environment::getTypeValue(Symbol name) {
  if (auto it = typeValues.find(name); it != typeValues.end()) {
    return it->second;
  }

  if (enclosing != nullptr) {
    return enclosing->getTypeValue(name);
  }

  return nullptr;
}

void Autumn::Environment::assign(Symbol name,
                                 const std::shared_ptr<AutumnValue> &value) {
  if (auto it = values.find(name); it != values.end()) {
    auto oldType = it->second->getType();
    auto newType = value->getType();
    if (oldType->toString() != newType->toString() &&
        std::dynamic_pointer_cast<AutumnList>(value) == nullptr) {
      throw Error(std::string("Cannot assign value of type '") +
                  newType->toString() + "' to variable of type '" +
                  oldType->toString() + "' for variable '" + *name + "'.");
    }
    int oldInstId = it->second->getInstId();
    it->second = value;
    it->second->setInstId(oldInstId);
    updateStates[name] = true;
    return;
  } else {
    auto ans = enclosing;
    while (ans != nullptr && ans->values.find(name) == ans->values.end()) {
      ans = ans->enclosing;
    }
    if (ans != nullptr) {
      ans->assign(name, value);
    } else {
      define(name, value);
    }
  }
}

void Autumn::Environment::assignType(Symbol name,
                                     const std::shared_ptr<AutumnType> &type) {
  assignedTypes[name] = type;
}

void Autumn::Environment::resetUpdateStates() {
  for (auto &entry : updateStates) {
    entry.second = false;
  }
}
