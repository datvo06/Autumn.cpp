#ifndef __AUTUMN_TYPE_HPP__
#define __AUTUMN_TYPE_HPP__

#include <memory>
#include <string>

namespace Autumn {
class AutumnType {
public:
  // Primary serialization: subclasses override this.
  // O(n) append with an accumulator — basically a Haskell difference list.
  virtual void toString(std::string &acc) const = 0;

  // Convenience wrapper. Non-virtual: one definition for every subclass.
  std::string toString() const {
    std::string s;
    toString(s);
    return s;
  }

  virtual ~AutumnType() = default;
  virtual bool operator==(const AutumnType &other) const {
    return toString() == other.toString();
  }

protected:
  // Passkey token used by nullary-singleton subclasses. Nested inside
  // AutumnType so only code inside this file can construct one, which
  // keeps the subclass constructors "private" for callers while still
  // allowing std::make_shared to invoke them.
  struct SingletonKey {
    explicit SingletonKey() = default;
  };
};

class AutumnNumberType : public AutumnType {
public:
  explicit AutumnNumberType(SingletonKey) {}
  using AutumnType::toString;
  static std::shared_ptr<AutumnNumberType> getInstance() {
    static auto instance = std::make_shared<AutumnNumberType>(SingletonKey{});
    return instance;
  }
  void toString(std::string &acc) const override { acc += "Number"; }
};

class AutumnStringType : public AutumnType {
public:
  explicit AutumnStringType(SingletonKey) {}
  using AutumnType::toString;
  static std::shared_ptr<AutumnStringType> getInstance() {
    static auto instance = std::make_shared<AutumnStringType>(SingletonKey{});
    return instance;
  }
  void toString(std::string &acc) const override { acc += "String"; }
};

class AutumnBoolType : public AutumnType {
public:
  explicit AutumnBoolType(SingletonKey) {}
  using AutumnType::toString;
  static std::shared_ptr<AutumnBoolType> getInstance() {
    static auto instance = std::make_shared<AutumnBoolType>(SingletonKey{});
    return instance;
  }
  void toString(std::string &acc) const override { acc += "Bool"; }
};

class AutumnUnknownType : public AutumnType {
public:
  explicit AutumnUnknownType(SingletonKey) {}
  using AutumnType::toString;
  bool operator==(const AutumnType &other) const override { return false; }
  static std::shared_ptr<AutumnUnknownType> getInstance() {
    static auto instance = std::make_shared<AutumnUnknownType>(SingletonKey{});
    return instance;
  }
  void toString(std::string &acc) const override { acc += "Unknown"; }
};

class AutumnListType : public AutumnType {
private:
  std::shared_ptr<AutumnType> elementType;

public:
  using AutumnType::toString;
  AutumnListType(std::shared_ptr<AutumnType> elementType)
      : elementType(elementType) {}

  static std::shared_ptr<AutumnListType> getInstance() {
    static std::shared_ptr<AutumnListType> instance =
        std::make_shared<AutumnListType>(AutumnUnknownType::getInstance());
    return instance;
  }

  static std::shared_ptr<AutumnListType>
  getInstance(std::shared_ptr<AutumnType> elementType) {
    return std::make_shared<AutumnListType>(elementType);
  }

  bool operator==(const AutumnType &other) const override {
    const AutumnListType *otherListType =
        dynamic_cast<const AutumnListType *>(&other);
    if (otherListType == nullptr) {
      return false;
    }
    return elementType->toString() == otherListType->elementType->toString();
  }

  void toString(std::string &acc) const override {
    acc += "List<";
    elementType->toString(acc);
    acc += ">";
  }

  std::shared_ptr<AutumnType> getElementType() const { return elementType; }
};

class AutumnMetaType : public AutumnType {
private:
  std::shared_ptr<AutumnType> type;

public:
  using AutumnType::toString;
  AutumnMetaType(std::shared_ptr<AutumnType> type) : type(type) {}

  static std::shared_ptr<AutumnMetaType> getInstance() {
    static std::shared_ptr<AutumnMetaType> instance =
        std::make_shared<AutumnMetaType>(AutumnUnknownType::getInstance());
    return instance;
  }

  static std::shared_ptr<AutumnMetaType>
  getInstance(std::shared_ptr<AutumnType> type) {
    return std::make_shared<AutumnMetaType>(type);
  }

  bool operator==(const AutumnType &other) const override {
    const AutumnMetaType *otherMetaType =
        dynamic_cast<const AutumnMetaType *>(&other);
    if (otherMetaType == nullptr) {
      return false;
    }
    return type->toString() == otherMetaType->type->toString();
  }

  void toString(std::string &acc) const override {
    acc += "Meta<";
    type->toString(acc);
    acc += ">";
  }

  std::shared_ptr<AutumnType> getType() const { return type; }
};

} // namespace Autumn

#endif
