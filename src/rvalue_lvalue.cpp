#include <iostream>
#include <vector>

/*
Every c++ expression return either lvalue or rvalue

1)lvalue: An object that occupies some identifiable address in memory (heap, stack) and not in CPU register.
lvalues can be written on the left of assignment operator (=)

2)rvalue: An object that is not lvalue (all the other expressions)

Explicit rvalue defined using &&

most useful place for rvalue is overloading copy constructor and copy assignment operator.
Move semantic is implemented for all STL container

Move semantic is useful when you need to have both copy by value and copy by reference at the same time,
if you have only copy by ref then you don't need it.



*/

template <typename T>
void print(T &t)
{
    std::cout<<"lvalue" <<std::endl;
}

template <typename T>
void print(T &&t)
{
    std::cout<<"rvalue" <<std::endl;
}

int getValue ()
{
    int ii = 10;
    return ii;
}

void rvalue_lvalue_example()
{
    int a; // "a" is an lvalue
    std::cout<<"int a is: ";
    print(a);

    int& a_ref = a; // "a" is an lvalue  "a_ref" is a reference to an lvalue
    std::cout<<"int& a_ref is: ";
    print(a_ref);

    a = 2 + 2; // "a" is an lvalue ,  "2 + 2" is an rvalue
    std::cout<<"2 + 2 is: ";
    print(2 + 2);

   //  "a + 2" is an rvalue

    std::cout<<"a + 2 is: ";
    print(a + 2);

    std::cout<<"int&& c is: ";
    int&& c = std :: move(a); // "c" is an rvalue
    print(c);


    std::cout<<"int *p is: " ;
    int *p=&a;// p lvalue
    print(p);

    //Error, since i+2 is rvalue
    //int *m=&(a+2);

    std::string hello="hello";
    std::cout<<"std::string hello is: " ;
    print(hello);


    std::cout<<"\"hello\"is: " ;
    print("hello");



    std::cout<<"std::move(hello) is: " ;
    print(std::move(hello));
    std::cout << hello << std::endl; // Undefined.


    std::cout<<"getValue(): " ;
    print(getValue());



    std::string hello2 = "hello";
    std::vector <std::string > owner;
    owner.emplace_back (hello2); // Copy.
    owner.emplace_back (move(hello2)); // Move.

}

class Vector
{
public:
    int m_length;
    int * m_data;
    

    Vector(){}

    Vector(int length):m_length(length),m_data(new int[length])
    {
        std::cout<<"constructor called with size:"<< m_length<<std::endl;
    }
    

    //copy constructor, deep copy very costly
    Vector(const Vector &rhs)
    {
        std::cout<<"copy constructor wit size: "<<rhs.m_length <<std::endl;
        m_length=rhs.m_length;
        m_data=new int[m_length];
        for(int i=0;i<rhs.m_length;i++)
        {
            m_data[i]=rhs.m_data[i];
        }

    }

    //assignment operator
    Vector & operator =(Vector const & rhs)
    {
        std::cout<<"copy assignment with size: "<<rhs.m_length <<std::endl;

        m_length=rhs.m_length;
        m_data=new int[m_length];
        for(int i=0;i<rhs.m_length;i++)
        {
            m_data[i]=rhs.m_data[i];
        }
        return *this;
    }

    //move constructor
    Vector(Vector &&rhs)
    {
        std::cout<<"move constructor with size: "<<rhs.m_length <<std::endl;
        m_length=rhs.m_length;
        m_data=rhs.m_data;
        rhs.m_data=nullptr;
        rhs.m_length=0;
    }
    
    //move assignment
    Vector & operator =(Vector  && rhs)
    {
        std::cout<<"move assignment with size:"<<rhs.m_length <<std::endl;
        delete[] m_data;
        m_length=rhs.m_length;
        m_data=rhs.m_data;


        rhs.m_length=0;
        rhs.m_data=nullptr;

        return *this;
    }

    ~Vector()
    {
        std::cout<<"destructor called with size:" <<m_length<<std::endl;
        delete[] m_data;
    }
    
};

void copyMoveExample1()
{

    Vector A(5);
    Vector B(A);
    Vector C;
    C=B;
    Vector D(std::move(A));
    D=std::move(B);
    Vector F(4);



    std::vector<Vector> vec;
    //if we dont reserve, we will have lots of copy as we reach the vector capacity, terrible indeed
    vec.reserve(10);

/*

Since C++11 supports rvalue references, the Standard Library functions such as vector::push_back()
now define two overloaded versions:
1) one that takes const T& for lvalue arguments as before,
2) new one that takes a parameter of type T&& for rvalue arguments
*/


    vec.push_back(std::move(F));
    vec.emplace_back(Vector(12));

/*
However, we can enforce the selection of push_back(T&&) even in this case by casting an
lvalue to an rvalue reference using static_cast:
*/
    vec.push_back(static_cast<Vector&&>(Vector(7)));

}

Vector createVector(int size)
{
    return Vector(size);
}

void vectorConsumer(Vector v)
{
    //some dummy operation which change the v
}

void copyMoveExample2()
{
    Vector resuable_vec(4);
    //here we want to make some changes to our vector so sending by ref may not be desired.
    vectorConsumer(resuable_vec);

    vectorConsumer(Vector(10));

    Vector A(7);
    vectorConsumer(std::move(A));



    vectorConsumer(createVector(15));
}

int main(int argc, char *argv[])
{
    std::cout<<"*********************** rvalue lvalue example ***********************"<<std::endl;
    rvalue_lvalue_example();

    std::cout<<"*********************** copy, Move Example1 ***********************"<<std::endl;
    copyMoveExample1();

    std::cout<<"*********************** copy, Move Example2 ***********************"<<std::endl;
    copyMoveExample2();
    return 0;
}

