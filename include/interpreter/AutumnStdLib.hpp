#ifndef AUTUMN_STD_LIB_HPP_
#define AUTUMN_STD_LIB_HPP_
#include "AutumnCallable.hpp"
#include "AutumnCallableValue.hpp"
#include "AutumnValue.hpp"
#include <memory>
#include <vector>

namespace Autumn {

class RenderAll : public AutumnCallable {
public:
  RenderAll() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 0; }
  std::string toString() const override { return "<native fn: RenderAll>"; }
};

class Defined : public AutumnCallable {
public:
  Defined() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Defined>"; }
};

class IsFreePos : public AutumnCallable {
public:
  IsFreePos() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: IsFreePos>"; }
};

class Print : public AutumnCallable {
public:
  Print() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Print>"; }
};

class Range : public AutumnCallable {
public:
  Range() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 2; }
  std::string toString() const override { return "<native fn: Range>"; }
};

class AllObjs : public AutumnCallable {
public:
  AllObjs() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 0; }
  std::string toString() const override { return "<native fn: AllObjs>"; }
};

class Rotate : public AutumnCallable {
public:
  Rotate() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Rotate>"; }
};

class Any : public AutumnCallable {
public:
  Any() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 2; }
  std::string toString() const override { return "<native fn: Any>"; }
};

class Foldl : public AutumnCallable {
public:
  Foldl() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 3; }
  std::string toString() const override { return "<native fn: Foldl>"; }
};

class IsList : public AutumnCallable {
public:
  IsList() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: IsList>"; }
};

class Length : public AutumnCallable {
public:
  Length() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: ListSize>"; }
};

class Head : public AutumnCallable {
public:
  Head() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Head>"; }
};

class At: public AutumnCallable {
public:
  At() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 2; }
  std::string toString() const override { return "<native fn: At>"; }
};

class Tail : public AutumnCallable {
public:
  Tail() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Tail>"; }
};

class Prev : public AutumnCallable {
public:
  Prev() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Prev>"; }
};

class UniformChoice : public AutumnCallable {
public:
  UniformChoice() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: UniformChoice>"; }

};
class IsOutSideBounds : public AutumnCallable {
public:
  IsOutSideBounds() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override {
    return "<native fn: IsOutSideBounds>";
  }

};

class Clicked : public AutumnCallable {
public:
  Clicked() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Clicked>"; }

};

class DownPressed : public AutumnCallable {
public:
  DownPressed() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: DownPressed>"; }

};

class LeftPressed : public AutumnCallable {
public:
  LeftPressed() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: LeftPressed>"; }

};

class RightPressed : public AutumnCallable {
public:
  RightPressed() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: RightPressed>"; }

};

class UpPressed : public AutumnCallable {
public:
  UpPressed() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: UpPressed>"; }

};

class ObjClicked : public AutumnCallable {
public:
  ObjClicked() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: ObjClicked>"; }

};
class AddObj : public AutumnCallable {
public:
  AddObj() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AddObj>"; }

};

class RemoveObj : public AutumnCallable {
public:
  RemoveObj() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: RemoveObj>"; }

};

class UpdateObj : public AutumnCallable {
public:
  UpdateObj() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: UpdateObj>"; }

};

class AdjPositions : public AutumnCallable {
public:
  AdjPositions() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AdjPositions>"; }

};

class IsFree : public AutumnCallable {
public:
  IsFree() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: IsFree>"; }

};

class Rect : public AutumnCallable {
public:
  Rect() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Rect>"; }

};

class Displacement : public AutumnCallable {
public:
  Displacement() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Displacement>"; }

};

class Adjacent : public AutumnCallable {
public:
  Adjacent() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Adjacent>"; }

};

class AdjacentObjs : public AutumnCallable {
public:
  AdjacentObjs() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AdjacentObjs>"; }

};

class AdjacentObjsDiagonal : public AutumnCallable {
public:
  AdjacentObjsDiagonal() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override {
    return "<native fn: AdjacentObjsDiagonal>";
  }

};

class AdjacentDiag : public AutumnCallable {
public:
  AdjacentDiag() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AdjacentDiag>"; }

};

class Adj : public AutumnCallable {
public:
  Adj() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Adj>"; }

};

class Map : public AutumnCallable {
public:
  Map() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Map>"; }

private:
  std::shared_ptr<AutumnValue> mapSequential(Interpreter &interpreter,
                                            std::shared_ptr<AutumnCallableValue> callable,
                                            const std::vector<std::shared_ptr<AutumnValue>> &arguments);
  std::shared_ptr<AutumnValue> mapParallelSTL(Interpreter &interpreter,
                                            std::shared_ptr<AutumnCallableValue> callable,
                                            const std::vector<std::shared_ptr<AutumnValue>> &arguments);
};

class Filter : public AutumnCallable {
public:
  Filter() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Filter>"; }

};

class IsWithinBounds : public AutumnCallable {
public:
  IsWithinBounds() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override {
    return "<native fn: IsWithinBounds>";
  }

};

class RandomPositions : public AutumnCallable {
public:
  RandomPositions() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override {
    return "<native fn: RandomPositions>";
  }

};

class AllPositions : public AutumnCallable {
public:
  AllPositions() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AllPositions>"; }

};

class Concat : public AutumnCallable {
public:
  Concat() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Concat>"; }

};

class ArrayEqual : public AutumnCallable {
public:
  ArrayEqual() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 2; }
  std::string toString() const override { return "<native fn: ArrayEqual>"; }
};

class Sqrt : public AutumnCallable {
public:
  Sqrt() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Sqrt>"; }
};

} // namespace Autumn

#endif // !AUTUMN_STD_LIB_HPP_
