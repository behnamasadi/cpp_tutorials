#include <iostream>
#include <map>
#include <algorithm>
#include<unordered_map>
#include <iostream>
#include <set>
#include <unordered_set>
#include <string>
#include <iostream>
#include <set>
#include <iostream>
#include <map>
#include <string>
#include <utility>
/*
Both std::set and std::map are associative containers. The difference is that std::sets contain
 only the key, while in std::map there is an associated value. Choosing one over the other
depends mainly on what the task at hand is. If you want to build a dictionary of all the words
 that appear in a text, you could use a std::set<std::string>, but if you also want to count
 how many times each word appeared (i.e. associate a value to the key)
then you would need an std::map<std::string,int>. If you don't need to associate that count,
 it does not make sense to have the int that is unnecessary.
 
 
Tie:

The work of tie() is to unpack the tuple values into seperate variables. There are two variants of tie(), with and without “ignore” , the “ignore” ignores a particular tuple element and stops it from getting unpacked.
*/

std::tuple<int, double, std::string> tupleExample()
{
    return std::make_tuple(1,2.3, "Hello");
}

void pairExample()
{
    //std::make_pair()
    std::pair<std::string, int> item1,item2;
    item1.first="wieght";
    item1.second=12;
    
    item2=std::make_pair("size",12);
}

void mapExample ()
{
    //item are stored sorted
    std::map<std::string, int> items;
    std::pair< std::string,int > single_item;
    single_item.first="book";
    single_item.second=45;
    
    items.insert(single_item);
    
    
    single_item.first="beer";
    single_item.second=10;
    
    items.insert(single_item);
    
    single_item.first="wine";
    single_item.second=14;
    
    items.insert(single_item);
    
    
    
    //keys shoudl be unique,
    items["melon"]=3;
    //this will replace the old value
    items["melon"]=6;
    // this even won't be added
    single_item.first="melon";
    single_item.second=18;
    items.insert(single_item);
    
    items["water melon"]=6;
    

    for(std::map<std::string,int>::iterator it=items.begin();it!=items.end();++it)
    {
        std::cout<<it->first<< ":"<<it->second <<std::endl;
    }

    
//finding an item based on the key for an item in map

    std::string searchingKey="melon";
    std::cout<<searchingKey <<(items.find(searchingKey)!=items.end()? " found" :" not found") <<std::endl;
}

void unordered_mapExampel()
{
/*
    In an unordered_map elements are stored in a key value pair combination. But elements are stored in arbitrary order unlike associative containers where elements were stored in sorted order of keys.
    The basic advantage of using unordered_map instead of associative map is the searching efficiency. In an unordered_map complexity to search for an element is O(1) if hash code are chosen efficiently.
*/

  //item are stored sorted
    std::map<std::string, int> items;
    std::pair< std::string,int > single_item;
    
    single_item.first="wine";
    single_item.second=14;
    items.insert(single_item);
    
    
    
    single_item.first="beer";
    single_item.second=10;
    items.insert(single_item);
    
    single_item.first="book";
    single_item.second=45;
    items.insert(single_item);
    

    for(std::map<std::string,int>::iterator it=items.begin();it!=items.end();++it)
    {
        std::cout<<it->first<< ":"<<it->second <<std::endl;
    }


}

void multimapExample()
{
    std::multimap<std::string,int> items;
    std::pair<std::string,int> item;
    
    
    item.first="water";
    item.second=4;
    items.insert(item);
    
    item.first="book";
    item.second=1;
    items.insert(item);
    
    item.first="book";
    item.second=2;
    items.insert(item);
    
    item.first="beer";
    item.second=5;
    items.insert(item);
    
    item.first="wine";
    item.second=8;
    items.insert(item);
    
    item.first="book";
    item.second=9;
    items.insert(item);
    
    
    
    std::cout<< "printing all items:"<<std::endl;

    for(auto i:items)
        std::cout<< i.first<< ", "<<i.second  <<std::endl;
    
    std::string searchingKey="book";
 
/*
    Finds an element with key key. If there are several elements with key in the container, 
    the one inserted earlier is selected.
 */
    
    std::cout<< "searching for "<<searchingKey  <<std::endl;
    std::multimap<std::string,int>::iterator it= items.find(searchingKey);
    std::cout<<searchingKey<<" " <<(it!=items.end()? std::to_string(it->second) :"not found") <<std::endl;

    
    //finding all elements with same key
    typedef std::multimap<std::string, int>::iterator MMAPIterator;
	std::pair<MMAPIterator, MMAPIterator> result = items.equal_range(searchingKey);
    for (MMAPIterator it = result.first; it != result.second; it++)
		std::cout <<it->first<<":" << it->second << std::endl;
    
    
    
    int count = std::distance(result.first, result.second);
	std::cout << "Total values for key: {" <<searchingKey <<"} are : " << count << std::endl;


}


int setExample()
{
    std::set<std::string> items;
//     std::cout<<items.size() <<std::endl;
    
    //items are unique
    items.insert("bread");
    items.insert("water");
    items.insert("condom");
    items.insert("beer");
    
  
    
    //this won't add beer again
    items.insert("beer");
    
    
    
    std::cout<<"All available items are: (itesm are stored sorted in set)" <<std::endl;
    for(std::set<std::string>::iterator it=items.begin(); it!= items.end();it++ )
    {
        std::cout<<*it <<std::endl;
    }
    
    
    std::string searchingKey="water";
    std::cout<<searchingKey <<(items.find(searchingKey)!=items.end()? " found" :" not found") <<std::endl;
    return 0;
}

class student
{
public:
    int id;
    std::string fisrt_name;
    std::string last_name;
};

bool fncomp (int lhs, int rhs) {return lhs<rhs;}

struct classcomp {
	bool operator() (const int& lhs, const int& rhs) const
	{return lhs<rhs;}
};


void setFromUserDefinedTypeExample()
{
    //first way
    auto cmp = [](student left, student right) { return  (left.id>right.id);};
    auto set_of_students = std::set<int,decltype(cmp)>( cmp );

    
    
    //second way
    std::set<int,classcomp> fifth;                 // class as Compare

    //third way
	bool(*fn_pt)(int,int) = fncomp;
	std::set<int,bool(*)(int,int)> sixth (fn_pt);  // function pointer as Compare
}



void unordered_setExample()
{
    // Creating an Unoredered_set of type string
	std::unordered_set<std::string> items;
 
	// Insert strings to the set
	items.insert("z");
	items.insert("a");
	items.insert("b");
 
	// Try to Insert a duplicate string in set
	items.insert("a");
 
	// Iterate Over the Unordered Set and display it
	for (auto s : items)
		std::cout << s << std::endl;
    
    
    
    //more examples:
    std::set<int> first;                           // empty set of ints

	int myints[]= {10,20,30,40,50};
	std::set<int> second (myints,myints+5);        // pointers used as iterators

	std::set<int> third (second);                  // a copy of second

	std::set<int> fourth (second.begin(), second.end());  // iterator ctor.
   
}

void tieExample()
{
    int i_val; 
    char ch_val; 
    float f_val;    
      
    // Initializing tuple 
    std::tuple <int,char,float> tup1(20,'g',17.5);
    
    
    std::tie(i_val,ch_val,f_val) = tup1;
    
    std::tie(i_val,std::ignore,f_val) = tup1;
    std::cout<<i_val<<" " <<ch_val<<" " <<f_val  <<std::endl;
    
}

int main ()
{
   
//     multimapExample();
//     mapExample ();
//     setExample();
//     setExample();
//     unordered_setExample();
//    setFromUserDefinedTypeExample();
    
//     int i;
//     double d; 
//     std::string s;
//     std::tie(i,d,s)=tupleExample();
    
    tieExample();
    return 0;
}


