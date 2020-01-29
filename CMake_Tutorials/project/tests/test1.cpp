#include <gtest/gtest.h>
#include <gmock/gmock.h>


TEST(testCaseName1, teseName1)
{
    ASSERT_TRUE(1==1);
}


int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
