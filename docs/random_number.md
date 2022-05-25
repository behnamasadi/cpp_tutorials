# Uniform Distribution Ranndom Generator
```cpp

#include <random>
#include <map>


int rangeFrom=0;
int rangeTo=1

std::random_device rand_dev;
std::mt19937 generator(rand_dev());
std::uniform_real_distribution<double> distr(rangeFrom, rangeTo);
double rnd_number= distr(generator);
```


generate random number between 0 and upperbound:
It is important to only invoke the srand call ONCE at the beginning of the program. There is no need for repeat calls to seed the random number generator 
(in fact, it will make your number less evenly distributed). 

```cpp
int upperbound
srand (time(NULL));
//Will return an integer between [0,upperbound)
std::cout<<(rand() % upperbound) <<std::endl;
```

# Uniform Distribution Histogram

```cpp
#include <random>
#include <map>
#include <iomanip>

double rangeFrom = 0;
double rangeTo = 20;
std::random_device rand_dev;
std::mt19937 generator{ rand_dev() };
std::uniform_real_distribution<double> d(rangeFrom, rangeTo);
std::map<int, int> hist{};
int number_of_samples = 1000000;
int histogram_bin_size = 2500;
for (int n = 0; n < number_of_samples; ++n)
{
  ++hist[std::round(d(generator))];
}
for (auto p : hist)
{
  std::cout << std::setw(2) << p.first << ' ' << std::string(p.second / histogram_bin_size, '*') << '\n';
}
```
the output is:

```
 0 *********
 1 *******************
 2 ********************
 3 ********************
 4 ********************
 5 *******************
 6 ********************
 7 *******************
 8 *******************
 9 ********************
10 *******************
11 ********************
12 ********************
13 ********************
14 ********************
15 *******************
16 ********************
17 ********************
18 *******************
19 ********************
20 **********
```
# Normal Distribution Histogram


```cpp
double mean = 5;
double sigma=3;
std::random_device rd{};
std::mt19937 gen{ rd() };
std::normal_distribution<> d{ mean, sigma };
std::map<int, int> hist{};
int number_of_samples = 1000000;
int histogram_bin_size = 2000;


for (int n = 0; n < number_of_samples; ++n)
{
  ++hist[std::round(d(gen))];
}
for (auto p : hist)
{
  std::cout << std::setw(2) << p.first << ' ' << std::string(p.second / histogram_bin_size, '*') << '\n';
}
```
the output is:

```
-9
-8
-7
-6
-5
-4
-3 *
-2 ****
-1 *********
 0 ****************
 1 ***************************
 2 ****************************************
 3 ****************************************************
 4 **************************************************************
 5 ******************************************************************
 6 **************************************************************
 7 ****************************************************
 8 ****************************************
 9 ***************************
10 ****************
11 *********
12 ****
13 *
14
15
16
17
18
20
```
