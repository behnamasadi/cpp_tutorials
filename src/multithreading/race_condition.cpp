#include <iostream>
#include <thread>
#include <vector>
#include<mutex>




class wallet 
{
public:
    int money;
    wallet():money(0){}
    void increaseMoney(int amount)
    {
       money=money+amount; 
    }
};


int racingProblem()
{
    wallet walletObject;
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread(&wallet::increaseMoney, &walletObject, 10));
    }
    
    for(int i = 0; i < threads.size() ; i++)
    {
        threads.at(i).join();
    }
    return walletObject.money;
 
}


void racingProblemExample()
{
    int val = 0;
    for(int k = 0; k < 10000; k++)
    {
        if((val = racingProblem()) != 50)
        {
            std::cout << "Error at count = "<<k<<" Money in Wallet = "<<val << std::endl;
        }
    }
}


int main()
{
}
