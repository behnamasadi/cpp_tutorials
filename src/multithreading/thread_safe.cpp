#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

namespace notThreadSafe
{
template<typename T>
class stack
{
private:
    std::mutex mx;
    std::vector<T> data;
public:
    T top()
    {
        return data.back();
    }

    void pop()
    {
        data.pop_back();
    }

    void push(T element)
    {
        data.push_back(element);
    }

};

}



namespace threadSafe
{
template<typename T>
class stack
{
private:
    std::mutex mx;
    std::vector<T> data;
public:
    T pop()
    {
        T tmp=data.back();
        data.pop_back();
        return tmp;
    }

    void push(T element)
    {
        data.push_back(element);
    }

};

}

int main()
{

}
