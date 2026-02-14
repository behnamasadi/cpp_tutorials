#include <iostream>
#include <utility>
#include <vector>

struct wallet {
  int m_id = 0;
  int m_size = 0;
  double *m_data = nullptr;

  wallet(int id, int size = 0)
      : m_id(id), m_size(size), m_data(size > 0 ? new double[size] : nullptr) {
    std::cout << "ctor\n";
  }

  wallet(const wallet &rhs)
      : m_id(rhs.m_id), m_size(rhs.m_size),
        m_data(rhs.m_size > 0 ? new double[rhs.m_size] : nullptr) {
    std::cout << "copy ctor\n";
    for (int i = 0; i < m_size; ++i)
      m_data[i] = rhs.m_data[i];
  }

  wallet &operator=(const wallet &rhs) {
    std::cout << "copy assign\n";
    if (this == &rhs)
      return *this;

    double *new_data = rhs.m_size > 0 ? new double[rhs.m_size] : nullptr;
    for (int i = 0; i < rhs.m_size; ++i)
      new_data[i] = rhs.m_data[i];

    delete[] m_data;
    m_id = rhs.m_id;
    m_size = rhs.m_size;
    m_data = new_data;

    return *this;
  }

  wallet(wallet &&rhs) noexcept
      : m_id(rhs.m_id), m_size(rhs.m_size), m_data(rhs.m_data) {
    std::cout << "move ctor\n";
    rhs.m_id = 0;
    rhs.m_size = 0;
    rhs.m_data = nullptr;
  }

  wallet &operator=(wallet &&rhs) noexcept {
    std::cout << "move assign\n";
    if (this == &rhs)
      return *this;

    delete[] m_data;
    m_id = rhs.m_id;
    m_size = rhs.m_size;
    m_data = rhs.m_data;

    rhs.m_id = 0;
    rhs.m_size = 0;
    rhs.m_data = nullptr;
    return *this;
  }

  ~wallet() noexcept {
    delete[] m_data;
    std::cout << "dtor\n";
  }

  static wallet pass_by_value(wallet w) { return w; }

  static wallet make_temp(int id, int size) { return wallet(id, size); }
};

int main() {

  std::cout << "\n=== A) Copy constructor cases ===\n";
  wallet a(1, 10);

  wallet b = a; // copy ctor
  wallet c(a);  // copy ctor

  wallet d = wallet::pass_by_value(a); // copy into param

  auto lam = [a]() {}; // copy ctor
  (void)lam;

  std::vector<wallet> v;
  v.push_back(a); // copy ctor

  std::cout << "\n=== B) Copy assignment cases ===\n";
  wallet x(100, 100);
  wallet y(200, 200);

  y = x; // copy assign
  y = y; // self-assign

  v.emplace_back(3, 30);
  v.emplace_back(4, 40);
  v[1] = v[0]; // copy assign

  std::cout << "\n=== C) Move constructor cases ===\n";
  wallet m1 = wallet::make_temp(7, 70); // elided or move
  wallet m2 = std::move(m1);            // move ctor

  wallet m3 = wallet::pass_by_value(wallet(8, 80)); // move ctor

  v.push_back(wallet(9, 90)); // move ctor

  std::cout << "\n=== D) Move assignment cases ===\n";
  wallet p(10, 100);
  wallet q(11, 110);

  q = std::move(p); // move assign
  q = std::move(q); // self-move safe

  std::cout << "\n=== End of main ===\n";
}
