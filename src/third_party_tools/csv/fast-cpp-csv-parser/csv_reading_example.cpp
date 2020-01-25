#define CSV_IO_NO_THREAD
#include "csv.h"
#include <iostream>



int main()
{
    io::CSVReader<2> in("../src/third_party_tools/data/data.csv");

    in.read_header(io::ignore_extra_column, "x", "y");
    std::string vendor; int x; double y;

    while(in.read_row(x,y))
    {
        std::cout<<"x: " <<x << " y: " <<y<<std::endl;
    }
}
