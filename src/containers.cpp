// Minimal demos of the STL containers listed in docs/containers.md.
// Each demo is the shortest snippet that shows the container.

#include <algorithm>
#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

static void header(const char* name) { std::cout << "\n[" << name << "]\n"; }

// -- Sequence containers --------------------------------------------------

static void demo_array() {
    header("std::array");
    std::array<int, 4> a{1, 2, 3, 4};
    std::cout << "size=" << a.size() << " front=" << a.front() << " back=" << a.back() << "\n";
}

static void demo_vector() {
    header("std::vector");
    std::vector<int> v{1, 2, 3};
    v.push_back(4);
    std::cout << "size=" << v.size() << " back=" << v.back() << "\n";
}

static void demo_deque() {
    header("std::deque");
    std::deque<int> d{2, 3};
    d.push_front(1);
    d.push_back(4);
    std::cout << "front=" << d.front() << " back=" << d.back() << "\n";
}

static void demo_forward_list() {
    header("std::forward_list");
    std::forward_list<int> fl{2, 3, 4};
    fl.push_front(1);
    std::cout << "elements:";
    for (int x : fl) std::cout << ' ' << x;
    std::cout << "\n";
}

static void demo_list() {
    header("std::list");
    std::list<int> l{1, 2, 4};
    auto it = std::find(l.begin(), l.end(), 4);
    l.insert(it, 3); // O(1) insert at known position
    std::cout << "elements:";
    for (int x : l) std::cout << ' ' << x;
    std::cout << "\n";
}

// -- Associative containers -----------------------------------------------

static void demo_set() {
    header("std::set");
    std::set<int> s{3, 1, 2, 1};
    std::cout << "sorted unique:";
    for (int x : s) std::cout << ' ' << x;
    std::cout << "\n";
}

static void demo_multiset() {
    header("std::multiset");
    std::multiset<int> ms{1, 2, 2, 3};
    std::cout << "count(2)=" << ms.count(2) << "\n";
}

static void demo_map() {
    header("std::map");
    std::map<std::string, int> m{{"b", 2}, {"a", 1}};
    m["c"] = 3;
    for (const auto& [k, v] : m) std::cout << k << '=' << v << ' ';
    std::cout << "\n";
}

static void demo_multimap() {
    header("std::multimap");
    std::multimap<std::string, int> mm{{"x", 1}, {"x", 2}, {"y", 3}};
    std::cout << "count(x)=" << mm.count("x") << "\n";
}

// -- Unordered associative containers -------------------------------------

static void demo_unordered_set() {
    header("std::unordered_set");
    std::unordered_set<int> us{1, 2, 3, 2};
    std::cout << "size=" << us.size() << " contains(2)=" << us.contains(2) << "\n";
}

static void demo_unordered_map() {
    header("std::unordered_map");
    std::unordered_map<std::string, int> um{{"a", 1}, {"b", 2}};
    std::cout << "a=" << um["a"] << " b=" << um["b"] << "\n";
}

static void demo_unordered_multiset() {
    header("std::unordered_multiset");
    std::unordered_multiset<int> ums{1, 1, 2, 3};
    std::cout << "count(1)=" << ums.count(1) << "\n";
}

static void demo_unordered_multimap() {
    header("std::unordered_multimap");
    std::unordered_multimap<std::string, int> umm{{"k", 1}, {"k", 2}};
    std::cout << "count(k)=" << umm.count("k") << "\n";
}

// -- Container adaptors ---------------------------------------------------

static void demo_stack() {
    header("std::stack");
    std::stack<int> st;
    st.push(1);
    st.push(2);
    st.push(3);
    std::cout << "top=" << st.top() << " size=" << st.size() << "\n";
}

static void demo_queue() {
    header("std::queue");
    std::queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    std::cout << "front=" << q.front() << " back=" << q.back() << "\n";
}

static void demo_priority_queue() {
    header("std::priority_queue");
    std::priority_queue<int> pq;
    for (int x : {3, 1, 4, 1, 5}) pq.push(x);
    std::cout << "top=" << pq.top() << " size=" << pq.size() << "\n";
}

// -- Specialized & others -------------------------------------------------

static void demo_span() {
    header("std::span");
    int raw[] = {10, 20, 30, 40};
    std::span<int> s{raw};
    std::cout << "size=" << s.size() << " s[2]=" << s[2] << "\n";
}

static void demo_vector_bool() {
    header("std::vector<bool>");
    std::vector<bool> bits{true, false, true, true};
    std::cout << "size=" << bits.size() << " bits[0]=" << bits[0] << " bits[1]=" << bits[1] << "\n";
}

static void demo_string() {
    header("std::string");
    std::string s = "hello";
    s += ", world";
    std::cout << s << " (size=" << s.size() << ")\n";
}

int main() {
    demo_array();
    demo_vector();
    demo_deque();
    demo_forward_list();
    demo_list();

    demo_set();
    demo_multiset();
    demo_map();
    demo_multimap();

    demo_unordered_set();
    demo_unordered_map();
    demo_unordered_multiset();
    demo_unordered_multimap();

    demo_stack();
    demo_queue();
    demo_priority_queue();

    demo_span();
    demo_vector_bool();
    demo_string();
    return 0;
}
