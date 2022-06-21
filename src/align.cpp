#include <iostream>


class foo1
{
    char c1;
    int i1;
};

class foo2
{
    char c1;
    int i1;
    char c2;
    int i2;
};

class foo3
{
    char c1;
    char c2;
    int i1;
    int i2;
};

class alignas(8) foo4
{
    char c1;
    int i1;
    char c2;
    int i2;
};



void perfomanceBenchmarking()
{

/*
    row major
    ------------------>
   ➘[][][][][][][][][][]
    ------------------>
   ➘[][][][][][][][][][]
    ------------------>
   ➘[][][][][][][][][][]
    ------------------>
   ➘[][][][][][][][][][]
    ------------------>
   ➘[][][][][][][][][][]
    ------------------>
   ➘[][][][][][][][][][]



    []|➚[]|➚[]|➚[]|➚[]
    []| []| []| []| []
    []| []| []| []| []
    []| []| []| []| []
    []| []| []| []| []
    []| []| []| []| []
    []| []| []| []| []
    []| []| []| []| []
      ▼   ▼   ▼   ▼


*/

    int m,n;
    m=10;
    n=10;
    //int myarray[m][n];
    
    
    int **myarray;
    myarray=new int*[m];
    for(int j=0;j<m;j++)
        myarray[j]=new int[j];

    //row major traverse
    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)
            myarray[i][j]=myarray[i][j]+1;


    //col major traverse
    for(int i=0;i<m;i++)
        for(int j=0;j<n;j++)
            myarray[j][i]=myarray[j][i]+1;


    constexpr size_t size=2<<26;


/*
If we draw perfomace for this code, for step=1..cache_line it is almost fixed and afterwards it start getting slower
the reason is cpu fetch data from memory( our array) on the chunk size of cache_line.

*/
    int step=1;//2,3,...2048

    int *array=new int[size];
    for(int i=0;i<int(size);i+=step)
        array[i]++;
    std::cout<<"size: " <<size <<std::endl;

    delete[] array;

}


int main(int argc, char *argv[])
{
    std::cout<<"size of char: " <<sizeof(char)  <<std::endl;
    std::cout<<"align of char: " <<alignof(char)  <<std::endl;

    std::cout<<"size of int: " << sizeof(int)  <<std::endl;
    std::cout<<"align of int: " << alignof(int)  <<std::endl;

    std::cout<<"size of foo1: " << sizeof(foo1)  <<std::endl;
    std::cout<<"align of foo1: " << alignof(foo1)  <<std::endl;

    std::cout<< "size of foo2: " <<sizeof(foo2)  <<std::endl;
    std::cout<< "align of foo2: " <<alignof(foo2)  <<std::endl;
    
    std::cout<<"size of foo3: " << sizeof(foo3)  <<std::endl;
    std::cout<<"align of foo3: " << alignof(foo3)  <<std::endl;

    //perfomanceBenchmarking();
    return 0;
}
