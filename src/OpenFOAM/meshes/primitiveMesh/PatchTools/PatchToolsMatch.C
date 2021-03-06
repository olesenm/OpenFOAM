/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "PatchTools.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template
<
    class FaceList1, class PointField1,
    class FaceList2, class PointField2
>
void Foam::PatchTools::matchPoints
(
    const PrimitivePatch<FaceList1, PointField1>& p1,
    const PrimitivePatch<FaceList2, PointField2>& p2,

    labelList& p1PointLabels,
    labelList& p2PointLabels
)
{
    p1PointLabels.setSize(p1.nPoints());
    p2PointLabels.setSize(p1.nPoints());

    label nMatches = 0;

    forAll(p1.meshPoints(), pointi)
    {
        const label meshPointi = p1.meshPoints()[pointi];

        const auto iter = p2.meshPointMap().cfind(meshPointi);

        if (iter.found())
        {
            p1PointLabels[nMatches] = pointi;
            p2PointLabels[nMatches] = iter.val();
            nMatches++;
        }
    }
    p1PointLabels.setSize(nMatches);
    p2PointLabels.setSize(nMatches);
}


template
<
    class FaceList1, class PointField1,
    class FaceList2, class PointField2
>
void Foam::PatchTools::matchEdges
(
    const PrimitivePatch<FaceList1, PointField1>& p1,
    const PrimitivePatch<FaceList2, PointField2>& p2,

    labelList& p1EdgeLabels,
    labelList& p2EdgeLabels,
    bitSet& sameOrientation
)
{
    p1EdgeLabels.setSize(p1.nEdges());
    p2EdgeLabels.setSize(p1.nEdges());
    sameOrientation.setSize(p1.nEdges());
    sameOrientation = false;

    label nMatches = 0;

    EdgeMap<label> edgeToIndex(2*p1.nEdges());
    forAll(p1.edges(), edgeI)
    {
        const edge& e = p1.edges()[edgeI];
        const edge meshE
        (
            p1.meshPoints()[e[0]],
            p1.meshPoints()[e[1]]
        );
        edgeToIndex.insert(meshE, edgeI);
    }

    forAll(p2.edges(), edgeI)
    {
        const edge& e = p2.edges()[edgeI];
        const edge meshE(p2.meshPoints()[e[0]], p2.meshPoints()[e[1]]);

        const auto iter = edgeToIndex.cfind(meshE);

        if (iter.found())
        {
            p1EdgeLabels[nMatches] = iter.val();
            p2EdgeLabels[nMatches] = edgeI;
            sameOrientation.set(nMatches, (meshE[0] == iter.key()[0]));
            ++nMatches;
        }
    }
    p1EdgeLabels.setSize(nMatches);
    p2EdgeLabels.setSize(nMatches);
    sameOrientation.setSize(nMatches);
}


// ************************************************************************* //
