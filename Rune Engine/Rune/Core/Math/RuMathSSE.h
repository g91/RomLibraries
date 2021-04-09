#ifndef _RUMATHSSE_H_
#define _RUMATHSSE_H_

#include <math.h>

#include "../../Core/Type/RuType_Base.h"

#pragma unmanaged

__forceinline void RuSSE_Reshuffle(__m128 *pM1, __m128 *pM2, __m128 *pM3, __m128 *pM4, __m128 *pR1, __m128 *pR2, __m128 *pR3, __m128 *pR4)
{
	register __m128 xmm0, xmm1;

	xmm0 = _mm_shuffle_ps(*pM1, *pM2, _MM_SHUFFLE(3, 2, 3, 2));				//	
	xmm1 = _mm_shuffle_ps(*pM3, *pM4, _MM_SHUFFLE(3, 2, 3, 2));				//
	*pR4 = _mm_shuffle_ps(xmm0, xmm1, _MM_SHUFFLE(3, 1, 3, 1));				//
	*pR3 = _mm_shuffle_ps(xmm0, xmm1, _MM_SHUFFLE(2, 0, 2, 0));				//
	xmm0 = _mm_shuffle_ps(*pM1, *pM2, _MM_SHUFFLE(1, 0, 1, 0));				//
	xmm1 = _mm_shuffle_ps(*pM3, *pM4, _MM_SHUFFLE(1, 0, 1, 0));				//
	*pR2 = _mm_shuffle_ps(xmm0, xmm1, _MM_SHUFFLE(3, 1, 3, 1));				//
	*pR1 = _mm_shuffle_ps(xmm0, xmm1, _MM_SHUFFLE(2, 0, 2, 0));				//
}

__forceinline void RuSSE_LoadFourFloats(const float *a0, const float *a1, const float *a2, const float *a3, __m128 *res)
{
	register __m128 xmm0 = _mm_load_ss(a0);									//  0  0  0 a0
	register __m128 xmm1 = _mm_load_ss(a1);									//  0  0  0 a1
	register __m128 xmm2 = _mm_load_ss(a2);									//  0  0  0 a2
	register __m128 xmm3 = _mm_load_ss(a3);									//  0  0  0 a3
	xmm0 = _mm_movelh_ps(xmm0, xmm2);										//  0 a2  0 a0
	xmm1 = _mm_shuffle_ps(xmm1, xmm3, _MM_SHUFFLE(0, 1, 0, 1));				// a3  0 a1  0
	*res = _mm_or_ps(xmm0, xmm1);											// a3 a2 a1 a0
}

__forceinline void RuSSE_StoreFourFloats(float *a0, float *a1, float *a2, float *a3, __m128 src)
{
	register __m128 xmm0 = _mm_unpackhi_ps(src, src);						// 
	register __m128 xmm1 = _mm_unpacklo_ps(src, src);						//	 
	register __m128 xmm2 = _mm_movehl_ps(xmm0, xmm0);						//
	register __m128 xmm3 = _mm_movehl_ps(xmm1, xmm1);						//
	_mm_store_ss(a0, xmm1);													//
	_mm_store_ss(a1, xmm3);													//
	_mm_store_ss(a2, xmm0);													//
	_mm_store_ss(a3, xmm2);													//
}

__forceinline void RuSSE_Collapse2Mat(__m128 *m1, __m128 *m2, float W1, float W2, __m128 *pR)
{
	register __m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6;

	// Load and propagate the matrix weight 1.
	xmm0 = _mm_load_ss(&W1);
	xmm0 = _mm_shuffle_ps(xmm0, xmm0, 0);

	// Multiply matrix 1 by weight 1.
	xmm1 = _mm_mul_ps(xmm0, m1[0]);
	xmm2 = _mm_mul_ps(xmm0, m1[1]);
	xmm3 = _mm_mul_ps(xmm0, m1[2]);

	// Load weight 2.
	xmm0 = _mm_load_ss(&W2);
	xmm0 = _mm_shuffle_ps(xmm0, xmm0, 0);

	// Multiply matrix 2 by weight 2.
	xmm4 = _mm_mul_ps(xmm0, m2[0]);
	xmm5 = _mm_mul_ps(xmm0, m2[1]);
	xmm6 = _mm_mul_ps(xmm0, m2[2]);

	// Add matrix 1 to matrix 2.
	pR[0] = _mm_add_ps(xmm1, xmm4);
	pR[1] = _mm_add_ps(xmm2, xmm5);
	pR[2] = _mm_add_ps(xmm3, xmm6);
}

__forceinline void RuSSE_Collapse4Mat(__m128 *m1, __m128 *m2, __m128 *m3, __m128 *m4, float W1, float W2, float W3, float W4, __m128 *pR)
{
	register __m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6;

	// Load and propagate the matrix weight 1.
	xmm0 = _mm_load_ss(&W1);
	xmm0 = _mm_shuffle_ps(xmm0, xmm0, 0);
	
	// Multiply matrix 1 by weight 1.
	xmm1 = _mm_mul_ps(xmm0, m1[0]);
	xmm2 = _mm_mul_ps(xmm0, m1[1]);
	xmm3 = _mm_mul_ps(xmm0, m1[2]);

	// Load weight 2.
	xmm0 = _mm_load_ss(&W2);
	xmm0 = _mm_shuffle_ps(xmm0, xmm0, 0);

	// Multiply matrix 2 by weight 2.
	xmm4 = _mm_mul_ps(xmm0, m2[0]);
	xmm5 = _mm_mul_ps(xmm0, m2[1]);
	xmm6 = _mm_mul_ps(xmm0, m2[2]);

	// Add matrix 1 to matrix 2.
	xmm1 = _mm_add_ps(xmm1, xmm4);
	xmm2 = _mm_add_ps(xmm2, xmm5);
	xmm3 = _mm_add_ps(xmm3, xmm6);

	// Load weight 3.
	xmm0 = _mm_load_ss(&W3);
	xmm0 = _mm_shuffle_ps(xmm0, xmm0, 0);

	// Multiply matrix 3 by weight 3.
	xmm4 = _mm_mul_ps(xmm0, m3[0]);
	xmm5 = _mm_mul_ps(xmm0, m3[1]);
	xmm6 = _mm_mul_ps(xmm0, m3[2]);

	// Add matrix 1 and 2 to matrix 3.
	xmm1 = _mm_add_ps(xmm1, xmm4);
	xmm2 = _mm_add_ps(xmm2, xmm5);
	xmm3 = _mm_add_ps(xmm3, xmm6);

	// Load weight 4.
	xmm0 = _mm_load_ss(&W4);
	xmm0 = _mm_shuffle_ps(xmm0, xmm0, 0);

	// Multiply matrix 4 by weight 4.
	xmm4 = _mm_mul_ps(xmm0, m4[0]);
	xmm5 = _mm_mul_ps(xmm0, m4[1]);
	xmm6 = _mm_mul_ps(xmm0, m4[2]);

	// Add matrix 1, 2 and 3 to matrix 4.
	pR[0] = _mm_add_ps(xmm1, xmm4);
	pR[1] = _mm_add_ps(xmm2, xmm5);
	pR[2] = _mm_add_ps(xmm3, xmm6);
}

__forceinline void RuSSE_Vert4Mat4Normalise(__m128 *pM, __m128 *pX, __m128 *pY, __m128 *pZ, CRuVector3 *vertexOut)
{
	// 9 xmm registers :-(.
	register __m128	xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6;

	///////////////////////////////////////////////////////////////////////////
	// We process the X,Y and Z components of the input position by the four 
	// bone matrices separately. So we do all the X's first etc. Here is the 
	// maths for the X's to give you an idea of what is happening.

	// Firstly the rotational part of the transform.
	// pR1->x=pM1->00*pP1->x+pM1->10*pP1->y+pM1->20*pP1->z;
	// pR2->x=pM2->00*pP2->x+pM2->10*pP2->y+pM2->20*pP2->z;
	// pR3->x=pM3->00*pP3->x+pM3->10*pP3->y+pM3->20*pP3->z;
	// pR4->x=pM4->00*pP4->x+pM4->10*pP4->y+pM4->20*pP4->z;

	// The translational component.
	// pR1->x += pM1->30;
	// pR2->x += pM2->30;
	// pR3->x += pM3->30;
	// pR4->x += pM4->30;

	///////////////////////////////////////////////////////////////////////////
	// Do the X's.

	// Load 1st column of each bone matrix.
	RuSSE_Reshuffle(&pM[0], &pM[3], &pM[6], &pM[9], &xmm0, &xmm1, &xmm2, &xmm3);
	
	// Do the multiplies for the rotational part of the bone matrices [POSITION].
	xmm4  = _mm_mul_ps(*pX, xmm0); 
	xmm5  = _mm_mul_ps(*pY, xmm1); 
	xmm6  = _mm_mul_ps(*pZ, xmm2); 
	
	// Do the adds for the rotational part of the bone matrices [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm5); 
	xmm4 = _mm_add_ps(xmm4, xmm6); 

	// Now add the translational part [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm3); 
		
	// Store transformed x components [POSITION].
	RuSSE_StoreFourFloats(&vertexOut[0].x, &vertexOut[1].x, &vertexOut[2].x, &vertexOut[3].x, xmm4);

	///////////////////////////////////////////////////////////////////////////
	// Do the Y's.

	// Reorder the data so that we use the second column of each matrix.
	RuSSE_Reshuffle(&pM[1], &pM[4], &pM[7], &pM[10], &xmm0, &xmm1, &xmm2, &xmm3);

	// Do the multiplies for the rotational part of the bone matrices [POSITION].
	xmm4  = _mm_mul_ps(*pX, xmm0); 
	xmm5  = _mm_mul_ps(*pY, xmm1); 
	xmm6  = _mm_mul_ps(*pZ, xmm2); 
	
	// Do the adds for the rotational part of the bone matrices [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm5); 
	xmm4 = _mm_add_ps(xmm4, xmm6); 

	// Now add the translational part [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm3); 
		
	// Store transformed y components [POSITION].
	RuSSE_StoreFourFloats(&vertexOut[0].y, &vertexOut[1].y, &vertexOut[2].y, &vertexOut[3].y, xmm4);

	///////////////////////////////////////////////////////////////////////////
	// Do the Z's.
	
	// Reorder the data so that we use the third column of each matrix.
	RuSSE_Reshuffle(&pM[2], &pM[5], &pM[8], &pM[11], &xmm0, &xmm1, &xmm2, &xmm3);

	// Do the multiplies for the rotational part of the bone matrices [POSITION].
	xmm4  = _mm_mul_ps(*pX, xmm0); 
	xmm5  = _mm_mul_ps(*pY, xmm1); 
	xmm6  = _mm_mul_ps(*pZ, xmm2); 
	
	// Do the adds for the rotational part of the bone matrices [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm5); 
	xmm4 = _mm_add_ps(xmm4, xmm6); 

	// Now add the translational part [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm3); 
		
	// Store transformed z components [POSITION].
	RuSSE_StoreFourFloats(&vertexOut[0].z, &vertexOut[1].z, &vertexOut[2].z, &vertexOut[3].z, xmm4);
}

__forceinline void RuSSE_Vert4Mat4Normalise(__m128 *pM, __m128 *pX, __m128 *pY, __m128 *pZ, __m128 *pNX, __m128 *pNY, __m128 *pNZ, CRuVector3 *vertexOut, CRuVector3 *normalOut)
{
	// 9 xmm registers :-(.
	register __m128	xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;

	///////////////////////////////////////////////////////////////////////////
	// We process the X,Y and Z components of the input position by the four 
	// bone matrices separately. So we do all the X's first etc. Here is the 
	// maths for the X's to give you an idea of what is happening.

	// Firstly the rotational part of the transform.
	// pR1->x=pM1->00*pP1->x+pM1->10*pP1->y+pM1->20*pP1->z;
	// pR2->x=pM2->00*pP2->x+pM2->10*pP2->y+pM2->20*pP2->z;
	// pR3->x=pM3->00*pP3->x+pM3->10*pP3->y+pM3->20*pP3->z;
	// pR4->x=pM4->00*pP4->x+pM4->10*pP4->y+pM4->20*pP4->z;

	// The translational component.
	// pR1->x += pM1->30;
	// pR2->x += pM2->30;
	// pR3->x += pM3->30;
	// pR4->x += pM4->30;

	///////////////////////////////////////////////////////////////////////////
	// Do the X's.

	// Load 1st column of each bone matrix.
	RuSSE_Reshuffle(&pM[0], &pM[3], &pM[6], &pM[9], &xmm0, &xmm1, &xmm2, &xmm3);
	
	// Do the multiplies for the rotational part of the bone matrices [POSITION].
	xmm4  = _mm_mul_ps(*pX, xmm0); 
	xmm5  = _mm_mul_ps(*pY, xmm1); 
	xmm6  = _mm_mul_ps(*pZ, xmm2); 
	
	// Do the adds for the rotational part of the bone matrices [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm5); 
	xmm4 = _mm_add_ps(xmm4, xmm6); 

	// Now add the translational part [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm3); 
		
	// Store transformed x components [POSITION].
	RuSSE_StoreFourFloats(&vertexOut[0].x, &vertexOut[1].x, &vertexOut[2].x, &vertexOut[3].x, xmm4);

	// Do the multiplies for the rotational part of the bone matrices [NORMAL].
	xmm4  = _mm_mul_ps(*pNX, xmm0); 
	xmm5  = _mm_mul_ps(*pNY, xmm1); 
	xmm6  = _mm_mul_ps(*pNZ, xmm2); 

	// Do the adds for the rotational part of the bone matrices [NORMAL].
	xmm4  = _mm_add_ps(xmm4, xmm5); 
	xmm7  = _mm_add_ps(xmm4, xmm6);

	///////////////////////////////////////////////////////////////////////////
	// Do the Y's.

	// Reorder the data so that we use the second column of each matrix.
	RuSSE_Reshuffle(&pM[1], &pM[4], &pM[7], &pM[10], &xmm0, &xmm1, &xmm2, &xmm3);

	// Do the multiplies for the rotational part of the bone matrices [POSITION].
	xmm4  = _mm_mul_ps(*pX, xmm0); 
	xmm5  = _mm_mul_ps(*pY, xmm1); 
	xmm6  = _mm_mul_ps(*pZ, xmm2); 
	
	// Do the adds for the rotational part of the bone matrices [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm5); 
	xmm4 = _mm_add_ps(xmm4, xmm6); 

	// Now add the translational part [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm3); 
		
	// Store transformed y components [POSITION].
	RuSSE_StoreFourFloats(&vertexOut[0].y, &vertexOut[1].y, &vertexOut[2].y, &vertexOut[3].y, xmm4);

	// Do the multiplies for the rotational part of the bone matrices [NORMAL].
	xmm4  = _mm_mul_ps(*pNX, xmm0); 
	xmm5  = _mm_mul_ps(*pNY, xmm1); 
	xmm6  = _mm_mul_ps(*pNZ, xmm2); 

	// Do the adds for the rotational part of the bone matrices [NORMAL].
	xmm4  = _mm_add_ps(xmm4, xmm5); 
	xmm8  = _mm_add_ps(xmm4, xmm6);

	///////////////////////////////////////////////////////////////////////////
	// Do the Z's.
	
	// Reorder the data so that we use the third column of each matrix.
	RuSSE_Reshuffle(&pM[2], &pM[5], &pM[8], &pM[11], &xmm0, &xmm1, &xmm2, &xmm3);

	// Do the multiplies for the rotational part of the bone matrices [POSITION].
	xmm4  = _mm_mul_ps(*pX, xmm0); 
	xmm5  = _mm_mul_ps(*pY, xmm1); 
	xmm6  = _mm_mul_ps(*pZ, xmm2); 
	
	// Do the adds for the rotational part of the bone matrices [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm5); 
	xmm4 = _mm_add_ps(xmm4, xmm6); 

	// Now add the translational part [POSITION].
	xmm4 = _mm_add_ps(xmm4, xmm3); 
		
	// Store transformed z components [POSITION].
	RuSSE_StoreFourFloats(&vertexOut[0].z, &vertexOut[1].z, &vertexOut[2].z, &vertexOut[3].z, xmm4);

	// Do the multiplies for the rotational part of the bone matrices [NORMAL].
	xmm4  = _mm_mul_ps(*pNX, xmm0); 
	xmm5  = _mm_mul_ps(*pNY, xmm1); 
	xmm6  = _mm_mul_ps(*pNZ, xmm2); 

	// Do the adds for the rotational part of the bone matrices [NORMAL].
	xmm4  = _mm_add_ps(xmm4, xmm5); 
	xmm4  = _mm_add_ps(xmm4, xmm6);

	///////////////////////////////////////////////////////////////////////////
	// Normalise the normals.
	xmm0 = _mm_mul_ps(xmm7, xmm7);
	xmm1 = _mm_mul_ps(xmm8, xmm8);
	xmm2 = _mm_mul_ps(xmm4, xmm4);
	xmm0 = _mm_add_ps(xmm0, xmm1);
	xmm0 = _mm_add_ps(xmm0, xmm2);
	xmm0 = _mm_rsqrt_ps(xmm0);
	xmm1 = _mm_mul_ps(xmm7, xmm0);
	xmm2 = _mm_mul_ps(xmm8, xmm0);
	xmm3 = _mm_mul_ps(xmm4, xmm0);
	
	// Store transformed x components [NORMAL].
	RuSSE_StoreFourFloats(&normalOut[0].x, &normalOut[1].x, &normalOut[2].x, &normalOut[3].x, xmm1);

	// Store transformed y components [NORMAL].
	RuSSE_StoreFourFloats(&normalOut[0].y, &normalOut[1].y, &normalOut[2].y, &normalOut[3].y, xmm2);

	// Store transformed z components [NORMAL].
	RuSSE_StoreFourFloats(&normalOut[0].z, &normalOut[1].z, &normalOut[2].z, &normalOut[3].z, xmm3);
}

__forceinline void RuSSE_Matrix4x4Invert(float *src)
{
	__m128 minor0, minor1, minor2, minor3;
	__m128 row0, row1, row2, row3;
	__m128 det, tmp1;

	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src)), (__m64*)(src+ 4));
	row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(src+8)), (__m64*)(src+12));
	row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
	row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src+ 2)), (__m64*)(src+ 6));
	row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(src+10)), (__m64*)(src+14));
	row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
	row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row2, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_mul_ps(row1, tmp1);
	minor1 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
	minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
	minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row1, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
	minor3 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
	minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	row2 = _mm_shuffle_ps(row2, row2, 0x4E);
	minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
	minor2 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
	minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
	minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
	// -----------------------------------------------
	det = _mm_mul_ps(row0, minor0);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
	det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
	tmp1 = _mm_rcp_ss(det);
	det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
	det = _mm_shuffle_ps(det, det, 0x00);
	minor0 = _mm_mul_ps(det, minor0);
	_mm_storel_pi((__m64*)(src), minor0);
	_mm_storeh_pi((__m64*)(src+2), minor0);
	minor1 = _mm_mul_ps(det, minor1);
	_mm_storel_pi((__m64*)(src+4), minor1);
	_mm_storeh_pi((__m64*)(src+6), minor1);
	minor2 = _mm_mul_ps(det, minor2);
	_mm_storel_pi((__m64*)(src+ 8), minor2);
	_mm_storeh_pi((__m64*)(src+10), minor2);
	minor3 = _mm_mul_ps(det, minor3);
	_mm_storel_pi((__m64*)(src+12), minor3);
	_mm_storeh_pi((__m64*)(src+14), minor3);
}
#pragma managed

#endif
