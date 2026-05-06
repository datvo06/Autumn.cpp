#ifndef _AUTUMN_ENVIRONMENT_HPP
#define _AUTUMN_ENVIRONMENT_HPP

#include "AutumnType.hpp"
#include "AutumnValue.hpp"
#include "Interner.hpp"
#include "Token.hpp"
#include <any>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <regex>
enum class EnvironmentType {
  GLOBAL,
  LOCAL,
  INSTANCE,
};
namespace Autumn {
class Environment;

using EnvironmentPtr = std::shared_ptr<Environment>;
struct pair_hash {
  inline std::size_t operator()(const std::pair<int, int> &v) const {
    return v.first * 31 + v.second;
  }
};

class Environment {
private:
  std::unordered_map<Symbol, std::shared_ptr<AutumnValue>> values;

  std::unordered_map<Symbol, bool> updateStates;
  std::unordered_map<Symbol, std::shared_ptr<AutumnType>> typeValues;
  std::unordered_map<Symbol, std::any> assignedTypes;

  std::vector<Symbol> definitionOrder;

  std::unordered_set<std::pair<int, int>, pair_hash> occupiedPositions;
  EnvironmentType environmentType;

  // Used by the string-based overloads to intern on-the-fly. Inherited from
  // the enclosing environment for sub-scopes; must be provided for globals.
  Interner *interner_;

protected:
  EnvironmentPtr enclosing;


public:
  // Constructors

  // Top-level constructor: caller must supply the Interner that will outlive
  // this environment and all its nested scopes.
  explicit Environment(Interner &interner);

  // Constructor with an enclosing environment: interner is inherited.
  Environment(EnvironmentPtr enclosingEnv, EnvironmentType environmentType=EnvironmentType::LOCAL);

  const std::vector<Symbol> &getDefinitionOrder() { return definitionOrder; }

  Interner &getInterner() { return *interner_; }

  void clearOccupied() { occupiedPositions.clear(); }

  void occupyPosition(int x, int y) { occupiedPositions.insert({x, y}); }

  bool isFreePos(int x, int y) {
    return occupiedPositions.find({x, y}) == occupiedPositions.end();
  }

  // --- Symbol-based hot-path APIs ---
  void define(Symbol name, std::shared_ptr<AutumnValue> value);
  void defineType(Symbol name, std::shared_ptr<AutumnType> type);
  std::shared_ptr<AutumnValue> get(Symbol name);
  std::shared_ptr<AutumnType> getTypeValue(Symbol name);
  void assign(Symbol name, const std::shared_ptr<AutumnValue> &value);
  void assignType(Symbol name, const std::shared_ptr<AutumnType> &type);

  bool isDefined(Symbol name) { return values.find(name) != values.end(); }

  // --- String-based overloads (intern on call; callers that have a pre-
  //      computed Symbol should prefer the Symbol-based versions above). ---
  void define(const std::string &name, std::shared_ptr<AutumnValue> value) {
    define(interner_->intern(name), std::move(value));
  }
  void defineType(const std::string &name, std::shared_ptr<AutumnType> type) {
    defineType(interner_->intern(name), std::move(type));
  }
  std::shared_ptr<AutumnValue> get(const std::string &name) {
    return get(interner_->intern(name));
  }
  std::shared_ptr<AutumnValue> get(const Token &name) {
    return get(interner_->intern(name.lexeme));
  }
  std::shared_ptr<AutumnType> getTypeValue(const Token &name) {
    return getTypeValue(interner_->intern(name.lexeme));
  }
  void assign(const Token &name, const std::shared_ptr<AutumnValue> &value) {
    assign(interner_->intern(name.lexeme), value);
  }
  void assign(const std::string &name,
              const std::shared_ptr<AutumnValue> &value) {
    assign(interner_->intern(name), value);
  }
  void assignType(const std::string &name,
                  const std::shared_ptr<AutumnType> &type) {
    assignType(interner_->intern(name), type);
  }
  bool isDefined(const std::string &name) {
    return values.find(interner_->intern(name)) != values.end();
  }

  // Get the value of a variable at a certain distance in the environment chain
  std::shared_ptr<AutumnValue> getAt(int distance, const std::string &name);

  // Assign a new value to a variable at a certain distance in the environment chain
  void assignAt(int distance, const Token &name,
                const std::shared_ptr<AutumnValue> &value);

  std::shared_ptr<Environment> getEnclosing() { return enclosing; }

  std::shared_ptr<AutumnType> getAssignedType(Symbol name) {
    if (auto it = assignedTypes.find(name); it != assignedTypes.end()) {
      return std::any_cast<std::shared_ptr<AutumnType>>(it->second);
    }
    if (enclosing != nullptr) {
      return enclosing->getAssignedType(name);
    }
    return nullptr;
  }
  std::shared_ptr<AutumnType> getAssignedType(const std::string &name) {
    return getAssignedType(interner_->intern(name));
  }

  void resetUpdateStates();
  std::string printAllAssignedTypes() {
    std::string result = "";
    for (const auto &[key, value] : assignedTypes) {
      result += *key + " : " +
                std::any_cast<std::shared_ptr<AutumnType>>(value)->toString() +
                "\n";
    }
    return result;
  }

  bool isUpdated(Symbol name) {
    if (auto it = updateStates.find(name); it != updateStates.end()) {
      return it->second;
    }
    if (enclosing != nullptr) {
      return enclosing->isUpdated(name);
    }
    return false;
  }
  bool isUpdated(const std::string &name) {
    return isUpdated(interner_->intern(name));
  }

  std::string printAllDefinedVariables() {
    std::string result = "";
    for (const auto &[key, value] : values) {
      result += *key + " : " + value->toString() + "\n";
    }
    return result;
  }

  std::string printAllDefinedVariablesCrossStack() {
    std::string result = "";
    for (const auto &[key, value] : values) {
      result += *key + " : " + value->toString() + "\n";
    }
    if (enclosing != nullptr) {
      result += enclosing->printAllDefinedVariablesCrossStack();
    }
    return result;
  }

  const std::unordered_map<Symbol, std::shared_ptr<AutumnValue>> &
  getDefinedVariables() {
    return values;
  }

  std::string printAllTypeValues() {
    std::string result = "";
    for (const auto &[key, value] : typeValues) {
      result += *key + " : " + value->toString() + "\n";
    }
    return result;
  }

  EnvironmentPtr copy(EnvironmentPtr copiedEnclosing) {
    EnvironmentPtr newEnv = std::make_shared<Environment>(copiedEnclosing, environmentType);
    // Constructor only inherits interner from the enclosing env; when
    // copiedEnclosing is nullptr (top-level of the copied chain) we must
    // propagate our own interner so the new env's string-based APIs work.
    newEnv->interner_ = this->interner_;
    newEnv->values = this->values;
    newEnv->updateStates = this->updateStates;
    newEnv->assignedTypes = this->assignedTypes;
    newEnv->typeValues = this->typeValues;
    return newEnv;
  }

  void copyAll(EnvironmentPtr from) {
    for (const auto &[key, value] : from->values) {
      values[key] = value;
    }
    for (const auto &[key, value] : from->typeValues) {
      typeValues[key] = value;
    }
    for (const auto &[key, value] : from->updateStates) {
      updateStates[key] = value;
    }
    for (const auto &[key, value] : from->assignedTypes) {
      assignedTypes[key] = value;
    }
    for (auto key : from->definitionOrder) {
      definitionOrder.push_back(key);
    }
  }

  void selectiveCopy(EnvironmentPtr from, const std::vector<Symbol> &keys) {
    auto inKeys = [&](Symbol k) {
      return std::find(keys.begin(), keys.end(), k) != keys.end();
    };
    for (const auto &[key, value] : from->values) {
      if (inKeys(key)) values[key] = value;
    }
    for (const auto &[key, value] : from->typeValues) {
      typeValues[key] = value;
    }
    for (const auto &[key, value] : from->updateStates) {
      if (inKeys(key)) updateStates[key] = value;
    }
    for (const auto &[key, value] : from->assignedTypes) {
      if (inKeys(key)) assignedTypes[key] = value;
    }
    for (auto key : from->definitionOrder) {
      if (inKeys(key)) definitionOrder.push_back(key);
    }
  }


  void setEnclosing(EnvironmentPtr enclosing) { this->enclosing = enclosing; }

  std::shared_ptr<AutumnValue> findId(int instId) {
    for (const auto &[key, value] : values) {
      (void)key;
      if (value->getInstId() == instId) {
        return value;
      }
    }
    if (enclosing != nullptr) {
      return enclosing->findId(instId);
    }
    return nullptr;
  }

  bool removeIdIfExist(int instId) {
    for (const auto &[key, value] : values) {
      if (value->getInstId() == instId) {
        values.erase(key);
        return true;
      }
    }
    if (enclosing != nullptr) {
      return enclosing->removeIdIfExist(instId);
    }
    return false;
  }

  EnvironmentType getEnvironmentType() { return environmentType; }


  std::string toJson(std::string childScope = ""){
    std::string result = "";
    std::unordered_map<std::string, std::string> varScope;
    for (const auto &[key, value] : values) {
        varScope[*key] = value->toString();
    }
    std::unordered_map<std::string, std::string> typeScope;
    for (const auto &[key, value] : typeValues) {
        typeScope[*key] = value->toString();
    }
    // Indent child scope
    std::string indent = "   ";
    std::vector<std::string> childScopeLines;
    std::stringstream ss(childScope);
    std::string line;
    while (std::getline(ss, line)) {
      childScopeLines.push_back(line);
    }
    for (size_t i = 0; i < childScopeLines.size(); i++) {
      childScopeLines[i] = indent + childScopeLines[i];
    }
    result += "{\n";
    result += indent + "\"varValues\": {\n";
    for (const auto &[key, value] : varScope) {
      std::string valueStr = value;
      valueStr = std::regex_replace(valueStr, std::regex("\\n"), "\\n");
      valueStr = std::regex_replace(valueStr, std::regex("\\r"), "\\r");
      valueStr = std::regex_replace(valueStr, std::regex("\\t"), "\\t");
      valueStr = std::regex_replace(valueStr, std::regex("\""), "\\\"");
      result += indent + "    \"" + key + "\": \"" + valueStr + "\",\n";
    }
    result += indent + "  },\n";
    result += indent + "\"typeValues\": {\n";
    for (const auto &[key, value] : typeScope) {
      result += indent + "    \"" + key + "\": \"" + value + "\",\n";
    }
    result += indent + "  }" + (childScope != "" ? ",\n" : "\n");
    if (childScope != "") {
      result += indent + "\"childScope\": \n";
      for (const auto &l : childScopeLines) {
        result += l + "\n";
      }
    }
    result += "}\n";
    return result;
  }

  void reset() {
    values.clear();
    updateStates.clear();
    typeValues.clear();
    assignedTypes.clear();
    definitionOrder.clear();
  }

private:
  // Helper function to traverse to the ancestor environment at a given distance
  Environment *ancestor(int distance);
};
} // namespace Autumn

#endif
