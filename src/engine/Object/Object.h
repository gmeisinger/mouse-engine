#ifndef MOUSE_OBJECT_H
#define MOUSE_OBJECT_H

#include "mouse_lua.h"
#include "types.h"
#include <atomic>

namespace mouse {

/**
 * Superclass for all object that should be able to  cross the Lua/C border
 * (this pertains to most objects).
 *
 * This class is an alternative to using smart pointers; it contains
 *retain/release methods, and will delete itself with the reference count hits
 *zero. The wrapper code assumes that all userdata inherits from this class.
 **/
class Object {
public:
  static mouse::Type type;

  /**
   * Constructor. Sets reference count to one.
   **/
  Object();
  Object(const Object &other);

  /**
   * Destructor.
   **/
  virtual ~Object() = 0;

  /**
   * Gets the reference count of this Object.
   * @returns The reference count.
   **/
  int getReferenceCount() const;

  /**
   * Retains the Object, i.e. increases the
   * reference count by one.
   **/
  void retain();

  /**
   * Releases one reference to the Object, i.e. decrements the
   * reference count by one, and potentially deletes the Object
   * if there are no more references.
   **/
  void release();

  int getLuaRef();
  void setLuaRef(int ref);

  int getScriptRef();
  void setScriptRef(int ref);

protected:
  // The reference count.
  std::atomic<int> count;

  int luaref;
  int scriptref;

}; // Object

enum class Acquire {
  RETAIN,
  NORETAIN,
};

template <typename T> class StrongRef {
public:
  StrongRef() : object(nullptr) {}

  StrongRef(T *obj, Acquire acquire = Acquire::RETAIN) : object(obj) {
    if (object && acquire == Acquire::RETAIN)
      object->retain();
  }

  StrongRef(const StrongRef &other) : object(other.get()) {
    if (object)
      object->retain();
  }

  StrongRef(StrongRef &&other) : object(other.object) {
    other.object = nullptr;
  }

  ~StrongRef() {
    if (object)
      object->release();
  }

  StrongRef &operator=(const StrongRef &other) {
    set(other.get());
    return *this;
  }

  T *operator->() const { return object; }

  explicit operator bool() const { return object != nullptr; }

  operator T *() const { return object; }

  void set(T *obj, Acquire acquire = Acquire::RETAIN) {
    if (obj && acquire == Acquire::RETAIN)
      obj->retain();
    if (object)
      object->release();
    object = obj;
  }

  T *get() const { return object; }

private:
  T *object;

}; // StrongRef

} // namespace mouse

#endif // MOUSE_OBJECT_H
