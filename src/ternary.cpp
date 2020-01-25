#include <iostream>

//ternary, one line if
void ternary()
{
    int a,b;
    a=10;
    b=5;
    //x = (condition) ? (value_if_true) : (value_if_false);
    std::cout <<  ((a>b) ? "a is bigger" :"b is bigger") << std::endl;
}

void ternaryElseIf()
{
    for(int i=0;i<10;i++)
    {
        std::cout<<( (i%4==0)?"mod is 0" 
                   :(i%4==1) ? "mod is 1"
                   :(i%4==2) ? "mod is 2"
                   : "mod is 3") <<std::endl;
    }
}

int main()
{
    //ternary();
    ternaryElseIf();
}
