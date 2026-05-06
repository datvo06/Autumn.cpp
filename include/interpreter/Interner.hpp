#ifndef _AUTUMN_INTERNER_HPP_
#define _AUTUMN_INTERNER_HPP_

#include <string>
#include <unordered_set>

namespace Autumn {

// An interned identifier. Pointer identity == string equality.
// Pointer is stable for the lifetime of the owning Interner because
// std::unordered_set guarantees node address stability across insertions.
using Symbol = const std::string *;

class Interner {
public:
  // Returns a stable pointer. Calling intern() with equal strings returns
  // the same pointer. Called at parse time, not in hot paths — the
  // std::string construction on probe is fine.
  Symbol intern(const std::string &s) {
    auto [it, _] = pool_.emplace(s);
    return &*it;
  }

  size_t size() const { return pool_.size(); }

private:
  std::unordered_set<std::string> pool_;
};

} // namespace Autumn

#endif
