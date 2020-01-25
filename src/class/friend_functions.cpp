/*
 * FriendFunctions.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: behnam
 */
//http://www.learncpp.com/cpp-tutorial/813-friend-functions-and-classes/
class Accumulator
{
private:
    int m_nValue;
public:
    Accumulator() { m_nValue = 0; }
    void Add(int nValue) { m_nValue += nValue; }

    // Make the Reset() function a friend of this class
    friend void Reset(Accumulator &cAccumulator);
};

// Reset() is now a friend of the Accumulator class
void Reset(Accumulator &cAccumulator)
{
    // And can access the private data of Accumulator objects
    cAccumulator.m_nValue = 0;
}



