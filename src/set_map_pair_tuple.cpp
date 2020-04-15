#include <iostream>
#include <algorithm>
#include <string>
#include <map>     
#include<unordered_map>
#include <set>
#include <unordered_set>
#include <sstream>

/*
//////////////////////////// std::map, std::set ///////////////////////
std::map is red black tree and NOT hash table.

Both std::set and std::map are associative containers. The difference is that std::sets contain
 only the key, while in std::map there is an associated value. Choosing one over the other
depends mainly on what the task at hand is. If you want to build a dictionary of all the words
 that appear in a text, you could use a std::set<std::string>, but if you also want to count
 how many times each word appeared (i.e. associate a value to the key)
then you would need an std::map<std::string,int>. If you don't need to associate that count,
 it does not make sense to have the int that is unnecessary.
 
 
Time complexity of map operations is O(Log n) while for unordered_set, it is O(1) on average.

//////////////////////////// unordered_map, multimap, unordered_set, multiset  ///////////////////////

C++ 11 added std::unordered_map (as well as std::unordered_set and multi versions of both), 
which is based on hashing.
Map is implemented as balanced tree structure that is why it is possible to maintain an order between the elements (by specific tree traversal). Time complexity of map operations is O(Log n) while for unordered_set, it is O(1) on
average.


To be able to use std::unordered_map (or one of the other unordered associative containers) with a user-defined 
key-type, you need to define two things:

1) A hash function; this must be a class that overrides operator() and calculates the hash value given an object of the key-type. One particularly straight-forward way of doing this is to specialize the std::hash template for your key-type.

2) A comparison function for equality; this is required because the hash cannot rely on the fact that the hash function will always provide a unique hash value for every distinct key (i.e., it needs to be able to deal with collisions), so it needs a way to compare two given keys for an exact match. You can implement this either as a class that overrides operator(), or as a specialization of std::equal, or – easiest of all – by overloading operator==() for your key type (as you did already).

 
//////////////////////////////  Tie ////////////////////////////// 

The work of tie() is to unpack the tuple values into seperate variables. There are two variants of tie(), with and without “ignore” , the “ignore” ignores a particular tuple element and stops it from getting unpacked.

////////////////////////////// tuple ////////////////////////////// 
A tuple is an object that can hold a number of elements. The elements can be of different data types.
*/

void tupleExample()
{                
    std::tuple<int, double, int, std::string > mytuple = std::make_tuple(10, 12.4, 3, "this is a tuple");
    //get() is used to access the tuple values and modify them, it accepts the index and tuple name as arguments 
    
    std::cout<<std::get<0>(mytuple) <<std::endl;
    std::cout << std::get<1>(mytuple) << std::endl;
    std::cout << std::get<2>(mytuple) << std::endl;
    std::cout << std::tuple_size<decltype(mytuple)>::value << std::endl;

    // tie() : unpack the tuple values into seperate variables. There are two variants of tie()

    int i; 
    double d; 
    int n; 
    std::string s;
    std::tie(i, d, n, s) = mytuple;
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

    

}

void checkExsitanceOfKeyInMap()
{
    //using std::map::count() 
    //finding an item based on the key for an item in map

    std::map<std::string, int> wordMap = { {"a",0}, {"b",1}, {"c",2} };

    if (wordMap.count("a") > 0)
    {
        std::cout << "'a' Found" << std::endl;
    }
    
    
    
    // using std::map::find
    std::map<std::string, int> items;
    std::string searchingKey="melon";
    std::cout<<searchingKey <<(items.find(searchingKey)!=items.end()? " found" :" not found") <<std::endl;
    
    
    // when key doesn't exist:
    if(items["mumbo jumo"]==NULL)
    {
        std::cout<<"not found" <<std::endl;
    }
    

}

///////////////////////// unordered_map ///////////////////////////

void unordered_mapExample()
{


    std::cout<<"item are sorted in std::map \nThe order that we inserted the items is: wine, beer, book" <<std::endl;
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
    
    std::cout<<"The order that we iterate items in map is:" <<std::endl;

    for(std::map<std::string,int>::iterator it=items.begin();it!=items.end();++it)
    {
        std::cout<<it->first<< ":"<<it->second <<std::endl;
    }
}

void wordFrequencyInString()
{
    std::unordered_map<std::string, int> wordFreq;
    std::string str="a a b d c a d x";
  
    // breaking input into word using string stream 
    std::stringstream ss(str);  // Used for breaking words 
    std::string word; // To store individual words 
    while (ss >> word) 
        wordFreq[word]++; 
  
    // now iterating over word, freq pair and printing 
    // them in <, > format 
    std::unordered_map<std::string, int>:: iterator p; 
    for (p = wordFreq.begin(); p != wordFreq.end(); p++) 
        std::cout << "(" << p->first << ", " << p->second << ")\n";
}


class student
{
public:
    int id;
    std::string first_name;
    std::string last_name;
    
    bool operator==(const student &other) const
    {
        return (first_name == other.first_name   && last_name == other.last_name  && id == other.id);
    }
};

// example for user-defined hash functions:

namespace std {

  template <>
  struct hash<student>
  {
    std::size_t operator()(const student& k) const
    {
    using std::size_t;
    using std::hash;
    using std::string;

    // Compute individual hash values for first,
    // second and third and combine them using XOR
    // and bit shifting:

    return ((hash<string>()(k.first_name)
            ^ (hash<string>()(k.last_name) << 1)) >> 1)
            ^ (hash<int>()(k.id) << 1);;
    }
  };

}

//If you don't want to specialize template inside the std namespace (although it's perfectly legal in this case), you can define the hash function as a separate class and add it to the template argument list for the map:

struct KeyHasher
{
  std::size_t operator()(const student& k) const
  {
    using std::size_t;
    using std::hash;
    using std::string;

    return ((hash<string>()(k.first_name)
             ^ (hash<string>()(k.last_name) << 1)) >> 1)
             ^ (hash<int>()(k.id) << 1);
  }
};

bool fncomp (int lhs, int rhs) {return lhs<rhs;}

struct classcomp {
	bool operator() (const int& lhs, const int& rhs) const
	{return lhs<rhs;}
};


void unordered_mapCustomClasstype()
{
    std::unordered_map<student,std::string> student_umap
    = {  { {1,"John", "Doe"}, "example"},  { {2,"Mary", "Sue"}, "another"} };
    
    
    std::unordered_map<student,std::string,KeyHasher> m6 = {
    { {1,"John", "Doe"}, "example"},
    { {2,"Mary", "Sue"}, "another"}};
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
    Finds an element with a key. If there are several elements with key in the container, 
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

template <typename T>
typename T::iterator min_map_element(T& m)
{
    return std::min_element(m.begin(), m.end(), [](typename T::value_type& l, typename T::value_type& r) -> bool { return l.second < r.second; });
}


int main ()
{
    unordered_mapExample();
    return 0;
}


