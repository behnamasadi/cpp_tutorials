/*
 * Class forward declaration
 *
 * The doc explains two situations:
 *
 *   1. Class B only refers to A via pointer or reference. A forward
 *      declaration ("class A;") is enough -- no need to pull in <A.h>.
 *      This breaks include cycles and shortens compile times.
 *
 *   2. Class B inherits from A, holds an A by value, or takes/returns
 *      A by value. Then the full definition of A must be visible, so
 *      <A.h> must be included.
 *
 * In real robotics code this matters a lot. A Robot owns Sensors and is
 * driven by a Controller; the Controller wants to talk back to the
 * Robot to read state and command actuators. If every header included
 * every other header we would get a circular dependency. Forward
 * declarations are how we cut the cycle.
 *
 * Here we simulate the headers as in-file blocks so a single TU
 * compiles and runs, but the structure mirrors a real project layout.
 */

#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------
// Case 1: forward declaration is enough
//
//   In "controller.h" we would write:
//
//       class Robot;                 // forward declaration
//
//       class Controller {
//           Robot* m_robot_ptr;      // pointer is fine
//           const Robot& m_robot_ref;// reference is fine
//       public:
//           explicit Controller(Robot& r);
//           void step();             // defined in .cpp where <robot.h> is included
//       };
//
//   No include of <robot.h> is required in the header, so robot.h is
//   free to include <controller.h> (or, more commonly, also use a
//   forward declaration of Controller) without a cycle.
// ---------------------------------------------------------------------

class Robot;  // forward declaration -- only the name is known here

class Controller {
    Robot* m_robot_ptr;     // pointer to incomplete type: OK
    const Robot& m_robot_ref;  // reference to incomplete type: OK
public:
    explicit Controller(Robot& r);
    void step();            // body deferred until Robot is complete
};

// ---------------------------------------------------------------------
// Case 2a: B holds an A by value -> need the full definition of A
//
//   A LidarSensor is a self-contained value type. The Robot owns one
//   directly, so robot.h must #include "lidar_sensor.h".
// ---------------------------------------------------------------------

class LidarSensor {
    std::string m_name;
    double m_last_range_m{0.0};
public:
    explicit LidarSensor(std::string name) : m_name(std::move(name)) {}
    void scan(double range_m) { m_last_range_m = range_m; }
    double last_range() const { return m_last_range_m; }
    const std::string& name() const { return m_name; }
};

// ---------------------------------------------------------------------
// Case 2b: B inherits from A -> need the full definition of A
//
//   IMUSensor : public Sensor. The compiler must see Sensor's layout
//   and its virtual table, so a forward declaration would not compile.
// ---------------------------------------------------------------------

class Sensor {
public:
    virtual ~Sensor() = default;
    virtual std::string read() const = 0;
};

class IMUSensor : public Sensor {     // needs full Sensor definition
    double m_yaw_deg{0.0};
public:
    void set_yaw(double y) { m_yaw_deg = y; }
    double yaw() const { return m_yaw_deg; }
    std::string read() const override {
        return "IMU yaw=" + std::to_string(m_yaw_deg) + " deg";
    }
};

// ---------------------------------------------------------------------
// Robot: owns sensors by value (Case 2a) and is referred to from
// Controller via pointer/reference (Case 1).
// ---------------------------------------------------------------------

class Robot {
    std::string m_id;
    LidarSensor m_lidar;       // by value -- LidarSensor must be complete
    IMUSensor   m_imu;         // by value -- IMUSensor must be complete
    double      m_velocity_mps{0.0};
public:
    Robot(std::string id, std::string lidar_name)
        : m_id(std::move(id)), m_lidar(std::move(lidar_name)) {}

    void tick_sensors() {
        m_lidar.scan(1.42);
        m_imu.set_yaw(m_imu.yaw() + 0.5);  // integrate a tiny yaw step
    }

    // Simplified accessors used by Controller (defined out-of-line below
    // so the example compiles even if Controller's body is in another TU).
    const std::string& id() const { return m_id; }
    double velocity() const { return m_velocity_mps; }
    void   set_velocity(double v) { m_velocity_mps = v; }
    const LidarSensor& lidar() const { return m_lidar; }
    const IMUSensor&   imu()   const { return m_imu; }
};

// ---------------------------------------------------------------------
// Controller member definitions live where Robot is complete.
//
// In a real project these would be in controller.cpp, which would
// #include both "controller.h" and "robot.h". The header stays clean.
// ---------------------------------------------------------------------

Controller::Controller(Robot& r)
    : m_robot_ptr(&r), m_robot_ref(r) {}

void Controller::step() {
    // Now that Robot is complete, we can call its member functions.
    const double v = m_robot_ref.velocity();
    m_robot_ptr->set_velocity(v + 0.1);  // simple P-controller stub

    std::cout << "[Controller] " << m_robot_ref.id()
              << " v=" << m_robot_ptr->velocity() << " m/s"
              << ", lidar(" << m_robot_ref.lidar().name() << ")="
              << m_robot_ref.lidar().last_range() << " m"
              << ", " << m_robot_ref.imu().read() << '\n';
}

int main() {
    Robot robot("rover-01", "front-lidar");
    Controller controller(robot);

    for (int i = 0; i < 3; ++i) {
        robot.tick_sensors();
        controller.step();
    }

    // Demonstrate polymorphic use of the Sensor base class
    // (Case 2b: inheritance requires the full base definition).
    std::vector<Sensor*> sensors;
    IMUSensor extra;
    extra.set_yaw(42.0);
    sensors.push_back(&extra);
    for (const auto* s : sensors) {
        std::cout << "[poly] " << s->read() << '\n';
    }

    return 0;
}
