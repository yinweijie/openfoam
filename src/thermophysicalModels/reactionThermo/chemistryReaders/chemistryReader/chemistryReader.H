/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2021 OpenCFD Ltd.
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
    Foam::chemistryReader

Group
    grpReactionThermophysicalChemistryReaders

Description
    Abstract class for reading chemistry

SourceFiles
    chemistryReader.C

\*---------------------------------------------------------------------------*/

#ifndef chemistryReader_H
#define chemistryReader_H

#include "db/typeInfo/typeInfo.H"
#include "specieElement/specieElement.H"
#include "reaction/ReactionsCaseDir/Reaction/Reaction.H"
#include "reaction/ReactionsCaseDir/ReactionList/ReactionList.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

typedef HashTable<List<specieElement>> speciesCompositionTable;

/*---------------------------------------------------------------------------*\
                      Class chemistryReader Declaration
\*---------------------------------------------------------------------------*/

template<class ThermoType>
class chemistryReader
{
public:

    //- Runtime type information
    TypeName("chemistryReader");

    //- The type of thermo package the reader was instantiated for
    typedef ThermoType thermoType;


    // Constructors

        //- Default construct
        chemistryReader()
        {}


    // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            chemistryReader,
            dictionary,
            (
                const dictionary& thermoDict,
                speciesTable& species
            ),
            (thermoDict, species)
        );


    // Selectors

        //- Select constructed from dictionary
        static autoPtr<chemistryReader> New
        (
            const dictionary& thermoDict,
            speciesTable& species
        );


    //- Destructor
    virtual ~chemistryReader() = default;


    // Member Functions

        //- Return access to the list of species
        virtual const speciesTable& species() const = 0;

        //- Table of species composition
        virtual const speciesCompositionTable& specieComposition() const = 0;

        //- Return access to the thermo packages
        virtual const ReactionTable<ThermoType>& speciesThermo() const = 0;

        //- Return access to the list of reactions
        virtual const ReactionList<ThermoType>& reactions() const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "chemistryReaders/chemistryReader/chemistryReader.C"
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


#define makeChemistryReader(Thermo)                                            \
    defineTemplateTypeNameAndDebug(chemistryReader<Thermo>, 0);                \
    defineTemplateRunTimeSelectionTable(chemistryReader<Thermo>, dictionary)


#define makeChemistryReaderType(Reader, Thermo)                                \
    defineNamedTemplateTypeNameAndDebug(Reader<Thermo>, 0);                    \
    chemistryReader<Thermo>::adddictionaryConstructorToTable<Reader<Thermo>>   \
        add##Reader##Thermo##ConstructorToTable_


// for non-templated chemistry readers
#define addChemistryReaderType(Reader, Thermo)                                 \
    defineTypeNameAndDebug(Reader, 0);                                         \
    chemistryReader<Thermo>::adddictionaryConstructorToTable<Reader>           \
        add##Reader##Thermo##ConstructorToTable_


// for templated chemistry readers
#define addTemplateChemistryReaderType(Reader, Thermo)                         \
    defineNamedTemplateTypeNameAndDebug(Reader, 0);                            \
    chemistryReader<Thermo>::adddictionaryConstructorToTable<Reader>           \
        add##Reader##Thermo##ConstructorToTable_


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
