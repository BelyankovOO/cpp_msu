#include <iostream>
#include <gtest/gtest.h>
#include "main.h"

TEST(Test, Create){
    TFactory fact;
    ASSERT_NO_THROW(fact.CreateObject("ident"));
    ASSERT_NO_THROW(fact.CreateObject("const", 23));
    ASSERT_NO_THROW(fact.CreateObject("power", 2));
    ASSERT_NO_THROW(fact.CreateObject("polynomial", std::vector<int>{1,2,5}));
    ASSERT_NO_THROW(fact.CreateObject("exp", 3));
    ASSERT_NO_THROW({
        auto a = fact.CreateObject("polynomial", std::vector<int>{1,6,-2});
        auto b = fact.CreateObject("power", 4);
        auto s1 = a + b;
        auto s2 = a - b;
        auto s3 = a * b;
        auto s4 = a / b;
    });
}

TEST(Test, Value){
    TFactory fact;
    ASSERT_EQ(322,(*fact.CreateObject("const", 322))(1));
    ASSERT_EQ(27,(*fact.CreateObject("power", 3))(3));
    ASSERT_EQ(14,(*fact.CreateObject("ident"))(14));
    ASSERT_EQ(11,(*fact.CreateObject("polynomial", std::vector<int>{1, 2}))(5));
    ASSERT_NEAR(54.59, (*fact.CreateObject("exp",2))(2), 0.01);
}

TEST(Test, Deriv){
    TFactory fact;
    auto f1 = fact.CreateObject("const", 14);
    auto f2 = fact.CreateObject("ident");
    auto f3 = fact.CreateObject("power", 3);
    auto f4 = fact.CreateObject("polynomial", std::vector<int>{1, 2, 3});
    auto f5 = fact.CreateObject("exp", 2);


    ASSERT_EQ(0, f1->GetDeriv(5));
    ASSERT_EQ(1, f2->GetDeriv(3));
    ASSERT_EQ(12, f3->GetDeriv(2));
    ASSERT_EQ(8, f4->GetDeriv(1));
    ASSERT_NEAR(109.19, f5->GetDeriv(2),0.01);

} 

TEST(Test, Logic_Error){
    TFactory fact;
    ASSERT_THROW({
        auto f = fact.CreateObject("exp", 4);
        auto s1 = f + "23";
        auto s2 = f + "abc";
        auto s3 = "33" + f;
    }, std::logic_error);
}

TEST(Test, Newton) {
    TFactory fact;
    auto f1 = fact.CreateObject("polynomial", std::vector<int>{-5,2,3});
    ASSERT_NEAR(-1.66, NewtonMethod(f1, -7, 100, 0.0001), 0.01);

    ASSERT_NEAR(1, NewtonMethod(f1, 5, 100, 0.0001), 0.000001);
}

TEST(Test, ComplexDeriv){
    TFactory fact;
    auto a = fact.CreateObject("polynomial", std::vector<int>{1,6});
    auto b = fact.CreateObject("power", 4);
    auto f1 = a * b;
    ASSERT_EQ(34,f1->GetDeriv(1));

    auto f2 = a / b;
    ASSERT_NEAR(93.53,f2->GetDeriv(2),0.01);
}

int main(int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

