#ifndef EXPR_H
#define EXPR_H

#include "Interner.hpp"
#include "Token.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Autumn {

class AutumnValue;
class Interpreter;
class AutumnType;

class Expr {
public:
  virtual std::shared_ptr<AutumnValue> eval(Interpreter &interp) = 0;
  virtual std::string prettyPrint() = 0;
  virtual std::vector<std::string> collectVars() = 0;
  virtual std::shared_ptr<AutumnType> resolveTypeExpr(Interpreter &interp);
  virtual ~Expr() = default;
};

class Assign : public Expr {
public:
  Assign(Token name, Symbol nameId, std::shared_ptr<Expr> value)
      : name(name), nameId(nameId), value(value) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const Token name;
  const Symbol nameId;
  const std::shared_ptr<Expr> value;
};

class Binary : public Expr {
public:
  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left(left), op(op), right(right) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};

class Call : public Expr {
public:
  Call(std::shared_ptr<Expr> callee,
       std::vector<std::shared_ptr<Expr>> arguments)
      : callee(callee), arguments(arguments) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::shared_ptr<Expr> callee;
  const std::vector<std::shared_ptr<Expr>> arguments;
};

class Get : public Expr {
public:
  Get(std::shared_ptr<Expr> object, Token name, Symbol nameId)
      : object(object), name(name), nameId(nameId) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::shared_ptr<Expr> object;
  const Token name;
  const Symbol nameId;
};

class Grouping : public Expr {
public:
  Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::shared_ptr<Expr> expression;
};

class IntLiteral : public Expr {
public:
  explicit IntLiteral(int value) : value(value) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override { return {}; }

  const int value;
};

class BoolLiteral : public Expr {
public:
  explicit BoolLiteral(bool value) : value(value) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override { return {}; }

  const bool value;
};

class StringLiteral : public Expr {
public:
  explicit StringLiteral(std::string value) : value(std::move(value)) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override { return {}; }

  const std::string value;
};

class Logical : public Expr {
public:
  Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left(left), op(op), right(right) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};

class Set : public Expr {
public:
  Set(std::shared_ptr<Expr> object, Token name, Symbol nameId,
      std::shared_ptr<Expr> value)
      : object(object), name(name), nameId(nameId), value(value) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::shared_ptr<Expr> object;
  const Token name;
  const Symbol nameId;
  const std::shared_ptr<Expr> value;
};

class Unary : public Expr {
public:
  Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const Token op;
  const std::shared_ptr<Expr> right;
};

class Lambda : public Expr {
public:
  Lambda(std::vector<Token> params, std::vector<Symbol> paramIds,
         std::shared_ptr<Expr> right)
      : params(params), paramIds(paramIds), right(right) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::vector<Token> params;
  const std::vector<Symbol> paramIds;
  const std::shared_ptr<Expr> right;
};

class Variable : public Expr {
public:
  Variable(Token name, Symbol nameId) : name(name), nameId(nameId) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const Token name;
  const Symbol nameId;
};

class TypeVariable : public Expr {
public:
  TypeVariable(Token name, Symbol nameId) : name(name), nameId(nameId) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override { return {}; }
  std::shared_ptr<AutumnType> resolveTypeExpr(Interpreter &interp) override;

  const Token name;
  const Symbol nameId;
};

class AutumnType; // forward decl for evalAsFieldDecl

class TypeDecl : public Expr {
public:
  TypeDecl(Token name, Symbol nameId, std::shared_ptr<Expr> typeexpr)
      : name(name), nameId(nameId), typeexpr(typeexpr) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::pair<std::string, std::shared_ptr<AutumnType>> evalAsFieldDecl(Interpreter &interp);
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override { return {}; }

  const Token name;
  const Symbol nameId;
  const std::shared_ptr<Expr> typeexpr;
};

class ListTypeExpr : public Expr {
public:
  ListTypeExpr(std::shared_ptr<Expr> typeexpr) : typeexpr(typeexpr) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override { return {}; }
  std::shared_ptr<AutumnType> resolveTypeExpr(Interpreter &interp) override;

  const std::shared_ptr<Expr> typeexpr;
};

class ListVarExpr : public Expr {
public:
  ListVarExpr(std::vector<std::shared_ptr<Expr>> varExprs)
      : varExprs(varExprs) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::vector<std::shared_ptr<Expr>> varExprs;
};

class IfExpr : public Expr {
public:
  IfExpr(std::shared_ptr<Expr> condition, std::shared_ptr<Expr> thenBranch,
         std::shared_ptr<Expr> elseBranch)
      : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::shared_ptr<Expr> condition;
  const std::shared_ptr<Expr> thenBranch;
  const std::shared_ptr<Expr> elseBranch;
};

class Let : public Expr {
public:
  Let(std::vector<std::shared_ptr<Expr>> exprs) : exprs(exprs) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::vector<std::shared_ptr<Expr>> exprs;
};

class InitNext : public Expr {
public:
  InitNext(std::shared_ptr<Expr> initializer, std::shared_ptr<Expr> nextExpr)
      : initializer(initializer), nextExpr(nextExpr) {}

  std::shared_ptr<AutumnValue> eval(Interpreter &interp) override;
  std::string prettyPrint() override;
  std::vector<std::string> collectVars() override;

  const std::shared_ptr<Expr> initializer;
  const std::shared_ptr<Expr> nextExpr;
};

} // namespace Autumn

#endif // EXPR_H
