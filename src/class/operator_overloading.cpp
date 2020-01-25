/*

In this example we gonna overload the following operators:
()
-
<<
>>
= deep copy (Copy constructor, Assignment operator)
*/
#include <iostream>
#include <istream>
#include <random>


/*
any of the following 38 (until C++20)40 (since C++20) operators:+ - * / % ^ & | ~ ! = < > += -=
*= /= %= ^= &= |= << >> >>= <<= == != <= >= <=> (since C++20) && || ++ -- , ->* -> ( ) [ ]
* co_await (since C++20)
*/

class money
{
private:
    int size=10;
    

public:
    int value;
    int *data;

money(int value=10)
{
    this->value=value;
    std::cout<< "Constructor money with value: "<< this->value<<std::endl;

    data=new int [size];
    
    int upperBound=20;
    int lowerBound=0;
    int numberOfRandomNumbers=size;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(lowerBound, upperBound);

    for (int n=0; n<numberOfRandomNumbers; ++n)
       data[n]= dis(gen);
}

~money()
{
    std::cout<< "Destructor money with value: "<< this->value <<std::endl;
    delete []data;
}

double operator ()()
{
    return value;
}

friend money operator -(const money &t1, int value);
friend std::ostream& operator<<(std::ostream& os, const money& t);
friend std::istream& operator >> (std::istream &is,const money& t);


bool operator < (const money& other) const
{
    return value<other.value;
}


money operator - (const money &other)
{
    value-other.value;
    return *this;
}



};


money operator -(const money &t1, int value)
{
    return money(t1.value-value);
}

std::ostream& operator<<(std::ostream& os, const money& t)
{
    for(int i=0;i<t.size;i++)
        os << t.data[i]<<" ";
    return os;
}



std::istream& operator >> (std::istream& is, const money& t)
{
    is>>t.value;
    return is;
}




int main()
{

////////////////////////// - operator overloading ///////////////////////

    money  money1(7),money2(3);
    std::cout<<"The operator - gives you:" <<std::endl;
    std::cout<< money2-money1<<std::endl;

////////////////////////// < operator overloading ///////////////////////

    std::cout<< (money2<money1)<<std::endl;

////////////////////////// () operator overloading ///////////////////////

    std::cout<< "The operator () gives you:\n"<<money1()<<std::endl;

//////////////////////// << operator overloading ///////////////////////

    std::cout<< "The operator () << gives you:\n"<< money1<<std::endl;
    
////////////////////// >> operator overloading ///////////////////////
//     getchar();
//     std::cin>> money1;
//     std::cout<< "The operator >> gives you:"<< money1<<std::endl;


}
