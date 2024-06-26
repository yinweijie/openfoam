Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);


Info<< "Creating face flux\n" << endl;
surfaceScalarField phi
(
    IOobject
    (
        "phi",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedScalar(mesh.Sf().dimensions()*U.dimensions(), Zero)
);


singlePhaseTransportModel laminarTransport(U, phi);

autoPtr<incompressible::turbulenceModel> turbulence
(
    incompressible::turbulenceModel::New(U, phi, laminarTransport)
);

dimensionedVector Ubar("Ubar", dimVelocity, laminarTransport);

vector flowDirection = (Ubar/mag(Ubar)).value();
tensor flowMask = sqr(flowDirection);

dimensionedVector gradP
(
    "gradP",
    dimensionSet(0, 1, -2, 0, 0),
    Zero
);

#include "cfdTools/general/include/createFvOptions.H"
