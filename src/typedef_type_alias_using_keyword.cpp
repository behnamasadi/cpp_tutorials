#include <iostream>
#include <array>
#include <memory>

/*
typedef/using type alias:
Both are doing the exact same thing

using alias = type;

typedef type alias;


*/

template <class T, int SIZE >
struct Image
{
    // Can be used in classes.
    using Ptr = std :: unique_ptr <Image <T, SIZE >>;
    std ::array <T, SIZE > data;
};

template <int SIZE >
using Imagef = Image <float , SIZE >;

int main()
{
    using Image3f = Imagef <3>;
    auto image_ptr = Image3f :: Ptr(new Image3f);

}
