# std::filesystem

The `std::filesystem` library in C++, introduced in C++17 and further expanded in C++20, provides facilities for performing operations on file systems and their components, such as paths, regular files, and directories. Here is a brief overview of the main classes and functions available in `std::filesystem`:

### Classes
1. **`std::filesystem::path`**: Represents a file system path.
2. **`std::filesystem::directory_entry`**: Represents a directory entry and provides information about the entry.
3. **`std::filesystem::directory_iterator`**: An iterator to iterate through the contents of a directory.
4. **`std::filesystem::recursive_directory_iterator`**: Like `directory_iterator`, but recursively iterates through a directory and all of its subdirectories.
5. **`std::filesystem::file_status`**: Provides information about the type and permissions of a file.
6. **`std::filesystem::filesystem_error`**: An exception type thrown by functions in the `std::filesystem` namespace.

### Functions
1. **Path Operations**
   - `absolute`
   - `canonical`
   - `relative`
   - `proximate`

2. **File Status and Properties**
   - `status`
   - `status_known`
   - `is_block_file`
   - `is_character_file`
   - `is_directory`
   - `is_empty`
   - `is_fifo`
   - `is_other`
   - `is_regular_file`
   - `is_socket`
   - `is_symlink`
   - `exists`
   - `file_size`
   - `hard_link_count`
   - `last_write_time`

3. **File and Directory Operations**
   - `create_directory`
   - `create_directories`
   - `create_hard_link`
   - `create_symlink`
   - `create_directory_symlink`
   - `copy`
   - `copy_file`
   - `copy_symlink`
   - `remove`
   - `remove_all`
   - `rename`
   - `resize_file`
   - `space`
   - `temp_directory_path`

4. **Directory Iteration**
   - `directory_iterator` and `recursive_directory_iterator` mentioned earlier.

5. **Miscellaneous**
   - `current_path`
   - `equivalent`
   - `permissions`

These classes and functions provide a comprehensive interface for filesystem operations, such as querying file attributes, creating and removing files and directories, and iterating through directories. The `std::filesystem` library greatly simplifies file system manipulation in C++, making it more intuitive and less error-prone compared to traditional file handling methods.

[code](../src/filesystem.cpp)



