//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     TOMAS DURIS <xduris05@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author TOMAS DURIS
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

namespace BlackBoxTesty 
{
    
    using namespace ::testing;
    
    class EmptyTree : public Test 
	{
        
    protected:
        BinaryTree tree;
        
        
    };
    

    class NonEmptyTree : public Test 
	{
        
        protected:
            BinaryTree tree;
                  
        void SetUp() override 
		{       
                tree.InsertNode(1);
				tree.InsertNode(5);
				tree.InsertNode(15);
				tree.InsertNode(375);
				tree.InsertNode(4);
				tree.InsertNode(22);
				tree.InsertNode(7);
        }    
	};


TEST_F(EmptyTree, Root) 
{    
    Node_t *root = tree.GetRoot();   
    EXPECT_EQ(root, nullptr);
}

TEST_F(EmptyTree, Insert)
{
	std::pair<bool, BinaryTree::Node_t *> firstNode = tree.InsertNode(1);
	Node_t * root = tree.GetRoot();

	EXPECT_TRUE(firstNode.first);
	EXPECT_TRUE(firstNode.second);	
	EXPECT_NE(root, nullptr);

	EXPECT_TRUE(firstNode.second->key == 1);
	EXPECT_TRUE(firstNode.second->pRight != nullptr);
	EXPECT_TRUE(firstNode.second->pLeft != nullptr);
	EXPECT_TRUE(firstNode.second->pParent == nullptr);
	EXPECT_EQ(firstNode.second->color, BinaryTree::BLACK);
}

TEST_F(NonEmptyTree, Insert)
{
	std::pair<bool, BinaryTree::Node_t *> firstNode = tree.InsertNode(375);
	EXPECT_TRUE(firstNode.second);
	EXPECT_FALSE(firstNode.first);

	EXPECT_TRUE(firstNode.second->key == 375);
	EXPECT_EQ(firstNode.second->color, BinaryTree::BLACK);
	EXPECT_TRUE(firstNode.second->pRight != nullptr);
	EXPECT_TRUE(firstNode.second->pLeft != nullptr);
	EXPECT_TRUE(firstNode.second->pParent != nullptr);
	EXPECT_TRUE(firstNode.second->pLeft->pLeft == nullptr);
	EXPECT_TRUE(firstNode.second->pLeft->pRight == nullptr);
	EXPECT_TRUE(firstNode.second->pLeft->color == BinaryTree::BLACK);
	EXPECT_TRUE(firstNode.second->pRight->color == BinaryTree::BLACK);
	EXPECT_TRUE(firstNode.second->pParent->key == 22);
	EXPECT_EQ(firstNode.second->pParent->color, BinaryTree::RED);
	EXPECT_TRUE(firstNode.second->pParent->pParent != nullptr);
	EXPECT_EQ(firstNode.second->pParent->pParent->color, BinaryTree::BLACK);
}

TEST_F(EmptyTree, DeleteNode) 
{        
	EXPECT_FALSE(tree.DeleteNode(5));	
	EXPECT_FALSE(tree.DeleteNode(3548.5));	
	EXPECT_FALSE(tree.DeleteNode(-5));	
}

TEST_F(NonEmptyTree, DeleteNode)
{
	EXPECT_TRUE(tree.DeleteNode(1));
	EXPECT_FALSE(tree.DeleteNode(2));
}


TEST_F(EmptyTree, FindNode)
{
	EXPECT_TRUE(tree.FindNode(2) == nullptr);
	EXPECT_TRUE(tree.FindNode(3155.5) == nullptr);
	EXPECT_TRUE(tree.FindNode(-21) == nullptr);
}

TEST_F(NonEmptyTree, FindNode)
{
	EXPECT_TRUE(tree.FindNode(1));
	EXPECT_FALSE(tree.FindNode(2));

	tree.InsertNode(2);
	EXPECT_TRUE(tree.FindNode(2));
}

TEST_F(EmptyTree, LeafNodes)
{
	std::vector<Node_t *> leafNodes;
	tree.GetLeafNodes(leafNodes);

	EXPECT_TRUE(leafNodes.empty());
}

TEST_F(NonEmptyTree, Axioms)
{	
	std::vector<Node_t *> LeafNodes;
	tree.GetLeafNodes(LeafNodes);
	for (int i = 0; i < LeafNodes.size(); i++) 
	{
            EXPECT_EQ(LeafNodes[i]->color, BinaryTree::BLACK);
			EXPECT_TRUE(LeafNodes[i]->pParent != nullptr);
    }
}

TEST_F(NonEmptyTree, Axioms2)
 {
        
        std::vector<Node_t *> NonLeafNodes;
        tree.GetNonLeafNodes(NonLeafNodes);
        
        for (int i = 0; i < NonLeafNodes.size(); i++) 
		{
            
            if(NonLeafNodes[i]->color == BinaryTree::RED) 
			{
                EXPECT_EQ(NonLeafNodes[i]->pLeft->color, BinaryTree::BLACK);
                EXPECT_EQ(NonLeafNodes[i]->pRight->color, BinaryTree::BLACK);
				EXPECT_EQ(NonLeafNodes[i]->pParent->color, BinaryTree::BLACK);
            }
            
        }
        
    }

TEST_F(NonEmptyTree, Axioms3)
	{
		std::vector<BinaryTree::Node_t *> LeafNodes;
		tree.GetLeafNodes(LeafNodes);

		int NewBlackNodesCount = 0;
		int option = 1;
		BinaryTree::Node_t *CurrentNodes;

		for (int i = 0;i < LeafNodes.size(); i++) 
		{
			CurrentNodes = LeafNodes[i];
			if (option == 1)
			{
				while (CurrentNodes != NULL) 
				{
					if (CurrentNodes->color == BinaryTree::BLACK) 
					{
						NewBlackNodesCount++;
					}
					CurrentNodes = CurrentNodes->pParent;
				}
			}
			else if (option == 0)
			{
				int BlackNodesCount = 0;
				while(CurrentNodes != NULL)
				{
					if (CurrentNodes->color == BinaryTree::BLACK)
					{
						BlackNodesCount++;
					}
					CurrentNodes = CurrentNodes->pParent;
				}

				EXPECT_EQ(BlackNodesCount, NewBlackNodesCount);
			}
			option = 0;
		}
	}

}
/*** Konec souboru black_box_tests.cpp ***/
