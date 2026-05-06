#ifndef AUTUMN_LAMBDA_HPP
#define AUTUMN_LAMBDA_HPP

#include "AutumnCallable.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Error.hpp"
#include "Expr.hpp"
#include "Interpreter.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Autumn {

class AutumnLambda : public AutumnCallable {
public:
  AutumnLambda(Lambda &declaration,
               std::shared_ptr<Environment> closure)
      : declaration_(std::make_unique<Lambda>(declaration)),
        closure_(closure) {
    lambdaName = declaration_->prettyPrint();
  }

  int arity() override { return declaration_->params.size(); }

  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override {
    auto environment =
        std::make_shared<Environment>(interpreter.getEnvironment());

    interpreter.setEnvironment(environment);
    for (size_t i = 0; i < declaration_->params.size(); ++i) {
      try {
        environment->define(declaration_->params[i].lexeme, arguments[i]);
      } catch (const Error &e) {
        throw Error("Error visiting: " + declaration_->prettyPrint() +
                    "\nGot: " + e.what());
      }
    }

    try {
      std::shared_ptr<AutumnValue> retVal = declaration_->right->eval(interpreter);
      interpreter.setEnvironment(environment->getEnclosing());
      return retVal;
    } catch (const Error &e) {
      throw Error("Error calling: " + declaration_->prettyPrint() +
                  ": " + e.what());
    }
  }

  std::string toString() const override { return std::string(lambdaName); }

private:
  std::string lambdaName;
  std::shared_ptr<Lambda> declaration_;
  std::shared_ptr<Environment> closure_;
};

} // namespace Autumn

#endif
