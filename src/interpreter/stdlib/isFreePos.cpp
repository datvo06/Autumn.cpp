#include "AutumnCallableValue.hpp"
#include "AutumnInstance.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include "sexpresso.hpp"
#include <Error.hpp>
#include <any>
#include <cassert>
#include <memory>
#include <string>

namespace Autumn {
std::shared_ptr<AutumnValue>
IsFreePos::call(Interpreter &interpreter,
                const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    throw Error("isFreePos requires 1 argument");
  }
  auto pInstance = std::dynamic_pointer_cast<AutumnInstance>(arguments[0]);
  if (pInstance == nullptr) {
    throw Error("isFreePos requires an instance as argument");
  }
  if (pInstance->getClass() != PositionClass) {
    throw Error("isFreePos requires a Position instance as argument");
  }
  auto xNum = std::dynamic_pointer_cast<AutumnNumber>(pInstance->get("x"));
  auto yNum = std::dynamic_pointer_cast<AutumnNumber>(pInstance->get("y"));
  if (!xNum || !yNum) {
    throw Error("isFreePos: Position x and y must be numbers");
  }
  int x = xNum->getNumber();
  int y = yNum->getNumber();
  return std::make_shared<AutumnBool>(
      interpreter.getGlobals()->isFreePos(x, y));
}
} // namespace Autumn
