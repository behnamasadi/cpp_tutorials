#include <iostream>
#include <vector>



void comparisonOfIntegerWithDifferentSignedness()
{
    int a=-1;
    
    // if we define b unsigned we will get b<a 
    //unsigned int b=10;
    std::size_t b=10;


    // change b to the signed one and the problem is gone
    //int b=10;
    
    std::cout<<"a="<<a<<" ,b="<<b <<std::endl;

    // in the comparision between singed and unsigned, the signed one will turn into unsigned!
    if(a<b)
        std::cout<<"a<b" <<std::endl;
    else
        std::cout<<"b<a" <<std::endl;

}

void roundingProblem()
{
    float a=1.99999999999999999;
    float b=2;
    // you expect that result is none zero so the if true while the if is false!
    if(b-a)
        std::cout<<"**" <<std::endl;
    //std::cout<< b-a<<std::endl;
    
}



// TrackMemoryAllocations

void * operator new (size_t size)
{
    std::cout<<"Allocating "<<size<<" bytes" <<std::endl;
    return malloc(size);
}

void  operator delete(void * memory, size_t size)
{
    std::cout<<"Freeing "<<size<<" bytes" <<std::endl;
    free(memory);
}

void  operator delete(void * memory )
{
    std::cout<<"Freeing " <<std::endl;
    free(memory);
}

void stlOnStack()
{
    std::string str;
    std::cout<<"allocating and freeing memory for strings" <<std::endl;
    for(std::size_t i=0;i<25;i++)
    {
        str =str+std::to_string(i);
        std::cout<<"size of string is: "<<str.size()<<" bytes and string is: "<< str <<std::endl;
    }   
}


int main(int argc, char **argv)
{

    //comparisonOfIntegerWithDifferentSignedness();
    //roundingProblem();
    //stlOnStack();

    return 0;
}

