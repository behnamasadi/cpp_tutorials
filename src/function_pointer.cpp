#include <iostream>
#include <vector>

/*
Function Pointer
A function pointer is a variable that stores the address of a function that can later be called through that function pointer.
*/

///////////////////////////////////Adder Example////////////////////////////////////

int adder(int a, int b)
{
    return a+b;
}

//defining
int (*adder_fn_ptr)(int, int);

/////////////////////////////////Foreach Example////////////////////////////////////

void foreach(std::vector<int> values, void(funcPrint)(int))
{
    for(auto value:values)
        funcPrint(value);
} 


void print(int i)
{
    std::cout<<i <<std::endl;
}




int main(int argc, char **argv)
{

    {//fisrt example, c-style
        /////////////////////adder function example  ///////////////////////////////
        
        //binding
        adder_fn_ptr=adder; //or adder_fn_ptr=&adder

        //calling
        std::cout<<adder_fn_ptr(2,3) <<std::endl;

        //or
        std::cout<< (*adder_fn_ptr) (2,3) <<std::endl;
    }
    
    
    {//second example, c11-style
        
        auto adder_func=adder;
        std::cout<< (*adder_func) (2,3) <<std::endl;
        std::cout<< adder_func(2,3) <<std::endl;
    }
    
    {//third example, function pointer as function parameter
        std::vector<int> values={1,2,3,4};
        foreach(values,print);
    }
    
    {//forth example, the above could be done with lambda
        
        std::vector<int> values={1,2,3,4};
        foreach(values,[](int value){std::cout<<value <<std::endl;});
    }
    



}
