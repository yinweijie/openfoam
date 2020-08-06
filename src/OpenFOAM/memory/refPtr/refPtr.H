/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
    Copyright (C) 2018-2020 OpenCFD Ltd.
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
    Foam::refPtr

Description
    A class for managing references or pointers (no reference counting)

SourceFiles
    refPtrI.H

See also
    Foam::autoPtr
    Foam::tmp

\*---------------------------------------------------------------------------*/

#ifndef refPtr_H
#define refPtr_H

#include "tmp.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                             Class refPtr Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class refPtr
{
    // Private Data

        //- Object types
        enum refType
        {
            PTR,    //!< Managing a pointer (not ref-counted)
            CREF,   //!< Using (const) reference to an object
            REF     //!< Using (non-const) reference to an object
        };

        //- The managed pointer or address of the object (reference)
        mutable T* ptr_;

        //- The type (managed pointer | object reference)
        mutable refType type_;


public:

    // STL type definitions

        //- Type of object being managed or referenced
        typedef T element_type;

        //- Pointer to type of object being managed or referenced
        typedef T* pointer;


    //- Null reference counter class
    typedef Foam::refCount::zero refCount;


    // Factory Methods

        //- Construct refPtr of T with forwarding arguments
        //  \param args list of arguments with which an instance of T
        //      will be constructed.
        //
        //  \note Similar to std::make_shared, but the overload for
        //      array types is not disabled.
        template<class... Args>
        inline static refPtr<T> New(Args&&... args);

        //- Construct refPtr from derived type with forwarding arguments
        //  \param args list of arguments with which an instance of U
        //      will be constructed.
        //
        //  \note Similar to New but for derived types
        template<class U, class... Args>
        inline static refPtr<T> NewFrom(Args&&... args);


    // Constructors

        //- Default construct, no managed pointer.
        inline constexpr refPtr() noexcept;

        //- Construct with no managed pointer.
        inline constexpr refPtr(std::nullptr_t) noexcept;

        //- Construct, taking ownership of the pointer.
        inline explicit refPtr(T* p) noexcept;

        //- Construct for a const reference to an object.
        inline refPtr(const T& obj) noexcept;

        //- Move construct, transferring ownership.
        inline refPtr(refPtr<T>&& rhs) noexcept;

        //- Copy construct
        inline refPtr(const refPtr<T>& rhs);

        //- Copy/move construct. Optionally reusing pointer.
        inline refPtr(const refPtr<T>& rhs, bool reuse);


    //- Destructor: deletes managed pointer
    inline ~refPtr();


    // Member Functions

        //- The type-name, constructed from type-name of T
        inline static word typeName();


    // Query

        //- Deprecated(2020-07) True if a null managed pointer
        //
        //  \deprecated(2020-07) - use bool operator
        FOAM_DEPRECATED_FOR(2020-07, "bool operator")
        bool empty() const noexcept { return !ptr_; }

        //- True for non-null pointer/reference
        bool valid() const noexcept { return ptr_; }

        //- True if this is a managed pointer (not a reference)
        bool is_pointer() const noexcept { return type_ == PTR; }

        //- Identical to is_pointer()
        bool isTmp() const noexcept { return type_ == PTR; }

        //- True if this is a non-null managed pointer
        inline bool movable() const noexcept;


    // Access

        //- Return pointer without nullptr checking.
        T* get() noexcept { return ptr_; }

        //- Return const pointer without nullptr checking.
        const T* get() const noexcept { return ptr_; }

        //- Return const reference to the object or to the contents
        //- of a (non-null) managed pointer.
        //  Fatal for a null managed pointer
        inline const T& cref() const;

        //- Return non-const reference to the contents of a non-null
        //- managed pointer.
        //  Fatal for a null managed pointer or if the object is const.
        inline T& ref() const;

        //- Return non-const reference to the object or to the contents
        //- of a (non-null) managed pointer, with an additional const_cast.
        //  Fatal for a null managed pointer.
        inline T& constCast() const;


    // Edit

        //- Return managed pointer for reuse, or clone() the object reference.
        inline T* ptr() const;

        //- If object pointer points to valid object:
        //- delete object and set pointer to nullptr
        inline void clear() const noexcept;

        //- Delete managed temporary object and set to new given pointer
        inline void reset(T* p = nullptr) noexcept;

        //- Clear existing and transfer ownership.
        inline void reset(refPtr<T>&& other) noexcept;

        //- Delete managed temporary object and set to (const) reference
        inline void cref(const T& obj) noexcept;

        //- Delete managed temporary object and set to (non-const) reference
        inline void ref(T& obj) noexcept;

        //- Swaps the managed object with other.
        inline void swap(refPtr<T>& other) noexcept;


    // Member Operators

        //- Identical to cref() method.
        inline const T& operator()() const;

        //- Cast to underlying data type, using the cref() method.
        inline operator const T&() const;

        //- Dereferences (const) pointer to the managed object.
        //  Fatal for a null managed pointer.
        inline const T* operator->() const;

        //- Dereferences (non-const) pointer to the managed object.
        //  Fatal for a null managed pointer or if the object is const.
        inline T* operator->();

        //- Non-null pointer/reference : valid()
        explicit operator bool() const noexcept { return ptr_; }

        //- Transfer ownership of the managed pointer.
        //  Fatal for a null managed pointer or if the object is const.
        inline void operator=(const refPtr<T>& t);

        //- Clear existing and transfer ownership.
        inline void operator=(refPtr<T>&& other) noexcept;

        //- Take ownership of the pointer.
        //  Fatal for a null pointer
        inline void operator=(T* p);

        //- Reset via assignment from literal nullptr
        inline void operator=(std::nullptr_t) noexcept;

        //- Conversion to tmp, releases pointer or shallow-copies reference
        inline operator tmp<T>();
};


// Global Functions

//- Specialized Swap algorithm for refPtr.
//  Swaps the pointers and types of lhs and rhs. Calls \c lhs.swap(rhs)
template<class T>
void Swap(refPtr<T>& lhs, refPtr<T>& rhs)
{
    lhs.swap(rhs);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "refPtrI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //