#include <iostream>
#include <string>


/*
you should use member initializer list everywhere because of perfocmance issues.
*/


namespace without_member_initializer_list 
{
    class student {
    public:
        student()
        {
            std::cout << "created empty" << std::endl;
        }
        student(int x)
        {
            std::cout << "created empty with x:" << x << std::endl;
        }
    };

    class lecture
    {
    public:
        student m_student;
        lecture()
        {
            m_student = student(10);
        }
    };
}

namespace with_member_initializer_list
{
    class student {
    public:
        student()
        {
            std::cout << "created empty" << std::endl;
        }
        student(int x)
        {
            std::cout << "created empty with x:" << x << std::endl;
        }
    };

    class lecture
    {
    public:
        student m_student;
        // both of the followings can be used.
        //lecture():m_student( student(10)) { } 
        lecture() :m_student(10) { }
    };
}


////////////////////////////////////// initializing super class //////////////////////////////////////

class Foo
{
        public:
        Foo( int x ) 
        {
                std::cout << "Foo's constructor "  << "called with "  << x   << std::endl; 
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

int main()
{
    /*
    The following will print this:
    created empty
    created empty with x:10
    Because we are initializing student 2 times. 
    */
    without_member_initializer_list::lecture lec_without_member_initializer_list;
    /*
    The following will only print this:
    created empty with x:10
    Because we are initializing student only once.
    */

    with_member_initializer_list::lecture lec_with_member_initializer_list;
	
    Bar obj_Bar(2,3);
    B objb(10,12.3);
    objb.Print();
}
