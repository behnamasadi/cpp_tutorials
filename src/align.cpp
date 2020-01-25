#include <iostream>

/*

memory is loaded into the CPU cache in chunks called "cache lines". This takes time,
and generally speaking the more cache lines loaded for your object, the longer it takes.
Otherwise, you might get away with sometimes only having part of your object in cache,
and the rest in main memory.


For primitive data type of size x, the address must be multiple of x.
size of int is 4. That means it can only be stored at addresses like 0, 4, 8, 12,...
1) In the case of foo1 we have the followings:

               +--+--+--+--+--+--+--+--+
               |c1|  |  |  |i1|i1|i1|i1|
               +--+--+--+--+--+--+--+--+
memory address  #0 #1 #2 #3 #4 #5 #6 #7


2) In the case of foo2 we have the followings:


               +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
               |c1|  |  |  |i2|i2|i2|i2|c2|  |  |  |i2|i2|i2|i2|
               +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
memory address  #0 #1 #2 #3 #4 #5 #6 #7 #8 #9 #a #b #c #d #e #f


2) In the case of foo3 we have the followings:


               +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
               |c1|c2|  |  |i1|i1|i1|i1|i2|i2|i2|i2|  |  |  |  |
               +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
memory address  #0 #1 #2 #3 #4 #5 #6 #7 #8 #9 #a #b #c #d #e #f


The starting address of struct or class is aligned to the maximum alignment requirement of it's member, so for
class foo3 it is 4.
So it's better to put the larger members first.
*/

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


/*

https://www.youtube.com/watch?v=BP6NxVxDQIs
https://github.com/cppcon/cppcon2016
https://www.youtube.com/watch?v=gonq59MxxcM

*/
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
    std::cout<<"size of int: " << sizeof(int)  <<std::endl;
    std::cout<<"size of foo1: " << sizeof(foo1)  <<std::endl;
    std::cout<< "size of foo2: " <<sizeof(foo2)  <<std::endl;
    std::cout<<"size of foo3: " << sizeof(foo3)  <<std::endl;


    std::cout<<"align of char: " <<alignof(char)  <<std::endl;
    std::cout<<"align of int: " << alignof(int)  <<std::endl;
    std::cout<<"align of foo1: " << alignof(foo1)  <<std::endl;
    std::cout<< "align of foo2: " <<alignof(foo2)  <<std::endl;
    std::cout<<"align of foo3: " << alignof(foo3)  <<std::endl;

    //perfomanceBenchmarking();
    return 0;
}
