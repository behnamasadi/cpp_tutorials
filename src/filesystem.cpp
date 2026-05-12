// Demo of std::filesystem APIs listed in docs/filesystem.md.
// Everything happens under /tmp/fs_demo so the example is self-contained.

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

int main() {
  const fs::path root = "/tmp/fs_demo";

  // Start from a clean slate.
  fs::remove_all(root);

  // --- path manipulation ---
  fs::path p = root / "sub" / "notes.txt";
  std::cout << "path:            " << p << '\n';
  std::cout << "parent_path:     " << p.parent_path() << '\n';
  std::cout << "filename:        " << p.filename() << '\n';
  std::cout << "stem:            " << p.stem() << '\n';
  std::cout << "extension:       " << p.extension() << '\n';

  // --- create_directory / create_directories ---
  fs::create_directories(root / "sub");
  fs::create_directory(root / "other");
  std::cout << "created:         " << root << '\n';

  // --- write a regular file ---
  {
    std::ofstream out(p);
    out << "hello filesystem\n";
  }
  std::ofstream(root / "other" / "a.log") << "log line\n";

  // --- exists / is_directory / is_regular_file ---
  std::cout << "exists(p):       " << fs::exists(p) << '\n';
  std::cout << "is_regular_file: " << fs::is_regular_file(p) << '\n';
  std::cout << "is_directory:    " << fs::is_directory(root / "sub") << '\n';

  // --- file_size ---
  std::cout << "file_size:       " << fs::file_size(p) << " bytes\n";

  // --- copy_file / copy ---
  fs::copy_file(p, root / "sub" / "notes_copy.txt");
  fs::copy(root / "other", root / "other_copy",
           fs::copy_options::recursive);
  std::cout << "copied notes.txt and other/\n";

  // --- current_path ---
  std::cout << "current_path:    " << fs::current_path() << '\n';

  // --- absolute / relative ---
  std::cout << "absolute:        " << fs::absolute("relative/path.txt") << '\n';
  std::cout << "relative:        " << fs::relative(p, root) << '\n';

  // --- last_write_time ---
  auto t = fs::last_write_time(p);
  std::cout << "last_write_time: "
            << std::chrono::duration_cast<std::chrono::seconds>(
                   t.time_since_epoch()).count()
            << " s since epoch\n";

  // --- directory_iterator ---
  std::cout << "directory_iterator(" << root << "):\n";
  for (const auto &entry : fs::directory_iterator(root)) {
    std::cout << "  " << entry.path() << '\n';
  }

  // --- recursive_directory_iterator ---
  std::cout << "recursive_directory_iterator(" << root << "):\n";
  for (const auto &entry : fs::recursive_directory_iterator(root)) {
    std::cout << "  " << entry.path() << '\n';
  }

  // --- remove / remove_all ---
  fs::remove(root / "sub" / "notes_copy.txt");
  std::cout << "removed one file\n";
  fs::remove_all(root);
  std::cout << "removed everything\n";
}
