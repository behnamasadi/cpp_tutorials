//http://www.cprogramming.com/tutorial/stl/stllist.html


#include <iostream>
#include <list>


/*
The Standard Template Library's list container is implemented as a doubly linked list. 
You might wonder why there are both list and vector containers in the STL -- the reason is that the underlying representations are different, 
and each representation has its own costs and benefits. The vector has relatively costly insertions into the middle of the vector, 
but fast random access, whereas the list allows cheap insertions, but slow access (because the list has to be traversed to reach any item)
*/

void print(std::list<int> &list)
{    
    std::cout<<"Iterating through the list" <<std::endl;    
    for(auto i:list)
        std::cout<<i <<std::endl;
}

int main ()
{

////////////////////////////////push_back, push_front///////////////////////////////
    std::list<int> list_of_numbers;
    list_of_numbers.push_back(1);
    list_of_numbers.push_back(2);
    list_of_numbers.push_back(2);
    list_of_numbers.push_back(3);
    list_of_numbers.push_back(4);
    list_of_numbers.push_front(0);
    print(list_of_numbers);

    
////////////////////////////////remove, remove_if///////////////////////////////

    std::cout<<"removing second element:" <<std::endl;
    std::list<int>::iterator itr = list_of_numbers.begin();
    itr++;
    list_of_numbers.erase( itr);
    print(list_of_numbers);

////////////////////////////////delete///////////////////////////////
    // delete all elements with value 2
    std::cout<<"delete all elements with value 2:" <<std::endl;
    list_of_numbers.remove(2);
    print(list_of_numbers);

////////////////////////////////remove, remove_if///////////////////////////////
    std::cout<<"delete all even numbers:" <<std::endl;
    auto even=[](int x){return( x%2==0);};
    list_of_numbers.remove_if(even);
    print(list_of_numbers);
    
////////////////////////////////front, back element///////////////////////////////

    std::cout<<"front element:" <<std::endl;
    std::cout<<list_of_numbers.front() <<std::endl;
    
    std::cout<<"back element:" <<std::endl;
    std::cout<<list_of_numbers.back() <<std::endl;
    
////////////////////////////////assign///////////////////////////////
    
    int count, value;
    count=3; 
    value=7;
    std::list<int> second_list;
    second_list.assign(count, value);
    list_of_numbers.assign(second_list.begin(), second_list.end());
    print(list_of_numbers);
}
