/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd.
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
    Foam::ThermoCloud

Group
    grpLagrangianIntermediateClouds

Description
    Templated base class for thermodynamic cloud

    - Adds to kinematic cloud
      - Heat transfer

SourceFiles
    ThermoCloudI.H
    ThermoCloud.C

\*---------------------------------------------------------------------------*/

#ifndef ThermoCloud_H
#define ThermoCloud_H

#include "clouds/Templates/KinematicCloud/KinematicCloudPascal.H"
#include "clouds/baseClasses/thermoCloud/thermoCloud.H"
#include "SLGThermo/SLGThermo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

template<class CloudType>
class HeatTransferModel;

/*---------------------------------------------------------------------------*\
                        Class ThermoCloud Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class ThermoCloud
:
    public CloudType,
    public thermoCloud
{
public:

    // Public typedefs

        //- Type of cloud this cloud was instantiated for
        typedef CloudType cloudType;

        //- Type of parcel the cloud was instantiated for
        typedef typename CloudType::particleType parcelType;

        //- Convenience typedef for this cloud type
        typedef ThermoCloud<CloudType> thermoCloudType;


private:

    // Private data

        //- Cloud copy pointer
        autoPtr<ThermoCloud<CloudType>> cloudCopyPtr_;


    // Private member functions

        //- No copy construct
        ThermoCloud(const ThermoCloud&) = delete;

        //- No copy assignment
        void operator=(const ThermoCloud&) = delete;


protected:

    // Protected data

        //- Thermo parcel constant properties
        typename parcelType::constantProperties constProps_;


        // References to the carrier gas fields

            //- SLG thermodynamics package
            const SLGThermo& thermo_;

            //- Temperature [K]
            const volScalarField& T_;

            //- Pressure [Pa]
            const volScalarField& p_;


        // References to the cloud sub-models

            //- Heat transfer model
            autoPtr<HeatTransferModel<ThermoCloud<CloudType>>>
                heatTransferModel_;


        // Reference to the particle integration schemes

            //- Temperature integration
            autoPtr<integrationScheme> TIntegrator_;


        // Modelling options

            //- Include radiation
            Switch radiation_;

            //- Radiation sum of parcel projected areas
            autoPtr<volScalarField::Internal> radAreaP_;

            //- Radiation sum of parcel temperature^4
            autoPtr<volScalarField::Internal> radT4_;

            //- Radiation sum of parcel projected areas * temperature^4
            autoPtr<volScalarField::Internal> radAreaPT4_;


        // Sources

            //- Sensible enthalpy transfer [J/kg]
            autoPtr<volScalarField::Internal> hsTrans_;

            //- Coefficient for carrier phase hs equation [W/K]
            autoPtr<volScalarField::Internal> hsCoeff_;


    // Protected Member Functions

         // Initialisation

            //- Set cloud sub-models
            void setModels();


        // Cloud evolution functions

            //- Reset state of cloud
            void cloudReset(ThermoCloud<CloudType>& c);


public:

    // Constructors

        //- Construct given carrier gas fields
        ThermoCloud
        (
            const word& cloudName,
            const volScalarField& rho,
            const volVectorField& U,
            const dimensionedVector& g,
            const SLGThermo& thermo,
            bool readFields = true
        );

        //- Copy constructor with new name
        ThermoCloud(ThermoCloud<CloudType>& c, const word& name);

        //- Copy constructor with new name - creates bare cloud
        ThermoCloud
        (
            const fvMesh& mesh,
            const word& name,
            const ThermoCloud<CloudType>& c
        );

        //- Construct and return clone based on (this) with new name
        virtual autoPtr<Cloud<parcelType>> clone(const word& name)
        {
            return autoPtr<Cloud<parcelType>>
            (
                new ThermoCloud(*this, name)
            );
        }

        //- Construct and return bare clone based on (this) with new name
        virtual autoPtr<Cloud<parcelType>> cloneBare(const word& name) const
        {
            return autoPtr<Cloud<parcelType>>
            (
                new ThermoCloud(this->mesh(), name, *this)
            );
        }


    //- Destructor
    virtual ~ThermoCloud() = default;


    // Member Functions

        // Access

            //- Return a reference to the cloud copy
            inline const ThermoCloud& cloudCopy() const;

            //- Return the constant properties
            inline const typename parcelType::constantProperties&
                constProps() const;

            //- Return access to the constant properties
            inline typename parcelType::constantProperties& constProps();

            //- Return const access to thermo package
            inline const SLGThermo& thermo() const;

            //- Return const access to the carrier temperature field
            inline const volScalarField& T() const;

            //- Return const access to the carrier pressure field
            inline const volScalarField& p() const;


            // Sub-models

                //- Return reference to heat transfer model
                inline const HeatTransferModel<ThermoCloud<CloudType>>&
                    heatTransfer() const;


            // Integration schemes

                //-Return reference to velocity integration
                inline const integrationScheme& TIntegrator() const;


            // Modelling options

                 //- Radiation flag
                inline bool radiation() const;

                //- Radiation sum of parcel projected areas [m2]
                inline volScalarField::Internal& radAreaP();

                //- Radiation sum of parcel projected areas [m2]
                inline const volScalarField::Internal&
                    radAreaP() const;

                //- Radiation sum of parcel temperature^4 [K4]
                inline volScalarField::Internal& radT4();

                //- Radiation sum of parcel temperature^4 [K4]
                inline const volScalarField::Internal& radT4() const;

                //- Radiation sum of parcel projected area*temperature^4 [m2K4]
                inline volScalarField::Internal& radAreaPT4();

                //- Radiation sum of parcel temperature^4 [m2K4]
                inline const volScalarField::Internal&
                    radAreaPT4() const;


            // Sources

                //- Transfer the effect of parcel to the carrier phase
                inline void transferToCarrier
                (
                    const parcelType& p,
                    const typename parcelType::trackingData& td
                );


                // Enthalpy

                    //- Sensible enthalpy transfer [J/kg]
                    inline volScalarField::Internal& hsTrans();

                    //- Sensible enthalpy transfer [J/kg]
                    inline const volScalarField::Internal&
                        hsTrans() const;

                    //- Return coefficient for carrier phase hs equation
                    inline volScalarField::Internal& hsCoeff();

                    //- Return const coefficient for carrier phase hs equation
                    inline const volScalarField::Internal&
                        hsCoeff() const;

                    //- Return sensible enthalpy source term [J/kg/m3/s]
                    inline tmp<fvScalarMatrix> Sh(volScalarField& hs) const;


                // Radiation - overrides thermoCloud virtual abstract members

                    //- Return tmp equivalent particulate emission
                    inline tmp<volScalarField> Ep() const;

                    //- Return tmp equivalent particulate absorption
                    inline tmp<volScalarField> ap() const;

                    //- Return tmp equivalent particulate scattering factor
                    inline tmp<volScalarField> sigmap() const;


        // Check

            //- Maximum temperature
            inline scalar Tmax() const;

            //- Minimum temperature
            inline scalar Tmin() const;


        // Cloud evolution functions

            //- Set parcel thermo properties
            void setParcelThermoProperties
            (
                parcelType& parcel,
                const scalar lagrangianDt
            );

            //- Check parcel properties
            void checkParcelProperties
            (
                parcelType& parcel,
                const scalar lagrangianDt,
                const bool fullyDescribed
            );

            //- Store the current cloud state
            void storeState();

            //- Reset the current cloud to the previously stored state
            void restoreState();

            //- Reset the cloud source terms
            void resetSourceTerms();

            //- Apply relaxation to (steady state) cloud sources
            void relaxSources(const ThermoCloud<CloudType>& cloudOldTime);

            //- Apply scaling to (transient) cloud sources
            void scaleSources();

            //- Pre-evolve
            void preEvolve(const typename parcelType::trackingData& td);

            //- Evolve the cloud
            void evolve();


        // Mapping

            //- Remap the cells of particles corresponding to the
            //  mesh topology change with a default tracking data object
            virtual void autoMap(const mapPolyMesh&);


        // I-O

            //- Print cloud information
            void info();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "clouds/Templates/ThermoCloud/ThermoCloudI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "clouds/Templates/ThermoCloud/ThermoCloudPascal.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
