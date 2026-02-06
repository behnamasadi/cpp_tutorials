#include <iostream>

#include <iostream>
#include <stdexcept> // throw demo (optional)
#include <utility>   // std::move
#include <vector>

class wallet {
public:
  // 1) Normal constructor
  wallet(int id, double balance) : m_id(id), m_balance(new double(balance)) {
    std::cout << "[ctor]       id=" << m_id << " balance=" << *m_balance
              << " this=" << this << " buf=" << m_balance << "\n";
  }

  // 2) Copy constructor (deep copy)
  wallet(const wallet &other)
      : m_id(other.m_id), m_balance(new double(*other.m_balance)) {
    std::cout << "[copy-ctor]  id=" << m_id << " copied_balance=" << *m_balance
              << " this=" << this << " newbuf=" << m_balance
              << " from=" << &other << " frombuf=" << other.m_balance << "\n";
  }

  // 3) Move constructor
  wallet(wallet &&other) noexcept
      : m_id(other.m_id), m_balance(other.m_balance) {
    other.m_id = -1;
    other.m_balance = nullptr;

    std::cout << "[move-ctor]  id=" << m_id << " this=" << this
              << " buf=" << m_balance << " from=" << &other
              << " frombuf(steal)\n";
  }

  // 4) Copy assignment operator (deep copy + self-assign safe)
  wallet &operator=(const wallet &other) {
    std::cout << "[copy-assign] this=" << this << " id=" << m_id
              << " <= from=" << &other << " id=" << other.m_id << "\n";

    if (this == &other) {
      std::cout << "             (self-assignment) no-op\n";
      return *this;
    }

    // Reuse buffer if possible; or replace safely
    if (!m_balance) {
      m_balance = new double(*other.m_balance);
    } else {
      *m_balance = *other.m_balance;
    }

    m_id = other.m_id;

    std::cout << "             result: id=" << m_id
              << " balance=" << (m_balance ? *m_balance : 0.0)
              << " buf=" << m_balance << "\n";
    return *this;
  }

  // 5) Move assignment operator (steal + cleanup + self-move safe)
  wallet &operator=(wallet &&other) noexcept {
    std::cout << "[move-assign] this=" << this << " id=" << m_id
              << " <= from=" << &other << " id=" << other.m_id << "\n";

    if (this == &other) {
      std::cout << "             (self-move) no-op\n";
      return *this;
    }

    delete m_balance;  // release current
    m_id = other.m_id; // steal state
    m_balance = other.m_balance;

    other.m_id = -1; // leave source valid
    other.m_balance = nullptr;

    std::cout << "             result: id=" << m_id << " buf=" << m_balance
              << "\n";
    return *this;
  }

  // Helper methods to trigger/label cases
  static wallet pass_by_value(wallet w) // copy-ctor or move-ctor into param
  {
    std::cout << "[func] pass_by_value got w thisparam=" << &w << "\n";
    return w; // return by value (often elided; otherwise move/copy)
  }

  static wallet make_temp(int id, double bal) {
    wallet tmp(id, bal);
    return tmp; // often elided (RVO)
  }

  void print(const char *tag) const {
    std::cout << "[" << tag << "] id=" << m_id
              << " balance=" << (m_balance ? *m_balance : 0.0)
              << " this=" << this << " buf=" << m_balance << "\n";
  }

  ~wallet() {
    std::cout << "[dtor]       id=" << m_id << " this=" << this
              << " buf=" << m_balance << "\n";
    delete m_balance;
  }

private:
  int m_id;
  double *m_balance;
};

int main() {
  std::cout << "\n=== A) Copy constructor cases ===\n";
  wallet a(1, 10.0);

  // 1) Copy-initialization
  wallet b = a; // copy-ctor
  // 2) Direct-initialization
  wallet c(a); // copy-ctor

  // 3) Pass-by-value (lvalue argument)
  wallet d =
      wallet::pass_by_value(a); // copy-ctor into param; return may elide/move

  // 4) Lambda capture by value
  auto lam = [a]() { /* copy-ctor into closure */ };
  (void)lam;

  // 5) Containers (copying into storage)
  std::vector<wallet> v;
  v.push_back(a); // copy-ctor (or move if you std::move)

  std::cout << "\n=== B) Copy assignment cases ===\n";
  wallet x(100, 1000.0);
  wallet y(200, 2000.0);

  // 1) Simple assignment (existing object overwritten)
  y = x; // copy-assign

  // 2) Self-assignment
  y = y; // copy-assign (self) -> no-op

  // 3) Assigning container elements
  v.emplace_back(3, 30.0);
  v.emplace_back(4, 40.0);
  v[1] = v[0]; // copy-assign

  std::cout << "\n=== C) Move constructor cases ===\n";
  // 1) Move-initialization from rvalue
  wallet m1 = wallet::make_temp(7, 70.0); // often elided; may move-ctor
  // 2) Explicit move from lvalue
  wallet m2 = std::move(m1); // move-ctor

  // 3) Pass-by-value with rvalue argument
  wallet m3 =
      wallet::pass_by_value(wallet(8, 80.0)); // move-ctor into param likely

  // 4) Containers moving (push_back from rvalue)
  v.push_back(wallet(9, 90.0)); // move-ctor (or elide)

  std::cout << "\n=== D) Move assignment cases ===\n";
  wallet p(10, 100.0);
  wallet q(11, 110.0);

  q = std::move(p); // move-assign
  q = std::move(q); // self-move -> no-op

  std::cout << "\n=== End of main (destructors run) ===\n";
  return 0;
}
