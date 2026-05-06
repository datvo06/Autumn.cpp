#include "Expr.hpp"
#include "Stmt.hpp"
#include <string>
#include <vector>

namespace Autumn {

// --- prettyPrint implementations ---

std::string Assign::prettyPrint() {
  return "(<assign> " + name.lexeme + " " + value->prettyPrint() + ")";
}

std::string Binary::prettyPrint() {
  return "(" + op.lexeme + " " + left->prettyPrint() + " " + right->prettyPrint() + ")";
}

std::string Call::prettyPrint() {
  std::string result = "(<call> " + callee->prettyPrint();
  for (const auto &arg : arguments) {
    result += " " + arg->prettyPrint();
  }
  return result + ")";
}

std::string Get::prettyPrint() {
  return "(<get>" + name.lexeme + " " + object->prettyPrint() + ")";
}

std::string Grouping::prettyPrint() {
  return "(group " + expression->prettyPrint() + ")";
}

std::string IntLiteral::prettyPrint() {
  return "(NUMBER " + std::to_string(value) + ")";
}

std::string BoolLiteral::prettyPrint() {
  return value ? std::string("(BOOL true)") : std::string("(BOOL false)");
}

std::string StringLiteral::prettyPrint() {
  return "(STRING " + value + ")";
}

std::string Logical::prettyPrint() {
  return "(" + op.lexeme + " " + left->prettyPrint() + " " + right->prettyPrint() + ")";
}

std::string Set::prettyPrint() {
  return "(<set>" + name.lexeme + " " + object->prettyPrint() + " " + value->prettyPrint() + ")";
}

std::string Unary::prettyPrint() {
  return "(" + op.lexeme + " " + right->prettyPrint() + ")";
}

std::string Lambda::prettyPrint() {
  std::string paramsStr = "(";
  for (const auto &p : params) paramsStr += p.lexeme + " ";
  paramsStr += ")";
  return "(lambda " + paramsStr + " " + right->prettyPrint() + ")";
}

std::string Variable::prettyPrint() {
  return "(var " + name.lexeme + ")";
}

std::string TypeVariable::prettyPrint() {
  return "(<type> " + name.lexeme + ")";
}

std::string TypeDecl::prettyPrint() {
  return "(<typeDecl> " + name.lexeme + " " + typeexpr->prettyPrint() + ")";
}

std::string ListTypeExpr::prettyPrint() {
  return "(<List> " + typeexpr->prettyPrint() + ")";
}

std::string ListVarExpr::prettyPrint() {
  std::string result = "[";
  for (const auto &v : varExprs) result += v->prettyPrint();
  return result + "]";
}

std::string IfExpr::prettyPrint() {
  return "(if " + condition->prettyPrint() + " " +
         thenBranch->prettyPrint() + " " + elseBranch->prettyPrint() + ")";
}

std::string Let::prettyPrint() {
  std::string result = "{";
  for (const auto &e : exprs) result += e->prettyPrint();
  return result + "}";
}

std::string InitNext::prettyPrint() {
  return "(initnext " + initializer->prettyPrint() + " " + nextExpr->prettyPrint() + ")";
}

// --- collectVars implementations ---

std::vector<std::string> Assign::collectVars() {
  return value->collectVars();
}

std::vector<std::string> Binary::collectVars() {
  auto l = left->collectVars();
  auto r = right->collectVars();
  l.insert(l.end(), r.begin(), r.end());
  return l;
}

std::vector<std::string> Call::collectVars() {
  auto result = callee->collectVars();
  for (const auto &arg : arguments) {
    auto v = arg->collectVars();
    result.insert(result.end(), v.begin(), v.end());
  }
  return result;
}

std::vector<std::string> Get::collectVars() {
  auto result = object->collectVars();
  result.push_back(name.lexeme);
  return result;
}

std::vector<std::string> Grouping::collectVars() {
  return expression->collectVars();
}

std::vector<std::string> Logical::collectVars() {
  auto l = left->collectVars();
  auto r = right->collectVars();
  l.insert(l.end(), r.begin(), r.end());
  return l;
}

std::vector<std::string> Set::collectVars() {
  return value->collectVars();
}

std::vector<std::string> Unary::collectVars() {
  return right->collectVars();
}

std::vector<std::string> Lambda::collectVars() {
  return right->collectVars();
}

std::vector<std::string> Variable::collectVars() {
  return {name.lexeme};
}

std::vector<std::string> ListVarExpr::collectVars() {
  std::vector<std::string> result;
  for (const auto &v : varExprs) {
    auto sub = v->collectVars();
    result.insert(result.end(), sub.begin(), sub.end());
  }
  return result;
}

std::vector<std::string> IfExpr::collectVars() {
  auto c = condition->collectVars();
  auto t = thenBranch->collectVars();
  auto e = elseBranch->collectVars();
  c.insert(c.end(), t.begin(), t.end());
  c.insert(c.end(), e.begin(), e.end());
  return c;
}

std::vector<std::string> Let::collectVars() {
  std::vector<std::string> result;
  for (const auto &e : exprs) {
    auto sub = e->collectVars();
    result.insert(result.end(), sub.begin(), sub.end());
  }
  return result;
}

std::vector<std::string> InitNext::collectVars() {
  auto i = initializer->collectVars();
  auto n = nextExpr->collectVars();
  i.insert(i.end(), n.begin(), n.end());
  return i;
}

// --- Stmt prettyPrint implementations ---

std::string Block::prettyPrint() {
  std::string result;
  for (const auto &stmt : statements) {
    result += stmt->prettyPrint();
  }
  return "{" + result + "}";
}

std::string Object::prettyPrint() {
  std::string fieldsStr = "<field>:(";
  for (const auto &field : fields) {
    fieldsStr += field->prettyPrint();
  }
  fieldsStr += ")";
  std::string cellStr = Cell->prettyPrint();
  return "(<object> " + name.lexeme + " " + fieldsStr + " " + cellStr + ")";
}

std::string Expression::prettyPrint() {
  return expression->prettyPrint();
}

std::string OnStmt::prettyPrint() {
  return "(on " + condition->prettyPrint() + ": " + expr->prettyPrint() + ")";
}

} // namespace Autumn
