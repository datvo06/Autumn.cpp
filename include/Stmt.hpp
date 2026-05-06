#ifndef STMT_H
#define STMT_H

#include "Expr.hpp"
#include "Token.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Autumn {

class Interpreter;

class Stmt {
public:
  virtual void exec(Interpreter &interp) = 0;
  virtual std::string prettyPrint() = 0;
  virtual ~Stmt() = default;
};

class Block : public Stmt {
public:
  Block(std::vector<std::shared_ptr<Stmt>> statements)
      : statements(statements) {}

  void exec(Interpreter &interp) override;
  std::string prettyPrint() override;

  const std::vector<std::shared_ptr<Stmt>> statements;
};

class Object : public Stmt {
public:
  Object(Token name, Symbol nameId, std::vector<std::shared_ptr<Expr>> fields,
         std::shared_ptr<Expr> Cell)
      : name(name), nameId(nameId), fields(fields), Cell(Cell) {}

  void exec(Interpreter &interp) override;
  std::string prettyPrint() override;

  const Token name;
  const Symbol nameId;
  const std::vector<std::shared_ptr<Expr>> fields;
  const std::shared_ptr<Expr> Cell;
};

class Expression : public Stmt {
public:
  Expression(std::shared_ptr<Expr> expression) : expression(expression) {}

  void exec(Interpreter &interp) override;
  std::string prettyPrint() override;

  const std::shared_ptr<Expr> expression;
};

class OnStmt : public Stmt {
public:
  OnStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Expr> expr)
      : condition(condition), expr(expr) {}

  void exec(Interpreter &interp) override;
  std::string prettyPrint() override;

  const std::shared_ptr<Expr> condition;
  const std::shared_ptr<Expr> expr;
};

} // namespace Autumn

#endif // STMT_H
