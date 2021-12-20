#include <iostream>
#include <memory>
/*

Ref: https://www.youtube.com/watch?v=sLlGEUO_EGE
Track Memoy Allocations

If we override the new oprator globaly, we are asking the compiler to do not use the "new"
operator that come with std library but use ours.

*/


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

struct object
{
    double a;
    int b;
    float c;
};

int main()
{
    std::string mystring="my name is Behnam Asadi";
    object* myobject=new object;

    std::unique_ptr<object> bject_ptr=std::make_unique<object>();


    delete myobject;
    return 0;
}

