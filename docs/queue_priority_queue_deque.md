# Queue

## 1) Queue processing systems

A **queue** is a FIFO data structure: first in, first out.

In C++, there are three related types you’ll see a lot:

* `std::vector` (a real container, contiguous)
* `std::deque` (a real container, segmented storage, fast at both ends)
* `std::queue` (NOT a container; an adaptor on top of another container, default is `deque`)

---

## `std::vector`

✅ **Contiguous memory**

* Elements live in one continuous block.
* `push_back` is **amortized O$1$**.
* `push_front` **does not exist** (for a reason).
* If you simulate `push_front` using `insert(begin(), x)` → **all elements are moved** → **O$n$**.

That’s exactly why `vector` is not used as a queue.

### Visual intuition

`std::vector`

```
| 1 | 2 | 3 | 4 | 5 |
```

Insert at front:

```
| X | 1 | 2 | 3 | 4 | 5 |   ← everything moved ❌
```

---

## `std::queue`

❌ **Not a container itself**

`std::queue` is a **container adaptor**.

This:

```cpp
std::queue<int> q;
```

does **NOT** mean:

* `queue` has its own storage
* or `queue` is a real container like `vector`

Instead, it means:

> `std::queue` **wraps another container** and exposes a restricted FIFO interface.

### The actual type (conceptually)

```cpp
template<
    class T,
    class Container = std::deque<T>
>
class queue;
```

So:

```cpp
std::queue<int> q;
```

is equivalent to:

```cpp
std::queue<int, std::deque<int>> q;
```

That is what “actually `std::deque<int>` underneath” means.

### What `std::queue` does

`std::queue`:

* stores elements **inside** its underlying container
* forwards operations to that container
* hides operations that would break FIFO

Example:

```cpp
q.push(10);   // calls deque.push_back(10)
q.pop();      // calls deque.pop_front()
q.front();    // calls deque.front()
```

You **cannot**:

```cpp
q[3];            // ❌ no random access
q.push_front(5); // ❌ forbidden
```

Even though `std::deque` supports them.

### Why `std::queue` exists

Why not just use `std::deque` directly?

Because `std::queue` **enforces intent**:

If you expose a `deque`, someone can:

* pop from the back
* insert in the middle
* break FIFO invariants

If you expose a `queue`, FIFO is enforced by the type system.

This is **API design**, not performance.

### You can choose a different underlying container

As long as it supports:

* `push_back`
* `pop_front`
* `front`
* `back`

Example:

```cpp
std::queue<int, std::list<int>> q;
```

But this is **rare** — `deque` is almost always the best choice.

### Important implications

1. **Memory layout**

* `queue` memory layout = underlying container memory layout
* Default → **non-contiguous**, because default is `deque`

2. **Complexity**

* `push` → O$1$
* `pop` → O$1$

3. **You cannot rely on contiguity**
   This is **invalid**:

```cpp
int* p = &q.front();   // ❌ meaningless assumption about contiguity
```

### Mental model

Think of `std::queue` as:

```cpp
class queue {
private:
    std::deque<T> data;
public:
    void push(const T&);
    void pop();
    T& front();
};
```

It’s **composition**, not inheritance.

### One-line takeaway

`std::queue` is **not a container**, it is a **restricted interface on top of** (by default) `std::deque`, enforcing FIFO.

---

## std::deque (what `std::queue` uses by default)

❌ **NOT contiguous**

* Internally: **multiple fixed-size blocks**
* Plus a small index structure pointing to those blocks
* Elements are **not stored in one contiguous chunk**

### Key properties

✅ `push_back` → **O$1$**
✅ `push_front` → **O$1$**
❌ No shifting of existing elements
❌ Not as cache-friendly as `vector`

### Important point

**`push_front` does NOT move all elements**.

Instead:

* a new block may be allocated (if needed)
* front pointer/index is adjusted

### Visual intuition

`std::deque`

```
Block A: | 1 | 2 |
Block B: | 3 | 4 |
Block C: | 5 |
```

Push front:

```
Block X: | X |
Block A: | 1 | 2 |
Block B: | 3 | 4 |
Block C: | 5 |
```

Only pointers/indices change ✅

---

## Summary table

| Container     | Contiguous | push_back  | push_front | Moves elements? |
| ------------- | ---------- | ---------- | ---------- | --------------- |
| `std::vector` | ✅ Yes      | O$1$ amort | ❌ O$n$     | ❌ Yes           |
| `std::deque`  | ❌ No       | O$1$       | O$1$       | ✅ No            |
| `std::queue`  | ❌ No       | O$1$       | O$1$       | ✅ No            |

---

## Practical rule (interview-grade)

* `vector` → fast iteration, cache-friendly, no front operations
* `deque` → real double-ended queue
* `queue` → FIFO abstraction, usually backed by `deque`
* Need push/pop at both ends → `deque`
* Need contiguous memory → `vector`

---

# Real-world `deque` / queue use cases

## 1) Job / task queue system

```cpp
// Job/task queue system
std::deque<Job> jobQueue;

void processNextJob() {
    if (!jobQueue.empty()) {
        Job& currentJob = jobQueue.front();  // Get next job
        currentJob.execute();
        jobQueue.pop_front();  // Remove after processing
    }
}

void addUrgentJob(const Job& job) {
    jobQueue.push_front(job);  // High priority at front
}

void addBackgroundJob(const Job& job) {
    jobQueue.push_back(job);   // Low priority at back
}
```

---

## 2) Sliding window maximum / minimum (LeetCode 239)

```cpp
// Algorithm for sliding window maximum (LeetCode 239)
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq;  // Stores indices, front has max for current window
    vector<int> result;
    
    for (int i = 0; i < nums.size(); i++) {
        // Remove elements outside window from front
        if (!dq.empty() && dq.front() == i - k) {
            dq.pop_front();
        }
        
        // Maintain decreasing order in deque
        while (!dq.empty() && nums[dq.back()] <= nums[i]) {
            dq.pop_back();
        }
        
        dq.push_back(i);
        
        // Get max from front for current window
        if (i >= k - 1) {
            result.push_back(nums[dq.front()]);  // Current window maximum
        }
    }
    return result;
}
```

---

## 3) LRU (Least Recently Used) cache implementation

```cpp
class LRUCache {
private:
    list<pair<int, int>> cache;  // (key, value)
    unordered_map<int, list<pair<int, int>>::iterator> map;
    int capacity;
    
public:
    int get(int key) {
        if (map.find(key) == map.end()) return -1;
        
        // Move accessed item to front (most recently used)
        cache.splice(cache.begin(), cache, map[key]);
        return cache.front().second;  // Return value from front
    }
    
    void put(int key, int value) {
        if (map.find(key) != map.end()) {
            // Update existing key
            cache.splice(cache.begin(), cache, map[key]);
            cache.front().second = value;  // Update value at front
            return;
        }
        
        if (cache.size() == capacity) {
            // Remove least recently used from back
            int lruKey = cache.back().first;
            map.erase(lruKey);
            cache.pop_back();
        }
        
        // Add new item to front
        cache.emplace_front(key, value);
        map[key] = cache.begin();
    }
};
```

---

## 4) Undo / redo system

```cpp
class TextEditor {
    std::deque<std::string> undoStack;
    std::deque<std::string> redoStack;
    std::string currentText;
    
public:
    void type(const std::string& text) {
        undoStack.push_back(currentText);  // Save state
        currentText += text;
        redoStack.clear();  // Clear redo on new action
    }
    
    void undo() {
        if (!undoStack.empty()) {
            redoStack.push_back(currentText);  // Save for redo
            currentText = undoStack.back();    // Get previous state
            undoStack.pop_back();
        }
    }
    
    void redo() {
        if (!redoStack.empty()) {
            undoStack.push_back(currentText);  // Save for undo
            currentText = redoStack.back();    // Get next state
            redoStack.pop_back();
        }
    }
    
    std::string getText() { return currentText; }
};
```

---

## 5) Priority queue simulation (two-level)

```cpp
// Simple priority system where front has highest priority
template<typename T>
class PriorityBuffer {
    std::deque<T> highPriority;
    std::deque<T> normalPriority;
    
public:
    void addHighPriority(const T& item) {
        highPriority.push_back(item);
    }
    
    void addNormalPriority(const T& item) {
        normalPriority.push_back(item);
    }
    
    T getNext() {
        if (!highPriority.empty()) {
            T item = highPriority.front();  // High priority first
            highPriority.pop_front();
            return item;
        } else if (!normalPriority.empty()) {
            T item = normalPriority.front();  // Then normal
            normalPriority.pop_front();
            return item;
        }
        throw std::runtime_error("Buffer empty");
    }
    
    const T& peekNext() const {
        if (!highPriority.empty()) {
            return highPriority.front();  // Peek without removing
        } else if (!normalPriority.empty()) {
            return normalPriority.front();
        }
        throw std::runtime_error("Buffer empty");
    }
};
```

---

## 6) File buffer with line access

```cpp
class FileBuffer {
    std::deque<std::string> lines;
    
public:
    void loadFile(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
    }
    
    std::string& getFirstLine() {
        if (lines.empty()) throw std::runtime_error("File empty");
        return lines.front();
    }
    
    std::string& getLastLine() {
        if (lines.empty()) throw std::runtime_error("File empty");
        return lines.back();
    }
    
    void appendLine(const std::string& line) {
        lines.push_back(line);
    }
    
    void prependHeader(const std::string& header) {
        lines.push_front(header);
    }
};
```

---

## 7) Real-time data processing (keep last N samples)

```cpp
// Processing streaming sensor data (keep only last N samples)
class SensorDataBuffer {
    std::deque<double> buffer;
    size_t maxSize;
    
public:
    SensorDataBuffer(size_t size) : maxSize(size) {}
    
    void addSample(double value) {
        if (buffer.size() >= maxSize) {
            buffer.pop_front();  // Remove oldest
        }
        buffer.push_back(value);  // Add newest
    }
    
    double getLatest() const {
        if (buffer.empty()) return 0.0;
        return buffer.back();  // Most recent sample
    }
    
    double getOldestInWindow() const {
        if (buffer.empty()) return 0.0;
        return buffer.front();  // Oldest in current window
    }
    
    // Calculate moving average
    double getMovingAverage() const {
        if (buffer.empty()) return 0.0;
        double sum = 0.0;
        for (double val : buffer) sum += val;
        return sum / buffer.size();
    }
};
```

---

## 8) Browser history navigation

```cpp
class BrowserHistory {
    std::deque<std::string> history;
    int currentIndex = -1;
    
public:
    void visit(const std::string& url) {
        // Clear forward history when visiting new page
        while (history.size() > currentIndex + 1) {
            history.pop_back();
        }
        history.push_back(url);
        currentIndex++;
    }
    
    std::string back() {
        if (currentIndex > 0) {
            currentIndex--;
            return history[currentIndex];  // Get from "middle" using front/back logic
        }
        return history.front();  // Return first if at beginning
    }
    
    std::string forward() {
        if (currentIndex < history.size() - 1) {
            currentIndex++;
            return history[currentIndex];
        }
        return history.back();  // Return last if at end
    }
};
```

---

# When to prefer `front()` / `back()` over `begin()` / `end()`

Use `front()` / `back()` when:

* you need direct access to first/last elements
* implementing FIFO/LIFO structures
* maintaining sliding windows
* accessing boundaries of data
* simple queue/deque operations

Use `begin()` / `end()` when:

* you need to iterate through elements
* using STL algorithms
* working with ranges
* need iterator arithmetic
* generic programming

The choice depends on whether you need **element access** (`front/back`) or **position/ranges** (`begin/end`).
