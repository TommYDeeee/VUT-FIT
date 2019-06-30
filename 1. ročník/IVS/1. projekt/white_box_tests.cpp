//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     TOMAS DURIS <xduris05@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author TOMAS DURIS
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//


TEST (MatrixTest, Construct)
{
	EXPECT_ANY_THROW(Matrix(0, 0));
	EXPECT_ANY_THROW(Matrix(1, 0));
	EXPECT_ANY_THROW(Matrix(-1, 1));
    
    EXPECT_NO_THROW(Matrix());
	EXPECT_NO_THROW(Matrix(1, 1));
	EXPECT_NO_THROW(Matrix(12, 23));
}

TEST (MatrixTest, Set1)
{
	Matrix test(2, 2);

    EXPECT_TRUE(test.set(0, 0, 100));
    EXPECT_TRUE(test.set(1, 0, 100));
    EXPECT_TRUE(test.set(0, 1, 100));
    EXPECT_TRUE(test.set(1, 1, 100));

    EXPECT_FALSE(test.set(5, 0, 100));
    EXPECT_FALSE(test.set(0, 10, 100));
    EXPECT_FALSE(test.set(-5, 10, 100));
    EXPECT_FALSE(test.set(11.4, 219.8, 118.5));
}

TEST (MatrixTest, Set2x2)
{
    Matrix test(2,2);
    
    EXPECT_TRUE(test.set(0,0,100));
    EXPECT_TRUE(test.set(1,0,-100));

    EXPECT_FALSE(test.set(3,2,100));
    EXPECT_FALSE(test.set(-1,2,100));
}

TEST (MatrixTest, Set5x5)
{
    Matrix test(5,5);

    EXPECT_TRUE(test.set(4,0,100));
    EXPECT_TRUE(test.set(2,1,-100));
   
    EXPECT_FALSE(test.set(5,2,100));
    EXPECT_FALSE(test.set(-5,0,100));
}

TEST (MatrixTest, SetVector)
{
    Matrix test(1,1);

    EXPECT_TRUE(test.set(std::vector<std::vector<double>> {{0}}));
    EXPECT_TRUE(test.set(std::vector<std::vector<double>> {{41281.2}}));

    EXPECT_FALSE(test.set(std::vector<std::vector<double>>(0, std::vector<double>(0, 0))));

}

TEST (MatrixTest, get2x2)
{
    Matrix test(2,2);

    test.set(0,0,100);
    EXPECT_EQ(100,test.get(0,0));

    EXPECT_ANY_THROW(test.get(2,3));
}

TEST (MatrixTest, equal)
{
    Matrix test1(2,2);
    Matrix test2(3,3);
    Matrix test3(2,2);

    test3.set(0,0,1);

    EXPECT_ANY_THROW(test1 == test2);
    EXPECT_TRUE(test1 == test1);
    EXPECT_ANY_THROW(test1 == test2);
    EXPECT_FALSE(test1 == test3);
}

TEST (MatrixTest, add)
{
    Matrix test1(2,2);
    Matrix test2(3,3);
    Matrix expected(2,2);

    test1.set(std::vector<std::vector<double>>{{1,2},{3,4}});
    expected.set(std::vector<std::vector<double>>{{2,4},{6,8}});

    EXPECT_ANY_THROW(test1 + test2);
    Matrix result = test1 + test1;
    EXPECT_TRUE(result == expected);
}

TEST (MatrixTest, times)
{
    Matrix test1(2,3);
    Matrix test2(2,4);
    Matrix test3(3,2);
    Matrix expected(2,2);

    test1.set(std::vector<std::vector<double>>{{1,2,3},{4,5,6}});
    test3.set(std::vector<std::vector<double>>{{1,2},{3,4},{5,6}});
    expected.set(std::vector<std::vector<double>>{{22,28},{49,64}});
     
    EXPECT_ANY_THROW(test1 * test2);
    Matrix result = test1 * test3;
    EXPECT_TRUE(result == expected);
}

TEST (MatrixTest, times2)
{
    Matrix test(2,2);

    Matrix result = test * 5;
    Matrix expected(2,2);
    EXPECT_TRUE(result == expected);

    test.set(std::vector<std::vector<double>>{{1,2},{3,4}});
    Matrix result2 = test * 2;
    Matrix expected2(2,2);
    expected2.set(std::vector<std::vector<double>>{{2,4},{6,8}});
    EXPECT_TRUE(result2 == expected2);

}

TEST (MatrixTest, equation)
{
    Matrix test1(1,1);
    Matrix test2(2,2);
    Matrix test3(4,4);

    EXPECT_ANY_THROW(test2.solveEquation(std::vector<double> {}));

    test2.set(std::vector<std::vector<double>>{{2,-4},{-4,8}});
    EXPECT_ANY_THROW(test2.solveEquation(std::vector<double>{2,10}));
    
    EXPECT_ANY_THROW(test1.solveEquation(std::vector<double>{1}));
    EXPECT_ANY_THROW(test3.solveEquation(std::vector<double>{1,2,3,4}));
}

TEST (MatrixTest, square)
{
    Matrix test(2,4);

    EXPECT_ANY_THROW(test.solveEquation(std::vector<double>{0,0,0,0}));
}

TEST (MatrixTest, 2x2equation)
{
    Matrix test(2,2);

	test.set(std::vector<std::vector<double>> {{1,2},{2,1},});
	
    EXPECT_TRUE(test.solveEquation(std::vector<double> {2,4}) == (std::vector<double> {2,0}));
}

TEST (MatrixTest, 3x3equation)
{
    Matrix test(3,3);

    test.set(std::vector<std::vector<double>> {{1,2,3},{1,1,1},{2,2,3},});

    EXPECT_TRUE(test.solveEquation(std::vector<double> {1,2,2}) == (std::vector<double> {1,3,-2}));
}

TEST (MatrixTest, 1x1det)
{
    Matrix test(4,4);

    EXPECT_ANY_THROW(test.solveEquation(std::vector<double>{1,2,3,4}));
}

TEST (TestingMatrix, 1x1equation)
{
    Matrix test = Matrix();
	test.set(0,0,1);

	EXPECT_TRUE(test.solveEquation(std::vector<double> {3}) == (std::vector<double> {3}));
}


/*** Konec souboru white_box_tests.cpp ***/
