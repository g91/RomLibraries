#include "../../Core/Math/RuMath_UVTransform.h"

#pragma unmanaged

CRuMatrix4x4 RuUVTransform_Projection(const CRuVector3 &origin, const CRuVector3 basisVectors[3], REAL range, REAL uOffset, REAL vOffset)
{
	CRuMatrix4x4 translationMatrix;
	CRuMatrix4x4 finalMatrix;

	translationMatrix.SetToTranslation(origin * -1.0f);

	// Basis vectors
	finalMatrix.SetToBasis(basisVectors[0], basisVectors[1], basisVectors[2]);

	// Translate to origin
	finalMatrix = translationMatrix * finalMatrix;

	// Scale by range
	finalMatrix.Scale(CRuVector3(1.0f / range, 1.0f / range, 1.0f));

	// Offset
	finalMatrix.Translate(CRuVector3(uOffset, vOffset, 0.0f));

	return finalMatrix;
}

CRuMatrix4x4 RuUVTransform_Projection(const CRuVector3 &origin, const CRuMatrix4x4 &preTransform, const CRuVector3 basisVectors[3], REAL range, REAL uOffset, REAL vOffset)
{
	CRuMatrix4x4 translationMatrix;
	CRuMatrix4x4 finalMatrix;

	translationMatrix.SetToTranslation(origin * -1.0f);

	// Basis vectors
	finalMatrix.SetToBasis(basisVectors[0], basisVectors[1], basisVectors[2]);

	// Translate to origin
	finalMatrix = (translationMatrix * preTransform) * finalMatrix;

	// Scale by range
	finalMatrix.Scale(CRuVector3(1.0f / range, 1.0f / range, 1.0f));

	// Offset
	finalMatrix.Translate(CRuVector3(uOffset, vOffset, 0.0f));

	return finalMatrix;
}

#pragma managed
