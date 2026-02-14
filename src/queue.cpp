#include <iostream>
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

struct job {

  int id = 0;
  void execute() { std::cout << "exec job id=" << id << std::endl; };
};

int main() {

  // std::deque<job> jobs;
  // job job1{1};
  // job job2{2};
  // job job3{3};
  // job job4{4};
  // job job5{5};

  // jobs.push_back(job1);
  // jobs.push_back(job2);
  // jobs.push_back(job3);

  // jobs.push_front(job4);
  std::cout << "exec job id=" << 0 << std::endl;

  std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
  int k = 3;
  maxSlidingWindow(nums, k);
}