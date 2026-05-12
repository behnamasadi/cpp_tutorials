// Companion to docs/cast_Base_to_Derived_to_Base.md
//
// Covers Derived->Base (upcast) and Base->Derived (downcast) conversions with
// both static_cast and dynamic_cast, the shared_ptr equivalents
// (static_pointer_cast / dynamic_pointer_cast), and a robotics-flavoured
// example that uses dynamic_cast to filter a heterogeneous list of Sensor*
// handles down to just the LidarSensor instances a SLAM front-end cares about.
//
// A class that declares or inherits a virtual function is a polymorphic class.
// dynamic_cast is a run-time cast (it consults RTTI), so its operand type must
// be polymorphic. The usual way to make a base polymorphic is to give it a
// virtual destructor -- which you want anyway for safe `delete base` through a
// base pointer.

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

// ---------------------------------------------------------------------------
// static_cast upcast (Derived -> Base)
// ---------------------------------------------------------------------------

namespace staticcast {
class Base {
public:
  void whoAmI() { std::cout << "I'm Base\n"; }
};
class Derived : public Base {
public:
  void whoAmI() { std::cout << "I'm Derived\n"; }
};
class anOtherClass : public Base {};
} // namespace staticcast

void staticcastD2B() {
  staticcast::Derived *d = new staticcast::Derived;
  d->whoAmI();

  // OK: Derived* -> Base* is a safe upcast.
  staticcast::Base *b = static_cast<staticcast::Base *>(d);
  b->whoAmI();

  // Won't compile: Derived and anOtherClass are unrelated siblings.
  // staticcast::anOtherClass* x =
  //     static_cast<staticcast::anOtherClass*>(d);

  delete d;
}

// ---------------------------------------------------------------------------
// dynamic_cast on a polymorphic hierarchy
// ---------------------------------------------------------------------------

namespace dynamiccast {
class Base {
  virtual void foo() {} // makes Base polymorphic; private is fine

public:
  virtual ~Base() = default;
  void whoAmI() { std::cout << "I'm Base\n"; }
};
class Derived : public Base {
public:
  void whoAmI() { std::cout << "I'm Derived\n"; }
};
class anOtherClass : public Base {
public:
  void whoAmI() { std::cout << "I'm anOtherClass\n"; }
};
} // namespace dynamiccast

void dynamiccastD2B() {
  dynamiccast::Derived *d = new dynamiccast::Derived;
  d->whoAmI();

  // OK: upcast.
  dynamiccast::Base *b = dynamic_cast<dynamiccast::Base *>(d);
  b->whoAmI();

  // Compiles, but the cross-cast fails at runtime: d is not an anOtherClass.
  dynamiccast::anOtherClass *x =
      dynamic_cast<dynamiccast::anOtherClass *>(d);
  std::cout << (x == nullptr ? "x is nullptr\n"
                             : "x has been cast to anOtherClass\n");

  delete d;
}

// Round-trip Base -> Derived -> Base via dynamic_cast. Exactly one of the two
// sibling downcasts succeeds on each iteration; the other returns nullptr.
dynamiccast::Base *CreateRandom() {
  if ((std::rand() % 2) == 0)
    return new dynamiccast::Derived;
  else
    return new dynamiccast::anOtherClass;
}

void dynamiccastB2D() {
  for (int n = 0; n < 10; ++n) {
    dynamiccast::Base *base = CreateRandom();

    auto *anOtherClassObject =
        dynamic_cast<dynamiccast::anOtherClass *>(base);
    auto *derivedObject = dynamic_cast<dynamiccast::Derived *>(base);

    std::cout << "================== n is: " << n
              << " =================\n";
    std::cout << (anOtherClassObject == nullptr
                      ? "anOtherClassObject is nullptr\n"
                      : "base has been cast to anOtherClassObject\n");
    std::cout << (derivedObject == nullptr
                      ? "derivedObject is nullptr\n"
                      : "base has been cast to derivedObject\n");

    delete base;
  }
}

// ---------------------------------------------------------------------------
// Casting smart pointers (static_pointer_cast / dynamic_pointer_cast)
// ---------------------------------------------------------------------------
//
// Raw-pointer casts don't compose with shared_ptr -- shared_ptr<Base> and
// shared_ptr<Derived> are unrelated template instantiations. The standard
// library provides dedicated helpers that share ownership of the underlying
// control block.

namespace smartptr {
struct Base {
  int a = 0;
  virtual void whoAmI() const { std::cout << "I am base!\n"; }
  virtual ~Base() = default;
};

struct Derived : Base {
  void whoAmI() const override { std::cout << "I am derived!\n"; }
};
} // namespace smartptr

void smartPointerBaseToDerived() {
  std::shared_ptr<smartptr::Base> ptr_to_base =
      std::make_shared<smartptr::Derived>();

  // ptr_to_base->whoAmI() resolves virtually to Derived::whoAmI(),
  // but if whoAmI() were Derived-only you'd need to cast first.

  // Shares ownership; constructs a temporary shared_ptr<Derived>.
  std::static_pointer_cast<smartptr::Derived>(ptr_to_base)->whoAmI();

  // Same dispatch, but no extra shared_ptr is created.
  static_cast<smartptr::Derived *>(ptr_to_base.get())->whoAmI();

  // Checked variant -- yields an empty shared_ptr if the runtime type is wrong.
  if (auto d = std::dynamic_pointer_cast<smartptr::Derived>(ptr_to_base)) {
    d->whoAmI();
  }

  // NEVER wrap ptr_to_base.get() in a new shared_ptr -- that creates a second
  // independent control block and a double-delete on scope exit.
}

// ---------------------------------------------------------------------------
// Robotics example: dispatching on sensor type
// ---------------------------------------------------------------------------
//
// The driver layer exposes a heterogeneous list of Sensor* handles. A SLAM
// front-end only knows what to do with a LidarSensor; IMU and GPS handles are
// fed to different consumers. dynamic_cast is the safe way to filter.
//
// Failure modes on a real robot:
//   - Forgetting `virtual ~Sensor()`: deleting through Sensor* leaks (or
//     corrupts the heap) for any derived type that owns resources.
//   - Hot loops: dynamic_cast is not free. At 1 kHz, prefer a tagged enum or a
//     visitor pattern, or cast once at registration time and store the typed
//     pointer.
//   - References vs. pointers: dynamic_cast<LidarSensor&>(*s) throws
//     std::bad_cast on mismatch.

struct Sensor {
  virtual ~Sensor() = default;
  virtual void poll() = 0;
};

struct LidarSensor : Sensor {
  void poll() override { /* read 360 deg scan from CAN */ }
  std::vector<float> latestScan() const { return {0.0f, 1.2f, 2.3f}; }
};

struct ImuSensor : Sensor {
  void poll() override { /* read IMU over SPI */ }
};

struct GpsSensor : Sensor {
  void poll() override { /* read NMEA over serial */ }
};

void feedSlam(const std::vector<std::unique_ptr<Sensor>> &sensors) {
  for (const auto &s : sensors) {
    s->poll();
    if (auto *lidar = dynamic_cast<LidarSensor *>(s.get())) {
      auto scan = lidar->latestScan();
      std::cout << "feeding SLAM with " << scan.size() << " ranges\n";
    }
    // IMU / GPS handles are ignored here -- they go through different consumers.
  }
}

void sensorDispatchDemo() {
  std::vector<std::unique_ptr<Sensor>> sensors;
  sensors.push_back(std::make_unique<LidarSensor>());
  sensors.push_back(std::make_unique<ImuSensor>());
  sensors.push_back(std::make_unique<GpsSensor>());
  sensors.push_back(std::make_unique<LidarSensor>());
  feedSlam(sensors);
}

int main() {
  std::cout << "--- staticcastD2B ---\n";
  staticcastD2B();

  std::cout << "\n--- dynamiccastD2B ---\n";
  dynamiccastD2B();

  std::cout << "\n--- dynamiccastB2D ---\n";
  dynamiccastB2D();

  std::cout << "\n--- smartPointerBaseToDerived ---\n";
  smartPointerBaseToDerived();

  std::cout << "\n--- sensorDispatchDemo ---\n";
  sensorDispatchDemo();
}
