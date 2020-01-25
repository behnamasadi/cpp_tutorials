#include <string>
#include <iostream>
/*
        
               base
            /        \ 
         superA     superB  
            \        /
               child
*/

namespace normalInheritance
{
    class base 
    {
    public:
        base()
        {
            std::cout<<"base constructor" <<std::endl;
        }
    };

    class superA: public base
    {
        public:
        superA()
        {
            std::cout<<"superA constructor" <<std::endl;
        } 
    };

    class superB: public base
    {
        public:
        superB()
        {
            std::cout<<"superB constructor" <<std::endl;
        } 
    };

    class child: public superA, superB
    {
        public:
        child()
        {
            std::cout<<"child constructor" <<std::endl;
        }
    };

}

namespace virtualInheritance
{
    class base 
    {
    public:
        base()
        {
            std::cout<<"base constructor" <<std::endl;
        }
    };

    class superA:virtual public base
    {
        public:
        superA()
        {
            std::cout<<"superA constructor" <<std::endl;
        } 
    };

    class superB: virtual public base
    {
        public:
        superB()
        {
            std::cout<<"superB constructor" <<std::endl;
        } 
    };

    class child: public superA, superB
    {
        public:
        child()
        {
            std::cout<<"child constructor" <<std::endl;
        }
    };

}

int main()
{
    std::cout<<"===============Normal Inheritance===============" <<std::endl;
    {
        normalInheritance::child childObject;
    }
    
/*
"virtual" keyword make the classes superA and superB as virtual base classes to avoid two copies of base in child class.
*/
    
    std::cout<<"===============Virtual Inheritance===============" <<std::endl;
    {
        virtualInheritance::child childObject;
    }
    
}
