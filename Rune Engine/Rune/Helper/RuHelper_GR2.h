/*!
	@project Rune
	@file RuHelper_GR2.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2007/01/27
*/

#ifndef _RUHELPER_GR2_H_
#define _RUHELPER_GR2_H_

#include "../Core/Math/RuMathBase.h"
#include "../Core/Stream/RuStreamIO.h"
#include "../Core/Utility/RuUtility.h"

#include "../ExtLink/RuExtLink_Granny.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

struct GrBoneDescriptor
{
	char*							m_userProps;
};

extern granny_data_type_definition GrBoneDescriptorType[];

// ************************************************************************************************************************************************************

extern granny_transform			GR2_Skeleton_GetBoneWorldTransform(granny_skeleton *skeleton, INT32 boneIndex);
extern BOOL						GR2_FreeSkeleton(granny_skeleton *skeleton);
extern granny_skeleton*			GR2_CloneSkeleton(granny_skeleton *srcSkeleton);
extern BOOL						GR2_SerializeSkeletonTo(IRuStream *stream, granny_skeleton *skeleton, BOOL *isOldVersion = NULL);
extern granny_skeleton*			GR2_SerializeSkeletonFrom(IRuStream *stream, BOOL *isOldVersionOut = NULL);
extern CRuString				GR2_GetProperty(const char *userProps, const char *propertyName);

extern CRuMemoryStream*			GR2_ConvertVariantToStream(granny_data_type_definition *type, void *object);

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
