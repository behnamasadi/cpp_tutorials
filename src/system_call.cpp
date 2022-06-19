#include <string>
#include <sys/stat.h>
#include <sys/types.h>

int main() 
{
   std::string cmd="mkdir foo";
   system(cmd.c_str());
   mkdir("bar", 0755);

   return 0;
}
