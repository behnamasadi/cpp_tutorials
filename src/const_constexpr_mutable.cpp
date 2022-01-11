#include <iostream>
#include <string>
#include <vector>

void constVariableExample()
{
/*
//////////////////////////////////// (1) Const Variables ////////////////////////////////////

When declaring a const variable, it is possible to put const either before or after the type: that is, both
int const x = 5;
and
const int x = 4;
result in x's being a constant integer.
*/
    int const x = 5;
    const int y = 4;
    std::cout<<x  << y<<std::endl;
}


/*
//////////////////////////////////// (2) Const parameter ////////////////////////////////////

Here, a str object is passed by reference into verifyObjectCorrectness. For safety's sake, const is used to ensure that verifyObjectCorrectness cannot change the object-
*/
void verifyObjectCorrectness(const std::string& str) 
{
    
}


/*
//////////////////////////////////// (4) Const Functions ////////////////////////////////////
<return-value> <class>::<member-function>(<args>) const
{
        // ...
}
It is recommended practice to make as many functions const as possible so that accidental changes to objects are avoided.
*/
class Test { 
    int value; 
public: 
    Test(int v = 0) {value = v;} 
      
    // We get compiler error if we add a line like "value = 100;" 
    // in this function. 
    int getValueConst() const 
    {
        return value;
    }
    
    int getValue() 
    {
        return value;
    } 
};
void constFunctionExample()
{
    Test t(20); 
    std::cout<<t.getValueConst();
    
    
//     const Test t; 
//     int n=t.getValue()
    //std::cout << t.getValue(); 
    
    return; 
}


/*
//////////////////////////////////// (5)Const iterators ////////////////////////////////////
*/
void const_iteratorExample()
{
    std::vector<int> vec;
    vec.push_back( 3 );
    vec.push_back( 4 );
    vec.push_back( 8 );

    for ( std::vector<int>::const_iterator itr = vec.begin(), end = vec.end(); itr != end; ++itr )
    {
        // just print out the values...
        std::cout<< *itr <<std::endl;
    }
}



/*
//////////////////////////////////// (6) Const Pointers ////////////////////////////////////
Declares a pointer whose data cannot be changed through the pointer:
    const int *p = &someInt;
Declares a pointer who cannot be changed to point to something else:
    int * const p = &someInt;
to make it easy to read remove the variable type,  then read it like:
    const int *p;  ==> const  *p ; ==> *p is which is data is fixed;
    int * const p ==>  * const p ==> p is fixed which is an address;
*/



/*
//////////////////////////////////// (7)Const cast ////////////////////////////////////
*/


/*
If we dont put mutable before debugCounter, we can't have debugCounter++ in getName() as it is a const function and can't change anything
*/
class student 
{
    std::string name;
    int mutable debugCounter;
public: 
    student(std::string name): name(name), debugCounter(0){}
    void setName(std::string name)
    {
        name=name;
    }
    const std::string getName() const
    {
        debugCounter++;
        return name;
    }
};

int mutableExample()
{
    //first example
    student stdObject("jumbo");
    
    
    //second example
    int x=0;
    auto f=[=]() mutable
    {
        x++;
        std::cout<<x <<std::endl;
    };
    return 0;
}
/*
////////////////////////////////////// (8) constexpr //////////////////////////////////////
//https://www.geeksforgeeks.org/understanding-constexper-specifier-in-c/
//https://www.youtube.com/watch?v=4Vnd2I91s2c&t=152s
constexpr specifies that the value of an object or a function can be evaluated at compile time
 and the expression can be used in other constant expressions.
For example, in below code product() is evaluated at compile time.
*/

//First example
// constexpr function for product of two numbers.
// By specifying constexpr, we suggest compiler to
// to evaluate value at compiler time
// you can call this function like:  const int x = product(10, 20);

constexpr int product(int x, int y)
{
    return (x * y);
}


//Second example
/*

you can call it like:
const long int res = fib(30);

if you remove the constexpr and check the run time you will see the difference
$time ./a.out

*/

//Third example
/*
This only works because of constexpr
 int a[fib(3)];

*/
constexpr long int fib(int n)
{
    return (n <= 1)? n : fib(n-1) + fib(n-2);
}




int main()
{
    //constFunctionExample();
    //constPointersExample();
}

