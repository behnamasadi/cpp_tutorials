//https://www.youtube.com/watch?v=QpFjOlzg1r4&t=450s
//https://stackoverflow.com/questions/38060436/what-are-the-new-features-in-c17
//ttps://www.geeksforgeeks.org/features-of-c17-with-examples/?ref=leftbar-rightbar
#include <experimental/filesystem>

//https://en.cppreference.com/w/cpp/filesystem
void filesystemEample()
{
    using namespace std::experimental;
    const auto FilePath {"FileToCopy"};

    // If any filepath exists
    if(filesystem::exists(FilePath)) {
        const auto FileSize {
             filesystem::file_size(FilePath)
        };

        filesystem::path tmpPath {"/tmp"};

        // If filepath is available or not
        if(filesystem::space(tmpPath)
                           .available > FileSize) {

           // Create Directory
           filesystem::create_directory(
                     tmpPath.append("example"));

           // Copy File to file path
           filesystem::copy_file(FilePath,
                                 tmpPath.append("newFile"));
        }
    }
}



int main()
{
}
