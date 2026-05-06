#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
std::shared_ptr<AutumnValue>
LeftPressed::call(Interpreter &interpreter,
                  const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  return std::make_shared<AutumnBool>(interpreter.getState().getLeft());
};

int LeftPressed::arity() { return 2; }
} // namespace Autumn
