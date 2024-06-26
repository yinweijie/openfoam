/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
    Copyright (C) 2022 OpenCFD Ltd.
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

#include "containers/LinkedLists/accessTypes/LPtrList/LPtrList.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class LListBase, class T>
Foam::LPtrList<LListBase, T>::LPtrList(const LPtrList<LListBase, T>& lst)
:
    LList<LListBase, T*>()
{
    for (auto iter = lst.cbegin(); iter != lst.cend(); ++iter)
    {
        this->push_back((*iter).clone().ptr());
    }
}


template<class LListBase, class T>
Foam::LPtrList<LListBase, T>::LPtrList(LPtrList<LListBase, T>&& lst)
:
    LList<LListBase, T*>()
{
    LList<LListBase, T*>::transfer(lst);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class LListBase, class T>
Foam::LPtrList<LListBase, T>::~LPtrList()
{
    this->clear();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class LListBase, class T>
void Foam::LPtrList<LListBase, T>::pop_front(label n)
{
    if (n > this->size())
    {
        n = this->size();
    }

    while (n > 0)
    {
        T* p = this->removeHead();
        delete p;
        --n;
    }
}


template<class LListBase, class T>
void Foam::LPtrList<LListBase, T>::clear()
{
    this->pop_front(this->size());
    LList<LListBase, T*>::clear();
}


template<class LListBase, class T>
void Foam::LPtrList<LListBase, T>::transfer(LPtrList<LListBase, T>& lst)
{
    clear();
    LList<LListBase, T*>::transfer(lst);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class LListBase, class T>
void Foam::LPtrList<LListBase, T>::operator=(const LPtrList<LListBase, T>& lst)
{
    clear();

    for (auto iter = lst.cbegin(); iter != lst.cend(); ++iter)
    {
        this->push_back((*iter).clone().ptr());
    }
}


template<class LListBase, class T>
void Foam::LPtrList<LListBase, T>::operator=(LPtrList<LListBase, T>&& lst)
{
    transfer(lst);
}


// ************************************************************************* //
