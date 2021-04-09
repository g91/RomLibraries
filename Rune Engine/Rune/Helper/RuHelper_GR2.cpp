/*!
	@project Rune
	@file RuHelper_GR2.cpp

	Copyright (c) 2004-2007 All rights reserved

	@author John Tang
	@date 2007/01/27
*/

#include "../Helper/RuHelper_GR2.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

granny_data_type_definition GrBoneDescriptorType[] =
{
	{	GrannyStringMember,			"UserDefinedProperties"	},

	{	GrannyEndMember	},
};

// ************************************************************************************************************************************************************

granny_transform GR2_Skeleton_GetBoneWorldTransform(granny_skeleton *skeleton, INT32 boneIndex)
{
	granny_transform worldTransform;
	GrannyMakeIdentity(&worldTransform);

	INT32 curBoneIndex = boneIndex;
	while(curBoneIndex >= 0)
	{
		GrannyPreMultiplyBy(&worldTransform, &skeleton->Bones[curBoneIndex].LocalTransform);
		curBoneIndex = skeleton->Bones[curBoneIndex].ParentIndex;
	}

	return worldTransform;
}

BOOL GR2_FreeSkeleton(granny_skeleton *skeleton)
{
	// Free all bone name arrays
	for(INT32 i = 0; i < skeleton->BoneCount; ++i)
	{
		delete[] skeleton->Bones[i].Name;
	}

	// Free bone array
	delete[] skeleton->Bones;

	// Free skeleton name array
	delete[] skeleton->Name;

	// Free skeleton object
	delete skeleton;

	return TRUE;
}

granny_skeleton *GR2_CloneSkeleton(granny_skeleton *srcSkeleton)
{
	granny_skeleton *dstSkeleton = ruNEW granny_skeleton;

	// Copy skeleton name
	if(srcSkeleton->Name)
	{
		char *skeletonName = ruNEW char [strlen(srcSkeleton->Name) + 1];
		strcpy(skeletonName, srcSkeleton->Name);
		dstSkeleton->Name = skeletonName;
	}

	// Allocate bone array
	dstSkeleton->BoneCount = srcSkeleton->BoneCount;
	dstSkeleton->Bones = ruNEW granny_bone [dstSkeleton->BoneCount];

	for(INT32 i = 0; i < srcSkeleton->BoneCount; ++i)
	{
		// Copy bone name
		if(srcSkeleton->Bones[i].Name)
		{
			char *boneName = ruNEW char [strlen(srcSkeleton->Bones[i].Name) + 1];
			strcpy(boneName, srcSkeleton->Bones[i].Name);
			dstSkeleton->Bones[i].Name = boneName;
		}

		// Copy parent index
		dstSkeleton->Bones[i].ParentIndex = srcSkeleton->Bones[i].ParentIndex;

		// Copy transform
		GrannySetTransform(&dstSkeleton->Bones[i].LocalTransform, srcSkeleton->Bones[i].LocalTransform.Position, srcSkeleton->Bones[i].LocalTransform.Orientation, (const granny_real32 *) srcSkeleton->Bones[i].LocalTransform.ScaleShear);
		dstSkeleton->Bones[i].LocalTransform.Flags = srcSkeleton->Bones[i].LocalTransform.Flags;

		// Copy inverse world 4x4
		memcpy(dstSkeleton->Bones[i].InverseWorld4x4, srcSkeleton->Bones[i].InverseWorld4x4, sizeof(float) * 16);

		// Copy LOD error
		dstSkeleton->Bones[i].LODError = srcSkeleton->Bones[i].LODError;

		// Ignore extended data
		dstSkeleton->Bones[i].ExtendedData.Object = NULL;
		dstSkeleton->Bones[i].ExtendedData.Type = NULL;
	}

	// Set LOD type
	dstSkeleton->LODType = srcSkeleton->LODType;

	return dstSkeleton;
}

BOOL GR2_SerializeSkeletonTo(IRuStream *stream, granny_skeleton *skeleton, BOOL *isOldVersion)
{
	if(isOldVersion && *isOldVersion)
	{
		// Write skeleton name
		INT32 skelNameLen = (INT32)strlen(skeleton->Name) + 1;
		stream->Write(&skelNameLen, sizeof(INT32));
		stream->Write(skeleton->Name, sizeof(UINT8) * skelNameLen);

		// Write number of bones
		INT32 boneCount = skeleton->BoneCount;
		stream->Write(&boneCount, sizeof(INT32));

		// Write each bone
		for(INT32 i = 0; i < boneCount; ++i)
		{
			// Write bone name
			INT32 boneNameLen = (INT32)strlen(skeleton->Bones[i].Name) + 1;
			stream->Write(&boneNameLen, sizeof(INT32));
			stream->Write(skeleton->Bones[i].Name, sizeof(UINT8) * boneNameLen);

			// Write parent index
			stream->Write(&skeleton->Bones[i].ParentIndex, sizeof(granny_int32));

			// Write transform
			stream->Write(&skeleton->Bones[i].LocalTransform.Position[0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Position[1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Position[2], sizeof(float));

			stream->Write(&skeleton->Bones[i].LocalTransform.Orientation[0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Orientation[1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Orientation[2], sizeof(float));

			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[0][0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[0][1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[0][2], sizeof(float));

			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[1][0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[1][1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[1][2], sizeof(float));

			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[2][0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[2][1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[2][2], sizeof(float));

			// Write inverse world transform
			stream->Write(skeleton->Bones[i].InverseWorld4x4, sizeof(float) * 16);

			// Write LOD error
			stream->Write(&skeleton->Bones[i].LODError, sizeof(granny_real32));
		}

		// Write LOD type
		stream->Write(&skeleton->LODType, sizeof(granny_int32));
	}
	else
	{
		// Write signature
		INT32 signature = 0xFFFFFFFF;
		stream->Write(&signature, sizeof(INT32));

		// Write version
		INT32 version = 2;
		stream->Write(&version, sizeof(INT32));

		// Write skeleton name
		INT32 skelNameLen = (INT32)strlen(skeleton->Name) + 1;
		stream->Write(&skelNameLen, sizeof(INT32));
		stream->Write(skeleton->Name, sizeof(UINT8) * skelNameLen);

		// Write number of bones
		INT32 boneCount = skeleton->BoneCount;
		stream->Write(&boneCount, sizeof(INT32));

		// Write each bone
		for(INT32 i = 0; i < boneCount; ++i)
		{
			// Write bone name
			INT32 boneNameLen = (INT32)strlen(skeleton->Bones[i].Name) + 1;
			stream->Write(&boneNameLen, sizeof(INT32));
			stream->Write(skeleton->Bones[i].Name, sizeof(UINT8) * boneNameLen);

			// Write parent index
			stream->Write(&skeleton->Bones[i].ParentIndex, sizeof(granny_int32));

			// Write transform
			stream->Write(&skeleton->Bones[i].LocalTransform.Flags, sizeof(granny_uint32));

			stream->Write(&skeleton->Bones[i].LocalTransform.Position[0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Position[1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Position[2], sizeof(float));

			stream->Write(&skeleton->Bones[i].LocalTransform.Orientation[0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Orientation[1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Orientation[2], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.Orientation[3], sizeof(float));

			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[0][0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[0][1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[0][2], sizeof(float));

			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[1][0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[1][1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[1][2], sizeof(float));

			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[2][0], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[2][1], sizeof(float));
			stream->Write(&skeleton->Bones[i].LocalTransform.ScaleShear[2][2], sizeof(float));

			// Write inverse world transform
			stream->Write(skeleton->Bones[i].InverseWorld4x4, sizeof(float) * 16);

			// Write LOD error
			stream->Write(&skeleton->Bones[i].LODError, sizeof(granny_real32));
		}

		// Write LOD type
		stream->Write(&skeleton->LODType, sizeof(granny_int32));
	}

	return TRUE;
}

granny_skeleton *GR2_SerializeSkeletonFrom(IRuStream *stream, BOOL *isOldVersionOut)
{
	granny_skeleton *skeleton = ruNEW granny_skeleton;

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	switch(version)
	{
		case 1:	// Totally and completely deprecated - this version is NOT EVEN CORRECT!!!!
			{
				// Read skeleton name
				INT32 skelNameLen = 0;
				stream->Read(&skelNameLen, sizeof(INT32));

				char *skeletonName = ruNEW char [skelNameLen];
				stream->Read(skeletonName, sizeof(UINT8) * skelNameLen);
				skeleton->Name = skeletonName;

				// Read number of bones
				INT32 boneCount = 0;
				stream->Read(&boneCount, sizeof(INT32));

				skeleton->BoneCount = boneCount;
				skeleton->Bones = ruNEW granny_bone [skeleton->BoneCount];

				for(INT32 i = 0; i < boneCount; ++i)
				{
					// Read bone name
					INT32 boneNameLen = 0;
					stream->Read(&boneNameLen, sizeof(INT32));

					char *boneName = ruNEW char [boneNameLen];
					stream->Read(boneName, sizeof(UINT8) * boneNameLen);
					skeleton->Bones[i].Name = boneName;

					// Read parent index
					stream->Read(&skeleton->Bones[i].ParentIndex, sizeof(granny_int32));

					// Read transform
					GrannyZeroTransform(&skeleton->Bones[i].LocalTransform);
					stream->Read(&skeleton->Bones[i].LocalTransform.Position[0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Position[1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Position[2], sizeof(float));

					stream->Read(&skeleton->Bones[i].LocalTransform.Orientation[0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Orientation[1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Orientation[2], sizeof(float));

					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[0][0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[0][1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[0][2], sizeof(float));

					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[1][0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[1][1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[1][2], sizeof(float));

					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[2][0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[2][1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[2][2], sizeof(float));

					// Read inverse world transform
					stream->Read(skeleton->Bones[i].InverseWorld4x4, sizeof(float) * 16);

					// Read LOD error
					stream->Read(&skeleton->Bones[i].LODError, sizeof(granny_real32));

					// Ignore extended data
					skeleton->Bones[i].ExtendedData.Object = NULL;
					skeleton->Bones[i].ExtendedData.Type = NULL;
				}

				// Read LOD type
				stream->Read(&skeleton->LODType, sizeof(granny_int32));
			}

			break;

		case 2:
			{
				// Read skeleton name
				INT32 skelNameLen = 0;
				stream->Read(&skelNameLen, sizeof(INT32));

				char *skeletonName = ruNEW char [skelNameLen];
				stream->Read(skeletonName, sizeof(UINT8) * skelNameLen);
				skeleton->Name = skeletonName;

				// Read number of bones
				INT32 boneCount = 0;
				stream->Read(&boneCount, sizeof(INT32));

				skeleton->BoneCount = boneCount;
				skeleton->Bones = ruNEW granny_bone [skeleton->BoneCount];

				for(INT32 i = 0; i < boneCount; ++i)
				{
					// Read bone name
					INT32 boneNameLen = 0;
					stream->Read(&boneNameLen, sizeof(INT32));

					char *boneName = ruNEW char [boneNameLen];
					stream->Read(boneName, sizeof(UINT8) * boneNameLen);
					skeleton->Bones[i].Name = boneName;

					// Read parent index
					stream->Read(&skeleton->Bones[i].ParentIndex, sizeof(granny_int32));

					// Read transform
					GrannyZeroTransform(&skeleton->Bones[i].LocalTransform);

					stream->Read(&skeleton->Bones[i].LocalTransform.Flags, sizeof(granny_uint32));

					stream->Read(&skeleton->Bones[i].LocalTransform.Position[0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Position[1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Position[2], sizeof(float));

					stream->Read(&skeleton->Bones[i].LocalTransform.Orientation[0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Orientation[1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Orientation[2], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.Orientation[3], sizeof(float));

					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[0][0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[0][1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[0][2], sizeof(float));

					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[1][0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[1][1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[1][2], sizeof(float));

					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[2][0], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[2][1], sizeof(float));
					stream->Read(&skeleton->Bones[i].LocalTransform.ScaleShear[2][2], sizeof(float));

					// Read inverse world transform
					stream->Read(skeleton->Bones[i].InverseWorld4x4, sizeof(float) * 16);

					// Read LOD error
					stream->Read(&skeleton->Bones[i].LODError, sizeof(granny_real32));

					// Ignore extended data
					skeleton->Bones[i].ExtendedData.Object = NULL;
					skeleton->Bones[i].ExtendedData.Type = NULL;
				}

				// Read LOD type
				stream->Read(&skeleton->LODType, sizeof(granny_int32));
			}

			break;
	}

	if(isOldVersionOut)
	{
		*isOldVersionOut = (version <= 1);
	}

	return skeleton;
}

CRuString GR2_GetProperty(const char *userProps, const char *propertyName)
{
	// Tokenize user defined properties and process them
	CRuString propertyBuffer(userProps);
	CRuStringTokenizer lineTokenizer;
	lineTokenizer.Tokenize(propertyBuffer, "\n\r");
	for(INT32 k = 0; k < lineTokenizer.GetNumTokens(); ++k)
	{
		CRuString &propertyString = lineTokenizer.GetToken(k);
		CRuStringTokenizer strTokenizer;
		strTokenizer.Tokenize(propertyString, "=");

		if(strTokenizer.GetNumTokens() == 2)
		{
			CRuString &currentPropertyName = strTokenizer.GetToken(0).Trim();
			CRuString &currentPropertyValue = strTokenizer.GetToken(1).Trim();

			if(stricmp(currentPropertyName, propertyName) == 0)
			{
				return currentPropertyValue;
			}
		}
	}

	return CRuString("");
}

// ************************************************************************************************************************************************************

CRuMemoryStream *GR2_ConvertVariantToStream(granny_data_type_definition *type, void *object)
{
	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();

	CRuMemoryStream *memStream = NULL;

	granny_file_builder *fileBuilder = GrannyBeginFileInMemory(1, GrannyCurrentGRNStandardTag, GrannyGRNFileMV_32Bit_LittleEndian, 4096);
	granny_file_data_tree_writer *dataTreeWriter = GrannyBeginFileDataTreeWriting(type, object, 0, 0);
	granny_file_writer *fileWriter = GrannyCreateMemoryFileWriter(__FILE__, __LINE__, 4096);

	GrannyWriteDataTreeToFileBuilder(dataTreeWriter, fileBuilder);
	GrannyEndFileDataTreeWriting(dataTreeWriter);
	GrannyEndFileToWriter(fileBuilder, fileWriter);

	UINT8 *buffer;
	INT32 bufferSize;
	GrannyStealMemoryWriterBuffer(fileWriter, &buffer, &bufferSize);

	memStream = ruNEW CRuMemoryStream();
	memStream->SetBufferSize(bufferSize);
	memStream->SetStreamSize(bufferSize);
	memcpy(memStream->GetBuffer(), buffer, bufferSize);

	GrannyFreeMemoryWriterBuffer(buffer);

	GrannyFreeMemoryFileWriter(__FILE__, __LINE__, fileWriter);

	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	return memStream;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
