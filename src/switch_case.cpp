#include <iostream>



int main(int argc, char *argv[])
{
    int number;
    std::cin>>number;
    switch (number)
    {
    case 2:
        std::cout<<"number is two." <<std::endl;
/*
        If we remove this break, our program will continue from next block so we will have in output
        number is two.
        number is ten.
*/
        break;
    case 10:
        std::cout<<"number is ten." <<std::endl;
        break;
    default:
            std::cout<<"number is neither two or ten." <<std::endl;
        break;
    }
    return 0;
}
