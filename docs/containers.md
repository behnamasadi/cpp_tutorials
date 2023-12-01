# C++ Containers
C++20, includes a variety of containers in the Standard Template Library (STL). These containers are used to manage collections of objects of various types. Here's a list of the key containers available in C++20:

1. **Sequence Containers**:
   - `std::array`: A fixed-size array of objects.
   - `std::vector`: A dynamic array that can resize itself automatically.
   - `std::deque`: A double-ended queue that allows insertion and deletion at both ends.
   - `std::forward_list`: A singly linked list that allows efficient insertion and removal of elements.
   - `std::list`: A doubly linked list that supports quick insertion and removal from anywhere in the list.

2. **Associative Containers**:
   - `std::set`: A collection of unique keys, sorted by keys.
   - `std::map`: A collection of key-value pairs, sorted by keys, where each key is unique.
   - `std::multiset`: Similar to `std::set`, but allows multiple entries for a key.
   - `std::multimap`: Similar to `std::map`, but allows multiple entries for a key.

3. **Unordered Associative Containers**:
   - `std::unordered_set`: A set implemented using hash table, where keys are unique.
   - `std::unordered_map`: A hash table of key-value pairs, with unique keys.
   - `std::unordered_multiset`: A `std::unordered_set` but allows multiple entries for a key.
   - `std::unordered_multimap`: A `std::unordered_map` but allows multiple entries for a key.

4. **Container Adaptors**:
   - `std::stack`: Adapts a container to provide stack (LIFO) operations.
   - `std::queue`: Adapts a container to provide queue (FIFO) operations.
   - `std::priority_queue`: A queue that retrieves elements based on a priority.

5. **Specialized Containers**:
   - `std::span` (introduced in C++20): A view over a contiguous sequence of objects, kind of like a lightweight, non-owning reference to an array.
   - `std::bit_vector`: A specialization of `std::vector` for `bool`, optimized for space.

6. **Others**:
   - `std::string` and `std::wstring`: Specialized containers for characters. They are not technically part of the container library but often considered as such due to their similarity.

These containers serve various purposes and have different performance characteristics, making them suitable for different situations in both application and library development.


[code](../src/containers.cpp)  
Refs: [1](#)
