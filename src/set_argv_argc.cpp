#include <iostream>
#include <string.h>

int main(int argc, char **argv) {

  if (argc < 2) {
    char *n_argv[] = {
        (char *)"sandbox", strdup("-w"),
        strdup("7"),       strdup("-h"),
        strdup("5"),       strdup("stereo_data/stereo_calib.xml")};

    int length = sizeof(n_argv) / sizeof(n_argv[0]);

    argc = length;
    argv = n_argv;
  }
  std::cout << argc << std::endl;

  for (int i = 0; i < argc; i++) {
    std::cout << argv[i] << std::endl;
  }
}
