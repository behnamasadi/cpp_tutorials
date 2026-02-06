#include <queue>
#include <vector>

// Algorithm for sliding window maximum (LeetCode 239)
std::vector<int> maxSlidingWindow(std::vector<int> &nums, int k) {
  std::deque<int> dq; // Stores indices, front has max for current window
  std::vector<int> result;

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
      result.push_back(nums[dq.front()]); // Current window maximum
    }
  }
  return result;
}

int main() {}