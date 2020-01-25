#include <stdio.h>
#include <iostream>
int main(int argc, char**argv)
{
    char* n_argv[] = { "sandbox", "-w", "7", "-h","5" ,"stereo_data/stereo_calib.xml"};

    int length = sizeof(n_argv)/sizeof(n_argv[0]);

    argc=length;
    argv = n_argv;
    std::cout<<argc <<std::endl;
    for(int i=0;i<argc;i++)
    {
        std::cout<<argv[i] <<std::endl;
    }


}
