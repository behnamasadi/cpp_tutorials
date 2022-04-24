#include <iostream>

class entity {
public:
    int x;
    std::string y;
};

class intArray
{
public:
    int size;
    int * data;

    intArray(int size)
    {
        size = size;
        data = new int[size];
    }

    ~intArray()
    {
        delete[] data;
    }

    intArray(std::initializer_list<int> list)
    {
        size = list.size();
        data = new int[list.size()];

        //std::initializer_list doesn't provide a subscript operator

        int count{ 0 };
        for (auto element : list)
        {
            data[count] = element;
            ++count;
        }

    }
};

int main()
{
    // Aggregate initialization (Initializations using public attributes)
    entity entity1{ 2011,"name" };
    entity entity2 = { 2011,"name"};


    /*
    initializer_list
    so far the only way to use our array is like this:
    */

    intArray array1(4);
    array1.data[0] =7 ;
    array1.data[1] = 3;
    array1.data[2] = 5;
    array1.data[3] = 6;

    /*
    with initializer_list, we can send an array directly:
    */


    intArray array2({ 1,2,3,4,5,6,7,8 });
    std::cout << array2.data[0] << std::endl;
    std::cout << array2.data[1] << std::endl;
    std::cout << array2.data[2] << std::endl;
}
