#include <random>
#include <iostream>
#include <ctime>
#include <map>
#include <iomanip>


void randomNumberWithoutSeeding()
{
    //std::rand() Returns a pseudo-random integral value between 0 and RAND_MAX
    std::cout<<"RAND_MAX is: "  << RAND_MAX <<std::endl;

    /*
    If rand() is used before any calls to std::srand( unsigned seed), rand() behaves as if it was seeded with std::srand(1)
    it produces the same sequence of values on successive calls, so you call this function multiple time you shoudl get the same random number
    */
    int upperbound=100;

    std::cout<<"random number without seeding:"   <<(rand() % upperbound) <<std::endl;
}

void randomNumberWithSeeding()
{
    //It is important to only invoke the srand call ONCE at the beginning of the program ( before any calls to rand()).

    int upperbound=100;

    //use current time as seed for random generator
    std::srand(std::time(0));

    //return a value of integral type holding 86400 times (number of seconds in day) the number of calendar days since the Epoch  1970
    //std::cout<<((std::time(nullptr))/86400 )/365 <<std::endl;



    //Will return an integer between [0,upperbound)
    std::cout<< "random number with seeding:"   <<  (rand() % upperbound) <<std::endl;
}

void ranndomGeneratorFromSpecificDistribution()
{
    int rangeFrom=0;
    int rangeTo=1;

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());

/*
    uniform_real_distribution in the above can be replaced with:

    bernoulli_distribution;
    binomial_distribution;
    exponential_distribution;
    gamma_distribution;
    geometric_distribution;
    normal_distribution;
    poisson_distribution;
    cauchy_distribution;
    chi_squared_distribution;
    discrete_distribution;
    extreme_value_distribution;
    fisher_f_distribution;
    lognormal_distribution;
    negative_binomial_distribution;
    piecewise_constant_distribution;
    piecewise_linear_distribution;
    student_t_distribution;
    uniform_int_distribution;
    uniform_real_distribution;
    weibull_distribution;
*/
    std::uniform_real_distribution<double> distr(rangeFrom, rangeTo);
    double rnd_number= distr(generator);
}


template < typename T>
void calculatePrintHistogram(T d, std::mt19937 gen, int number_of_samples = 1000000, int histogram_bin_size = 4000)
{
    std::map<int, int> hist{};


    for (int n = 0; n < number_of_samples; ++n)
    {
        ++hist[std::round(d(gen))];
    }
    for (auto p : hist)
    {
        std::cout << std::setw(2) << p.first << ' ' << std::string(p.second / histogram_bin_size, '*') << '\n';
    }

}

void histogramOfDistribution()
{

    enum Distributions
    {
        bernoulli_distribution,
        exponential_distribution,
        gamma_distribution,
        normal_distribution,
        poisson_distribution,
        uniform_real_distribution
    };


    int number_of_samples = 1000000;
    int histogram_bin_size = 2500;


    std::random_device rd{};
    std::mt19937 gen{ rd() };

    /*parameters for normal_distribution*/
    double mean = 5;
    double sigma = 3;


    /*parameters for uniform_real_distribution*/
    double rangeFrom = 0;
    double rangeTo = 20;

    /*parameters for bernoulli_distribution
    give "true" 1/4 of the time, give "false" 3/4 of the time*/
    double chance_of_wining = 0.25;


    /*parameters for poisson_distribution
    if an event occurs 4 times a minute on average
    how often is it that it occurs n times in one minute?*/
    int frequency_of_occurance_in_a_minute_on_average = 4;


    /* parameters for exponential_distribution
    if particles decay once per second on average,
    how much time, in seconds, until the next one?*/
    int decay_once_per_second_on_average = 1;


    /*
    A gamma distribution with alpha = 1, and beta = 2
    approximates an exponential distribution.
    */
    int alpha = 1;
    int  beta = 2;



    for (int it = bernoulli_distribution; it != uniform_real_distribution; it++)
    {
        Distributions distribution = static_cast<Distributions>(it);

        switch (distribution)
        {
        case bernoulli_distribution:
        {
            std::bernoulli_distribution d(chance_of_wining);
            std::cout<<"------------------------bernoulli distribution------------------------" <<std::endl;
            calculatePrintHistogram(d, gen);
        }
            break;
        case exponential_distribution:
        {
            std::exponential_distribution<> d(decay_once_per_second_on_average);
            std::cout << "------------------------exponential distribution------------------------" << std::endl;
            calculatePrintHistogram(d, gen);
        }
            break;
        case gamma_distribution:
        {
            std::gamma_distribution<> d(alpha, beta);
            std::cout << "------------------------gamma distribution------------------------" << std::endl;
            calculatePrintHistogram(d, gen);
        }
            break;
        case normal_distribution:
        {
            std::normal_distribution<> d{ mean, sigma };
            std::cout << "------------------------normal distribution------------------------" << std::endl;
            calculatePrintHistogram(d, gen);
        }
            break;

        case poisson_distribution:
        {
            std::poisson_distribution<> d(frequency_of_occurance_in_a_minute_on_average);
            std::cout << "------------------------poisson distribution------------------------" << std::endl;
            calculatePrintHistogram(d, gen);
        }
            break;

        case uniform_real_distribution:
        {
            std::uniform_real_distribution<double> d(rangeFrom, rangeTo);
            std::cout << "------------------------uniform real distribution------------------------" << std::endl;
            calculatePrintHistogram(d, gen);
        }
            break;

        default:
            break;
        }
    }
}


int main()
{
    randomNumberWithoutSeeding();
    randomNumberWithSeeding();
    histogramOfDistribution();
}
