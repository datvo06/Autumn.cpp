#include "Interpreter.hpp"
#include "Parser.hpp"
#include "Stmt.hpp"
#include "Expr.hpp"
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

namespace py = pybind11;

// InterpreterWrapper class as provided
class InterpreterWrapper {
public:
  Autumn::Interpreter interpreter;

  std::string getBackground() { return interpreter.getBackground(); }
  int getFrameRate() { return interpreter.getFrameRate(); }

  std::string runScript(const std::string &script,
                        const std::string &stdlib = "",
                        const std::string &triggeringCondition = "",
                        uint64_t randomSeed = 42) {
    Autumn::SExpParser parser(script, interpreter.getInterner());
    std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();
    try {
      interpreter.start(stmts, stdlib, triggeringCondition, randomSeed);
    } catch (const std::runtime_error &e) {
      return std::string("Runtime Error: ") + e.what();
    }
    return "Script executed successfully.";
  }

  std::string tmpExecuteStmt(const std::string &script) {
    std::shared_ptr<sexpresso::Sexp>  sexp = sexpresso::parse(script);
    Autumn::SExpParser parser(script, interpreter.getInterner());
    std::shared_ptr<Autumn::Stmt> stmt = parser.parseStmt(sexp, -1);
    try {
      interpreter.tmpExecuteStmt(stmt);
      return interpreter.getEnvironmentString();
    } catch (const std::runtime_error &e) {
      return std::string("Runtime Error: ") + e.what();
    }
  }

  std::string getEnvironmentString() {
    return interpreter.getEnvironmentString();
  }

  void restoreEnvironment(const std::string &script, const std::string &stdlib, const std::string &triggeringCondition) {
    Autumn::SExpParser parser(script, interpreter.getInterner());
    std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();
    try {
      interpreter.start(stmts, stdlib, triggeringCondition);
      interpreter.restoreEnvironment();
    } catch (const std::runtime_error &e) {
      throw std::runtime_error("Runtime Error: " + std::string(e.what()));
    }
  }

  // Example method to execute a single expression
  std::string step() {
    interpreter.step();
    return "Step executed.";
  }

  void click(int x, int y) { interpreter.getState().click(x, y); }
  void left() { interpreter.getState().pushLeft(); }
  void right() { interpreter.getState().pushRight(); }
  void up() { interpreter.getState().pushUp(); }
  void down() { interpreter.getState().pushDown(); }

  bool getTriggerState() { return interpreter.getTriggerState(); }

  std::string renderAll() { return interpreter.renderAll(); }
  void setVerbose(bool verbose) { interpreter.setVerbose(verbose); }
  bool getVerbose() { return interpreter.getVerbose(); }

  void setRandomSeed(uint64_t seed) { interpreter.setRandomSeed(seed); }
  uint64_t getRandomSeed() { return interpreter.getRandomGenerator()->getSeed(); }

  std::string evaluateToString(std::string expr) { return interpreter.evaluateToString(expr); }
  int getOnClauseCount() { return interpreter.getOnClauseCount(); }
  int getCoveredOnClauseCount() { return interpreter.getCoveredOnClauseCount(); }

  std::vector<int> getCoveredOnClauseIndices() {
    const auto &covered = interpreter.getCoveredOnClauseIndices();
    return std::vector<int>(covered.begin(), covered.end());
  }

  // Helper to convert type expression to string
  std::string typeExprToString(std::shared_ptr<Autumn::Expr> typeexpr) {
    auto typeVar = std::dynamic_pointer_cast<Autumn::TypeVariable>(typeexpr);
    if (typeVar != nullptr) {
      return typeVar->name.lexeme;
    }
    auto listType = std::dynamic_pointer_cast<Autumn::ListTypeExpr>(typeexpr);
    if (listType != nullptr) {
      return "List[" + typeExprToString(listType->typeexpr) + "]";
    }
    return "Unknown";
  }

  // Extract types using C++ parser
  py::dict extractTypes(const std::string &script) {
    Autumn::SExpParser parser(script, interpreter.getInterner());
    std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();

    py::dict types;
    py::dict globalVars;

    for (const auto &stmt : stmts) {
      // Check for Object statement
      auto objStmt = std::dynamic_pointer_cast<Autumn::Object>(stmt);
      if (objStmt != nullptr) {
        py::dict fields;
        // Extract fields (TypeDecl expressions)
        for (const auto &field : objStmt->fields) {
          auto typeDecl = std::dynamic_pointer_cast<Autumn::TypeDecl>(field);
          if (typeDecl != nullptr) {
            fields[typeDecl->name.lexeme.c_str()] = typeExprToString(typeDecl->typeexpr);
          }
        }
        // All objects have origin
        fields["origin"] = "Position";
        types[objStmt->name.lexeme.c_str()] = fields;
      }

      // Check for TypeDecl in Expression statements (global variable declarations)
      auto exprStmt = std::dynamic_pointer_cast<Autumn::Expression>(stmt);
      if (exprStmt != nullptr) {
        auto typeDecl = std::dynamic_pointer_cast<Autumn::TypeDecl>(exprStmt->expression);
        if (typeDecl != nullptr) {
          globalVars[typeDecl->name.lexeme.c_str()] = typeExprToString(typeDecl->typeexpr);
        }
      }
    }

    // Return as dict with types and global_vars
    py::dict result;
    result["types"] = types;
    result["global_vars"] = globalVars;
    return result;
  }
};

PYBIND11_MODULE(interpreter_module, m) {
  m.doc() = "Autumn Interpreter";

  py::class_<InterpreterWrapper, std::shared_ptr<InterpreterWrapper>> cls(m, "Interpreter");
  cls.def(py::init<>())
      .def("step", &InterpreterWrapper::step, "Execute a step")
      .def("click", &InterpreterWrapper::click, "Simulate a click at (x, y)",
           py::arg("x"), py::arg("y"))
      .def("left", &InterpreterWrapper::left, "Push left")
      .def("right", &InterpreterWrapper::right, "Push right")
      .def("up", &InterpreterWrapper::up, "Push up")
      .def("down", &InterpreterWrapper::down, "Push down")
      .def("get_trigger_state", &InterpreterWrapper::getTriggerState,
           "Get trigger state")
      .def("render_all", &InterpreterWrapper::renderAll, "Render all")
      .def("get_environment_string", &InterpreterWrapper::getEnvironmentString, "Get environment string")
      .def("get_on_clause_count", &InterpreterWrapper::getOnClauseCount, "Get on clause count")
      .def("get_covered_on_clause_count", &InterpreterWrapper::getCoveredOnClauseCount, "Get covered on clause count")
      .def("get_covered_on_clause_indices", &InterpreterWrapper::getCoveredOnClauseIndices, "Get list of covered on clause indices")
      .def("evaluate_to_string", &InterpreterWrapper::evaluateToString, "Evaluate to string")
      .def("restore_environment", &InterpreterWrapper::restoreEnvironment, "Restore environment")
      .def("tmp_execute_stmt", &InterpreterWrapper::tmpExecuteStmt, "Execute a statement")
      .def("set_verbose", &InterpreterWrapper::setVerbose, "Set verbose")
      .def("get_verbose", &InterpreterWrapper::getVerbose, "Get verbose")
      .def("set_random_seed", &InterpreterWrapper::setRandomSeed, "Set random seed")
      .def("get_random_seed", &InterpreterWrapper::getRandomSeed, "Get random seed")
      .def("get_background", &InterpreterWrapper::getBackground, "Get background")
      .def("get_frame_rate", &InterpreterWrapper::getFrameRate, "Get frame rate")
      .def("extract_types", &InterpreterWrapper::extractTypes, "Extract types from script");
    cls.def("run_script", &InterpreterWrapper::runScript, "Run a script",
           py::arg("script"), py::arg("stdlib"), py::arg("triggeringCondition"), py::arg("randomSeed") = 42);
}
