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

\*---------------------------------------------------------------------------*/

#include "submodels/kinematic/injectionModel/curvatureSeparation/curvatureSeparation.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "fvMesh/fvMesh.H"
#include "db/Time/TimeOpenFOAM.H"
#include "fields/volFields/volFields.H"
#include "kinematicSingleLayer/kinematicSingleLayer.H"
#include "interpolation/surfaceInterpolation/surfaceInterpolation/surfaceInterpolate.H"
#include "finiteVolume/fvc/fvcDiv.H"
#include "finiteVolume/fvc/fvcGrad.H"
#include "primitives/strings/lists/stringListOps.H"
#include "meshes/polyMesh/polyPatches/constraint/cyclic/cyclicPolyPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(curvatureSeparation, 0);
addToRunTimeSelectionTable
(
    injectionModel,
    curvatureSeparation,
    dictionary
);

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

tmp<volScalarField> curvatureSeparation::calcInvR1
(
    const volVectorField& U
) const
{
    // method 1
/*
    tmp<volScalarField> tinvR1
    (
        new volScalarField("invR1", fvc::div(film().nHat()))
    );
*/

    // method 2
    dimensionedScalar smallU("smallU", dimVelocity, ROOTVSMALL);
    volVectorField UHat(U/(mag(U) + smallU));
    tmp<volScalarField> tinvR1
    (
        new volScalarField("invR1", UHat & (UHat & gradNHat_))
    );


    scalarField& invR1 = tinvR1.ref().primitiveFieldRef();

    // apply defined patch radii
    const scalar rMin = 1e-6;
    const fvMesh& mesh = film().regionMesh();
    const polyBoundaryMesh& pbm = mesh.boundaryMesh();
    forAll(definedPatchRadii_, i)
    {
        label patchi = definedPatchRadii_[i].first();
        scalar definedInvR1 = 1.0/max(rMin, definedPatchRadii_[i].second());
        UIndirectList<scalar>(invR1, pbm[patchi].faceCells()) = definedInvR1;
    }

    // filter out large radii
    const scalar rMax = 1e6;
    forAll(invR1, i)
    {
        if (mag(invR1[i]) < 1/rMax)
        {
            invR1[i] = -1.0;
        }
    }

    if (debug && mesh.time().writeTime())
    {
        tinvR1().write();
    }

    return tinvR1;
}


tmp<scalarField> curvatureSeparation::calcCosAngle
(
    const surfaceScalarField& phi
) const
{
    const fvMesh& mesh = film().regionMesh();
    const vectorField nf(mesh.Sf()/mesh.magSf());
    const labelUList& own = mesh.owner();
    const labelUList& nbr = mesh.neighbour();

    scalarField phiMax(mesh.nCells(), -GREAT);
    scalarField cosAngle(mesh.nCells(), Zero);
    forAll(nbr, facei)
    {
        label cellO = own[facei];
        label cellN = nbr[facei];

        if (phi[facei] > phiMax[cellO])
        {
            phiMax[cellO] = phi[facei];
            cosAngle[cellO] = -gHat_ & nf[facei];
        }
        if (-phi[facei] > phiMax[cellN])
        {
            phiMax[cellN] = -phi[facei];
            cosAngle[cellN] = -gHat_ & -nf[facei];
        }
    }

    forAll(phi.boundaryField(), patchi)
    {
        const fvsPatchScalarField& phip = phi.boundaryField()[patchi];
        const fvPatch& pp = phip.patch();
        const labelList& faceCells = pp.faceCells();
        const vectorField nf(pp.nf());
        forAll(phip, i)
        {
            label celli = faceCells[i];
            if (phip[i] > phiMax[celli])
            {
                phiMax[celli] = phip[i];
                cosAngle[celli] = -gHat_ & nf[i];
            }
        }
    }
/*
    // correction for cyclics - use cyclic pairs' face normal instead of
    // local face normal
    const fvBoundaryMesh& pbm = mesh.boundary();
    forAll(phi.boundaryField(), patchi)
    {
        if (isA<cyclicPolyPatch>(pbm[patchi]))
        {
            const scalarField& phip = phi.boundaryField()[patchi];
            const vectorField nf(pbm[patchi].nf());
            const labelList& faceCells = pbm[patchi].faceCells();
            const label sizeBy2 = pbm[patchi].size()/2;

            for (label face0=0; face0<sizeBy2; face0++)
            {
                label face1 = face0 + sizeBy2;
                label cell0 = faceCells[face0];
                label cell1 = faceCells[face1];

                // flux leaving half 0, entering half 1
                if (phip[face0] > phiMax[cell0])
                {
                    phiMax[cell0] = phip[face0];
                    cosAngle[cell0] = -gHat_ & -nf[face1];
                }

                // flux leaving half 1, entering half 0
                if (-phip[face1] > phiMax[cell1])
                {
                    phiMax[cell1] = -phip[face1];
                    cosAngle[cell1] = -gHat_ & nf[face0];
                }
            }
        }
    }
*/
    // checks
    if (debug && mesh.time().writeTime())
    {
        volScalarField volCosAngle
        (
            IOobject
            (
                "cosAngle",
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ),
            mesh,
            dimensionedScalar(dimless, Zero),
            fvPatchFieldBase::zeroGradientType()
        );
        volCosAngle.primitiveFieldRef() = cosAngle;
        volCosAngle.correctBoundaryConditions();
        volCosAngle.write();
    }

    return clamp(cosAngle, scalarMinMax(-1, 1));
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

curvatureSeparation::curvatureSeparation
(
    surfaceFilmRegionModel& film,
    const dictionary& dict
)
:
    injectionModel(type(), film, dict),
    gradNHat_(fvc::grad(film.nHat())),
    deltaByR1Min_(coeffDict_.getOrDefault<scalar>("deltaByR1Min", 0)),
    definedPatchRadii_(),
    magG_(mag(film.g().value())),
    gHat_(Zero)
{
    if (magG_ < ROOTVSMALL)
    {
        FatalErrorInFunction
            << "Acceleration due to gravity must be non-zero"
            << exit(FatalError);
    }

    gHat_ = film.g().value()/magG_;

    List<Tuple2<word, scalar>> prIn(coeffDict_.lookup("definedPatchRadii"));
    const wordList& allPatchNames = film.regionMesh().boundaryMesh().names();

    DynamicList<Tuple2<label, scalar>> prData(allPatchNames.size());

    labelHashSet uniquePatchIDs;

    forAllReverse(prIn, i)
    {
        labelList patchIDs = findIndices(allPatchNames, prIn[i].first());
        forAll(patchIDs, j)
        {
            const label patchi = patchIDs[j];

            if (!uniquePatchIDs.found(patchi))
            {
                const scalar radius = prIn[i].second();
                prData.append(Tuple2<label, scalar>(patchi, radius));

                uniquePatchIDs.insert(patchi);
            }
        }
    }

    definedPatchRadii_.transfer(prData);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

curvatureSeparation::~curvatureSeparation()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void curvatureSeparation::correct
(
    scalarField& availableMass,
    scalarField& massToInject,
    scalarField& diameterToInject
)
{
    const kinematicSingleLayer& film =
        refCast<const kinematicSingleLayer>(this->film());
    const fvMesh& mesh = film.regionMesh();

    const volScalarField& delta = film.delta();
    const volVectorField& U = film.U();
    const surfaceScalarField& phi = film.phi();
    const volScalarField& rho = film.rho();
    const scalarField magSqrU(magSqr(film.U()));
    const volScalarField& sigma = film.sigma();

    const scalarField invR1(calcInvR1(U));
    const scalarField cosAngle(calcCosAngle(phi));

    // calculate force balance
    const scalar Fthreshold = 1e-10;
    scalarField Fnet(mesh.nCells(), Zero);
    scalarField separated(mesh.nCells(), Zero);
    forAll(invR1, i)
    {
        if ((invR1[i] > 0) && (delta[i]*invR1[i] > deltaByR1Min_))
        {
            scalar R1 = 1.0/(invR1[i] + ROOTVSMALL);
            scalar R2 = R1 + delta[i];

            // inertial force
            scalar Fi = -delta[i]*rho[i]*magSqrU[i]*72.0/60.0*invR1[i];

            // body force
            scalar Fb =
              - 0.5*rho[i]*magG_*invR1[i]*(sqr(R1) - sqr(R2))*cosAngle[i];

            // surface force
            scalar Fs = sigma[i]/R2;

            Fnet[i] = Fi + Fb + Fs;

            if (Fnet[i] + Fthreshold < 0)
            {
                separated[i] = 1.0;
            }
        }
    }

    // inject all available mass
    massToInject = separated*availableMass;
    diameterToInject = separated*delta;
    availableMass -= separated*availableMass;

    addToInjectedMass(sum(separated*availableMass));

    if (debug && mesh.time().writeTime())
    {
        volScalarField volFnet
        (
            IOobject
            (
                "Fnet",
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ),
            mesh,
            dimensionedScalar(dimForce, Zero),
            fvPatchFieldBase::zeroGradientType()
        );
        volFnet.primitiveFieldRef() = Fnet;
        volFnet.correctBoundaryConditions();
        volFnet.write();
    }

    injectionModel::correct();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// ************************************************************************* //
