#include <iostream>
#include <string>


class Foo
{
private:
    int b;
    double c;
public:
    Foo(int i, double j):b(i), c(j)
    {
        std::cout << "Foo's constructor "  << "called with: "  << "b is:"<<b<<" and c is: "<< c   << std::endl;
    }

    Foo();
    void Print()
    {
        std::cout<< "b is:"<<b<<" and c is: "<< c <<std::endl;
    }
};



class Bar : public Foo
{
        public:
        Bar(int z, int y) : Foo( z,y )
        { 
            std::cout << "Bar's constructor" << std::endl;
        }
};



int main()
{
    Bar obj_Bar(2,3);
}
