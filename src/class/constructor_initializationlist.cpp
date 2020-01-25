//http://www.cprogramming.com/tutorial/initialization-lists-c++.html
//http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8l.doc%2Flanguage%2Fref%2Fcplr388.htm
//http://www.devx.com/getHelpOn/10MinuteSolution/17298/1954

#include <iostream>
using namespace std;

//Using Initialization Lists to Initialize  class base constructor


class Foo
{
        public:
        Foo( int x ) 
        {
                std::cout << "Foo's constructor " 
                          << "called with " 
                          << x 
                          << std::endl; 
        }
};

class Bar : public Foo
{
        public:
        Bar(int z, int y) : Foo( y )  // construct the Foo part of Bar
        { 
            std::cout << z << std::endl; 
	    std::cout << "Bar's constructor" << std::endl; 
        }
};



//Using Initialization Lists to Initialize Fields
class B
{
private:
	int b;
	double c;
public:
	B(int i, double j):b(i), c(j){}

	B();
	void Print()
	{
		std::cout<< "b is:"<<b<<" and c is: "<< c <<std::endl;

	}
};




using namespace std;
int main()
{
	Bar obj_Bar(2,3);
	B objb(10,12.3);
	objb.Print();
}


