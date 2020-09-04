#include <iostream>

/*
1. Static Cast
    It is a compile time cast. static_cast performs a tight type checking.  

2. Dynamic Cast, 
    This cast is executed at runtime, not compile time. It is exclusively used for handling polymorphism.
    
    You can use it for more than just casting downwards – you can cast sideways or even up another chain. The dynamic_cast will seek out the desired object and return it if possible. If it can't, it will return nullptr in the case of a pointer, or throw std::bad_cast in the case of a reference.
    
    RTTI (Run-time type Information):
    In C++, RTTI (Run-time type information) is a mechanism that exposes information about an object’s data type at runtime and is available only for the classes which have at least one virtual function. It allows the type of an object to be determined during program execution
    

3. Const Cast
//https://www.geeksforgeeks.org/const_cast-in-c-type-casting-operators/

4. Reinterpret Cast
    It is used to convert one pointer of another pointer of any type, no matter either the class is related to each other or not. It does not check if the pointer type and data pointed by the pointer is same or not.
    
*/


/////////////////////////////////Static Cast/////////////////////////////////////
class Int 
{ 
    int x; 
  
public:

//     Int(int x_in = 0): x(x_in){}
     Int(int x_in = 0): x{x_in}{}
 
    operator std::string() 
    { 
        std::cout << "Conversion Operator" << std::endl; 
        return std::to_string(x); 
    } 
}; 

void staticCastExmple()
{
    {// first example
        float f = 3.5; 
        int a = f; // c style 
        int b = static_cast<int>(f);// recommended
        std::cout <<"b: "<<b <<"a:"<<a << std::endl;
    }
    
    {//second example
       
        char c = 'a'; 

        //c style casting,pass at compile time, may fail at run time 
        int* q = (int*)&c;
        std::cout<< "q"<<* q<<std::endl;
        // this is will fail in comile time
        //int* p = static_cast<int*>(&c);
    }
    
    {// third example
        
        Int obj(3);
        // compiler will not thrown an error as we have defined the Conversion operator.
        std::string str = obj; 
        
        obj = 20; 
        std::string str2 = static_cast<std::string>(obj); 
        obj = static_cast<Int>(30); 
        
    }
    
    
}


//////////////////////////////Dynamic Cast///////////////////////////
class Base
{
/*
RTTI (Run-time type information) is a mechanism that exposes information about an object’s data type at runtime and is available only for the classes which have at least one virtual function.
*/
    virtual void f() { }
public:
    void whoAmI()
    {
        std::cout<<"Base" <<std::endl;
    }
};
class Derived:public Base
{
    public:
    void whoAmI()
    {
        std::cout<<"Derived" <<std::endl;
    }
};

class Foo{};

void dynamicCastExample()
{
    Base base;
    Derived derived;


    Derived* derived1 = dynamic_cast<Derived*> (&base);  // nullptr, because 'base' is not a 'derived'
    std::cout<<"derived1 is: " <<(derived1 ==nullptr ? "nullptr":"object of Derived class")<<std::endl;
    
    
    
    Base* basePointer = new Base;
    Derived* derived2 = dynamic_cast<Derived*> (basePointer); 
    std::cout<<"derived2 is: " <<(derived2 ==nullptr ? "nullptr":"object of Derived class")<<std::endl;

    
    Base *b = new Derived; 
    Derived *d = dynamic_cast<Derived*>(b);
    std::cout<<"d is: " <<(d ==nullptr ? "nullptr":"object of Derived class")<<std::endl;
   
    
    Foo* foo = dynamic_cast<Foo*> (basePointer);  // nullptr
    std::cout<<"foo is: " <<(foo ==nullptr ? "nullptr":"object of Derived class")<<std::endl;
    
    
    Base& baseReference = dynamic_cast<Base&> (*basePointer); // Ok.
   
    
//     Derived& derivedReference = dynamic_cast<Derived&> (*basePointer); // Ok.
    
    //this will create a std::bad_cast
    //Foo& fooReference = dynamic_cast<Foo&> (*basePointer);
    
    
}

/////////////////////////////////Const Cast///////////////////////////////////


/////////////////////////////Reinterpret Cast/////////////////////////////////
struct mystruct { 
    int x; 
    int y; 
    char c; 
    bool b; 
}; 
void reinterpretCastExample()
{
    {
        int* p = new int(65); 
        char* ch = reinterpret_cast<char*>(p); 
        std::cout << *p << std::endl; 
        std::cout << *ch << std::endl; 
        std::cout << p << std::endl; 
        std::cout << ch << std::endl;
    }
    
    
    {
    
        mystruct s; 
    
        // Assigning values 
        s.x = 5; 
        s.y = 10; 
        s.c = 'a'; 
        s.b = true; 
    
        // data type must be same during casting 
        // as that of original 
    
        // converting the pointer of 's' to, 
        // pointer of int type in 'p'. 
        int* p = reinterpret_cast<int*>(&s); 
    
        std::cout << sizeof(s) << std::endl; 
    
        // printing the value currently pointed by *p 
        std::cout << *p << std::endl; 
    
        // incrementing the pointer by 1 
        p++; 
    
        // printing the next integer value 
        std::cout << *p << std::endl; 
    
        p++; 
    
        // we are casting back char * pointed 
        // by p using char *ch. 
        char* ch = reinterpret_cast<char*>(p); 
    
        // printing the character value 
        // pointed by (*ch) 
        std::cout << *ch << std::endl; 
    
        ch++; 
    
        /* since, (*ch) now points to boolean value, 
        so it is required to access the value using  
        same type conversion.so, we have used  
        data type of *n to be bool. */
    
        bool* n = reinterpret_cast<bool*>(ch); 
        std::cout << *n << std::endl; 
    
        // we can also use this line of code to 
        // print the value pointed by (*ch). 
        std::cout << *(reinterpret_cast<bool*>(ch));
    }
    
}



int main()
{
    //staticCastExmple();
    dynamicCastExample();
    //reinterpretCastExample();
}
