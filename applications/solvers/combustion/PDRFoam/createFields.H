Info<< "Reading thermophysical properties\n" << endl;

autoPtr<psiuReactionThermo> pThermo
(
    psiuReactionThermo::New(mesh)
);
psiuReactionThermo& thermo = pThermo();
thermo.validate(args.executable(), "ha", "ea");

basicSpecieMixture& composition = thermo.composition();

volScalarField rho
(
    IOobject
    (
        "rho",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    thermo.rho()
);

volScalarField& p = thermo.p();

volScalarField& b = composition.Y("b");
Info<< "min(b) = " << min(b).value() << endl;

Info<< "\nReading field U\n" << endl;
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

#include "cfdTools/compressible/compressibleCreatePhi.H"

mesh.setFluxRequired(p.name());

Info<< "Creating turbulence model\n" << endl;
autoPtr<compressible::RASModel> turbulence
(
    compressible::New<compressible::RASModel>
    (
        rho,
        U,
        phi,
        thermo
    )
);


Info<< "Creating the unstrained laminar flame speed\n" << endl;
autoPtr<laminarFlameSpeed> unstrainedLaminarFlameSpeed
(
    laminarFlameSpeed::New(thermo)
);


Info<< "Reading strained laminar flame speed field Su\n" << endl;
volScalarField Su
(
    IOobject
    (
        "Su",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading field betav\n" << endl;
volScalarField betav
(
    IOobject
    (
        "betav",
        mesh.facesInstance(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    ),
    mesh
);

Info<< "Reading field Lobs\n" << endl;
volScalarField Lobs
(
    IOobject
    (
        "Lobs",
        mesh.facesInstance(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    ),
    mesh
);

Info<< "Reading field CT\n" << endl;
volSymmTensorField CT
(
    IOobject
    (
        "CT",
        mesh.facesInstance(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    ),
    mesh
);

Info<< "Reading field Nv\n" << endl;
volScalarField Nv
(
    IOobject
    (
        "Nv",
        mesh.facesInstance(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    ),
    mesh
);

Info<< "Reading field nsv\n" << endl;
volSymmTensorField nsv
(
    IOobject
    (
        "nsv",
        mesh.facesInstance(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    ),
    mesh
);

IOdictionary PDRProperties
(
    IOobject
    (
        "PDRProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ_IF_MODIFIED,
        IOobject::NO_WRITE
    )
);

//- Create the drag model
autoPtr<PDRDragModel> drag = PDRDragModel::New
(
    PDRProperties,
    *turbulence,
    rho,
    U,
    phi
);

//- Create the flame-wrinkling model
autoPtr<XiModel> flameWrinkling = XiModel::New
(
    PDRProperties,
    thermo,
    *turbulence,
    Su,
    rho,
    b,
    phi
);

Info<< "Calculating turbulent flame speed field St\n" << endl;
volScalarField St
(
    IOobject
    (
        "St",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    flameWrinkling->Xi()*Su
);


multivariateSurfaceInterpolationScheme<scalar>::fieldTable fields;

if (composition.contains("ft"))
{
    fields.add(composition.Y("ft"));
}

fields.add(b);
fields.add(thermo.he());
fields.add(thermo.heu());
flameWrinkling->addXi(fields);

#include "cfdTools/compressible/createDpdt.H"

#include "cfdTools/general/include/createK.H"

#include "cfdTools/general/include/createMRF.H"
#include "cfdTools/general/include/createFvOptions.H"
