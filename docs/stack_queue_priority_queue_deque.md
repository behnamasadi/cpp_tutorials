# Queue 
## 1. **Queue Processing Systems**
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

## 2. **Sliding Window Maximum/Minimum**
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

## 3. **LRU (Least Recently Used) Cache Implementation**
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

## 4. **Undo/Redo System**
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

## 5. **Priority Queue Simulation**
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

## 6. **File Buffer with Line Access**
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

## 7. **Real-time Data Processing**
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

## 8. **Browser History Navigation**
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

## When to Prefer `front()`/`back()` over `begin()`/`end()`:

**Use `front()`/`back()` when:**
- You need direct access to first/last elements
- Implementing FIFO/LIFO structures
- Maintaining sliding windows
- Accessing boundaries of data
- Simple queue/dequeue operations

**Use `begin()`/`end()` when:**
- You need to iterate through elements
- Using STL algorithms
- Working with ranges
- Need iterator arithmetic
- Generic programming

The choice depends on whether you need **element access** (`front/back`) or **position/ranges** (`begin/end`).