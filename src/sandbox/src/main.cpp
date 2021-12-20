#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

class Base
{
    int * data;
public:
    Base(int size)
    {
        std::cout<<"base constrcutor" <<std::endl;

        data=new int[size];
    }
    virtual ~Base()
    {
        std::cout<<"base destrcutor" <<std::endl;
        delete []data;
    }
};

class Derived: public Base
{
    double * points;
    public:
        Derived(int amount,int size):Base(size)
        {
            std::cout<<"derived constrcutor" <<std::endl;

            points=new double[amount];
        }
        ~Derived()
        {
            delete [] points;
            std::cout<<"derived destrcutor" <<std::endl;
        }
};


class wallet
{
public:
    int * data;
    int m_size;
    wallet(int size)
    {
        m_size=size;
        data=new int[m_size];
        std::cout<<"cons called" <<std::endl;
    }

    ~wallet()
    {
        delete [] data;
        std::cout<<"des called" <<std::endl;
    }

//    wallet& operator =()
//    {

//    }


    int operator ()()
    {
        return m_size;
    }
};

#include <iostream>
#include <memory>

class person
{
public:
    std::string name;
    int id;
    person()
    {
        std::cout<<"person constructed" <<std::endl;
        id=-1;
    }
    person(int id)
    {
        this->id=id;
        std::cout<<"person with id "<< id<<" constructed" <<std::endl;
    }
    ~person()
    {
        std::cout<< ((id>-1)? std::string("person with ") + std::to_string(this->id)  +std::string(" destructed"):"person destructed"   )<<std::endl;
    }




};

class employee: public person
{
public:
    int id;
};

class manager:public person
{
public:
    std::vector<int> listOfDepartments;
};



int main()
{
    // OLD, problem with dangling pointer
    // PROBLEM: ref will point to undefined data!

    int* ptr = new int(10);
    int* ref = ptr;
    delete ptr;

    // NEW
    // SOLUTION: check expired() or lock() to determine if pointer is valid

    // empty definition
    std::shared_ptr<int> sptr;

    // takes ownership of pointer
    sptr.reset(new int);
    *sptr = 10;

    // get pointer to data without taking ownership
    std::weak_ptr<int> weak1 = sptr;

    // deletes managed object, acquires new pointer
    sptr.reset(new int);
    *sptr = 5;

    // get pointer to new data without taking ownership
    std::weak_ptr<int> weak2 = sptr;

    // weak1 is expired!
    if(auto tmp = weak1.lock())
        std::cout << *tmp << '\n';
    else
        std::cout << "weak1 is expired\n";

    // weak2 points to new data (5)
    if(auto tmp = weak2.lock())
        std::cout << *tmp << '\n';
    else
        std::cout << "weak2 is expired\n";


    std::cout<<"sizeof(char *)= " << sizeof(char *)<<std::endl;
    std::cout<<"sizeof(int *)= " <<sizeof(int *)<<std::endl;
    std::cout<<"sizeof(void *)= " <<sizeof(void *)<<std::endl;
    std::cout<<"sizeof(double *)= " <<sizeof(double *)<<std::endl;
    std::cout<<"sizeof(person *)= " <<sizeof(person *)<<std::endl;
    std::cout<<"sizeof(employee *)= " <<sizeof(employee *)<<std::endl;
}
