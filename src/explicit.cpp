#include <iostream>
#include <string>
#include <algorithm>
#include <tuple>

//https://www.geeksforgeeks.org/advanced-c-conversion-operators/
//https://www.geeksforgeeks.org/g-fact-93/

namespace Implicit
{
    class Entity 
    {
        private:
            std::string name;
            int age;
        public:
            Entity (std::string name):name(name),age(-1){}
            Entity (int age):name("Unknown"),age(age){}
            std::tuple<int, std::string> getEntityElements()
            {
                return std::tuple<int,std::string>(this->age,this->name);
            }
    };
    void print(Entity &entity)
    {
        int age;
        std::string name;
        std::tie(age,name)=  entity.getEntityElements();
        std::cout<< "name:" << name<< " age: " <<age<<std::endl;
    } 

   
}

namespace Explicit
{
    class Entity 
    {
        private:
            std::string name;
            int age;
        public:
            explicit Entity (std::string name):name(name),age(-1){}
            explicit Entity (int age):name("Unknown"),age(age){}
            std::tuple<int, std::string> getEntityElements()
            {
                return std::tuple<int,std::string>(this->age,this->name);
            }

    };
    void print(Entity &entity)
    {
        int age;
        std::string name;
        std::tie(age,name)=  entity.getEntityElements();
        std::cout<< "name:" << name<< " age: " <<age<<std::endl;
    } 

   
}


int main()
{
    {
        Implicit::Entity a=Implicit::Entity(34);
        Implicit::Entity b=Implicit::Entity("Behnam");
        print(a);
        print(b);
    }
    
    {
/*
     This is actually possible because Entity has a constructor that can get only an integer or a string
*/ 
        Implicit::Entity a=34;
        Implicit::Entity b=std::string("Behnam");
        print(a);
        print(b);
    }
    
/*
        explicit keyword will disable the above conversion, so we should do it by casting
*/
    {
        Explicit::Entity a=(Explicit::Entity)34;
        Explicit::Entity b=(Explicit::Entity)std::string("Behnam");
        print(a);
        print(b);
    }
}
