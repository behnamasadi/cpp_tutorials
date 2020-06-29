#include <iostream>

/* Function Object */

class myFunctorClass
{
    public:
        myFunctorClass (int x) : _x( x ) {}
        int operator() (int y) { return _x + y; }
    private:
        int _x;
};

/*
Most vexing parse
It is an  ambiguity resolution in the C++. A piece of code could be parsed as function deceleration
or  variable definition.
*/

class Timer {
 public:
    Timer(){}
};

class TimeKeeper {
 public:
    TimeKeeper(const Timer& t){}

  int get_time();
};

void f(double adouble) {
  int i(int(adouble));
}

int main()
{
/////////////////////////// Function Object ///////////////////////////

    //constructor
    myFunctorClass addFive( 5 );

    //() operator
    std::cout << addFive( 6 );

/////////////////////////// Most vexing parse ///////////////////////////


/*
This line:
    TimeKeeper time_keeper(Timer());
Could cause confuion and has multiple interpertations:
1) This a function delration for a function named "time_keeper" which has return type of
 "TimeKeeper" and has a single (unnamed) parameter that is a pointer to function returning
an object of type Timer (and taking no input)

2) A variable definition for variable time_keeper of class TimeKeeper, initialized with
an anonymous instance of class Timer or
*/

//force the compiler to consider this as a variable definition are:

    {
        /*
        1) Adding extra pair of parentheses will tell the compiler that this doesn't
        sound like function deceleration, as we usually don't have multiple parentheses
        around input parameters.
        */
        TimeKeeper time_keeper((Timer()));

    }

    {
        /*
         2) Using copy initialization:
        */
        TimeKeeper time_keeper = TimeKeeper(Timer());

    }

    {
        /*
         3)Using uniform initialization:
        */
        TimeKeeper time_keeper1{Timer()};
        TimeKeeper time_keeper2(Timer{});
        TimeKeeper time_keeper3{Timer{}};
    }

}
