# algorithm
The `<algorithm>` header in C++20 includes a wide range of functions that are essential for various operations on containers like arrays, lists, and vectors. Some of the key components and functions included in this header are:

1. **Non-modifying Sequence Operations:**
   - `std::for_each`: Apply a function to a range of elements.
   - `std::count` and `std::count_if`: Count elements in a range.
   - `std::find`, `std::find_if`, and `std::find_if_not`: Find elements in a range.
   - `std::search`: Search for a subsequence in a range.

2. **Modifying Sequence Operations:**
   - `std::copy`, `std::copy_if`, `std::copy_n`, and `std::copy_backward`: Copy elements.
   - `std::move` and `std::move_backward`: Move elements.
   - `std::fill` and `std::fill_n`: Assign a value to a range of elements.
   - `std::transform`: Apply a function and store the result.
   - `std::replace`, `std::replace_if`: Replace values in a range.
   - `std::swap_ranges`: Swap ranges of elements.

3. **Partitioning Operations:**
   - `std::partition`: Reorder elements in a range so that they satisfy a predicate.
   - `std::stable_partition`: Stable version of `std::partition`.
   - `std::partition_copy`: Copy elements that satisfy a predicate into another range.
   - `std::partition_point`: Find the partition point.

4. **Sorting Operations:**
   - `std::sort`: Sort elements in a range.
   - `std::stable_sort`: Stable sort elements.
   - `std::partial_sort`: Partially sort elements in a range.
   - `std::nth_element`: Partially sort such that the nth element is in its sorted position.

5. **Binary Search Operations (on sorted ranges):**
   - `std::lower_bound` and `std::upper_bound`: Find bounds in a sorted range.
   - `std::binary_search`: Test if a value exists in a sorted sequence.
   - `std::equal_range`: Get the range of equal elements.

6. **Set Operations (on sorted ranges):**
   - `std::merge`: Merge two sorted ranges.
   - `std::set_union`, `std::set_intersection`, `std::set_difference`, `std::set_symmetric_difference`: Perform set operations.

7. **Heap Operations:**
   - `std::make_heap`, `std::push_heap`, `std::pop_heap`, `std::sort_heap`: Functions for managing heaps.

8. **Min/Max Operations:**
   - `std::min`, `std::max`, `std::minmax`: Find minimum, maximum, and both.
   - `std::min_element`, `std::max_element`, `std::minmax_element`: Find elements.

9. **Comparison Operations:**
   - `std::lexicographical_compare`: Compare sequences.
   - `std::equal`: Test if two ranges are equal.

10. **Permutation Operations:**
    - `std::next_permutation`, `std::prev_permutation`: Generate permutations.

11. **Numeric Operations:**
    - `std::iota`: Fill a range with successive increments of a start value.
    - `std::accumulate`: Sum up elements.
    - `std::inner_product`: Compute the inner product of two ranges.
    - `std::adjacent_difference`: Compute the difference between adjacent elements.
    - `std::reduce`, `std::transform_reduce`, `std::inclusive_scan`, `std::exclusive_scan`: Parallel versions of numeric operations (C++17 and above).

12. **Other Operations:**
    - `std::ranges::sort`, `std::ranges::copy`: Range versions of algorithm functions (C++20).
    
    
## std::ranges::transform    
`std::ranges::transform` is a function in C++20 that applies a given function to each element in a range and stores the result in another range. It's part of the Ranges library, which provides a more modern and flexible way to work with sequences of data.

Here's a basic explanation and some examples:

### Explanation
- **Function**: `std::ranges::transform(InputRange, OutputIterator, UnaryOperation)`
- **Parameters**:
  - **InputRange**: The range of elements to transform.
  - **OutputIterator**: Where to store the results of the transformation.
  - **UnaryOperation**: A function or function object that will be applied to each element in the input range.

### Example 1: Simple Transformation
```cpp
#include <iostream>
#include <vector>
#include <ranges>

int main() {
    std::vector<int> input = {1, 2, 3, 4};
    std::vector<int> output(input.size());

    std::ranges::transform(input, output.begin(), [](int x) { return x * x; });

    for (int val : output) {
        std::cout << val << ' ';
    }
}
```
In this example, each element of `input` is squared and stored in `output`.

### Example 2: Using Different Input and Output Types
```cpp
#include <iostream>
#include <string>
#include <vector>
#include <ranges>

int main() {
    std::vector<std::string> input = {"Hello", "World"};
    std::vector<size_t> output(input.size());

    std::ranges::transform(input, output.begin(), [](const std::string& s) { return s.size(); });

    for (auto val : output) {
        std::cout << val << ' ';
    }
}
```
Here, `std::ranges::transform` calculates the length of each string in `input` and stores it in `output`.

### Example 3: In-Place Transformation
```cpp
#include <iostream>
#include <vector>
#include <ranges>

int main() {
    std::vector<int> data = {1, 2, 3, 4};

    std::ranges::transform(data, data.begin(), [](int x) { return x + 10; });

    for (int val : data) {
        std::cout << val << ' ';
    }
}
```
In this example, each element in `data` is increased by 10. The transformation is done in-place.

### Notes
- `std::ranges::transform` is a more flexible and "range-aware" version of the traditional `std::transform`.
- It works well with the new range-based for loops and other range utilities in C++20.


### std::back_inserter
`std::back_inserter` is a convenient way to append elements to a container that supports push back operations, such as `std::vector`, `std::list`, etc. When used with `std::ranges::transform`, it allows you to transform elements from one range and automatically append the results to another container.

Here's an example demonstrating how you can use `std::ranges::transform` with `std::back_inserter`:

### Example: Transform with Back Inserter

```cpp
#include <iostream>
#include <vector>
#include <ranges>
#include <iterator> // For std::back_inserter

int main() {
    std::vector<int> input = {1, 2, 3, 4};
    std::vector<int> output; // Notice, we don't need to pre-size the output vector

    // Apply transformation and append results to 'output'
    std::ranges::transform(input, std::back_inserter(output), [](int x) { return x * 2; });

    // Print the transformed output
    for (int val : output) {
        std::cout << val << ' ';
    }
}
```

### Explanation:
- **`std::vector<int> output;`**: Initially, `output` is an empty vector.
- **`std::ranges::transform(..., std::back_inserter(output), ...);`**: For each element in `input`, the lambda function is applied, and the result is appended to the end of `output`.
- The lambda function here simply multiplies each element by 2.
- **Printing the Results**: The transformed elements in `output` are then printed.

This approach is particularly useful when you don't know the size of the output container in advance or when you want to append the transformed elements to an existing container.

[code](../src/algorithms_library.cpp)
