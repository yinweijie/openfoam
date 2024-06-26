/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 OpenFOAM Foundation
    Copyright (C) 2019-2020 OpenCFD Ltd.
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

\*---------------------------------------------------------------------------*/

#include "chemistryModel/TDACChemistryModel/tabulation/ISAT/binaryTree/binaryTree.H"
#include "containers/Lists/SortableList/SortableList.H"
#include "include/demandDrivenData.H"

// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::insertNode
(
    chemPoint*& phi0,
    node*& newNode
)
{
    if (phi0 == phi0->node()->leafRight())
    {
        // phi0 is on the right
        phi0->node()->leafRight() = nullptr;
        phi0->node()->nodeRight() = newNode;
        return;
    }
    else if (phi0 == phi0->node()->leafLeft())
    {
        // phi0 is on the left
        phi0->node()->leafLeft() = nullptr;
        phi0->node()->nodeLeft() = newNode;
        return;

    }

    // if we reach this point, there is an issue with the addressing
    FatalErrorInFunction
        << "trying to insert a node with a wrong pointer to a chemPoint"
        << exit(FatalError);
}


template<class CompType, class ThermoType>
bool Foam::binaryTree<CompType, ThermoType>::inSubTree
(
    const scalarField& phiq,
    node* y,
    chemPoint* x
)
{
    if ((n2ndSearch_ < max2ndSearch_) && (y != nullptr))
    {
        scalar vPhi = 0;
        const scalarField& v = y->v();
        const scalar a = y->a();

        // compute v*phi
        for (label i=0; i<phiq.size(); ++i)
        {
            vPhi += phiq[i]*v[i];
        }
        if (vPhi <= a)
        {
            // on the left side of the node
            if (y->nodeLeft() == nullptr)
            {
                // left is a chemPoint
                ++n2ndSearch_;
                if (y->leafLeft()->inEOA(phiq))
                {
                    x = y->leafLeft();
                    return true;
                }
            }
            else
            {
                // the left side is a node
                if (inSubTree(phiq, y->nodeLeft(), x))
                {
                    return true;
                }
            }

            // not on the left side, try the right side
            if ((n2ndSearch_ < max2ndSearch_) && y->nodeRight() == nullptr)
            {
                ++n2ndSearch_;
                // we reach the end of the subTree we can return the result
                if (y->leafRight()->inEOA(phiq))
                {
                    x = y->leafRight();
                    return true;
                }
                else
                {
                    x = nullptr;
                    return false;
                }
            }

            // Test for n2ndSearch is done in the call of inSubTree
            return inSubTree(phiq, y->nodeRight(), x);
        }
        else
        {
            // on right side (symmetric of above)

            if (y->nodeRight() == nullptr)
            {
                ++n2ndSearch_;
                if (y->leafRight()->inEOA(phiq))
                {
                    return true;
                }
            }
            else // the right side is a node
            {
                if (inSubTree(phiq, y->nodeRight(), x))
                {
                    x = y->leafRight();
                    return true;
                }
            }

            // if we reach this point, the retrieve has
            // failed on the right side, explore the left side
            if ((n2ndSearch_ < max2ndSearch_) && y->nodeLeft() == nullptr)
            {
                ++n2ndSearch_;
                if (y->leafLeft()->inEOA(phiq))
                {
                    x = y->leafLeft();
                    return true;
                }
                else
                {
                    x = nullptr;
                    return false;
                }
            }

            return inSubTree(phiq, y->nodeLeft(), x);
        }
    }

    return false;
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::deleteSubTree(node* subTreeRoot)
{
    if (subTreeRoot != nullptr)
    {
        deleteDemandDrivenData(subTreeRoot->leafLeft());
        deleteDemandDrivenData(subTreeRoot->leafRight());
        deleteSubTree(subTreeRoot->nodeLeft());
        deleteSubTree(subTreeRoot->nodeRight());
        deleteDemandDrivenData(subTreeRoot);
    }
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::transplant(node* u, node* v)
{
    if (v != nullptr)
    {
        // u is root_
        if (u->parent() == nullptr)
        {
            root_ = v;
        }
        // u is on the left of its parent
        else if (u == u->parent()->nodeLeft())
        {
            u->parent()->nodeLeft() = v;
        }
        // u is ont the right of its parent
        else if (u == u->parent()->nodeRight())
        {
            u->parent()->nodeRight() = v;
        }
        else
        {
            FatalErrorInFunction
                << "wrong addressing of the initial node"
                << exit(FatalError);
        }
        v->parent() = u->parent();
    }
    else
    {
        FatalErrorInFunction
            << "trying to transplant a nullptr node"
            << exit(FatalError);
    }
}


template<class CompType, class ThermoType>
Foam::chemPointISAT<CompType, ThermoType>*
Foam::binaryTree<CompType, ThermoType>::chemPSibling(node* y)
{
    if (y->parent() != nullptr)
    {
        if (y == y->parent()->nodeLeft())// y is on the left, return right side
        {
            // might return nullptr if the right side is a node
            return y->parent()->leafRight();
        }
        else if (y == y->parent()->nodeRight())
        {
            return y->parent()->leafLeft();
        }

        FatalErrorInFunction
            << "wrong addressing of the initial node"
            << exit(FatalError);
    }

    // the binaryNode is root_ and has no sibling
    return nullptr;
}


template<class CompType, class ThermoType>
Foam::chemPointISAT<CompType, ThermoType>*
Foam::binaryTree<CompType, ThermoType>::chemPSibling(chemPoint* x)
{
    if (size_ > 1)
    {
        if (x == x->node()->leafLeft())
        {
            // x is on the left, return right side
            // might return nullptr if the right side is a node
            return x->node()->leafRight();
        }
        else if (x == x->node()->leafRight())
        {
            // x is on the right, return left side
            return x->node()->leafLeft();
        }

        FatalErrorInFunction
            << "wrong addressing of the initial leaf"
            << exit(FatalError);
    }

    // there is only one leaf attached to the root_, no sibling
    return nullptr;
}


template<class CompType, class ThermoType>
Foam::binaryNode<CompType, ThermoType>*
Foam::binaryTree<CompType, ThermoType>::nodeSibling(node* y)
{
    if (y->parent() != nullptr)
    {
        if (y == y->parent()->nodeLeft())
        {
            // y is on the left, return right side
            return y->parent()->nodeRight();
        }
        else if (y == y->parent()->nodeRight())
        {
            return y->parent()->nodeLeft();
        }

        FatalErrorInFunction
            << "wrong addressing of the initial node"
            << exit(FatalError);
    }

    return nullptr;
}


template<class CompType, class ThermoType>
Foam::binaryNode<CompType, ThermoType>*
Foam::binaryTree<CompType, ThermoType>::nodeSibling(chemPoint* x)
{
    if (size_ > 1)
    {
        if (x == x->node()->leafLeft())
        {
            // x is on the left, return right side
            return x->node()->nodeRight();
        }
        else if (x == x->node()->leafRight())
        {
            // x is on the right, return left side
            return x->node()->nodeLeft();
        }

        FatalErrorInFunction
            << "wrong addressing of the initial leaf"
            << exit(FatalError);
    }

    return nullptr;
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::deleteAllNode(node* subTreeRoot)
{
    if (subTreeRoot != nullptr)
    {
        deleteAllNode(subTreeRoot->nodeLeft());
        deleteAllNode(subTreeRoot->nodeRight());
        deleteDemandDrivenData(subTreeRoot);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CompType, class ThermoType>
Foam::binaryTree<CompType, ThermoType>::binaryTree
(
    TDACChemistryModel<CompType, ThermoType>& chemistry,
    dictionary coeffsDict
)
:
    chemistry_(chemistry),
    root_(nullptr),
    maxNLeafs_(coeffsDict.get<label>("maxNLeafs")),
    size_(0),
    n2ndSearch_(0),
    max2ndSearch_(coeffsDict.getOrDefault("max2ndSearch", 0)),
    coeffsDict_(coeffsDict)
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CompType, class ThermoType>
Foam::label Foam::binaryTree<CompType, ThermoType>::depth(node* subTreeRoot)
{
    // when we reach the leaf, we return 0
    if (subTreeRoot == nullptr)
    {
        return 0;
    }
    else
    {
        return
            1
          + max
            (
                depth(subTreeRoot->nodeLeft()),
                depth(subTreeRoot->nodeRight())
            );
    }
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::insertNewLeaf
(
    const scalarField& phiq,
    const scalarField& Rphiq,
    const scalarSquareMatrix& A,
    const scalarField& scaleFactor,
    const scalar& epsTol,
    const label nCols,
    chemPoint*& phi0
)
{
    if (size_ == 0) // no points are stored
    {
        // create an empty binary node and point root_ to it
        root_ = new node();
        // create the new chemPoint which holds the composition point
        // phiq and the data to initialize the EOA
        chemPoint* newChemPoint =
            new chemPoint
            (
                chemistry_,
                phiq,
                Rphiq,
                A,
                scaleFactor,
                epsTol,
                nCols,
                coeffsDict_,
                root_
            );
        root_->leafLeft() = newChemPoint;
    }
    else // at least one point stored
    {
        // no reference chemPoint, a BT search is required
        if (phi0 == nullptr)
        {
            binaryTreeSearch(phiq, root_, phi0);
        }
        // access to the parent node of the chemPoint
        node* parentNode = phi0->node();

        // create the new chemPoint which holds the composition point
        // phiq and the data to initialize the EOA
        chemPoint* newChemPoint =
            new chemPoint
            (
                chemistry_,
                phiq,
                Rphiq,
                A,
                scaleFactor,
                epsTol,
                nCols,
                coeffsDict_
            );
        // insert new node on the parent node in the position of the
        // previously stored leaf (phi0)
        // the new node contains phi0 on the left and phiq on the right
        // the hyper plane is computed in the binaryNode constructor
        node* newNode;
        if (size_ > 1)
        {
            newNode = new node(phi0, newChemPoint, parentNode);
            // make the parent of phi0 point to the newly created node
            insertNode(phi0, newNode);
        }
        else // size_ == 1 (because not equal to 0)
        {
            // when size is 1, the binaryNode is without hyperplane
            deleteDemandDrivenData(root_);
            newNode = new node(phi0, newChemPoint, nullptr);
            root_ = newNode;
        }

        phi0->node() = newNode;
        newChemPoint->node()=newNode;
    }

    ++size_;
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::binaryTreeSearch
(
    const scalarField& phiq,
    node* node,
    chemPoint*& nearest
)
{
    if (size_ > 1)
    {
        scalar vPhi=0.0;
        const scalarField& v = node->v();
        const scalar& a = node->a();
        // compute v*phi
        for (label i=0; i<phiq.size(); ++i)
        {
            vPhi += phiq[i]*v[i];
        }


        if (vPhi > a) // on right side (side of the newly added point)
        {
            if (node->nodeRight() != nullptr)
            {
                binaryTreeSearch(phiq, node->nodeRight(), nearest);
            }
            else // the terminal node is reached, store leaf on the right
            {
                nearest = node->leafRight();
                return;
            }
        }
        else // on left side (side of the previously stored point)
        {
            if (node->nodeLeft() != nullptr)
            {
                binaryTreeSearch(phiq, node->nodeLeft(), nearest);
            }
            else // the terminal node is reached, return element on right
            {
                nearest = node->leafLeft();
                return;
            }
        }
    }
    // only one point stored (left element of the root)
    else if (size_ == 1)
    {
        nearest = root_->leafLeft();
    }
    else // no point stored
    {
        nearest = nullptr;
    }
}


template<class CompType, class ThermoType>
bool Foam::binaryTree<CompType, ThermoType>::secondaryBTSearch
(
    const scalarField& phiq,
    chemPoint*& x
)
{
    // initialize n2ndSearch_
    n2ndSearch_ = 0;
    if ((n2ndSearch_ < max2ndSearch_) && (size_ > 1))
    {
        chemPoint* xS = chemPSibling(x);
        if (xS != nullptr)
        {
            n2ndSearch_++;
            if (xS->inEOA(phiq))
            {
                x = xS;
                return true;
            }
        }
        else if (inSubTree(phiq, nodeSibling(x), x))
        {
            return true;
        }

        // if we reach this point, no leafs were found at this depth or lower
        // we move upward in the tree
        node* y = x->node();
        while ((y->parent()!= nullptr) && (n2ndSearch_ < max2ndSearch_))
        {
            xS = chemPSibling(y);
            if (xS != nullptr)
            {
                n2ndSearch_++;
                if (xS->inEOA(phiq))
                {
                    x=xS;
                    return true;
                }
            }
            else if (inSubTree(phiq, nodeSibling(y),x))
            {
                return true;
            }
            y = y->parent();
        }

        // if we reach this point it is either because
        // we did not find another covering EOA in the entire tree or
        // we reach the maximum number of secondary search
        return false;
    }

    return false;
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::deleteLeaf(chemPoint*& phi0)
{
    if (size_ == 1) // only one point is stored
    {
        deleteDemandDrivenData(phi0);
        deleteDemandDrivenData(root_);
    }
    else if (size_ > 1)
    {
        node* z = phi0->node();
        node* x;
        chemPoint* siblingPhi0 = chemPSibling(phi0);

        if (siblingPhi0 != nullptr)// the sibling of phi0 is a chemPoint
        {
            // z was root (only two chemPoints in the tree)
            if (z->parent() == nullptr)
            {
                root_ = new node();
                root_->leafLeft()=siblingPhi0;
                siblingPhi0->node()=root_;
            }
            else if (z == z->parent()->nodeLeft())
            {
                z->parent()->leafLeft() = siblingPhi0;
                z->parent()->nodeLeft() = nullptr;
                siblingPhi0->node() = z->parent();
            }
            else if (z == z->parent()->nodeRight())
            {
                z->parent()->leafRight() = siblingPhi0;
                z->parent()->nodeRight() = nullptr;
                siblingPhi0->node() = z->parent();
            }
            else
            {
                FatalErrorInFunction
                    << "wrong addressing of the initial leaf"
                    << exit(FatalError);
            }
        }
        else
        {
            x = nodeSibling(phi0);
            if (x !=nullptr)
            {
                transplant(z, x);
            }
            else
            {
                FatalErrorInFunction
                    << "inconsistent structure of the tree, no leaf and no node"
                    << exit(FatalError);
            }
        }
        deleteDemandDrivenData(phi0);
        deleteDemandDrivenData(z);
    }

    --size_;
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::balance()
{
    //1) walk through the entire tree by starting with the tree's most left
    // chemPoint
    chemPoint* x = treeMin();
    List<chemPoint*> chemPoints(size_);
    label chemPointi = 0;

    //2) compute the mean composition
    label n = x->phi().size();
    scalarField mean(n, Zero);
    while (x != nullptr)
    {
        const scalarField& phij = x->phi();
        mean += phij;
        chemPoints[chemPointi++] = x;
        x = treeSuccessor(x);
    }
    mean /= scalar(size_);

    //3) compute the variance for each space direction
    List<scalar> variance(n, Zero);
    forAll(chemPoints, j)
    {
        const scalarField& phij = chemPoints[j]->phi();
        forAll(variance, vi)
        {
            variance[vi] += sqr(phij[vi] - mean[vi]);
        }
    }

    //4) analyze what is the direction of the maximal variance
    scalar maxVariance(-1.0);
    label maxDir(-1);
    forAll(variance, vi)
    {
        if (maxVariance < variance[vi])
        {
            maxVariance = variance[vi];
            maxDir = vi;
        }
    }
    // maxDir indicates the direction of maximum variance
    // we create the new root node by taking the two extreme points
    // in this direction if these extreme points were not deleted in the
    // cleaning that come before the balance function they are still important
    // and the tree should therefore take them into account
    SortableList<scalar> phiMaxDir(chemPoints.size(), Zero);
    forAll(chemPoints, j)
    {
        phiMaxDir[j] = chemPoints[j]->phi()[maxDir];
    }

    phiMaxDir.sort();
    // delete reference to all node since the tree is reshaped
    deleteAllNode();
    root_ = nullptr;

    // add the node for the two extremum
    node* newNode = new node
    (
        chemPoints[phiMaxDir.indices()[0]],
        chemPoints[phiMaxDir.indices()[phiMaxDir.size()-1]],
        nullptr
    );
    root_ = newNode;

    chemPoints[phiMaxDir.indices()[0]]->node() = newNode;
    chemPoints[phiMaxDir.indices()[phiMaxDir.size()-1]]->node() = newNode;

    for (label cpi=1; cpi<chemPoints.size()-1; ++cpi)
    {
        chemPoint* phi0;
        binaryTreeSearch
        (
            chemPoints[phiMaxDir.indices()[cpi]]->phi(),
            root_,
            phi0
        );
        // add the chemPoint
        node* nodeToAdd =
            new node(phi0, chemPoints[phiMaxDir.indices()[cpi]], phi0->node());
        // make the parent of phi0 point to the newly created node
        insertNode(phi0, nodeToAdd);
        phi0->node() = nodeToAdd;
        chemPoints[phiMaxDir.indices()[cpi]]->node() = nodeToAdd;
    }
}


template<class CompType, class ThermoType>
Foam::chemPointISAT<CompType, ThermoType>*
Foam::binaryTree<CompType, ThermoType>::treeMin(node* subTreeRoot)
{
    if (subTreeRoot != nullptr)
    {
        while (subTreeRoot->nodeLeft() != nullptr)
        {
            subTreeRoot = subTreeRoot->nodeLeft();
        }
        return subTreeRoot->leafLeft();
    }

    return nullptr;
}


template<class CompType, class ThermoType>
Foam::chemPointISAT<CompType, ThermoType>*
Foam::binaryTree<CompType, ThermoType>::treeSuccessor(chemPoint* x)
{
    if (size_ > 1)
    {
        if (x == x->node()->leafLeft())
        {
            if (x->node()->nodeRight() == nullptr)
            {
                return x->node()->leafRight();
            }
            else
            {
                return treeMin(x->node()->nodeRight());
            }
        }
        else if (x == x->node()->leafRight())
        {
            node* y = x->node();
            while ((y->parent() != nullptr))
            {
                if (y == y->parent()->nodeLeft())
                {
                    if (y->parent()->nodeRight() == nullptr)
                    {
                        return y->parent()->leafRight();
                    }
                    else
                    {
                        return treeMin(y->parent()->nodeRight());
                    }
                }
                y = y->parent();
            }

            // when we reach this point, y points to the root and
            // never entered in the if loop (coming from the right)
            // so we are at the tree maximum and there is no successor
            return nullptr;
        }

        FatalErrorInFunction
            << "inconsistent structure of the tree, no leaf and no node"
            << exit(FatalError);
    }

    return nullptr;
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::clear()
{
    // Recursively delete the element in the subTree
    deleteSubTree();

    // Reset root node (should already be nullptr)
    root_ = nullptr;

    // Reset size_
    size_ = 0;
}


template<class CompType, class ThermoType>
bool Foam::binaryTree<CompType, ThermoType>::isFull()
{
    return size_ >= maxNLeafs_;
}


template<class CompType, class ThermoType>
void Foam::binaryTree<CompType, ThermoType>::resetNumRetrieve()
{
    // Has to go along each chemPoint of the tree
    if (size_ > 0)
    {
        // First finds the first leaf
        chemPoint* chemPoint0 = treeMin();
        chemPoint0->resetNumRetrieve();

        // Then go to each successor
        chemPoint* nextchemPoint = treeSuccessor(chemPoint0);
        while (nextchemPoint != nullptr)
        {
            nextchemPoint->resetNumRetrieve();
            nextchemPoint = treeSuccessor(nextchemPoint);
        }
    }
}


// ************************************************************************* //
