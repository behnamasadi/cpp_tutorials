#include <iostream>
class base1
{
public:
    void foo()
    {
        std::cout<<"foo from base1" <<std::endl;
    }
protected:
    void info()
    {
        std::cout<<"base1" <<std::endl;
    }
};

class base2
{
public:
    void foo()
    {
        std::cout<<"foo from base2" <<std::endl;
    }
protected:
    void info()
    {
        std::cout<<"base2" <<std::endl;
    }
};

class derived : public base1,public base2
{
public:

    void foo()
    {
        std::cout<<"foo from base1" <<std::endl;
    }

    void info()
    {
        base1::info();
        base2::info();
        std::cout<<"derived" <<std::endl;
    }
};



int main()
{
    derived derivedObject;

    derivedObject.info();


    derivedObject.base1::foo();
    derivedObject.base2::foo();
    derivedObject.foo();
}
