#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

void lexicalNormalRelativePath() {

  std::filesystem::path foo =
      std::filesystem::path(std::filesystem::current_path());

  std::cout << foo.lexically_normal() << std::endl;
  // std::cout << foo.lexically_proximate() << std::endl;
  std::cout << std::filesystem::path(std::filesystem::current_path() / "../../")
                   .lexically_normal()
            << std::endl;
}

void filesystemExample() {

  const auto FilePath{"FileToCopy"};

  // If any filepath exists
  if (std::filesystem::exists(FilePath)) {
    const auto FileSize{std::filesystem::file_size(FilePath)};

    std::filesystem::path tmpPath{"/tmp"};

    // If filepath is available or not
    if (std::filesystem::space(tmpPath).available > FileSize) {

      // Create Directory
      std::filesystem::create_directory(tmpPath.append("example"));

      // Copy File to file path
      std::filesystem::copy_file(FilePath, tmpPath.append("newFile"));
    }
  }

  // fs::path("a/./b/..").lexically_normal() == "a/"
}

void directoryIterator() {

  std::filesystem::create_directories(std::filesystem::temp_directory_path() /
                                      "foo/dir1");

  std::filesystem::create_directories(std::filesystem::temp_directory_path() /
                                      "foo/dir1/child1");
  std::filesystem::create_directories(std::filesystem::temp_directory_path() /
                                      "foo/dir2");

  std::filesystem::current_path(std::filesystem::temp_directory_path() / "foo");

  std::ofstream{std::filesystem::current_path() / "foo/dir1/file1.txt"};
  std::ofstream{std::filesystem::current_path() / "foo/dir2/file2.txt"};

  for (auto const &iter :
       std::filesystem::directory_iterator{std::filesystem::current_path()}) {
    /*
    Socket (S_IFSOCK)
    Symbolic link (S_IFLNK)
    Regular File (S_IFREG)
    Block special file (S_IFBLK)
    Directory (S_IFDIR)
    Character device (S_IFCHR)
    FIFO (named pipe) (S_IFIFO)
    */

    std::cout << iter.path() << std::endl;

    if (std::filesystem::is_directory(iter)) {
      std::cout << iter.path().filename().string() << " is a directory"
                << std::endl;
    }

    if (std::filesystem::is_regular_file(iter)) {
      std::cout << iter.path().filename().string() << " is a file" << std::endl;
    }
  }

  std::cout << "Iterating " << std::filesystem::current_path() << " recursively"
            << '\n';

  for (auto const &dir_entry : std::filesystem::recursive_directory_iterator{
           std::filesystem::current_path()}) {
    std::cout << dir_entry << '\n';
  }

  // std::ranges::for_each(
  //     std::filesystem::directory_iterator{std::filesystem::current_path()},
  //     [](const auto &dir_entry) { std::cout << dir_entry << '\n'; });

  try {
    std::filesystem::remove_all(std::filesystem::temp_directory_path() /
                                "foo/dir1/child1");

    std::filesystem::remove_all(std::filesystem::temp_directory_path() /
                                "foo/dir1");

    std::filesystem::remove_all(std::filesystem::temp_directory_path() /
                                "foo/dir2");

  } catch (std::error_code e) {
    std::cout << e.message() << std::endl;
  } catch (...) {
    std::cout << "exception" << std::endl;
  }
}

template <typename TP> std::time_t to_time_t(TP tp) {
  using namespace std::chrono;
  auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() +
                                                      system_clock::now());
  return system_clock::to_time_t(sctp);
}

int main(int argc, char **argv) {
  //

  // directoryIterator();
  // lexicallyNormalRelativePath();

  // fs::remove(p);

  for (auto const &iter :
       std::filesystem::directory_iterator{std::filesystem::current_path()}) {
    if (std::filesystem::is_regular_file(iter)) {
      std::cout << iter.path().filename().extension() << std::endl;
      std::cout << iter.path().filename() << std::endl;
      std::cout << "size = " << std::filesystem::file_size(iter) << '\n';

      std::cout << "last_write_time = "
                << std::filesystem::last_write_time(iter) << '\n';
    }
  }

  std::filesystem::file_time_type file_time =
      std::filesystem::last_write_time(__FILE__);
  std::time_t tt = to_time_t(file_time);
  std::tm *gmt = std::gmtime(&tt);
  std::stringstream buffer;
  buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");
  std::string formattedFileTime = buffer.str();
  std::cout << formattedFileTime << '\n';
}

/*
How to add a path with  "\"
std::string path{R"(C:\Program Files\DB Browser for SQLite\foo.db)"};

*/
