/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::binaryTree

Description

    Data storage of the chemistryOnLineLibrary according to a binary
    tree structure.

            0 (root node)
         /     \
        0       0
      /   \   /   \
     L     R L     0
    / \
   L   R

    L: leafLeft_
    R: leafRight_

\*---------------------------------------------------------------------------*/

#ifndef binaryTree_H
#define binaryTree_H

#include "chemistryModel/TDACChemistryModel/tabulation/ISAT/binaryNode/binaryNode.H"
#include "chemistryModel/TDACChemistryModel/tabulation/ISAT/chemPointISAT/chemPointISAT.H"

namespace Foam
{

template<class CompType, class ThermoType>
class TDACChemistryModel;

template<class CompType, class ThermoType>
class binaryTree
{

public:

    typedef binaryNode<CompType, ThermoType> node;
    typedef chemPointISAT<CompType, ThermoType> chemPoint;


private:

    //- Reference to the chemistryModel
    TDACChemistryModel<CompType, ThermoType>& chemistry_;

    //- Root node of the binary tree
    node *root_;

    //- Maximum number of elements in the binary tree
    label maxNLeafs_;

    //- Size of the BST (= number of chemPoint stored)
    label size_;

    //- Secondary retrieve search variables
    label n2ndSearch_;
    label max2ndSearch_;

    //- Insert new node at the position of phi0
    //  phi0 should be already attached to another node or the pointer to it
    //  will be lost
    void insertNode
    (
        chemPoint*& phi0,
        node*& newNode
    );

    //- Perform a search in the subtree starting from the subtree node y
    //  This search continue to use the hyperplan to walk the tree
    //  If covering EOA is found return true and x points to the chemPoint
    bool inSubTree
    (
        const scalarField& phiq,
        node* y,
        chemPoint* x
    );

    void deleteSubTree(binaryNode<CompType, ThermoType>* subTreeRoot);

    inline void deleteSubTree()
    {
        deleteSubTree(root_);
    }

    //- Replace the binaryNode u with v
    void transplant(node* u, node* v);

    chemPoint* chemPSibling(node* y);
    chemPoint* chemPSibling(chemPoint* x);

    node* nodeSibling(node* y);
    node* nodeSibling(chemPoint* x);

    void deleteAllNode(node* subTreeRoot);

    dictionary coeffsDict_;


public:

    // Constructors

        //- Construct from dictionary and chemistryOnLineLibrary
        binaryTree
        (
            TDACChemistryModel<CompType, ThermoType>& chemistry,
            dictionary coeffsDict
        );


    // Member functions

        inline label size()
        {
            return size_;
        }

        //- Computes iteratively the depth of the subTree
        label depth(node* subTreeRoot);

        inline label depth()
        {
            return depth(root_);
        }

        inline node* root()
        {
            return root_;
        }

        inline label maxNLeafs()
        {
            return maxNLeafs_;
        }

        // Insert a new leaf starting from the parent node of phi0
        // Parameters: phi0 the leaf to replace by a node
        // phiq the new composition to store
        // Rphiq the mapping of the new composition point
        // A the mapping gradient matrix
        // B the matrix used to initialize the EOA
        // nCols the size of the matrix
        // Returns: void
        // Description :
        //1) Create a new leaf with the data to initialize the EOA and to
        // retrieve the mapping by linear interpolation (the EOA is
        // initialize in the chemPoint constructor)
        //2) Get the parent node of phi0 and connect a new node in place of the
        // leaf of phi0. This new node is constructed with phi0 on the left
        // and phiq on the right (the hyperplane is computed inside the
        // binaryNode constructor)
        void insertNewLeaf
        (
            const scalarField& phiq,
            const scalarField& Rphiq,
            const scalarSquareMatrix& A,
            const scalarField& scaleFactor,
            const scalar& epsTol,
            const label nCols,
            chemPoint*& phi0
        );



        // Search the binaryTree until the nearest leaf of a specified
        // leaf is found.
        void binaryTreeSearch
        (
            const scalarField& phiq,
            node* node,
            chemPoint*& nearest
        );

        // Perform a secondary binary tree search starting from a failed
        // chemPoint x, with a depth-first search algorithm
        // If another candidate is found return true and x points to the chemP
        bool secondaryBTSearch(const scalarField& phiq, chemPoint*& x);

        //- Delete a leaf from the binary tree and reshape the binary tree for
        //  the following binary tree search
        //  Return the index in the nodeList of the removed node
        //  (-1 when no node)
        void deleteLeaf(chemPoint*& phi0);

        //- Cheap balance function
        //  This function just roughly separate the space in two parts
        //  with a hyperplane which separate the two extreme chemPoint in the
        //  direction of the maximum the variance
        //  Then, it repopulate the tree with this hyperplane stored at the root
        //  and by inserting the chemPoint in increasing order of value in that
        //  direction
        void balance();

        inline void deleteAllNode()
        {
            deleteAllNode(root_);
        }

        chemPoint* treeMin(node* subTreeRoot);

        inline chemPoint* treeMin()
        {
            return treeMin(root_);
        }

        chemPoint* treeSuccessor(chemPoint* x);

        //- Removes every entries of the tree and delete the associated objects
        void clear();

        //- ListFull
        bool isFull();

        void resetNumRetrieve();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "chemistryModel/TDACChemistryModel/tabulation/ISAT/binaryTree/binaryTree.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
