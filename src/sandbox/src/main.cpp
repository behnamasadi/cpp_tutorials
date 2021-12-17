#include <iostream>
#include <vector>
#include <algorithm>


class  base
{
private:
    int x;
    int y;
protected:
    int z;
public:
    int virtual getx()=0 ;
    int gety(){}

};


class derived : public base
{
public:
    int  getx() override ;
};

int derived::getx()
{
    return z;
}


int main ()
{
    //base b;
    //std::cout<<b.getx() <<std::endl;
}



