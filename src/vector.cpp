#include <algorithm>
#include <cstring>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

template <typename T> void printVector(std::vector<T> v) {
  for (T entity : v)
    std::cout << entity << std::endl;
}

template <typename T> void printArray(T array, std::size_t arraySize) {
  for (std::size_t i = 0; i < arraySize; i++)
    std::cout << array[i] << std::endl;
}

// vector to c style array
void cStyleArrayToVecExample() {
  int numbers[4] = {3, 1, -1, 5};
  // first way
  std::cout << "c style array to vector first way" << std::endl;
  std::vector<int> vec1FromArray(std::begin(numbers), std::end(numbers));
  printVector(vec1FromArray);

  // second way
  std::cout << "c style array to vector second way" << std::endl;
  std::vector<int> vec2FromArray(numbers, numbers + sizeof(numbers) /
                                                        sizeof(numbers[0]));
  printVector(vec2FromArray);

  // when we don't know the type of array elements
  int x[] = {1, 2, 3};
  // First method
  using arrElemType = std::remove_reference<decltype(*x)>::type;

  // Secodn Method
  // typedef std::remove_reference<decltype( *x )>::type arrElemType;
  std::vector<arrElemType> z(std::begin(x), std::end(x));
  printVector(z);
}

void vecTocStyleArrayExample() {
  std::cout << "Vector to c style array first way (no copying)" << std::endl;
  std::vector<int> vec = {3, 1, -1, 5};
  printArray(&vec[0], vec.size());

  // second way
  std::cout << "Vector to c style array second way (memory copying)"
            << std::endl;

  int array[4];
  std::memcpy(array, &vec[0], vec.size() * sizeof(vec[0]));
  printArray(array, vec.size());

  // third way
  std::cout << "Vector to c style array second way (std::copy)" << std::endl;
  std::copy(vec.begin(), vec.end(), array);
  printArray(array, vec.size());
}

void stringToCharVec() {
  std::string str("ali baba");
  std::vector<char> charVec(str.begin(), str.end());
  printVector(charVec);
}

void reinterpretVector() {
  // https://stackoverflow.com/questions/1953621/reinterpret-float-vector-as-unsigned-char-array-and-back
  std::vector<double> p_vector;
  const unsigned char *sourceArray =
      reinterpret_cast<const unsigned char *>(&(p_vector[0]));

  typedef unsigned char byte;
  std::vector<float> original_data;

  if (!original_data.empty()) {
    const float *p_floats = &(original_data[0]);
    const byte *p_bytes = reinterpret_cast<const byte *>(p_floats);
  }
}

void vectorExample() {
  // testvector has a 3 element, all of them are 100
  std::vector<int> testvector(3, 100);

  std::vector<int> vec = {3, 4, 1, 5, 2, 7, 8, 0};
  std::cout << "vec.size()=" << vec.size() << std::endl;
  std::cout << "vec.capacity()=" << vec.capacity() << std::endl;

  std::cout << "Iterating over a vector with subscripts is more efficient "
               "because at(i) is a function call"
            << std::endl;
  for (unsigned int i = 0; i < vec.size(); i++)
    std::cout << vec[i] << std::endl;

  std::cout << "Iterating over a vector with an iterator" << std::endl;
  for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it)
    std::cout << *it << std::endl;

  //  Difference between std::vector::front() and begin()
  // Difference between Iterators and Pointers
  // https://www.geeksforgeeks.org/difference-between-iterators-and-pointers-in-c-c-with-examples/?ref=leftbar-rightbar
  std::cout << "front() Returns a reference to the first element in the vector "
               "container."
            << std::endl;
  std::cout << vec.front() << std::endl;
  std::cout << "begin returns an iterator to this same element." << std::endl;
  std::cout << *vec.begin() << std::endl;

  std::cout
      << "Returns a reference to the last element in the vector container."
      << std::endl;
  std::cout << vec.back() << std::endl;

  std::cout << "pop_back" << std::endl;
  while (!vec.empty()) {
    std::cout << vec.back() << std::endl;
    vec.pop_back();
  }

  vec = {3, 4, 1, 5, 2, 7, 8, 0};
  // erase the 3th element
  vec.erase(vec.begin() + 1);

  // erase the first 2 elements:
  vec.erase(vec.begin(), vec.begin() + 2);

  // the 0th element would be 200
  testvector.insert(testvector.begin(), 200);

  // it will add two element
  testvector.insert(testvector.begin(), 2, 300);

  // or we can add an other vector to testvector
  testvector.insert(testvector.begin(), vec.begin(), vec.end());

  // assignment (equal sign operator) in vectors is a deep copy
  std::vector<int> vec1, vec2;
  vec1 = {3, 4, 5, 5};
  vec2 = vec1;
  vec1.clear();
  std::cout << "vec1:" << std::endl;
  printVector(vec1);
  std::cout << "vec2:" << std::endl;
  printVector(vec2);
}

/*
very important tip, since here pushing during the loop, the destination vector
might need to resize couple of times, so we resize it once in the begining
*/
void vectorPushBack() {
  int size = 10;
  int value = 0;
  std::vector<int> src(size, value);
  std::vector<int> dst;
  dst.resize(src.size());
  for (auto i : src)
    dst.push_back(i);
}

/*
Very important tip, we can access vector element both by [i] operator and by
.at(i) function. at(i) is a function call while [] is a direct access so it is
cheaper and more efficient
*/
void vectorAccessingElements() {
  int size = 10;
  int value = 0;
  std::vector<int> src(size, value);
  for (auto i : src)
    std::cout << src[i];
}

void vectorInitialization() {
  // Initializing by pushing values one by one :
  std::vector<int> vect1;

  vect1.push_back(10);
  vect1.push_back(20);
  vect1.push_back(30);

  // Specifying size and initializing all values  Create a vector of size 10
  // with  all values as 0.5.
  int size = 10;
  double value = 0.5;
  std::vector<double> vect2(size, value);

  // Initializing like arrays :
  std::vector<int> vect3{1, 2, 3};
}

class human {

  double randomGenerator(double rangeFrom = 150, double rangeTo = 200) {
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_real_distribution<double> distr(rangeFrom, rangeTo);
    // std::cout<< distr(generator)<<std::endl;
    return distr(generator);
  }

public:
  int age;
  std::string name;
  double height;

  human(int age, std::string name) {
    this->age = age;
    this->name = name;
    std::cout << "constructor!" << std::endl;
    height = randomGenerator();
  }
  human() {}
};

void resizeVSreserve() {

  /*
  https://stackoverflow.com/questions/7397768/choice-between-vectorresize-and-vectorreserve
  https://www.geeksforgeeks.org/using-stdvectorreserve-whenever-possible/

  The resize() method (and passing argument to constructor is equivalent to
  that) will insert or delete appropriate number of elements to the vector to
  make it given size (it has optional second argument to specify their value).
  It will affect the size(), iteration will go over all those elements,
  push_back will insert after them and you can directly access them using the
  operator[].

  The reserve() method only allocates memory, but leaves it uninitialized. It
  only affects capacity(), but size() will be unchanged. There is no value for
  the objects, because nothing is added to the vector. If you then insert the
  elements, no reallocation will happen, because it was done in advance, but
  that's the only effect.

  */

  std::vector<human> people;
  //    human youngman(20,std::string("mumbo"));
  //    std::cout<<youngman.height<<std::endl;
  //    people.resize(10,youngman);

  people.resize(10, human(20, std::string("mumbo")));

  for (auto i : people) {
    std::cout << i.name << std::endl;
    std::cout << i.height << std::endl;
  }
}

struct S {
  S(int) { std::cout << "int constructor" << std::endl; }
  S() { std::cout << "constructor" << std::endl; }
  ~S() { std::cout << "deconstructor" << std::endl; }
  S(const S &rhs) { std::cout << "copy constructor" << std::endl; }
  S(S &&rhs) { std::cout << "move constructor" << std::endl; }
};

void emplace_back_VS_Push_back() {
  /*
  When we use push_back(), we create an object and then insert it into the
  vector. With emplace_back(), the object is constructed in-place and saves an
  unnecessary copy.
  */

  std::vector<S> s_vec;

  //    std::cout<<"push back" <<std::endl;
  //    s_vec.push_back(S(10));

  std::cout << "emplace back" << std::endl;
  s_vec.emplace_back(10);
}

void copyVectors() {

  {
    std::cout << "copying vectors, assignment “=” operator" << std::endl;
    std::vector<int> vect1{1, 2, 3, 4};
    std::vector<int> vect2;
    vect2 = vect1;
    vect1.clear();
    printVector(vect2);
  }

  {
    std::cout << "copying vectors, calling copy constructor" << std::endl;
    std::vector<int> vect1{1, 2, 3, 4};
    std::vector<int> vect2(vect1);
    vect1.clear();
    printVector(vect2);
  }

  {
    std::cout << "copying vectors, calling assign" << std::endl;
    std::vector<int> vect1{1, 2, 3, 4};
    std::vector<int> vect2;
    vect2.reserve(vect1.size());
    vect2.assign(vect1.begin(), vect1.end());
    vect1.clear();
    printVector(vect2);
  }

  {
    std::cout << "copying vectors, calling std::copy" << std::endl;
    std::vector<int> vect1{1, 2, 3, 4};
    std::vector<int> vect2;
    std::copy(vect1.begin(), vect1.end(), std::back_inserter(vect2));
    vect1.clear();
    printVector(vect2);
  }
}

/*

std::remove :
It doesn’t actually delete elements from the container but only shunts
non-deleted elements forwards on top of deleted elements.

vector::erase :
Removes from the vector either a single element (position) or a range of
elements ([first, last)).

By using vector::erase all elements in a std::vector will be shifted by
1 causing a large amount of copies; std::remove does just a ‘logical’
delete and leaves the vector unchanged by moving things around.

If you need to remove multiple elements from the vector, the std::remove will
copy each, not removed element only once to its final location, while the
vector::erase approach would move all of the elements from the position to the
end multiple times.

*/
void eraseVSremove() {
  /*
      Using erase, If you went over the vector removing elements one by one,
      you would remove the 1, causing copies of the remainder elements that get
     shifted (4). Then you would remove 2 and shift all remaining elements by
     one (3)… if you see the pattern this is a O(N^2) algorithm.
  */
  std::vector v{1, 2, 3, 4, 5};
  std::vector v_copy(v);

  v.erase(v.begin());
  v.erase(v.begin());
  v.erase(v.begin());
  v.erase(v.begin());

  /*
  std::remove does not actually erase the element from the container, but it
  does return the new end iterator which can be passed to container_type::erase
  to do the REAL removal of the extra elements that are now at the end of the
  container:
  */

  std::vector<int> vec;
  // .. put in some values ..
  int int_to_remove = 5;
  vec.erase(std::remove(vec.begin(), vec.end(), int_to_remove), vec.end());

  /*
  If you want to remove an item, the following will be a bit more efficient.
  */

  auto it = std::find(v_copy.begin(), v_copy.end(), 5);
  if (it != v.end())
    v_copy.erase(it);

  /*
      or you may avoid overhead of moving the items if the order does not matter
     to you:
  */

  it = std::find(v_copy.begin(), v_copy.end(), 4);

  if (it != v_copy.end()) {
    using std::swap;

    // swap the one to be removed with the last element
    // and remove the item at the end of the container
    // to prevent moving all items after '5' by one
    swap(*it, v_copy.back());
    v_copy.pop_back();
  }
}

int main() {
  // cStyleArrayToVecExample();
  // vecTocStyleArrayExample();
  // stringToCharVec();
  vectorExample();

  // resizeVSreserve();

  // emplace_back_VS_Push_back();
  // copyVectors();
}
