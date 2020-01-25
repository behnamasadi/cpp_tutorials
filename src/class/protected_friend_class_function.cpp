#include <iostream>
#include <memory>
/*
You can only access protected members in instances of your type (or derived from your type).
You cannot access protected members of an instance of a parent or cousin type.
*/


class base
{
    friend class frindClass;
    friend void friendFunction(int y);
private :
    int x;
    void privateMethod()
    {
        std::cout<<"privateMethod from base" <<std::endl;
    }

protected:
    void info()
    {
        std::cout<<"info from base" <<std::endl;
    }
public:
    base(int x):x(x){};
    void print()
    {
        std::cout<<"base" <<std::endl;
    }
};


class derived:public base
{
public:
    derived(int x):base(x){};
    void print()
    {
        std::cout<<"derived" <<std::endl;
    }
    void infoCaller()
    {
        info();
    }

};

void friendFunction(int y)
{
    base baseObject(5);
    std::cout<<baseObject.x+y <<std::endl;
}

class frindClass
{
    std::shared_ptr<base> b ;
public:
    frindClass(int x)
    {
        b.reset(new base(x) );
    }
    void getinfo()
    {
        b->info();
        std::cout<<b->x <<std::endl;
    }
};
int main(int argc, char *argv[])
{

    base baseObject(5);
    /*This will give you an error*/
    //baseObject.info();


    derived derivedObject(10);
    /*This will give you an error*/
    //derivedObject.info();

    std::cout<<"The correct way to call a protectd method from base class in derived class"<<std::endl;
    derivedObject.infoCaller();

    std::cout<<"Accessing private member in a friend function"<<std::endl;
    int y=3;
    friendFunction(y);


    std::cout<<"Accessing private member in a friend class"<<std::endl;
    frindClass frindClassObject(y);
    frindClassObject.getinfo();


    return 0;
}

