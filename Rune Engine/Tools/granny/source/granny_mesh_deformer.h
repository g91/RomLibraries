#if !defined(GRANNY_MESH_DEFORMER_H)
#include "header_preamble.h"
// ========================================================================
// $File: //jeffr/granny/rt/granny_mesh_deformer.h $
// $DateTime: 2006/10/19 16:35:09 $
// $Change: 13632 $
// $Revision: #7 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_NAMESPACE_H)
#include "granny_namespace.h"
#endif

#if !defined(GRANNY_TYPES_H)
#include "granny_types.h"
#endif

#if !defined(GRANNY_DEFORMERS_H)
#include "granny_deformers.h"
#endif

BEGIN_GRANNY_NAMESPACE EXPGROUP(MeshDeformerGroup);

EXPTYPE struct mesh_deformer;

EXPTYPE enum deformer_tail_flags
{
    DontAllowUncopiedTail,
    AllowUncopiedTail
};


EXPAPI GS_READ mesh_deformer *NewMeshDeformer(data_type_definition const *InputVertexLayout,
                                              data_type_definition const *OutputVertexLayout,
                                              deformation_type DeformationType,
                                              deformer_tail_flags TailFlag);
EXPAPI GS_PARAM void FreeMeshDeformer(mesh_deformer *Deformer);

EXPAPI GS_PARAM void DeformVertices(mesh_deformer const &Deformer,
                                    int32x const *MatrixIndices,
                                    real32 const *MatrixBuffer4x4,
                                    int32x VertexCount,
                                    void const *SourceVertices,
                                    void *DestVertices);

END_GRANNY_NAMESPACE;

#include "header_postfix.h"
#define GRANNY_MESH_DEFORMER_H
#endif
