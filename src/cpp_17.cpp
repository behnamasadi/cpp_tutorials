//https://www.youtube.com/watch?v=QpFjOlzg1r4&t=450s
//https://stackoverflow.com/questions/38060436/what-are-the-new-features-in-c17
//ttps://www.geeksforgeeks.org/features-of-c17-with-examples/?ref=leftbar-rightbar
#include <filesystem>

//https://en.cppreference.com/w/cpp/filesystem
void filesystemEample()
{
    const auto FilePath {"FileToCopy"};

    // If any filepath exists
    if(std::filesystem::exists(FilePath)) {
        const auto FileSize {
             std::filesystem::file_size(FilePath)
        };

        std::filesystem::path tmpPath {"/tmp"};

        // If filepath is available or not
        if(std::filesystem::space(tmpPath)
                           .available > FileSize) {

           // Create Directory
           std::filesystem::create_directory(
                     tmpPath.append("example"));

           // Copy File to file path
           std::filesystem::copy_file(FilePath,
                                 tmpPath.append("newFile"));
        }
    }
}



int main()
{
}
