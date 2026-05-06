#include "Interpreter.hpp"
#include "AutumnCallableValue.hpp"
#include "AutumnExprValue.hpp"
#include "AutumnInstance.hpp"
#include "AutumnLambda.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnType.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Expr.hpp"
#include "Parser.hpp"
#include "Stmt.hpp"
#include <any>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <cstdio>

static std::string readFile(const std::string &filename) {
  std::ifstream file(filename);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

namespace Autumn {
Interpreter::Interpreter() {
}

void Interpreter::init(std::string stdlib) {
  auto &interp = *this;
  globals = std::make_shared<Environment>(interner_);
  environment = globals;
  // Reset onClauseCovered
  onClauseCovered.clear();
  onStmts.clear();
  // std::cout << globals->printAllDefinedVariables() << std::endl;

  std::shared_ptr<AutumnCallable> map = std::make_shared<Map>();
  try {
    globals->define("GRID_SIZE", std::make_shared<AutumnNumber>(16));
    globals->define("map", std::make_shared<AutumnCallableValue>(map));
    globals->define("concat", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<Concat>()));
    globals->define("filter", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<Filter>()));
    globals->define("foldl", std::make_shared<AutumnCallableValue>(
                                 std::make_shared<Foldl>()));
    globals->define("length", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<Length>()));
    globals->define("head", std::make_shared<AutumnCallableValue>(
                                std::make_shared<Head>()));
    globals->define("at", std::make_shared<AutumnCallableValue>(
                                std::make_shared<At>()));
    globals->define("tail", std::make_shared<AutumnCallableValue>(
                                std::make_shared<Tail>()));
    globals->define("renderAll", std::make_shared<AutumnCallableValue>(
                                     std::make_shared<RenderAll>()));
    globals->define("defined", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<Defined>()));
    globals->define("arrayEqual", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<ArrayEqual>()));
    globals->define("sqrt", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<Sqrt>()));
  } catch (const Error &e) {
    if (getVerbose()) {
      std::cerr << "Error in initializing interpreter: " << e.what() << std::endl;
    }
  }
  globals->defineType("Bool", AutumnBoolType::getInstance());
  globals->defineType("String", AutumnStringType::getInstance());
  globals->defineType("Number", AutumnNumberType::getInstance());
  globals->defineType("Int", AutumnNumberType::getInstance());
  globals->defineType("Position", PositionClass);
  globals->defineType("Cell", CellClass);
  globals->defineType("RenderedElem", RenderedElemClass);
  try {
    globals->define("prev", std::make_shared<AutumnCallableValue>(
                                std::make_shared<Prev>()));
    globals->define(
        "any", std::make_shared<AutumnCallableValue>(std::make_shared<Any>()));
    globals->define("isList", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<IsList>()));
    globals->define("uniformChoice", std::make_shared<AutumnCallableValue>(
                                         std::make_shared<UniformChoice>()));
    globals->define("adjPositions", std::make_shared<AutumnCallableValue>(
                                        std::make_shared<AdjPositions>()));
    globals->define("addObj", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<AddObj>()));
    globals->define("isFreePos", std::make_shared<AutumnCallableValue>(
                                     std::make_shared<IsFreePos>()));
    globals->define("range", std::make_shared<AutumnCallableValue>(
                                 std::make_shared<Range>()));

    globals->define("print", std::make_shared<AutumnCallableValue>(
                                 std::make_shared<Print>()));
    globals->define("removeObj", std::make_shared<AutumnCallableValue>(
                                     std::make_shared<RemoveObj>()));
    globals->define("updateObj", std::make_shared<AutumnCallableValue>(
                                     std::make_shared<UpdateObj>()));
    globals->define("randomPositions",
                    std::make_shared<AutumnCallableValue>(
                        std::make_shared<RandomPositions>()));

    globals->define("allPositions", std::make_shared<AutumnCallableValue>(
                                        std::make_shared<AllPositions>()));
    globals->define("clicked", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<Clicked>()));

    globals->define("left", std::make_shared<AutumnCallableValue>(
                                std::make_shared<LeftPressed>()));
    globals->define("right", std::make_shared<AutumnCallableValue>(
                                 std::make_shared<RightPressed>()));
    globals->define("up", std::make_shared<AutumnCallableValue>(
                              std::make_shared<UpPressed>()));
    globals->define("down", std::make_shared<AutumnCallableValue>(
                                std::make_shared<DownPressed>()));
    globals->define("isWithinBounds", std::make_shared<AutumnCallableValue>(
                                          std::make_shared<IsWithinBounds>()));

    globals->define("allObjs", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<AllObjs>()));
    globals->define("rotate", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<Rotate>()));
  } catch (const Error &e) {
    if (getVerbose()) {
      std::cerr << "Error in initializing interpreter: " << e.what() << std::endl;
    }
  }
  // read AutumnStdLib
  if (stdlib == "") {
    std::string autumnStdLib = readFile("autumnstdlib/stdlib.sexp");
    SExpParser parser(autumnStdLib, interner_);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parseStmt();
    for (const auto &stmt : stmts) {
      stmt->exec(interp);
    }
  } else {
    SExpParser parser(stdlib, interner_);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parseStmt();
    for (const auto &stmt : stmts) {
      stmt->exec(interp);
    }
  }
}

std::shared_ptr<AutumnValue> IntLiteral::eval(Interpreter &interp) {
  return std::shared_ptr<AutumnValue>(
      std::make_shared<AutumnNumber>(value));
}

std::shared_ptr<AutumnValue> BoolLiteral::eval(Interpreter &interp) {
  return std::shared_ptr<AutumnValue>(
      std::make_shared<AutumnBool>(value));
}

std::shared_ptr<AutumnValue> StringLiteral::eval(Interpreter &interp) {
  return std::shared_ptr<AutumnValue>(
      std::make_shared<AutumnString>(value));
}

std::shared_ptr<AutumnValue> Grouping::eval(Interpreter &interp) {
  throw Error("This language does not supposed to have grouping :)");
}

std::shared_ptr<AutumnValue> Unary::eval(Interpreter &interp) {
  return right->eval(interp)->eval_unary(op);
}

std::shared_ptr<AutumnValue> Binary::eval(Interpreter &interp) {
  auto leftVal = left->eval(interp);
  auto rightVal = right->eval(interp);
  return leftVal->binop_on(*rightVal, op);
}

std::shared_ptr<AutumnValue> Variable::eval(Interpreter &interp) {
  // Check type environment for class constructors
  auto tv = interp.environment->getTypeValue(nameId);
  if (tv != nullptr) {
    auto cls = std::dynamic_pointer_cast<AutumnClass>(tv);
    if (cls) return std::make_shared<AutumnClassValue>(cls);
    // Non-class type (e.g., primitive type annotation) — fall through to value lookup
  }
  return interp.environment->get(nameId);
}

std::shared_ptr<AutumnValue> Assign::eval(Interpreter &interp) {
  // std::cerr << "Assigning " << name.lexeme << std::endl;
  std::shared_ptr<InitNext> initNext =
      std::dynamic_pointer_cast<InitNext>(value);
  if (initNext != nullptr) {
    interp.initNextVars.push_back(
        {nameId, initNext->initializer, initNext->nextExpr});
    return nullptr;
  }

  std::shared_ptr<AutumnValue> retVal = value->eval(interp);
  try {
    std::shared_ptr<AutumnValue> value =
        retVal;
    auto isNameInGlobal = interp.globals->isDefined(nameId);
    // Get type value
    std::shared_ptr<AutumnType> tv =
        interp.environment->getAssignedType(nameId);
    if (tv != nullptr &&
        std::dynamic_pointer_cast<AutumnListType>(tv) == nullptr) {
      if (tv->toString() != value->getType()->toString()) {
        throw Error("Cannot assign value of type '" +
                    value->getType()->toString() + "' to variable of type '" +
                    tv->toString() + "' for variable '" + name.lexeme +
                    "'.");
      }
    }
    interp.environment->assign(nameId, value);
    if (interp.environment != interp.globals && !isNameInGlobal) {
      if (interp.globals->isDefined(nameId)) {
        throw Error("Variable assigned at local scope became defined at global "
                    "scope: " +
                    name.lexeme); //+
                    // "\n Global: " + interp.globals->printAllDefinedVariables() +
                    // "\n Local: " + interp.environment->printAllDefinedVariables());
      }
    }
    // std::cerr << "Assigned " << name.lexeme << " to " <<
    // value->toString()
    //           << std::endl;
    return value;
  } catch (const std::bad_any_cast &e) {
    throw Error("Assign only accept either value or InitNext " +
                value->prettyPrint());
  }
}

std::shared_ptr<AutumnValue> Logical::eval(Interpreter &interp) {
  auto left_val = left->eval(interp);
  auto callableLeft = std::dynamic_pointer_cast<AutumnCallableValue>(left_val);
  if (callableLeft != nullptr) {
    left_val = callableLeft->call(interp, {});
  }
  std::shared_ptr<AutumnValue> ret = nullptr;
  if (op.type == TokenType::OR) {
    if (left_val->isTruthy()) {
      ret = std::dynamic_pointer_cast<AutumnValue>(
          std::make_shared<AutumnBool>(true));
      // std::cerr << "Returning from logical: " << ret->toString() <<
      // std::endl;
      return ret;
    }
  } else {
    if (!left_val->isTruthy()) {
      ret = std::dynamic_pointer_cast<AutumnValue>(
          std::make_shared<AutumnBool>(false));
      return ret;
    }
  }
  try {
    auto right_val =
        right->eval(interp);
    auto callableRight =
        std::dynamic_pointer_cast<AutumnCallableValue>(right_val);
    if (callableRight != nullptr) {
      right_val = callableRight->call(interp, {});
    }
    return std::dynamic_pointer_cast<AutumnValue>(
        std::make_shared<AutumnBool>(right_val->isTruthy()));
  } catch (const std::bad_any_cast &e) {
    // std::cerr << "Logical operator must be applied to a value" << std::endl;
    throw Error("Logical operator must be applied to a value" +
                right->prettyPrint());
  }
}

static std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>>
getAllArgs(Call &expr, Interpreter &interpreter) {
  auto vals = std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>();
  vals->reserve(expr.arguments.size());
  for (const auto &argument : expr.arguments) {
    try {
      vals->push_back(argument->eval(interpreter));
    } catch (const Error &e) {
      throw Error("Call argument processing error while visiting: \n" +
                  expr.prettyPrint() + "\n Got: \n" + e.what());
    }
  }
  return vals;
}


std::shared_ptr<AutumnValue> Call::eval(Interpreter &interp) {
  std::shared_ptr<AutumnValue> calleeVal =
      callee->eval(interp);

  auto classVal = std::dynamic_pointer_cast<AutumnClassValue>(calleeVal);
  if (classVal != nullptr) {
      std::shared_ptr<AutumnClass> cls = classVal->getClass();
      {
        if (cls->name == "Cell") {
          std::vector<Symbol> varExprs;
          for (const auto &arg : arguments) {
            auto subVarExprs = arg->collectVars();
            for (const auto &v : subVarExprs) {
              varExprs.push_back(interp.interner_.intern(v));
            }
          }
          // std::cout << "VarExprs: " << varExprs->size() << std::endl;
          // for (const auto &varExpr : *varExprs) {
          //   std::cout << "VarExpr: " << varExpr << std::endl;
          // }
          std::vector<std::shared_ptr<AutumnValue>> cellArgs = {};
          // Create a closure
          EnvironmentPtr subEnv = std::make_shared<Environment>(interp.interner_);
          // Copy everthing we can except the nearest instance enclosing.
          EnvironmentPtr skippingEnv = nullptr;
          // Find the nearest instance enclosing
          EnvironmentPtr currentEnv = interp.environment;

          std::stack<EnvironmentPtr> envStack;
          while (currentEnv != nullptr) {
            envStack.push(currentEnv);
            currentEnv = currentEnv->getEnclosing();
          }

          while (!envStack.empty()) {
            EnvironmentPtr currentEnv = envStack.top();
            envStack.pop();
            if (currentEnv->getEnvironmentType() == EnvironmentType::INSTANCE
                && skippingEnv == nullptr) {
              skippingEnv = currentEnv;
            }
            else {
                // Copy everything over
                subEnv->selectiveCopy(currentEnv, varExprs);
            }
          }
          EnvironmentPtr currEnv = interp.getEnvironment();
          interp.setEnvironment(subEnv);
          // TODO: Separate out/ remove outer attribute that are not evaluated
          for (const auto &arg : arguments) {
            std::shared_ptr<AutumnExprValue> exprValue =
                std::make_shared<AutumnExprValue>(arg, subEnv);
            try {

              std::shared_ptr<AutumnValue> pValue =

                      arg->eval(interp);
              auto pNumber = std::dynamic_pointer_cast<AutumnNumber>(pValue);
              if (pNumber != nullptr) {
                cellArgs.push_back(std::make_shared<AutumnExprValue>(
                    std::make_shared<IntLiteral>(pNumber->getNumber()), subEnv));
                continue;
              }
              cellArgs.push_back(exprValue);
            } catch (Error &e) {
              cellArgs.push_back(exprValue);
            }
          }
          interp.setEnvironment(currEnv);
          return std::dynamic_pointer_cast<AutumnValue>(
              std::make_shared<AutumnInstance>(cls, cellArgs));
        }
        // First get all the argument
        auto arguments = getAllArgs(*this, interp);
        try {
          std::vector<std::shared_ptr<AutumnValue>> args = *arguments;
          if (cls->getInitializer() != nullptr) {
            std::shared_ptr<AutumnValue> pValue =
                cls->getInitializer()->call(interp, *arguments);
            std::shared_ptr<AutumnList> list =
                std::dynamic_pointer_cast<AutumnList>(pValue);
            if (list != nullptr) {
              args = std::vector<std::shared_ptr<AutumnValue>>(
                  list->getValues()->begin(), list->getValues()->end());
            }
          }
          auto retVal = std::dynamic_pointer_cast<AutumnValue>(
              std::make_shared<AutumnInstance>(cls, args));
          return retVal;
        } catch (const Error &e) {
          throw Error(
              std::string(
                  "DEBUG: Cannot instantiate class without initializer") +
              e.what());
        }
      }
  }
  try {
    std::shared_ptr<AutumnExprValue> exprValue =
        std::dynamic_pointer_cast<AutumnExprValue>(calleeVal);
    if (exprValue != nullptr) {
      // Print out this and the enclosing interp.environment interp.environment type
      std::string thisEnvType = interp.getEnvironment()->getEnvironmentType() == EnvironmentType::INSTANCE ? "Instance" : "Global";
      std::string enclosingEnvType = interp.getEnvironment()->getEnclosing() == nullptr ? "Global" : interp.getEnvironment()->getEnclosing()->getEnvironmentType() == EnvironmentType::INSTANCE ? "Instance" : "Global";
      EnvironmentPtr subEnv = exprValue->getCenv();
      EnvironmentPtr currEnv = interp.getEnvironment();
      subEnv->setEnclosing(currEnv);
      interp.setEnvironment(subEnv);
      auto val = exprValue->getExpr()->eval(interp);
      subEnv->setEnclosing(nullptr);
      interp.setEnvironment(currEnv);
      return val;
    }
  } catch (Error &e) {
    std::shared_ptr<AutumnExprValue> exprValue =
        std::dynamic_pointer_cast<AutumnExprValue>(calleeVal);
    throw Error(std::string("Error in visiting evaluation of expression: ") + exprValue->getExpr()->prettyPrint() + "\n" +
                e.what());
  }
  std::shared_ptr<AutumnCallableValue> callable =
      std::dynamic_pointer_cast<AutumnCallableValue>(calleeVal);
  if (callable == nullptr) {
    throw Error("Can only call functions and classes");
  }
  if (callable->toString() == "<native fn: Prev>" || callable->toString() == "<native fn: <native fn: Prev>>") {
    // Do not evaluate the arguments
    if (arguments.size() != 1) {
      throw Error("[Prev] Prev() takes 1 argument");
    }
    std::shared_ptr<Variable> varExpr = std::dynamic_pointer_cast<Variable>(arguments[0]);
    if (varExpr == nullptr) { // This is already a string
      std::shared_ptr<AutumnValue> retVal =

            callable->call(interp, *getAllArgs(*this, interp));
      return retVal;
    }
    std::shared_ptr<AutumnValue> varName = std::make_shared<AutumnString>(varExpr->name.lexeme);
    std::shared_ptr<AutumnValue> retVal =

            callable->call(interp, {varName});
    return retVal;
  }
  else {
    std::shared_ptr<AutumnValue> retVal =

            callable->call(interp, *getAllArgs(*this, interp));
    return retVal;
  }
}

std::shared_ptr<AutumnValue> Get::eval(Interpreter &interp) {
  // std::cerr << "Visiting get: " << name.lexeme << std::endl;
  std::shared_ptr<AutumnValue> object_val =
      object->eval(interp);
  if (object_val == nullptr) {
    throw Error("Error interpreting: " + prettyPrint() +
                ": Cannot get property from null");
  }
  std::shared_ptr<AutumnInstance> instance =
      std::dynamic_pointer_cast<AutumnInstance>(object_val);
  if (instance == nullptr) {
    throw Error("Error interpreting: " + prettyPrint() +
                "Only instances have properties, instead got " +
                object_val->getType()->toString());
  }
  std::shared_ptr<AutumnValue> value =
      AutumnInstance::getWithMethod(instance, name.lexeme);
  if (value == nullptr) {
    throw Error("Error interpreting: " + prettyPrint() +
                "Undefined property '" + name.lexeme + "'");
  }
  // std::cerr << "Returning from get: " << value->toString() << std::endl;
  return value;
}

std::shared_ptr<AutumnValue> Set::eval(Interpreter &interp) {
  std::shared_ptr<AutumnValue> object_val =
      object->eval(interp);
  if (object_val == nullptr) {
    throw Error("Cannot set property on null");
  }
  std::shared_ptr<AutumnInstance> instance =
      std::dynamic_pointer_cast<AutumnInstance>(object_val);
  if (instance == nullptr) {
    throw Error("Only instances have fields");
  }
  std::shared_ptr<AutumnValue> value_val =
      value->eval(interp);
  instance->set(name.lexeme, value_val);
  return value_val;
}

std::shared_ptr<AutumnValue> Lambda::eval(Interpreter &interp) {
  // std::cerr << "Visiting lambda: "
  //           << std::get<std::string>(accept(printer)) <<
  //           std::endl;
  std::shared_ptr<Environment> closure = interp.environment; /// TODO: Fix if bug
  std::shared_ptr<AutumnLambda> lambda =
      std::make_shared<AutumnLambda>(*this, closure);
  if (lambda == nullptr) {
    throw Error("visitingLambdaExpr:: Error creating lambda");
  }
  try {
    std::shared_ptr<AutumnCallableValue> pCallableValue =
        std::make_shared<AutumnCallableValue>(lambda);
    // std::cerr << "Lambda created: " << pCallableValue->toString()
    //           << lambda->toString() << std::endl;
    return std::dynamic_pointer_cast<AutumnValue>(pCallableValue);
  } catch (const Error &e) {
    throw Error("visitingLambdaExpr:: Error creating callable value");
  }
}

std::shared_ptr<AutumnValue> TypeVariable::eval(Interpreter &interp) {
  // TypeVariable is handled by resolveTypeExpr in type declaration contexts.
  // If called directly as a value, try to find a class constructor.
  auto type = interp.environment->getTypeValue(nameId);
  if (type != nullptr) {
    auto cls = std::dynamic_pointer_cast<AutumnClass>(type);
    if (cls) return std::make_shared<AutumnClassValue>(cls);
  }
  throw Error("Type '" + name.lexeme + "' cannot be used as a value");
}

// Default: evaluate and extract either an AutumnClassValue's class or the
// value's runtime type. Specialized for ListTypeExpr and TypeVariable.
std::shared_ptr<AutumnType> Expr::resolveTypeExpr(Interpreter &interp) {
  return eval(interp)->getType();
}

std::shared_ptr<AutumnType> ListTypeExpr::resolveTypeExpr(Interpreter &interp) {
  auto innerType = typeexpr->resolveTypeExpr(interp);
  return AutumnListType::getInstance(innerType);
}

std::shared_ptr<AutumnType> TypeVariable::resolveTypeExpr(Interpreter &interp) {
  auto type = interp.getEnvironment()->getTypeValue(name);
  if (type) return type;
  throw Error("Undefined type '" + name.lexeme + "'");
}

std::shared_ptr<AutumnValue> TypeDecl::eval(Interpreter &interp) {
  auto type = typeexpr->resolveTypeExpr(interp);
  interp.environment->assignType(nameId, type);
  return nullptr;
}

std::pair<std::string, std::shared_ptr<AutumnType>> TypeDecl::evalAsFieldDecl(Interpreter &interp) {
  auto type = typeexpr->resolveTypeExpr(interp);
  return std::make_pair(name.lexeme, type);
}

std::shared_ptr<AutumnValue> ListTypeExpr::eval(Interpreter &interp) {
  throw Error("ListTypeExpr cannot be evaluated as a value");
}

std::shared_ptr<AutumnValue> ListVarExpr::eval(Interpreter &interp) {
  try {
    auto pVarExprs =
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>();
    for (auto subexpr : varExprs) {
      try {
        auto valueAny = subexpr->eval(interp);
        auto autumnValue =
            valueAny;
        pVarExprs->push_back(autumnValue);
      } catch (const std::bad_any_cast &e) {
        throw Error("visitListVarExprExpr Failed To Interpret " +
                    subexpr->prettyPrint());
      }
    }
    auto plist = std::make_shared<AutumnList>(pVarExprs);
    return std::dynamic_pointer_cast<AutumnValue>(plist);
  } catch (const std::bad_any_cast &e) {
    throw Error("List must have values,got " + prettyPrint());
  }
}

std::shared_ptr<AutumnValue> IfExpr::eval(Interpreter &interp) {
  std::shared_ptr<AutumnValue> condVal = this->condition->eval(interp);
  bool isCondTruthy = false;
  try {
    isCondTruthy = condVal->isTruthy();
  } catch (const Error &e) {
    throw Error("If condition must be a boolean expression, instead got value: " +
                condVal->toString() + " - " + e.what());
  }
  if (isCondTruthy) {
    try {
      return thenBranch->eval(interp);
    } catch (const Error &e) {
      throw Error("If then branch error: " + thenBranch->prettyPrint() +
      " - " + std::string(e.what()));
    }
  } else {
    try {
      return elseBranch->eval(interp);
    } catch (const Error &e) {
      throw Error("If else branch error: " + elseBranch->prettyPrint() +
      " - " + std::string(e.what()));
    }
  }
}

std::shared_ptr<AutumnValue> Let::eval(Interpreter &interp) {
  auto subEnv = std::make_shared<Environment>(interp.environment);
  interp.setEnvironment(subEnv);
  std::shared_ptr<AutumnValue> value = nullptr;
  std::shared_ptr<AutumnValue> ret;
  for (const auto &subexpr : exprs) {
    ret = subexpr->eval(interp);
  }
  try {
    value = ret;
  } catch (const std::bad_any_cast &e) {
    throw Error("Error visiting Let Expr- " + prettyPrint() +
          "\nGot:" + e.what());
  }
  interp.setEnvironment(subEnv->getEnclosing());
  return value;
}

std::shared_ptr<AutumnValue> InitNext::eval(Interpreter &interp) {
  return nullptr;
}

void Object::exec(Interpreter &interp) {
  interp.stateStack.push(Interpreter::OBJECT);
  std::vector<std::pair<std::string, std::shared_ptr<AutumnType>>> fieldDecls;
  for (const auto &field : this->fields) {
    auto typeDecl = std::dynamic_pointer_cast<TypeDecl>(field);
    if (typeDecl) {
      fieldDecls.push_back(typeDecl->evalAsFieldDecl(interp));
    }
  }
  auto clsType = makeObjectClass(interp, name.lexeme, fieldDecls, Cell);
  interp.environment->defineType(nameId, clsType);
  interp.stateStack.pop();
  // std::cerr << "Defined object " << name.lexeme << " with type "
  //           << std::get<std::shared_ptr<AutumnType>>(
  //                  interp.environment->getTypeValue(name))
  //                  ->toString()
  //           << std::endl;
}

void Block::exec(Interpreter &interp) {
  throw Error("Block statement is not allowed in this language");
}

void Expression::exec(Interpreter &interp) {
  expression->eval(interp);
}

void OnStmt::exec(Interpreter &interp) {
  interp.onStmts.push_back(std::make_shared<OnStmt>(condition, expr));
}

void Interpreter::start(const std::vector<std::shared_ptr<Stmt>> &stmts,
                        std::string stdlib, std::string triggeringCondition,
                        uint64_t randomSeed) {
  auto &interp = *this;
  setRandomSeed(randomSeed);
  init(stdlib);
  environment->assign("SpecialConditionTriggered",
                  std::make_shared<AutumnBool>(false));
  if (triggeringCondition != "") {
    try {
      SExpParser parser(triggeringCondition, interner_);
      std::shared_ptr<Expr> expr =
          parser.parseExpr(sexpresso::parse(triggeringCondition));
      this->triggeringConditionExpr = expr;
    } catch (const Error &e) {
      throw Error(std::string("Failed to parse triggering condition: ") +
                  e.what());
    }
  }
  for (const auto &stmt : stmts) {
    stmt->exec(interp);
  }

  // Evaluate init expressions in declaration order.
  for (const auto &var : initNextVars) {
    if (var.initExpr == nullptr) continue;
    try {
      std::shared_ptr<AutumnValue> value = var.initExpr->eval(interp);
      const std::string &name = *var.name;
      if (value == nullptr) {
        throw Error("Inititalizing: " + name + ": " +
                    "must have a value instead got null ptr");
      }
      if (getVerbose()) {
        std::cerr << "Init " << name << ": " << std::endl
                  << value->toString() << std::endl
                  << std::endl;
      }
      std::shared_ptr<AutumnType> type = environment->getAssignedType(var.name);
      environment->define(var.name, value);
    } catch (const std::bad_any_cast &e) {
      throw Error("Init must have a value: " + var.initExpr->prettyPrint());
    } catch (const RuntimeError &e) {
      if (getVerbose()) {
        std::cerr << "Warning:: Exception " << e.token.lexeme << std::endl;
      }
      throw Error("Init failed: " + std::string(e.what()));
    } catch (const Error &e) {
      throw Error("Init failed: " + std::string(e.what()));
    }
  }
  renderAll();
}

void Interpreter::tmpExecuteStmt(const std::shared_ptr<Stmt> &stmt) {
  auto &interp = *this;
  try {
    cacheEnvironment(environment);
    std::shared_ptr<Expression> exprStmt = std::dynamic_pointer_cast<Expression>(stmt);
    std::shared_ptr<OnStmt> onStmt = std::dynamic_pointer_cast<OnStmt>(stmt);
    if (exprStmt != nullptr) {
    std::shared_ptr<Assign> assign = std::dynamic_pointer_cast<Assign>(exprStmt->expression);
    if (assign != nullptr) {
      // visit this
      assign->eval(interp);
      std::shared_ptr<InitNext> initNext = std::dynamic_pointer_cast<InitNext>(assign->value);
      if (initNext != nullptr) {
        environment->define(assign->name.lexeme,
        initNext->initializer->eval(interp));
      }
      return;
    }
  } else if (onStmt != nullptr) {
      onStmts.push_back(onStmt);
      return;
    }
    else{
    stmt->exec(interp);
    }
  }catch (const Error &e) {
    throw Error("Error in tmpExecuteStmt: " + std::string(e.what()));
  }
}

void Interpreter::step() {
  auto &interp = *this;
  // Copy previous globals
  // Delete previous environment
  auto old_prev_environment = prev_environment;
  if (prev_environment == nullptr) {
    prev_environment = std::make_shared<Environment>(interner_);
  }
  // Copy the stack
  std::stack<std::shared_ptr<Environment>> stack;
  auto current = environment;
  while (current != nullptr) {
    stack.push(current);
    current = current->getEnclosing();
  }
  std::shared_ptr<Environment> curr_ancestor = nullptr;
  while (!stack.empty()) {
    auto current = stack.top();
    auto new_env = current->copy(curr_ancestor);
    curr_ancestor = new_env;
    stack.pop();
  }
  prev_environment = curr_ancestor;
  environment->resetUpdateStates();

  // Execute triggeringCondition
  if (this->triggeringConditionExpr != nullptr) {
    std::shared_ptr<AutumnValue> condition =

            this->triggeringConditionExpr->eval(interp);
    if (!condition->isTruthy()) {
      environment->assign("SpecialConditionTriggered",
                      std::make_shared<AutumnBool>(false));
    } else {
      environment->assign("SpecialConditionTriggered",
                      std::make_shared<AutumnBool>(true));
    }
  }
  // Execute onStmts
  std::vector<std::shared_ptr<std::string>> updated;
  for (int i = 0; i < onStmts.size(); i++) {
    auto stmt = onStmts[i];
    std::shared_ptr<AutumnValue> condition = nullptr;
    std::shared_ptr<OnStmt> onStmt = std::dynamic_pointer_cast<OnStmt>(stmt);
    try {
      condition =
          onStmt->condition->eval(interp);
      std::cerr << "On condition: " << condition->toString() << std::endl;
    } catch (const std::bad_any_cast &e) {
      throw Error("On condition must be a value");
    }
    auto conditionBool = std::dynamic_pointer_cast<AutumnBool>(condition);
    auto callable = std::dynamic_pointer_cast<AutumnCallableValue>(condition);
    if (conditionBool == nullptr && callable == nullptr) {
      throw Error("On condition must be a boolean expression");
    }
    std::shared_ptr<AutumnBool> condEval;
    if (conditionBool != nullptr) {
      condEval = conditionBool;
    } else {
      std::vector<std::shared_ptr<AutumnValue>> args = {};
      condEval =
          std::dynamic_pointer_cast<AutumnBool>(callable->call(*this, args));
    }
    // std::cout << "Condition evaluated to " << condEval->getBool() <<
    // std::endl;
    if (condEval->isTruthy()) {
      onClauseCovered.insert(i);
      std::shared_ptr<AutumnValue> value =

              onStmt->expr->eval(interp);
    }
  }
  for (const auto &var : initNextVars) {
    if (var.nextExpr == nullptr) continue;
    if (environment->isUpdated(var.name)) continue;
    auto &allDefineds = environment->getDefinedVariables();
    if (allDefineds.find(var.name) == allDefineds.end()) continue;
    std::shared_ptr<AutumnValue> value = var.nextExpr->eval(interp);
    environment->assign(var.name, value);
  }
  state.reset();
}

static std::shared_ptr<AutumnList>
renderValue(Interpreter &interpreter, std::shared_ptr<AutumnValue> value) {
  auto pList = std::dynamic_pointer_cast<AutumnList>(value);
  auto pInstance = std::dynamic_pointer_cast<AutumnInstance>(value);
  if (pList != nullptr) {
    std::vector<std::shared_ptr<AutumnValue>> values;
    values.reserve(pList->getValues()->size()*3);
    for (auto &elem : *(pList->getValues())) {
      auto pRendered = renderValue(interpreter, elem);
      const auto& renderedValues = pRendered->getValues();
      values.insert(values.end(), renderedValues->begin(), renderedValues->end());
    }
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(values));
  } else if (pInstance != nullptr) {
    if (pInstance->getClass()->findMethod("render") == nullptr) {
      return std::make_shared<AutumnList>(
          std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
              std::vector<std::shared_ptr<AutumnValue>>()));
    }
    std::shared_ptr<AutumnCallableValue> render =
        std::dynamic_pointer_cast<AutumnCallableValue>(
            AutumnInstance::getWithMethod(pInstance, "render"));
    std::shared_ptr<AutumnValue> result = render->call(interpreter, {});
    return std::dynamic_pointer_cast<AutumnList>(result);
  } else {
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
            std::vector<std::shared_ptr<AutumnValue>>({})));
  }
}

std::string Interpreter::renderAll() {
  environment->clearOccupied();
  std::string result = "{";
  result.reserve(10000);
  auto allDefineds = environment->getDefinedVariables();
  auto vars = environment->getDefinitionOrder();
  auto visited = std::unordered_set<Symbol>();
  visited.reserve(initNextVars.size() + vars.size());
  auto renderOrder = std::vector<Symbol>();
  renderOrder.reserve(initNextVars.size() + vars.size());
  for (const auto &var : initNextVars) {
    if (visited.find(var.name) == visited.end()) {
      renderOrder.push_back(var.name);
      visited.insert(var.name);
    }
  }
  for (auto &elem : vars) {
    if (visited.find(elem) == visited.end()) {
      renderOrder.push_back(elem);
      visited.insert(elem);
    }
  }
  auto pAllElems = std::make_shared<AutumnList>();
  pAllElems->getValues()->reserve(10000);
  static constexpr const char* ELEMENTS_TEMPLATE = "\"%s\": [";
  static constexpr const char* POSITION_TEMPLATE =
    "{\"position\": {\"x\": %d, \"y\": %d}, \"color\": %s}, ";
  for (auto k : renderOrder) {
    auto pList = renderValue(*this, allDefineds[k]);
    if (pList->getValues()->size() == 0) {
      continue;
    }
    char buffer[256];
    snprintf(buffer, sizeof(buffer), ELEMENTS_TEMPLATE, k->c_str());
    result += buffer;
    for (auto &elem : *(pList->getValues())) {
      auto elemInstance = std::dynamic_pointer_cast<AutumnInstance>(elem);
      pAllElems->getValues()->push_back(elemInstance);
      auto pos = std::dynamic_pointer_cast<AutumnInstance>(
          elemInstance->get("position"));
      auto x =
          std::dynamic_pointer_cast<AutumnNumber>(pos->get("x"))->getNumber();
      auto y =
          std::dynamic_pointer_cast<AutumnNumber>(pos->get("y"))->getNumber();
      environment->occupyPosition(x, y);
      auto color =
          std::dynamic_pointer_cast<AutumnString>(elemInstance->get("color"))
              ->getString();
      char buffer[256];
      snprintf(buffer, sizeof(buffer), POSITION_TEMPLATE,
              x, y, color.c_str());
      result += buffer;
    }
    if (pList->getValues()->size() > 0) {
      result = result.substr(0, result.size() - 2);
    }
    result += "], ";
  }
  result += std::string("\"GRID_SIZE\": ") +
            std::to_string(std::dynamic_pointer_cast<AutumnNumber>(
                               environment->get("GRID_SIZE"))
                               ->getNumber());
  result += "}";
  this->getEnvironment()->assign("cacheRendered", pAllElems);
  return result;
}

std::string Interpreter::getBackground() {
  auto var = globals->get("background");
  if (var == nullptr) {
    return "black";
  }
  return std::dynamic_pointer_cast<AutumnString>(var)->getString().substr(1, std::dynamic_pointer_cast<AutumnString>(var)->getString().size() - 2);
}

int Interpreter::getFrameRate() {
  auto var = globals->get("FRAME_RATE");
  if (var == nullptr) {
    return 16;
  }
  return std::dynamic_pointer_cast<AutumnNumber>(var)->getNumber();
}


std::string Interpreter::evaluateToString(std::string expr) {
  auto &interp = *this;
  SExpParser parser(expr, interner_);
  auto parsedExpr = parser.parseExpr(sexpresso::parse(expr));
  auto result = parsedExpr->eval(interp);
  if (result == nullptr) {
    return "NULL";
  }
  return result->toString();
}

} // namespace Autumn
