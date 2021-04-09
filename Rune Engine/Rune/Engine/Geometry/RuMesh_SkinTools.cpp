#include "../../Engine/Geometry/RuMesh_SkinTools.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

INT32 RuSkinTool_FindGreatestInfluencingBone(IRuMesh **meshes, INT32 meshCount, CRuVector3 position, REAL envelopeRadius)
{
	// Initialize aggregate weight array
	REAL aggregateWeights[256];
	memset(aggregateWeights, 0, sizeof(REAL) * 256);

	// Iterate through all meshes in the array
	for(INT32 meshIdx = 0; meshIdx < meshCount; ++meshIdx)
	{
		IRuMesh *mesh = meshes[meshIdx];

		if(!mesh)
			continue;

		INT32 vertexCount = mesh->GetNumVertices();
		IRuMorphTarget *morphTarget = mesh->GetMorphTarget(0);

		if(!morphTarget)
			continue;

		CRuVector3 *points = morphTarget->GetPosition();
		UINT16 *blendIdx = morphTarget->GetBlendIndex();
		REAL *blendWeight = morphTarget->GetBlendWeight();

		if(!points || !blendIdx || !blendWeight)
			continue;

		// Build aggregate weights for all bones
		for(INT32 i = 0, j = 0; i < vertexCount; ++i)
		{
			REAL distWeight = max(0.0f, envelopeRadius - (points[i] - position).Magnitude()) / envelopeRadius;

			for(INT32 k = 0; k < 4; ++j, ++k)
			{
				if(blendWeight[j] < ruEPSILON)
					continue;

				// Accumulate weight
				aggregateWeights[blendIdx[j]] += blendWeight[j] * distWeight;
			}
		}
	}

	// Find bone with greatest influence
	INT32 curMaxBone = -1;
	REAL curMaxWeight = ruEPSILON;
	for(INT32 i = 0; i < 256; ++i)
	{
		if(aggregateWeights[i] > curMaxWeight)
		{
			curMaxBone = i;
			curMaxWeight = aggregateWeights[i];
		}
	}

	return curMaxBone;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
