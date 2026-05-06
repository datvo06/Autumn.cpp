#ifndef _AUTUMN_INTERPRETER_HPP_
#define _AUTUMN_INTERPRETER_HPP_
#include "Environment.hpp"
#include "Error.hpp"
#include "Expr.hpp"
#include "Interner.hpp"
#include "State.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "RandomGenerator.hpp"

namespace Autumn {

class Interpreter {
  // Must be constructed first and destroyed last — Token/AST nodes and
  // Environment maps hold interned Symbol* pointers into this pool.
  Interner interner_;

  State state;
  std::stack<std::shared_ptr<Environment>> tmpEnvStack;

  std::shared_ptr<Environment> globals = std::make_shared<Environment>(interner_);
  std::shared_ptr<Environment> environment = globals;
  std::shared_ptr<Environment> prev_environment = nullptr;

  std::shared_ptr<RandomGenerator> randomGen;
  bool verbose = false;

  void init(std::string = "");

  enum InterpretingState { NONE, OBJECT };
  std::stack<InterpretingState> stateStack;

  // Variables declared with (= name (initnext init-expr next-expr)).
  // Stored in declaration order so iteration (in start() and step()) is
  // deterministic and matches program text. Replaces the previous
  // (initOrder + initMap + nextMap) trio — deterministic, cache-friendly,
  // and no hash lookup on the hot path.
  struct InitNextVar {
    Symbol name;
    std::shared_ptr<Expr> initExpr;
    std::shared_ptr<Expr> nextExpr;
  };
  std::vector<InitNextVar> initNextVars;

  std::vector<std::shared_ptr<Stmt>> onStmts;
  std::shared_ptr<Expr> triggeringConditionExpr = nullptr;
  std::unordered_set<int> onClauseCovered;

public:
  // Allow Expr/Stmt subclasses to access interpreter internals for eval/exec
  friend class Assign;
  friend class Binary;
  friend class Call;
  friend class Get;
  friend class Grouping;
  friend class IntLiteral;
  friend class BoolLiteral;
  friend class StringLiteral;
  friend class Logical;
  friend class Set;
  friend class Unary;
  friend class Lambda;
  friend class Variable;
  friend class TypeVariable;
  friend class TypeDecl;
  friend class ListTypeExpr;
  friend class ListVarExpr;
  friend class IfExpr;
  friend class Let;
  friend class InitNext;
  friend class Block;
  friend class Object;
  friend class Expression;
  friend class OnStmt;

  std::string evaluateToString(std::string expr);
  int getOnClauseCount() { return onStmts.size(); }
  int getCoveredOnClauseCount() { return onClauseCovered.size(); }
  const std::unordered_set<int>& getCoveredOnClauseIndices() { return onClauseCovered; }

  void start(const std::vector<std::shared_ptr<Stmt>> &stmts, std::string = "",
             std::string = "", uint64_t randomSeed = 0);
  void step();
  void reloadCode(const std::vector<std::shared_ptr<Stmt>> &stmts, std::string stdlib, std::string triggeringCondition);
  void tmpExecuteStmt(const std::shared_ptr<Stmt> &stmt);
  State &getState() { return state; }

  const std::shared_ptr<Environment> &getGlobals() { return globals; }
  const std::shared_ptr<Environment> &getPrevEnvironment() { return prev_environment; }

  std::shared_ptr<Environment> getEnvironment() { return environment; }
  Interner &getInterner() { return interner_; }
  Interpreter();

  bool getTriggerState() {
    if (globals->get("SpecialConditionTriggered") != nullptr) {
      return std::dynamic_pointer_cast<AutumnBool>(
                 globals->get("SpecialConditionTriggered"))
          ->isTruthy();
    } else {
      return false;
    }
  }

  void setEnvironment(std::shared_ptr<Environment> env) { environment = env; }

  std::string renderAll();
  std::string getEnvironmentString() {
    return environment->toJson();
  }

  void cacheEnvironment(std::shared_ptr<Environment> env) {
    tmpEnvStack.push(environment);
    environment = env->copy(environment->getEnclosing());
  }

  void restoreEnvironment() { environment = tmpEnvStack.top(); tmpEnvStack.pop(); }

  void setVerbose(bool verbose) { this->verbose = verbose; }
  bool getVerbose() { return verbose; }

  void setRandomSeed(uint64_t seed) {
        if (!randomGen) {
            randomGen = std::make_shared<RandomGenerator>(seed);
        } else {
            randomGen->setSeed(seed);
        }
  }

  std::shared_ptr<RandomGenerator> getRandomGenerator() const {
      return randomGen;
  }

  std::string getBackground();
  int getFrameRate();
};
} // namespace Autumn
#endif
