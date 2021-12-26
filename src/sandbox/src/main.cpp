#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <string>

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
    //std::vector<person> vectPerson={person(10),person(12),person(3)};
    std::vector<person> vectPerson;
//    vectPerson.emplace_back(person(10));
//    vectPerson.emplace_back(person(12));
//    vectPerson.emplace_back(person(3));


    vectPerson.push_back(person(10));
    vectPerson.push_back(person(12));
    vectPerson.push_back(person(3));


    for(auto &p:vectPerson)
    {
        std::cout<<p.id<<std::endl;
    }
}
