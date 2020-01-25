#include <iostream>
#include <functional>// for bind()
#include <vector>
#include <algorithm>
#include <cmath>
/*
std::bind works as a Functional Adaptor i.e. it takes a function as input and returns a new function Object as an output with with one or more of the arguments of passed function bound or rearranged (partial function application).
*/


//https://www.youtube.com/watch?v=ZlHi8txU4aQ

template <typename T>
void printTemplate(T value)
{
    std::cout<< value<<std::endl;
}


void print(int firstParam,int secondParam)
{
    std::cout<<"First Param is: "<<firstParam <<" Second Param is: " <<secondParam  <<std::endl;
}

int pow(int i)
{
    return i*i;
}

int main()
{

   
    {// first example
        int firstParam=3;
        int secondParam=5;
        auto firstPrinter=std::bind(&print,firstParam,secondParam);
        auto secondPrinter=std::bind(&print,std::ref(firstParam),std::ref(secondParam) );
        
        firstParam=4;
        secondParam=6;
        firstPrinter();
        secondPrinter();
    }
    
    
    {// second example
        auto reversePrintFunc= std::bind(&print,std::placeholders::_2,std::placeholders::_1);

        int firstParam=3;
        int secondParam=5;

        print(firstParam,secondParam);
        reversePrintFunc(firstParam,secondParam);
    }
    
    {// third example
        
        int firstParam=3;
        int secondParam=5;
        auto printer=std::bind(&print,firstParam,secondParam);
        
        //std::function< void(int,int) > f2(printer);
        std::function< void(int,int) > f2;
    }
    
    {
        int param=3;
        //auto printer=std::bind(&printTemplate<int>,param);
        auto printer=std::bind(&printTemplate<int>,std::placeholders::_1);
        std::function<void (int)>f(printer);
        f(param);
    }
    
    {
        std::vector<int> vec={1,2,3};
        std::vector<int> values(vec.size(),0);
        
        // raise every value in vec to the power of 3
        auto f=std::bind(&std::pow<int,int>,std::placeholders::_1,3);
        std::transform(vec.begin(), vec.end(), values.begin(), f);
        
        for(auto value:values)
            std::cout<< value<<std::endl;

    }
    

   
   
   
}
