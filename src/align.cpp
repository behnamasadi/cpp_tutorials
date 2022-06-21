#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h> // for offsetof()

//#pragma pack(1)


struct Foo1
{
    char c1;
    int i1;
};

struct Foo2
{
    char c1;
    int i1;
    char c2;
    int i2;
};

struct Foo3UnPacked
{
    char c1;
    char c2;
    int i1;
    int i2;
};

const int aligned_value=8;

struct Foo3
{
    char c1 __attribute__((packed,aligned(aligned_value)));;
    char c2 __attribute__((packed,aligned(aligned_value)));;
    int i1 __attribute__((packed,aligned(aligned_value)));;
    int i2 __attribute__((packed,aligned(aligned_value)));;
};



struct alignas(8) Foo4
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
    // std::cout<<"size of char: " <<sizeof(char)  <<std::endl;
    // std::cout<<"align of char: " <<alignof(char)  <<std::endl;

    // std::cout<<"size of int: " << sizeof(int)  <<std::endl;
    // std::cout<<"align of int: " << alignof(int)  <<std::endl;

    // std::cout<<"size of Foo1: " << sizeof(Foo1)  <<std::endl;
    // std::cout<<"align of Foo1: " << alignof(Foo1)  <<std::endl;

    // std::cout<< "size of Foo2: " <<sizeof(Foo2)  <<std::endl;
    // std::cout<< "align of Foo2: " <<alignof(Foo2)  <<std::endl;




    //std::cout<<sysconf (_SC_LEVEL1_DCACHE_LINESIZE)<<std::endl;


    //Foo3 foo_object ={2,1,100,200} ; 

    //Foo3 foo_object __attribute__((packed,aligned(1))) ;// ={2,1,100,200} ; 
    Foo3 foo_object  ={2,1,100,200} ; 
    //char c __attribute__((packed,aligned(4)));


    std::cout<<"size of byte: " << sizeof(std::byte)  <<std::endl;

    std::cout<<"size of char: " << sizeof(char)  <<std::endl;
    std::cout<< "align of char: " <<alignof(char)  <<std::endl;

    std::cout<<"size of int: " << sizeof(int)  <<std::endl;
    std::cout<< "align of int: " <<alignof(int)  <<std::endl;

    std::cout<<"size of Foo3: " << sizeof(Foo3)  <<std::endl;
    std::cout<<"align of Foo3: " << alignof(Foo3)  <<std::endl;

        std::cout<<"size of Foo3: " << sizeof(std::word)  <<std::endl;

    printf("offsetof(struct Foo3, c1) = %zu\n", offsetof(struct Foo3, c1));
    printf("offsetof(struct Foo3, c2) = %zu\n", offsetof(struct Foo3, c2));
    printf("offsetof(struct Foo3, i1) = %zu\n", offsetof(struct Foo3, i1));
    printf("offsetof(struct Foo3, i2) = %zu\n", offsetof(struct Foo3, i2));


    //perfomanceBenchmarking();



   


    unsigned char memory_data;
    for(std::size_t i=0;i<sizeof(Foo3);i++)
    {
        memory_data=*(((unsigned char*)&foo_object)+i);
        //std::cout<< memory_data  <<std::endl;
        //printf("%04x ",memory_data);
        printf("%03hhu ",memory_data);


        //std::cout<<  std::setbase(10) <<memory_data  <<std::endl;

    }
    printf("\n");

    return 0;
}
