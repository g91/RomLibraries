#include "RoleData/ObjectDataClass.h"
#include "MyBaseInc.h"
#include "../Tools/BaseFunction/BaseFunction.h"
#include <fstream>
#include <string.h>

#ifndef _CLIENT
#include "ControllerClient/ControllerClient.h"
#pragma comment(lib, "Rune.lib") 
#pragma comment(lib, "Rune_NULL.lib") 
#endif

#include "../../../Libraries/Rune Engine/Rune/Rune.h"
#include "../../../Libraries/Rune Engine/Rune/Rune Engine NULL.h"
#pragma warning (disable:4949)
#pragma unmanaged
//////////////////////////////////////////////////////////////////////////
//IRuStream
/*
class RuStreamFile : public MyFileBase
{
	CRuFileStream	_Stream;

public:

	bool	Open( const char* FileName , int Type = EM_FileBaseType_Read | EM_FileBaseType_Binary )
	{
		return ( g_ruResourceManager->OpenTempFileStream( FileName , _Stream ) != FALSE );
	};
	virtual bool    IsOpen( )									{ return (_Stream.IsOpen() != FALSE);	};
	virtual void    Close( )									{ _Stream.Close();						};
	virtual int     Read( void *DestBuf , int Size )			{ return _Stream.Read( DestBuf , Size );};
	virtual void    Seek( int Offset , FileBaseSeekENUM Type )
	{
		switch(Type)
		{
		case EM_FileSeek_Begin:
			_Stream.Seek( Offset , ruSSM_Begin );
			break;
		case EM_FileSeek_Current:
			_Stream.Seek( Offset , ruSSM_Current );
			break;
		case EM_FileSeek_End:
			_Stream.Seek( Offset , ruSSM_End );
			break;
		}
	};
};
*/
class RuStreamFile : public MyFileBase
{
	IRuStream* _Stream;

public:

	bool	Open(const char* FileName, int Type = EM_FileBaseType_Read | EM_FileBaseType_Binary)
	{
#ifdef _CLIENT
		_Stream = g_ruResourceManager->LoadStream(FileName, true);
#else
		_Stream = g_ruResourceManager->LoadStream(FileName, true);
#endif
#ifdef KALYDO
		return _Stream != NULL;
#else
		return true;
#endif
	};

	virtual bool    IsOpen() { return (_Stream->IsOpen() != FALSE); };
	virtual void    Close() { _Stream->Close();	delete _Stream; _Stream = NULL; };
	virtual int     Read(void* DestBuf, int Size) { return _Stream->Read(DestBuf, Size); };
	virtual void    Seek(int Offset, FileBaseSeekENUM Type)
	{
		switch (Type)
		{
		case EM_FileSeek_Begin:
			_Stream->Seek(Offset, ruSSM_Begin);
			break;
		case EM_FileSeek_Current:
			_Stream->Seek(Offset, ruSSM_Current);
			break;
		case EM_FileSeek_End:
			_Stream->Seek(Offset, ruSSM_End);
			break;
		}
	}
};
//////////////////////////////////////////////////////////////////////////

/*
**	Copyright (c) Leigh Brasington 2012.  All rights reserved.
**	This code may be used and reproduced without written permission.
**
**	To uppercase and lowercase functions for Unicode - language agnostic
**
**	Keywords: unicode toupper tolower C/C++
**
**	The tables were constructed from
**	http://publib.boulder.ibm.com/infocenter/iseries/v7r1m0/index.jsp?topic=%2Fnls%2Frbagslowtoupmaptable.htm
**
**	To use: call the functions at the end of this file. Use at your own risk!
*/

static const wchar_t lowers[] = {
	0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069,
	0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072,
	0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x00E0,
	0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9,
	0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x00F0, 0x00F1, 0x00F2,
	0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC,
	0x00FD, 0x00FE, 0x00FF, 0x0101, 0x0103, 0x0105, 0x0107, 0x0109, 0x010B,
	0x010D, 0x010F, 0x0111, 0x0113, 0x0115, 0x0117, 0x0119, 0x011B, 0x011D,
	0x011F, 0x0121, 0x0123, 0x0125, 0x0127, 0x0129, 0x012B, 0x012D, 0x012F,
	0x0131, 0x0133, 0x0135, 0x0137, 0x013A, 0x013C, 0x013E, 0x0140, 0x0142,
	0x0144, 0x0146, 0x0148, 0x014B, 0x014D, 0x014F, 0x0151, 0x0153, 0x0155,
	0x0157, 0x0159, 0x015B, 0x015D, 0x015F, 0x0161, 0x0163, 0x0165, 0x0167,
	0x0169, 0x016B, 0x016D, 0x016F, 0x0171, 0x0173, 0x0175, 0x0177, 0x017A,
	0x017C, 0x017E, 0x0183, 0x0185, 0x0188, 0x018C, 0x0192, 0x0199, 0x01A1,
	0x01A3, 0x01A5, 0x01A8, 0x01AD, 0x01B0, 0x01B4, 0x01B6, 0x01B9, 0x01BD,
	0x01C6, 0x01C9, 0x01CC, 0x01CE, 0x01D0, 0x01D2, 0x01D4, 0x01D6, 0x01D8,
	0x01DA, 0x01DC, 0x01DF, 0x01E1, 0x01E3, 0x01E5, 0x01E7, 0x01E9, 0x01EB,
	0x01ED, 0x01EF, 0x01F3, 0x01F5, 0x01FB, 0x01FD, 0x01FF, 0x0201, 0x0203,
	0x0205, 0x0207, 0x0209, 0x020B, 0x020D, 0x020F, 0x0211, 0x0213, 0x0215,
	0x0217, 0x0253, 0x0254, 0x0257, 0x0258, 0x0259, 0x025B, 0x0260, 0x0263,
	0x0268, 0x0269, 0x026F, 0x0272, 0x0275, 0x0283, 0x0288, 0x028A, 0x028B,
	0x0292, 0x03AC, 0x03AD, 0x03AE, 0x03AF, 0x03B1, 0x03B2, 0x03B3, 0x03B4,
	0x03B5, 0x03B6, 0x03B7, 0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD,
	0x03BE, 0x03BF, 0x03C0, 0x03C1, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
	0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, 0x03E3, 0x03E5,
	0x03E7, 0x03E9, 0x03EB, 0x03ED, 0x03EF, 0x0430, 0x0431, 0x0432, 0x0433,
	0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C,
	0x043D, 0x043E, 0x043F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445,
	0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E,
	0x044F, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458,
	0x0459, 0x045A, 0x045B, 0x045C, 0x045E, 0x045F, 0x0461, 0x0463, 0x0465,
	0x0467, 0x0469, 0x046B, 0x046D, 0x046F, 0x0471, 0x0473, 0x0475, 0x0477,
	0x0479, 0x047B, 0x047D, 0x047F, 0x0481, 0x0491, 0x0493, 0x0495, 0x0497,
	0x0499, 0x049B, 0x049D, 0x049F, 0x04A1, 0x04A3, 0x04A5, 0x04A7, 0x04A9,
	0x04AB, 0x04AD, 0x04AF, 0x04B1, 0x04B3, 0x04B5, 0x04B7, 0x04B9, 0x04BB,
	0x04BD, 0x04BF, 0x04C2, 0x04C4, 0x04C8, 0x04CC, 0x04D1, 0x04D3, 0x04D5,
	0x04D7, 0x04D9, 0x04DB, 0x04DD, 0x04DF, 0x04E1, 0x04E3, 0x04E5, 0x04E7,
	0x04E9, 0x04EB, 0x04EF, 0x04F1, 0x04F3, 0x04F5, 0x04F9, 0x0561, 0x0562,
	0x0563, 0x0564, 0x0565, 0x0566, 0x0567, 0x0568, 0x0569, 0x056A, 0x056B,
	0x056C, 0x056D, 0x056E, 0x056F, 0x0570, 0x0571, 0x0572, 0x0573, 0x0574,
	0x0575, 0x0576, 0x0577, 0x0578, 0x0579, 0x057A, 0x057B, 0x057C, 0x057D,
	0x057E, 0x057F, 0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586,
	0x10D0, 0x10D1, 0x10D2, 0x10D3, 0x10D4, 0x10D5, 0x10D6, 0x10D7, 0x10D8,
	0x10D9, 0x10DA, 0x10DB, 0x10DC, 0x10DD, 0x10DE, 0x10DF, 0x10E0, 0x10E1,
	0x10E2, 0x10E3, 0x10E4, 0x10E5, 0x10E6, 0x10E7, 0x10E8, 0x10E9, 0x10EA,
	0x10EB, 0x10EC, 0x10ED, 0x10EE, 0x10EF, 0x10F0, 0x10F1, 0x10F2, 0x10F3,
	0x10F4, 0x10F5, 0x1E01, 0x1E03, 0x1E05, 0x1E07, 0x1E09, 0x1E0B, 0x1E0D,
	0x1E0F, 0x1E11, 0x1E13, 0x1E15, 0x1E17, 0x1E19, 0x1E1B, 0x1E1D, 0x1E1F,
	0x1E21, 0x1E23, 0x1E25, 0x1E27, 0x1E29, 0x1E2B, 0x1E2D, 0x1E2F, 0x1E31,
	0x1E33, 0x1E35, 0x1E37, 0x1E39, 0x1E3B, 0x1E3D, 0x1E3F, 0x1E41, 0x1E43,
	0x1E45, 0x1E47, 0x1E49, 0x1E4B, 0x1E4D, 0x1E4F, 0x1E51, 0x1E53, 0x1E55,
	0x1E57, 0x1E59, 0x1E5B, 0x1E5D, 0x1E5F, 0x1E61, 0x1E63, 0x1E65, 0x1E67,
	0x1E69, 0x1E6B, 0x1E6D, 0x1E6F, 0x1E71, 0x1E73, 0x1E75, 0x1E77, 0x1E79,
	0x1E7B, 0x1E7D, 0x1E7F, 0x1E81, 0x1E83, 0x1E85, 0x1E87, 0x1E89, 0x1E8B,
	0x1E8D, 0x1E8F, 0x1E91, 0x1E93, 0x1E95, 0x1EA1, 0x1EA3, 0x1EA5, 0x1EA7,
	0x1EA9, 0x1EAB, 0x1EAD, 0x1EAF, 0x1EB1, 0x1EB3, 0x1EB5, 0x1EB7, 0x1EB9,
	0x1EBB, 0x1EBD, 0x1EBF, 0x1EC1, 0x1EC3, 0x1EC5, 0x1EC7, 0x1EC9, 0x1ECB,
	0x1ECD, 0x1ECF, 0x1ED1, 0x1ED3, 0x1ED5, 0x1ED7, 0x1ED9, 0x1EDB, 0x1EDD,
	0x1EDF, 0x1EE1, 0x1EE3, 0x1EE5, 0x1EE7, 0x1EE9, 0x1EEB, 0x1EED, 0x1EEF,
	0x1EF1, 0x1EF3, 0x1EF5, 0x1EF7, 0x1EF9, 0x1F00, 0x1F01, 0x1F02, 0x1F03,
	0x1F04, 0x1F05, 0x1F06, 0x1F07, 0x1F10, 0x1F11, 0x1F12, 0x1F13, 0x1F14,
	0x1F15, 0x1F20, 0x1F21, 0x1F22, 0x1F23, 0x1F24, 0x1F25, 0x1F26, 0x1F27,
	0x1F30, 0x1F31, 0x1F32, 0x1F33, 0x1F34, 0x1F35, 0x1F36, 0x1F37, 0x1F40,
	0x1F41, 0x1F42, 0x1F43, 0x1F44, 0x1F45, 0x1F51, 0x1F53, 0x1F55, 0x1F57,
	0x1F60, 0x1F61, 0x1F62, 0x1F63, 0x1F64, 0x1F65, 0x1F66, 0x1F67, 0x1F80,
	0x1F81, 0x1F82, 0x1F83, 0x1F84, 0x1F85, 0x1F86, 0x1F87, 0x1F90, 0x1F91,
	0x1F92, 0x1F93, 0x1F94, 0x1F95, 0x1F96, 0x1F97, 0x1FA0, 0x1FA1, 0x1FA2,
	0x1FA3, 0x1FA4, 0x1FA5, 0x1FA6, 0x1FA7, 0x1FB0, 0x1FB1, 0x1FD0, 0x1FD1,
	0x1FE0, 0x1FE1, 0x24D0, 0x24D1, 0x24D2, 0x24D3, 0x24D4, 0x24D5, 0x24D6,
	0x24D7, 0x24D8, 0x24D9, 0x24DA, 0x24DB, 0x24DC, 0x24DD, 0x24DE, 0x24DF,
	0x24E0, 0x24E1, 0x24E2, 0x24E3, 0x24E4, 0x24E5, 0x24E6, 0x24E7, 0x24E8,
	0x24E9, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48,
	0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 0xFF50, 0xFF51,
	0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A,
	0x0000 };

static const wchar_t uppers[] = {
	0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049,
	0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052,
	0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x00C0,
	0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9,
	0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1, 0x00D2,
	0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC,
	0x00DD, 0x00DE, 0x0178, 0x0100, 0x0102, 0x0104, 0x0106, 0x0108, 0x010A,
	0x010C, 0x010E, 0x0110, 0x0112, 0x0114, 0x0116, 0x0118, 0x011A, 0x011C,
	0x011E, 0x0120, 0x0122, 0x0124, 0x0126, 0x0128, 0x012A, 0x012C, 0x012E,
	0x0049, 0x0132, 0x0134, 0x0136, 0x0139, 0x013B, 0x013D, 0x013F, 0x0141,
	0x0143, 0x0145, 0x0147, 0x014A, 0x014C, 0x014E, 0x0150, 0x0152, 0x0154,
	0x0156, 0x0158, 0x015A, 0x015C, 0x015E, 0x0160, 0x0162, 0x0164, 0x0166,
	0x0168, 0x016A, 0x016C, 0x016E, 0x0170, 0x0172, 0x0174, 0x0176, 0x0179,
	0x017B, 0x017D, 0x0182, 0x0184, 0x0187, 0x018B, 0x0191, 0x0198, 0x01A0,
	0x01A2, 0x01A4, 0x01A7, 0x01AC, 0x01AF, 0x01B3, 0x01B5, 0x01B8, 0x01BC,
	0x01C4, 0x01C7, 0x01CA, 0x01CD, 0x01CF, 0x01D1, 0x01D3, 0x01D5, 0x01D7,
	0x01D9, 0x01DB, 0x01DE, 0x01E0, 0x01E2, 0x01E4, 0x01E6, 0x01E8, 0x01EA,
	0x01EC, 0x01EE, 0x01F1, 0x01F4, 0x01FA, 0x01FC, 0x01FE, 0x0200, 0x0202,
	0x0204, 0x0206, 0x0208, 0x020A, 0x020C, 0x020E, 0x0210, 0x0212, 0x0214,
	0x0216, 0x0181, 0x0186, 0x018A, 0x018E, 0x018F, 0x0190, 0x0193, 0x0194,
	0x0197, 0x0196, 0x019C, 0x019D, 0x019F, 0x01A9, 0x01AE, 0x01B1, 0x01B2,
	0x01B7, 0x0386, 0x0388, 0x0389, 0x038A, 0x0391, 0x0392, 0x0393, 0x0394,
	0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D,
	0x039E, 0x039F, 0x03A0, 0x03A1, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
	0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x038C, 0x038E, 0x038F, 0x03E2, 0x03E4,
	0x03E6, 0x03E8, 0x03EA, 0x03EC, 0x03EE, 0x0410, 0x0411, 0x0412, 0x0413,
	0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C,
	0x041D, 0x041E, 0x041F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425,
	0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E,
	0x042F, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408,
	0x0409, 0x040A, 0x040B, 0x040C, 0x040E, 0x040F, 0x0460, 0x0462, 0x0464,
	0x0466, 0x0468, 0x046A, 0x046C, 0x046E, 0x0470, 0x0472, 0x0474, 0x0476,
	0x0478, 0x047A, 0x047C, 0x047E, 0x0480, 0x0490, 0x0492, 0x0494, 0x0496,
	0x0498, 0x049A, 0x049C, 0x049E, 0x04A0, 0x04A2, 0x04A4, 0x04A6, 0x04A8,
	0x04AA, 0x04AC, 0x04AE, 0x04B0, 0x04B2, 0x04B4, 0x04B6, 0x04B8, 0x04BA,
	0x04BC, 0x04BE, 0x04C1, 0x04C3, 0x04C7, 0x04CB, 0x04D0, 0x04D2, 0x04D4,
	0x04D6, 0x04D8, 0x04DA, 0x04DC, 0x04DE, 0x04E0, 0x04E2, 0x04E4, 0x04E6,
	0x04E8, 0x04EA, 0x04EE, 0x04F0, 0x04F2, 0x04F4, 0x04F8, 0x0531, 0x0532,
	0x0533, 0x0534, 0x0535, 0x0536, 0x0537, 0x0538, 0x0539, 0x053A, 0x053B,
	0x053C, 0x053D, 0x053E, 0x053F, 0x0540, 0x0541, 0x0542, 0x0543, 0x0544,
	0x0545, 0x0546, 0x0547, 0x0548, 0x0549, 0x054A, 0x054B, 0x054C, 0x054D,
	0x054E, 0x054F, 0x0550, 0x0551, 0x0552, 0x0553, 0x0554, 0x0555, 0x0556,
	0x10A0, 0x10A1, 0x10A2, 0x10A3, 0x10A4, 0x10A5, 0x10A6, 0x10A7, 0x10A8,
	0x10A9, 0x10AA, 0x10AB, 0x10AC, 0x10AD, 0x10AE, 0x10AF, 0x10B0, 0x10B1,
	0x10B2, 0x10B3, 0x10B4, 0x10B5, 0x10B6, 0x10B7, 0x10B8, 0x10B9, 0x10BA,
	0x10BB, 0x10BC, 0x10BD, 0x10BE, 0x10BF, 0x10C0, 0x10C1, 0x10C2, 0x10C3,
	0x10C4, 0x10C5, 0x1E00, 0x1E02, 0x1E04, 0x1E06, 0x1E08, 0x1E0A, 0x1E0C,
	0x1E0E, 0x1E10, 0x1E12, 0x1E14, 0x1E16, 0x1E18, 0x1E1A, 0x1E1C, 0x1E1E,
	0x1E20, 0x1E22, 0x1E24, 0x1E26, 0x1E28, 0x1E2A, 0x1E2C, 0x1E2E, 0x1E30,
	0x1E32, 0x1E34, 0x1E36, 0x1E38, 0x1E3A, 0x1E3C, 0x1E3E, 0x1E40, 0x1E42,
	0x1E44, 0x1E46, 0x1E48, 0x1E4A, 0x1E4C, 0x1E4E, 0x1E50, 0x1E52, 0x1E54,
	0x1E56, 0x1E58, 0x1E5A, 0x1E5C, 0x1E5E, 0x1E60, 0x1E62, 0x1E64, 0x1E66,
	0x1E68, 0x1E6A, 0x1E6C, 0x1E6E, 0x1E70, 0x1E72, 0x1E74, 0x1E76, 0x1E78,
	0x1E7A, 0x1E7C, 0x1E7E, 0x1E80, 0x1E82, 0x1E84, 0x1E86, 0x1E88, 0x1E8A,
	0x1E8C, 0x1E8E, 0x1E90, 0x1E92, 0x1E94, 0x1EA0, 0x1EA2, 0x1EA4, 0x1EA6,
	0x1EA8, 0x1EAA, 0x1EAC, 0x1EAE, 0x1EB0, 0x1EB2, 0x1EB4, 0x1EB6, 0x1EB8,
	0x1EBA, 0x1EBC, 0x1EBE, 0x1EC0, 0x1EC2, 0x1EC4, 0x1EC6, 0x1EC8, 0x1ECA,
	0x1ECC, 0x1ECE, 0x1ED0, 0x1ED2, 0x1ED4, 0x1ED6, 0x1ED8, 0x1EDA, 0x1EDC,
	0x1EDE, 0x1EE0, 0x1EE2, 0x1EE4, 0x1EE6, 0x1EE8, 0x1EEA, 0x1EEC, 0x1EEE,
	0x1EF0, 0x1EF2, 0x1EF4, 0x1EF6, 0x1EF8, 0x1F08, 0x1F09, 0x1F0A, 0x1F0B,
	0x1F0C, 0x1F0D, 0x1F0E, 0x1F0F, 0x1F18, 0x1F19, 0x1F1A, 0x1F1B, 0x1F1C,
	0x1F1D, 0x1F28, 0x1F29, 0x1F2A, 0x1F2B, 0x1F2C, 0x1F2D, 0x1F2E, 0x1F2F,
	0x1F38, 0x1F39, 0x1F3A, 0x1F3B, 0x1F3C, 0x1F3D, 0x1F3E, 0x1F3F, 0x1F48,
	0x1F49, 0x1F4A, 0x1F4B, 0x1F4C, 0x1F4D, 0x1F59, 0x1F5B, 0x1F5D, 0x1F5F,
	0x1F68, 0x1F69, 0x1F6A, 0x1F6B, 0x1F6C, 0x1F6D, 0x1F6E, 0x1F6F, 0x1F88,
	0x1F89, 0x1F8A, 0x1F8B, 0x1F8C, 0x1F8D, 0x1F8E, 0x1F8F, 0x1F98, 0x1F99,
	0x1F9A, 0x1F9B, 0x1F9C, 0x1F9D, 0x1F9E, 0x1F9F, 0x1FA8, 0x1FA9, 0x1FAA,
	0x1FAB, 0x1FAC, 0x1FAD, 0x1FAE, 0x1FAF, 0x1FB8, 0x1FB9, 0x1FD8, 0x1FD9,
	0x1FE8, 0x1FE9, 0x24B6, 0x24B7, 0x24B8, 0x24B9, 0x24BA, 0x24BB, 0x24BC,
	0x24BD, 0x24BE, 0x24BF, 0x24C0, 0x24C1, 0x24C2, 0x24C3, 0x24C4, 0x24C5,
	0x24C6, 0x24C7, 0x24C8, 0x24C9, 0x24CA, 0x24CB, 0x24CC, 0x24CD, 0x24CE,
	0x24CF, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 0xFF28,
	0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F, 0xFF30, 0xFF31,
	0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A,
	0x0000 };

wchar_t ObjectDataClass::tolowerUnic(wchar_t c)
{
	const wchar_t* p;

	if ((p = wcschr(uppers, c)) != NULL)
		return *(lowers + (p - uppers));
	return c;
}

wchar_t ObjectDataClass::toupperUnic(wchar_t c)
{
	const wchar_t* p;

	if ((p = wcschr(lowers, c)) != NULL)
		return *(uppers + (p - lowers));
	return c;
}

void ObjectDataClass::tolowerUnicStr(wchar_t* p)
{
	while (*p)
		*p++ = tolowerUnic(*p);
}

void ObjectDataClass::toupperUnicStr(wchar_t* p)
{
	while (*p)
		*p++ = toupperUnic(*p);
}

ObjectDataClass* g_ObjectData = NULL;
int		ObjectDataClass::RelationCount = 0;
map<string, MoneyExchangeKeyValueStruct>	ObjectDataClass::_MoneyExchangeKeyValue;
vector<HouseEnergyCostStruct>			ObjectDataClass::_HouseEnergyCost;
vector<wstring>							ObjectDataClass::_SpecialName;
int										ObjectDataClass::m_iDataLanguageID = 0;
int										ObjectDataClass::m_iStringLanguageID = 0;
map< string, string >					ObjectDataClass::_DBStringMap;		//營運商改的字串表
map< string, int >						ObjectDataClass::_LanguageNameIDMap;
map< string, int >						ObjectDataClass::_CountryNameIDMap;
string									ObjectDataClass::_ErrorStr;
bool									ObjectDataClass::_LoadAll;
HANDLE									ObjectDataClass::m_ghShareMemory = NULL;
//----------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
void ObjectDataClass::InitReader()
{

	memset((char*)&_Reader_Base, 0, sizeof(_Reader_Base));
	memset((char*)&_Reader_Item, 0, sizeof(_Reader_Item));

	_Reader_Item[EM_Item_Normal] = GameObjDbStructEx::ObjReader_Item();
	_Reader_Item[EM_Item_Weapon] = GameObjDbStructEx::ObjReader_Weapon();
	_Reader_Item[EM_Item_Armor] = GameObjDbStructEx::ObjReader_Armor();
	_Reader_Item[EM_Item_Rune] = GameObjDbStructEx::ObjReader_Rune();
	_Reader_Item[EM_Item_Card] = GameObjDbStructEx::ObjReader_Card();

	_Reader_Base[EM_ObjectClass_Player] = GameObjDbStructEx::ObjReader_Player();
	_Reader_Base[EM_ObjectClass_NPC] = GameObjDbStructEx::ObjReader_NPC();
	_Reader_Base[EM_ObjectClass_MagicBase] = GameObjDbStructEx::ObjReader_MagicBase();
	_Reader_Base[EM_ObjectClass_BodyObj] = GameObjDbStructEx::ObjReader_Body();
	_Reader_Base[EM_ObjectClass_Attribute] = GameObjDbStructEx::ObjReader_AddPower();
	_Reader_Base[EM_ObjectClass_QuestDetail] = GameObjDbStructEx::ObjReader_QuestDetail();
	_Reader_Base[EM_ObjectClass_Title] = GameObjDbStructEx::ObjReader_Title();
	_Reader_Base[EM_ObjectClass_KeyItem] = GameObjDbStructEx::ObjReader_KeyItem();
	_Reader_Base[EM_ObjectClass_Recipe] = GameObjDbStructEx::ObjReader_Recipe();
	_Reader_Base[EM_ObjectClass_Mine] = GameObjDbStructEx::ObjReader_Mine();
	_Reader_Base[EM_ObjectClass_Flag] = GameObjDbStructEx::ObjReader_Flag();
	_Reader_Base[EM_ObjectClass_Image] = GameObjDbStructEx::ObjReader_Image();
	_Reader_Base[EM_ObjectClass_QuestNPC] = GameObjDbStructEx::ObjReader_QuestNPC();
	_Reader_Base[EM_ObjectClass_LearnMagic] = GameObjDbStructEx::ObjReader_LearnMagic();
	_Reader_Base[EM_ObjectClass_Shop] = GameObjDbStructEx::ObjReader_Shop();
	_Reader_Base[EM_ObjectClass_Suit] = GameObjDbStructEx::ObjReader_Suit();
	_Reader_Base[EM_ObjectClass_Camp] = GameObjDbStructEx::ObjReader_Camp();
	_Reader_Base[EM_ObjectClass_Treasure] = GameObjDbStructEx::ObjReader_Treasure();
	_Reader_Base[EM_ObjectClass_MagicCollect] = GameObjDbStructEx::ObjReader_MagicCollect();
	_Reader_Base[EM_ObjectClass_EqRefineAbility] = GameObjDbStructEx::ObjReader_EqRefineAbility();
	_Reader_Base[EM_ObjectClass_Zone] = GameObjDbStructEx::ObjReader_Zone();
	_Reader_Base[EM_ObjectClass_CreateRole] = GameObjDbStructEx::ObjReader_CreateRole();
	_Reader_Base[EM_ObjectClass_PE] = GameObjDbStructEx::ObjReader_PE();
	_Reader_Base[EM_ObjectClass_Phantom] = GameObjDbStructEx::ObjReader_Phantom();
}
//////////////////////////////////////////////////////////////////////////
bool ObjectDataClass::LoadTableFile()
{
	char FilePath[512];
	char Buf[512];
	int  i;
	//-------------------------------------------------------------------------------------------------------
	char* LoadTable[] = { "FixTable.Db" , "RaceTable.DB" , "SexTable.DB" , "VocTable.DB" , "ExpTable.DB" , "SkillBaseTable.DB"
		, "JobSkill.DB" , "JobEqSkillTable.DB" , "WeaponArgTable.DB", "ArmorArgTable.DB"
		, "BaseMagicList.DB" /*, "ResistArgTable.DB"*/ , "ExpTable.DB" , /*"EqRefine.DB" ,*/ "CreateRole.DB"
		/*, "DissolutionTable.DB"*/ , "ComboTable.DB" , "MotionTable.DB" , "TeachTable.DB" , "ColoringShopTable.DB"
		, "WorldMapTable.DB" /*, "SysColorDefineTable.DB"*/ , "GuildLvInfo.DB"
		, "SysKeyFunction.DB" , "LotteryInfoTable.DB" , "SysKeyValue.DB" , "InherentRate.DB" , "HouseShopInfoTable.DB"
		, "PowerRateTable.DB" , "RentFunctionTable.DB" , "GuildFlagTable.DB"
		, "MapPoint.DB" , "RelationMagic.DB" , "RuneExchangeEx.DB" , "GuildBuilding.DB" , "GuildBaseBuilding.DB"
		, "GuildHouseItemPageCost.DB", "CultivatePetLifeSkill.DB" , "CultivatePetLearnMagic.DB" , /*"PlantGrowAction.DB", */"PlantProduct.DB"
		, "GambleItemState.DB" ,"ServantEvent.DB" , "ServantHire.DB" , "ServantStartValue.DB" , "ItemCombine.DB" , "LifeSkillRareEx.DB"
		, "GuildHouseNpcReplace.DB" , "GuildHouseNpcReplaceRightCost.DB" , "PetCollection.DB"
		, "PhantomSkill.DB" , "PhantomLv.DB" , "PhantomRank.DB" , "AbilityParameters.DB" ,  NULL };


	ObjectDataFileClass<LoadTableStruct, RuStreamFile> ObjectTB;

	ObjectTB.SetOutPutFunc(Print);

	for (i = 0; LoadTable[i] != NULL; i++)
	{
		sprintf(FilePath, "data\\%s", LoadTable[i]);
		bool Ret = (TRUE == g_ruResourceManager->FileExists(FilePath));

		if (Ret != false)
		{
			ObjectTB.OpenFile(FilePath, EM_ObjectDataLoadType_LoadAll_NoFree, NULL);
		}
		else
		{
			Print(LV5, "ObjectDataClass", "[Init] LoadFile failed = %s\n", FilePath);
			sprintf(Buf, "ObjectDataClass [Init] LoadFile failed = %s", FilePath);

#ifndef _CLIENT
			PrintToController(true, Buf);
#else
			MessageBox(NULL, Buf, "", MB_OK);
#endif
			_OKFlag = false;
			_ErrorStr = Buf;
			return false;
		}

	}

	Print(LV2, "ObjectDataClass", "[Init] Loading table\n", Buf); fflush(NULL);

	//載入Table表
	//    char* TempPoint;
	LoadTableStruct* TempTable;

	TempTable = ObjectTB.GetObj(Def_ObjectClass_Table + _TABLE_BASE_ARMORARGTABLE_);
	if (TempTable)
	{
		memcpy(RoleDataEx::ArmorArg, TempTable->Data, sizeof(RoleDataEx::ArmorArg));
	}


	for (i = 0; i < _MAX_RACE_COUNT_; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table);

		if (TempTable == NULL)
			break;

		RoleValueData::RaceArg[i] = TempTable->ArgTB;
	}

	for (i = 0; i < _MAX_VOCATION_COUNT_ * 2; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_VOCTABLE_);

		if (TempTable == NULL)
			break;

		RoleValueData::VocArg[i / 2][i % 2] = TempTable->ArgTB;
	}

	for (i = 0; i < _MAX_SEX_COUNT_; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_SEXTABLE_);

		if (TempTable == NULL)
			break;

		RoleValueData::SexArg[i] = TempTable->ArgTB;
	}


	for (int i = 0; i < 2; i++)
	{
		TempTable = ObjectTB.GetObj(Def_ObjectClass_Table + _TABLE_BASE_FIXTABLE_ + i);

		if (TempTable != NULL)
		{
			RoleValueData::FixArgTable[i] = TempTable->FixTB;
		}
	}

	for (i = 0; i < _MAX_LEVEL_; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_SKILLBASETABLE_);

		if (TempTable == NULL)
			break;

		memcpy(RoleValueData::SkillValueBase[i], TempTable->SkillValueBase.SkillValueBase, sizeof(RoleValueData::SkillValueBase[i]));
	}

	for (i = 0; i < _MAX_SKILLVALUE_TYPE; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_JOBSKILLTABLE_);

		if (TempTable == NULL)
			break;

		memcpy(RoleValueData::JobSkillValue[i], TempTable->JobSkillValue.JobSkillValue, sizeof(RoleValueData::JobSkillValue[i]));
		//		RoleValueData::WeaponBaseDelayTime[ i ] = TempTable->DelayTime;
	}

	TempTable = ObjectTB.GetObj(Def_ObjectClass_Table + _TABLE_BASE_JOBEQSKILLTABLE_);

	if (TempTable != NULL)
	{
		for (i = 0; i < _MAX_VOCATION_COUNT_; i++)
			RoleValueData::JobWearSkill[i]._Skill = TempTable->JobWearSkill.JobWearSkill[i]._Skill;
	}


	TempTable = ObjectTB.GetObj(Def_ObjectClass_Table + _TABLE_BASE_WEAPONARGTABLE_);

	if (TempTable != NULL)
	{
		for (i = 0; i < _MAX_SKILLVALUE_TYPE; i++)
		{
			RoleValueData::WeaponBaseDelayTime[i] = TempTable->WeaponBase.WeaponBaseDelayTime[i];
			RoleValueData::WeaponBaseDmg[i] = TempTable->WeaponBase.WeaponBaseDmg[i];
			RoleValueData::WeaponBaseRand[i] = TempTable->WeaponBase.WeaponBaseRand[i];
		}
	}
	//	裝備比例資料
	//	TempTable = ObjectTB.GetObj(  Def_ObjectClass_Table + 9000 );

	Print(LV2, "ObjectDataClass", "[Init] Loading spell list\n", Buf); fflush(NULL);
	//基本法術列表
	TempTable = ObjectTB.GetObj(Def_ObjectClass_Table + _TABLE_BASE_BASEMAGICTABLE_);

	if (TempTable != NULL)
	{
		memcpy(RoleDataEx::BaseMagicList, TempTable->BaseMagicList.BaseMagicList, sizeof(RoleDataEx::BaseMagicList));
	}

	Print(LV2, "ObjectDataClass", "[Init] Loading exp table\n", Buf); fflush(NULL);

	RoleDataEx::JobSkillExpTable.clear();
	RoleDataEx::JobExp_LvGrowRateTable.clear();
	RoleDataEx::RefineExpTable.clear();
	RoleDataEx::JobExpTable.clear();
	RoleDataEx::SkillExpTable.clear();
	RoleDataEx::GatherExpTable.clear();
	RoleDataEx::PlantExpTable.clear();
	RoleDataEx::PlantEffectPointTable.clear();
	RoleDataEx::PlotExpTable.clear();
	RoleDataEx::PlotMoneyTable.clear();
	RoleDataEx::HouseExpArgTable.clear();
	RoleDataEx::MPCostPowerRate.clear();
	RoleDataEx::PetExpTable.clear();
	RoleDataEx::PetLifeSkillTable.clear();

	RoleDataEx::MPCostPowerRate.push_back(1);
	//經驗值表
	for (i = 0; i < _MAX_LEVEL_; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_EXPTABLE_);

		if (TempTable == NULL)
			break;

		RoleDataEx::RefineExpTable.push_back(TempTable->AllExp.RefineExp);	//煉製的經驗值表

		int ValueExp = int(TempTable->AllExp.JobExp * TempTable->AllExp.JobExp_LvGrowRate);
		if (ValueExp < 1000)
			ValueExp = ValueExp / 10 * 10;
		else if (ValueExp < 10000)
			ValueExp = ValueExp / 100 * 100;
		else
			ValueExp = ValueExp / 1000 * 1000;

		RoleDataEx::JobExpTable.push_back(ValueExp);	//經驗值表
		RoleDataEx::SkillExpTable.push_back(TempTable->AllExp.SkillExp);	//技能經驗值表
		RoleDataEx::GatherExpTable.push_back(TempTable->AllExp.GatherExp);	//採集經驗值表
		RoleDataEx::PlotExpTable.push_back(TempTable->AllExp.PlotExp);	//劇情可獲得經驗值
		RoleDataEx::PlotMoneyTable.push_back(TempTable->AllExp.PlotMoney);	//劇情可獲得金錢
		RoleDataEx::JobExp_LvGrowRateTable.push_back(TempTable->AllExp.JobExp_LvGrowRate);
		RoleDataEx::JobSkillExpTable.push_back(TempTable->AllExp.JobMagicExp);
		RoleDataEx::HouseExpArgTable.push_back(TempTable->AllExp.HouseExpArg);
		RoleDataEx::MPCostPowerRate.push_back(TempTable->AllExp.MagicMPGrowRate);
		RoleDataEx::PlantExpTable.push_back(TempTable->AllExp.PlantExp);
		RoleDataEx::PlantEffectPointTable.push_back(TempTable->AllExp.PlantEffectPoint);

		RoleDataEx::PetExpTable.push_back(TempTable->AllExp.Pet_Exp);

		if (TempTable->AllExp.Pet_LifeSkill == 0)
			TempTable->AllExp.Pet_LifeSkill = 1;
		RoleDataEx::PetLifeSkillTable.push_back(TempTable->AllExp.Pet_LifeSkill);
	}


	//-------------------------------------------------------------------------------------------------------
	RoleValueData::CalBaseTB();
	//-------------------------------------------------------------------------------------------------------
	//創角資料
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading createrole table\n", Buf); fflush(NULL);

	TempTable = ObjectTB.GetObj(Def_ObjectClass_Table + _TABLE_BASE_CREATEROLETABLE_);

	_MainColor.numColors = 0;
	_SecondColor.numColors = 0;
	if (TempTable != NULL)
	{
		_CreateRoleTable = TempTable->CreateRole;

		memset(_SkinColor, 0, sizeof(_SkinColor));
		memset(_HairColor, 0, sizeof(_HairColor));

		for (int i = 0; i < _CreateRoleTable.ColorCount; i++)
		{
			/*
			switch (_CreateRoleTable.Color[i].Type)
			{
			case 0:
				_MainColor.color[_MainColor.numColors].r = _CreateRoleTable.Color[i].R;
				_MainColor.color[_MainColor.numColors].g = _CreateRoleTable.Color[i].G;
				_MainColor.color[_MainColor.numColors].b = _CreateRoleTable.Color[i].B;
				_MainColor.numColors++;
				break;

			case 1:
				_SecondColor.color[_SecondColor.numColors].r = _CreateRoleTable.Color[i].R;
				_SecondColor.color[_SecondColor.numColors].g = _CreateRoleTable.Color[i].G;
				_SecondColor.color[_SecondColor.numColors].b = _CreateRoleTable.Color[i].B;
				_SecondColor.numColors++;
				break;
			}
			*/

			switch (_CreateRoleTable.Color[i].Type)
			{
			case 0:
				if (_CreateRoleTable.Color[i].Race >= 0 && _CreateRoleTable.Color[i].Race < 4)
				{
					int index = _CreateRoleTable.Color[i].Race;

					_SkinColor[index].color[_SkinColor[index].numColors].r = _CreateRoleTable.Color[i].R;
					_SkinColor[index].color[_SkinColor[index].numColors].g = _CreateRoleTable.Color[i].G;
					_SkinColor[index].color[_SkinColor[index].numColors].b = _CreateRoleTable.Color[i].B;
					_SkinColor[index].numColors++;
				}
				break;

			case 1:
				if (_CreateRoleTable.Color[i].Race >= 0 && _CreateRoleTable.Color[i].Race < 4)
				{
					int index = _CreateRoleTable.Color[i].Race;

					_HairColor[index].color[_HairColor[index].numColors].r = _CreateRoleTable.Color[i].R;
					_HairColor[index].color[_HairColor[index].numColors].g = _CreateRoleTable.Color[i].G;
					_HairColor[index].color[_HairColor[index].numColors].b = _CreateRoleTable.Color[i].B;
					_HairColor[index].numColors++;
				}
				break;
			}
		}

		_Hair.clear();
		_Face.clear();
		for (int i = 0; i < _CreateRoleTable.BodyCount; i++)
		{
			switch (_CreateRoleTable.Body[i].Type)
			{
				// 臉型
			case 0:
				_Face.push_back(_CreateRoleTable.Body[i].OrgID);
				break;

				// 頭髮
			case 1:
				_Hair.push_back(_CreateRoleTable.Body[i].OrgID);
				break;
			}
		}

		_CreateHair.clear();
		_CreateFace.clear();
		for (int i = 0; i < _CreateRoleTable.BodyCount; i++)
		{
			if (_CreateRoleTable.Body[i].Enabled == false)
				continue;

			switch (_CreateRoleTable.Body[i].Type)
			{
				// 臉型
			case 0:
				_CreateFace.push_back(_CreateRoleTable.Body[i].OrgID);
				break;

				// 頭髮
			case 1:
				_CreateHair.push_back(_CreateRoleTable.Body[i].OrgID);
				break;
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------
	//連攜表
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading ComboInfo table\n", Buf); fflush(NULL);
	RoleDataEx::ComboInfo.clear();

	//經驗值表	
	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_COMBOTABLETABLE_);

		if (TempTable == NULL)
			break;

		RoleDataEx::ComboInfo.push_back(TempTable->Combo);
	}
	//-------------------------------------------------------------------------------------------------------
	//讀取Motion資料
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading Motion table\n", Buf); fflush(NULL);
	_MotionInfo.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_MOTIONTABLE_);

		if (TempTable == NULL)
			break;

		_MotionInfo.push_back(TempTable->Motion);
	}
	//-------------------------------------------------------------------------------------------------------
	//教學
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading Teach table\n", Buf); fflush(NULL);
	_TeachInfo.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_TEACHTABLE_);

		if (TempTable == NULL)
			break;

		_TeachInfo.push_back(TempTable->Teach);
	}
	//-------------------------------------------------------------------------------------------------------
	//ColoringShop
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading ColoringShop table\n", Buf); fflush(NULL);
	_ColoringShop.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_COLORINGSHOPTABLE_);

		if (TempTable == NULL)
			break;

		if (TempTable->ColoringShop.Cost < 0)
			TempTable->ColoringShop.Cost = 100000;

		_ColoringShop.push_back(TempTable->ColoringShop);
	}
	//-------------------------------------------------------------------------------------------------------
	//MiniMapInfo
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading _WorldMapInfo table\n", Buf); fflush(NULL);
	_WorldMapInfo.clear();
	_WofldMapNameMap.clear();
	_WofldMapIDMap.clear();
	_WorldMapInfoTables.clear();
	_IsWorldMapInfoInited = false;

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_WORLDMAPINFO_);

		if (TempTable == NULL)
			break;

		_WorldMapInfo.push_back(TempTable->WorldMapInfo);
	}

	//-------------------------------------------------------------------------------------------------------
	//GuildShop
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading_GuildLv table\n", Buf); fflush(NULL);
	_GuildLvTable.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_GUILDLVINFO_);

		if (TempTable == NULL)
			break;


		_GuildLvTable.push_back(TempTable->GuildLvInfo);
	}
	//-------------------------------------------------------------------------------------------------------
	//AccountShop
	//-------------------------------------------------------------------------------------------------------
	// TODO:: Not use
	//Print(LV2, "ObjectDataClass", "[Init] Loading _AccountShop table\n", Buf); fflush(NULL);
	//_AccountShop.clear();

	//for (int i = 0; ; i++)
	//{
	//	TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_ACCOUNTSHOPINFO_);

	//	if (TempTable == NULL)
	//		break;


	//	_AccountShop.push_back(TempTable->AccountShopInfo);
	//}
	//-------------------------------------------------------------------------------------------------------
	//SysKeyFunction
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading _SysKeyFunction table\n", Buf); fflush(NULL);
	_SysKeyFunction.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_SYSKEYFUNCTION_);

		if (TempTable == NULL)
			break;

		_SysKeyFunction[TempTable->SysKeyFunction.KeyItemID] = TempTable->SysKeyFunction;
	}
	//-------------------------------------------------------------------------------------------------------
	//LotteryInfo
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading _LotteryInfoTable table\n", Buf); fflush(NULL);
	_LotteryInfoTable.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_LOTTERYINFO_);

		if (TempTable == NULL)
			break;

		_LotteryInfoTable.push_back(TempTable->LotteryInfoTable);
	}
	//-------------------------------------------------------------------------------------------------------
	//SysKeyValue
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading _SysKeyValeMap table\n", Buf); fflush(NULL);
	_SysKeyValeMap.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_SYSKEYVALUE_);

		if (TempTable == NULL)
			break;

		_SysKeyValeMap[TempTable->SysKeyValue.KeyStr] = TempTable->SysKeyValue.Value;
	}
	//-------------------------------------------------------------------------------------------------------
	//InherentRateTable
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading _InherentRateTable table\n", Buf); fflush(NULL);
	_InherentRateTable.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_INHERENTRATE_);

		if (TempTable == NULL)
			break;

		_InherentRateTable.push_back(TempTable->InheretRate);
	}
	//-------------------------------------------------------------------------------------------------------
	//HouseShop
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading _HouseShop table\n", Buf); fflush(NULL);
	_HouseShop.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_HOUSESHOPINFO_);

		if (TempTable == NULL)
			break;


		_HouseShop.push_back(TempTable->HouseShopInfo);
	}
	//-------------------------------------------------------------------------------------------------------
	//能量換算能力加乘表
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading _PowerRate table\n", Buf); fflush(NULL);
	_PowerRate.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_POWERRATE_);

		if (TempTable == NULL)
			break;

		_PowerRate.push_back(TempTable->PowerRate);
	}
	//-------------------------------------------------------------------------------------------------------
	//租用資訊
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading _RentFunctionTable table\n", Buf); fflush(NULL);
	_RentFunctionTable.clear();
	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_RENTFUNCTION_);

		if (TempTable == NULL)
			break;

		_RentFunctionTable.push_back(TempTable->RentFunction);
	}
	//-------------------------------------------------------------------------------------------------------
	//GuildFlag
	//-------------------------------------------------------------------------------------------------------
	//_GuildFlagTable.clear();
	_GuildFlagInsigniaTable.clear();
	_GuildFlagBannerTable.clear();
	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_GUIDFLAGTABLE_);

		if (TempTable == NULL)
			break;

		//_GuildFlagTable.push_back( TempTable->GuildFlagTable );

		if (TempTable->GuildFlagTable.Type == 1)
		{
			_GuildFlagInsigniaTable.push_back(TempTable->GuildFlagTable.Str);
		}
		else if (TempTable->GuildFlagTable.Type == 2)
		{
			_GuildFlagBannerTable.push_back(TempTable->GuildFlagTable.Str);
		}
	}
	//-------------------------------------------------------------------------------------------------------
	//MapPoint
	//-------------------------------------------------------------------------------------------------------
	_MapMapInfo.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_MAPPOINT_);

		if (TempTable == NULL)
			break;

		_MapMapInfo.push_back(TempTable->MapPoint);
	}
	//-------------------------------------------------------------------------------------------------------
	//RelationMagic
	//-------------------------------------------------------------------------------------------------------
	for (int i = 0; i < EM_RelationType_MaxRelation; i++)
	{
		_RelationMagic[i].clear();
	}

	RelationMagicStruct EmptyRelationInfo;
	EmptyRelationInfo.Init();
	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_RELATIONMAGIC_);

		if (TempTable == NULL)
			break;

		if ((unsigned)TempTable->RelationMagic.Relation >= EM_RelationType_MaxRelation)
			continue;
		if ((unsigned)TempTable->RelationMagic.Lv > 100)
			continue;

		vector< RelationMagicStruct >& Vec = _RelationMagic[TempTable->RelationMagic.Relation];

		while ((int)Vec.size() <= TempTable->RelationMagic.Lv)
		{
			Vec.push_back(EmptyRelationInfo);
		}
		Vec[TempTable->RelationMagic.Lv] = TempTable->RelationMagic;
	}
	//-------------------------------------------------------------------------------------------------------
	//公會建築物
	//-------------------------------------------------------------------------------------------------------
	_GuildBuildingTable.clear();
	_BaseGuildBuildingInfo.clear();
	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_GUIDBUILDING_);

		if (TempTable == NULL)
			break;

		_GuildBuildingTable[TempTable->GuildBuilding.ID] = TempTable->GuildBuilding;
	}
	//-------------------------------------------------------------------------------------------------------
	//讀取一開始 公會建築就要有的建築物
	//-------------------------------------------------------------------------------------------------------
	_GuildBaseBuildingTable.clear();

	for (int i = 0; ; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_GUIDBASEBUILDING_);

		if (TempTable == NULL)
			break;
		_GuildBaseBuildingTable.push_back(TempTable->GuildBaseBuilding);
	}
	//-------------------------------------------------------------------------------------------------------
	//讀取公會物品頁價格表
	//-------------------------------------------------------------------------------------------------------
	{
		GuildHouseItemPageCostTableStruct TempData;
		TempData.Init();
		_GuildHousePageCostTable.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_GUIDHOUSEITEMPAGE_);
			if (TempTable == NULL)
				break;

			while ((int)_GuildHousePageCostTable.size() <= TempTable->GuildHouseItemPageCost.ItemPageID)
				_GuildHousePageCostTable.push_back(TempData);

			_GuildHousePageCostTable[TempTable->GuildHouseItemPageCost.ItemPageID] = TempTable->GuildHouseItemPageCost;
		}

	}
	//-------------------------------------------------------------------------------------------------------
	//工會樣式置換資訊
	//-------------------------------------------------------------------------------------------------------
	{

		_GuildHouseNpcReplace.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_GUILDHOUSE_NPC_REPLACE);
			if (TempTable == NULL)
				break;

			GuildHouseNpcReplaceTableStruct& TempData = TempTable->GuildHouseNpcReplace;


			_GuildHouseNpcReplace[TempData.Type][TempData.OrgNpcID] = TempData.NpcID;
		}
	}
	//-------------------------------------------------------------------------------------------------------
	//工會樣式購買
	//-------------------------------------------------------------------------------------------------------
	{

		_GuildHouseNpcReplaceCost.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_GUILDHOUSE_NPC_REPLACE_COST);
			if (TempTable == NULL)
				break;
			GuildHouseNpcReplaceCostTableStruct& TempData = TempTable->GuildHouseNpcReplaceCost;

			_GuildHouseNpcReplaceCost[TempData.Type] = TempData;
		}
	}
	//-------------------------------------------------------------------------------------------------------
	//雕文系統
	//-------------------------------------------------------------------------------------------------------
	{/*
		_RuneSystemTable.clear();
		for( int i = 0 ; ; i++ )
		{
			TempTable = ObjectTB.GetObj( i + Def_ObjectClass_Table + _TABLE_BASE_RUNESYSTEM_EVENT_ );
			if( TempTable == NULL )
				break;
			RuneSystemStruct& TempData=TempTable->RuneSystem;
			_RuneSystemTable.push_back( TempData );
		}*/
	}
	//-------------------------------------------------------------------------------------------------------
	//讀取符文兌換表
	//-------------------------------------------------------------------------------------------------------
	/*
	for( int i = 0 ; i < 50 ; i++ )
	{
		TempTable = ObjectTB.GetObj( i + Def_ObjectClass_Table + _TABLE_BASE_RUNEEXCHANGE_ );

		if( TempTable == NULL )
			break;
		memcpy( _RuneExchangeTable[ i ] , TempTable->RuneExchangeTable.ExchangeTable , sizeof(TempTable->RuneExchangeTable.ExchangeTable) );
	}
	*/
	_RuneExchangeTable.clear();
	for (int i = 0; i < 1000; i++)
	{
		TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_RUNEEXCHANGE_);

		if (TempTable == NULL)
			break;
		int KeyValue;
		if (TempTable->RuneExchangeExTable.Type1 > TempTable->RuneExchangeExTable.Type2)
		{
			KeyValue = TempTable->RuneExchangeExTable.Type1 * 10000 + TempTable->RuneExchangeExTable.Type2;
		}
		else
		{
			KeyValue = TempTable->RuneExchangeExTable.Type2 * 10000 + TempTable->RuneExchangeExTable.Type1;
		}
		_RuneExchangeTable[KeyValue] = TempTable->RuneExchangeExTable.ResultType;
	}
	//////////////////////////////////////////////////////////////////////////
	//養成寵物資料
	//////////////////////////////////////////////////////////////////////////
	{
		_CultivatePetLearnMagicTable.clear();

		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_CULTIVATE_PET_LEARNMAGIC_);

			if (TempTable == NULL)
				break;
			_CultivatePetLearnMagicTable.push_back(TempTable->CultivatePetLearnMagic);
		}
		sort(_CultivatePetLearnMagicTable.begin(), _CultivatePetLearnMagicTable.end());
	}
	//////////////////////////////////////////////////////////////////////////
	{
		_CultivatePetLifeSkillTable.clear();

		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_CULTIVATE_PET_LIFESKILL_);

			if (TempTable == NULL)
				break;
			_CultivatePetLifeSkillTable.push_back(TempTable->CultivatePetLifeSkill);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//種植資料
	//////////////////////////////////////////////////////////////////////////
	/*
	{
		vector< PlantGrowActionTableStruct > TempVector;
		_PlantGrowActionTable.clear();

		for( int i = 0 ; ; i++ )
		{
			TempTable = ObjectTB.GetObj( i + Def_ObjectClass_Table + _TABLE_BASE_PLANT_GROW_ACTION_ );

			if( TempTable == NULL )
				break;

			while( (int)_PlantGrowActionTable.size() <= TempTable->PlantGrowAction.Type )
				_PlantGrowActionTable.push_back( TempVector );
			TempTable->PlantGrowAction.ID = (int)_PlantGrowActionTable[ TempTable->PlantGrowAction.Type ].size();
			_PlantGrowActionTable[ TempTable->PlantGrowAction.Type ].push_back( TempTable->PlantGrowAction );
			//_PlantGrowActionTable.push_back( TempTable->PlantGrowAction );
		}
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	{
		_PlantProductTable.clear();

		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_PLANT_PRODUCT_);

			if (TempTable == NULL)
				break;
			_PlantProductTable[TempTable->PlantProduct.SeedObjID].push_back(TempTable->PlantProduct);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//裝備屬性賭博表
	{
		_GambleItemStateTable.clear();

		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_PLANT_GAMBLEITEMSTATE_);

			if (TempTable == NULL)
				break;
			_GambleItemStateTable.push_back(TempTable->GambleItemState);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//女僕資料讀取
	//////////////////////////////////////////////////////////////////////////
	{
		_ServantEventList.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_SERVANT_EVENT_);

			if (TempTable == NULL)
				break;
			_ServantEventList.push_back(TempTable->ServantEvent);
		}
	}

	{
		_ServantHire_Character.clear();
		_ServantHire_FaceID.clear();
		_ServantHire_HairID.clear();
		_ServantHire_SkinColor.clear();
		_ServantHire_HairColor.clear();
		_ServantHire_ClothColor1.clear();
		_ServantHire_ClothColor2.clear();

		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_SERVANT_HIRE_);

			if (TempTable == NULL)
				break;
			if (TempTable->ServantHire.FaceID > 0)
				_ServantHire_FaceID[TempTable->ServantHire.NPCObjID].push_back(TempTable->ServantHire.FaceID);
			if (TempTable->ServantHire.HairID > 0)
				_ServantHire_HairID[TempTable->ServantHire.NPCObjID].push_back(TempTable->ServantHire.HairID);
			if (TempTable->ServantHire.SkinColor > 0)
				_ServantHire_SkinColor[TempTable->ServantHire.NPCObjID].push_back(TempTable->ServantHire.SkinColor);
			if (TempTable->ServantHire.HairColor > 0)
				_ServantHire_HairColor[TempTable->ServantHire.NPCObjID].push_back(TempTable->ServantHire.HairColor);
			if (TempTable->ServantHire.ClothColor1 > 0)
				_ServantHire_ClothColor1[TempTable->ServantHire.NPCObjID].push_back(TempTable->ServantHire.ClothColor1);
			if (TempTable->ServantHire.ClothColor2 > 0)
				_ServantHire_ClothColor2[TempTable->ServantHire.NPCObjID].push_back(TempTable->ServantHire.ClothColor2);
			if (TempTable->ServantHire.Character > 0)
				_ServantHire_Character[TempTable->ServantHire.NPCObjID].push_back(TempTable->ServantHire.Character);

			//			if( std::find( _ServantHire_NPCObjID.begin() , _ServantHire_NPCObjID.end() , TempTable->ServantHire.NPCObjID ) == _ServantHire_NPCObjID.end() )
			//				_ServantHire_NPCObjID.push_back( TempTable->ServantHire.NPCObjID );
		}


	}

	{
		_ServantHire_NPCObjID.clear();
		_ServantStartValueMap.clear();
		_ServantHire_LookNPCObjID.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_SERVANT_STRATVALUE_);

			if (TempTable == NULL)
				break;

			_ServantStartValueMap[TempTable->ServantStartValue.NPCObjID] = TempTable->ServantStartValue;
			_ServantHire_NPCObjID.push_back(TempTable->ServantStartValue.NPCObjID);
			_ServantHire_LookNPCObjID.push_back(TempTable->ServantStartValue.LookNpcObjID);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//合成物品資料
//map< int , vector< ItemCombineStruct > >		_ItemCombineList;
	{
		RoleDataEx::ItemCombineList.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_ITEM_COMBINE_);

			if (TempTable == NULL)
				break;

			ItemCombineStruct info = TempTable->ItemCombine;

			map< int, int > fixInfoMap;
			int x;
			for (x = 0; x < 5; x++)
			{
				if (info.SourceCount[x] == 0 || info.SourceID[x] == 0)
					continue;
				fixInfoMap[info.SourceID[x]] += info.SourceCount[x];
			}
			info.Init();

			map< int, int >::iterator iter;
			for (x = 0, iter = fixInfoMap.begin(); iter != fixInfoMap.end(); iter++, x++)
			{
				info.SourceCount[x] = iter->second;
				info.SourceID[x] = iter->first;
			}
			int hashID = GetHashCode(&info, sizeof(info));
			info.ProductCount = TempTable->ItemCombine.ProductCount;
			info.ProductID = TempTable->ItemCombine.ProductID;
			RoleDataEx::ItemCombineList[hashID].push_back(info);
		}

	}
	//////////////////////////////////////////////////////////////////////////
	//煉化 物品稀有度兌換表
	{
		_LifeSkillRareExMap.clear();

		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_LIFESKILL_RARE_EX_);

			if (TempTable == NULL)
				break;
			//_GambleItemStateTable.push_back( TempTable->GambleItemState );
			_LifeSkillRareExMap[TempTable->LifeSkillRareEx.ItemID] = TempTable->LifeSkillRareEx;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//寵物卡
	{
		_PetCardList.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_PETCOLLECTION_);

			if (TempTable == NULL)
				break;

			while (_PetCardList.size() <= TempTable->PetCard.GUID)
			{
				_PetCardList.push_back(0);
			}
			_PetCardList[TempTable->PetCard.GUID] = TempTable->PetCard.ObjID;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		RoleDataEx::PhantomAbiArg.clear();

		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_ABIPARAMETER_);

			if (TempTable == NULL)
				break;

			RoleDataEx::PhantomAbiArg[TempTable->PhantomAbi.GUID] = TempTable->PhantomAbi.Value;
		}

		RoleDataEx::PhantomRank.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_PHANTOMRANK_);

			if (TempTable == NULL)
				break;

			RoleDataEx::PhantomRank[TempTable->PhantomRank.Rank] = TempTable->PhantomRank;
		}

		RoleDataEx::PhantomLv.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_PHANTOMLV_);

			if (TempTable == NULL)
				break;

			RoleDataEx::PhantomLv[TempTable->PhantomLv.Type][TempTable->PhantomLv.LV] = TempTable->PhantomLv;
		}

		RoleDataEx::PhantomSkill.clear();
		for (int i = 0; ; i++)
		{
			TempTable = ObjectTB.GetObj(i + Def_ObjectClass_Table + _TABLE_BASE_PHANTOMSKILL_);

			if (TempTable == NULL)
				break;

			int id1 = TempTable->PhantomSkill.Phantom[0];
			int id2 = TempTable->PhantomSkill.Phantom[1];
			if (id1 < id2)
				swap(id1, id2);
			INT64 keyid = id1 + id2 * 0x100000000;
			RoleDataEx::PhantomSkill[keyid] = TempTable->PhantomSkill.SkillID;
		}

	}
	//////////////////////////////////////////////////////////////////////////
	return true;
	//////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////
void SetBodyObjectTable(vector<int>* pTable, GameObjDbStruct* objDB)
{
	if (pTable == NULL || objDB == NULL)
		return;

	// 人男
	if (objDB->BodyObj.Limit.Race.Race0 && objDB->BodyObj.Limit.Sex.Boy)
		pTable[0].push_back(objDB->GUID);

	// 人女
	if (objDB->BodyObj.Limit.Race.Race0 && objDB->BodyObj.Limit.Sex.Girl)
		pTable[1].push_back(objDB->GUID);

	// 精靈男
	if (objDB->BodyObj.Limit.Race.Race1 && objDB->BodyObj.Limit.Sex.Boy)
		pTable[2].push_back(objDB->GUID);

	// 精靈女
	if (objDB->BodyObj.Limit.Race.Race1 && objDB->BodyObj.Limit.Sex.Girl)
		pTable[3].push_back(objDB->GUID);

	// 矮人男
	if (objDB->BodyObj.Limit.Race.Race2 && objDB->BodyObj.Limit.Sex.Boy)
		pTable[4].push_back(objDB->GUID);
}

//////////////////////////////////////////////////////////////////////////
void ObjectDataClass::LoadBodyObject()
{
	GameObjDbStruct* objDB;
	int offset = 2000;

	// 初始化
	for (int i = 0; i < 6; i++)
	{
		_CreateRoleHair[i].clear();
		_CreateRoleFace[i].clear();
		_BeautyShopHair[i].clear();
		_BeautyShopFace[i].clear();
	}

	// 搜尋前1000筆物件資料
	for (int i = 0; i < 1000; i++)
	{
		if ((objDB = GetObj(offset + i)) != NULL)
		{
			// 創角用
			if (objDB->BodyObj.Limit.CreateRole)
			{
				switch (objDB->BodyObj.Type)
				{
				case EM_BodyObjectType_Hair:
					SetBodyObjectTable(_CreateRoleHair, objDB);
					break;

				case EM_BodyObjectType_Face:
					SetBodyObjectTable(_CreateRoleFace, objDB);
					break;
				}
			}

			// 造型用
			if (objDB->BodyObj.Limit.DepartmentStore)
			{
				switch (objDB->BodyObj.Type)
				{
				case EM_BodyObjectType_Hair:
					SetBodyObjectTable(_BeautyShopHair, objDB);
					break;

				case EM_BodyObjectType_Face:
					SetBodyObjectTable(_BeautyShopFace, objDB);
					break;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool ObjectDataClass::LoadStringFile()
{
	char	Buf[512];
	char	FilePath[512];
	int		StringCount = 0;
	//-------------------------------------------------------------------------------------------------------
	// 讀取字串表
	//-------------------------------------------------------------------------------------------------------
	if (_Stringlanguage.size() != 0)
	{
		Print(LV2, "ObjectDataClass", "[Init] Loading String table\n", Buf); fflush(NULL);

		fstream file1;
		sprintf(FilePath, "data\\string_%s.db", ObjectDataClass::QueryLanguageString(GetStringLanguageID()).c_str());

		//FileName = CombineFilePath( InitPath , szFileName );

		//file1.open( FileName , ios::in | ios::binary );

		bool Ret = (TRUE == g_ruResourceManager->FileExists(FilePath));


		//if( Refile1.is_open() == false )
		if (Ret == false)
		{
			//MessageBoxA( NULL ,  "String 開檔失敗" , "警告" , MB_OK );			
#ifndef _CLIENT
			PrintToController(true, "Open string file error, file='%s'", FilePath);
#else
			MessageBox(NULL, "Open string file error", "", MB_OK);
#endif
			_OKFlag = false;
			_ErrorStr = "Open string file error";
			return false;

		}
		else
		{
			StaticString<64>	KeyStr;
			int					ContentSize;
			char				Content[8192];

			int					iStreamSize = 0;
			char* ptr = NULL;

			IRuStream* pStream = g_ruResourceManager->LoadStream(FilePath);

			//stream->GetStreamSize()

			if (pStream)
			{
				iStreamSize = pStream->GetStreamSize();
				ptr = (char*)pStream->OpenStreamMapping();

				while (iStreamSize > 0)
				{
					//ptr = 
					memcpy((char*)KeyStr.Begin(), ptr, sizeof(KeyStr));
					ptr += sizeof(KeyStr);
					iStreamSize -= sizeof(KeyStr);

					memcpy((char*)&ContentSize, ptr, sizeof(ContentSize));
					ptr += sizeof(ContentSize);
					iStreamSize -= sizeof(ContentSize);

					memcpy((char*)Content, ptr, ContentSize);
					ptr += ContentSize;
					iStreamSize -= ContentSize;

					char* temp = Content;
					char* end = Content + ContentSize - 1;
					while (temp <= end)
					{
						char* space = (char*)strstr(temp, "\\32");
						if (space)
						{
							space[0] = ' ';
							memmove(space + 1, space + 3, end - space);
							temp = space + 1;
							ContentSize -= 2;
						}
						else
							break;
					}

					temp = Content;
					while (temp <= end)
					{
						char* space = (char*)strstr(temp, "\\n");
						if (space)
						{
							space[0] = '\n';
							memmove(space + 1, space + 2, end - space);
							temp = space + 1;
							ContentSize -= 1;
						}
						else
							break;
					}

					if (ContentSize > 1)
					{
						//						_StringMap[ KeyStr.Begin() ] = Content;
						StringCount++;

						for (int i = 0; i <= KeyStr.Size(); i++)
						{
							_AllString.push_back(KeyStr[i]);
						}

						for (int i = 0; i <= strlen(Content); i++)
						{
							_AllString.push_back(Content[i]);
						}
					}
				}
				delete pStream;

				for (unsigned int i = 0; i < _AllString.size(); )
				{
					char* Key = (&_AllString[i]);
					//					strupr( Key );
					i += ((int)strlen(Key) + 1);
					char* Content = (&_AllString[i]);
					i += ((int)strlen(Content) + 1);
					_StringMap[Key] = Content;
				}
			}
			else
			{

#ifndef _CLIENT
				PrintToController(true, "Open string file error, file='%s'", FilePath);
#else
				MessageBox(NULL, "Open string file error", "", MB_OK);
#endif
				_OKFlag = false;
				_ErrorStr = "Open string file error";
				return false;

			}
		}

	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
struct ObjectDataLoadFileStruct
{
	char* File;
	ObjectDataLoadTypeENUM CliLoadType;
	ReaderClass< DB_GameObjDbStruct >* Reader;
};
bool ObjectDataClass::LoadObjectFile()
{
#ifdef _CLIENT
	ObjectDataLoadFileStruct LoadFile[] =
	{
		{ "ImageObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Image()				},
		{ "AddPowerObject.DB"		, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_AddPower() 			},
		{ "ArmorObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Armor()				},
		{ "BodyObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Body()				},
		{ "CampObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Camp()				},
		{ "CreateRoleObject.DB"		, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_CreateRole()			},
		{ "CardObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Card()				},
		{ "EqRefineAbilityObject.DB", EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_EqRefineAbility()	},
		{ "FlagObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Flag()				},
		{ "ItemObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Item()				},
		{ "KeyItemObject.DB"		, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_KeyItem()			},
		{ "LearnMagic.DB"			, EM_ObjectDataLoadType_LoadAll			, GameObjDbStructEx::ObjReader_LearnMagic()			},
		{ "MagicObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_MagicBase()			},
		{ "MagicCollectObject.DB"	, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_MagicCollect()		},
		{ "MineObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Mine()				},
		{ "NPCObject.DB"			, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_NPC()				},
		{ "PlayerObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Player()				},
		{ "QuestDetailObject.DB"	, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_QuestDetail()		},
		{ "QuestNPCObject.DB"		, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_QuestNPC()			},
		{ "RecipeObject.DB"			, EM_ObjectDataLoadType_None			, GameObjDbStructEx::ObjReader_Recipe()				},
		{ "RuneObject.DB"			, EM_ObjectDataLoadType_LoadAll			, GameObjDbStructEx::ObjReader_Rune()				},
		{ "ShopObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Shop()				},
		{ "SuitObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Suit()				},
		{ "TitleObject.DB"			, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_Title()				},
		{ "TreasureObject.DB"		, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Treasure()			},
		{ "WeaponObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Weapon()				},
		{ "ZoneObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Zone()				},
		{ "PhantomObject.DB"		, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Phantom()			},
		{ NULL						, EM_ObjectDataLoadType_LoadAll_NoFree	, NULL }
	};
#else//Server模式
	ObjectDataLoadFileStruct LoadFile[] =
	{
		{ "ImageObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Image()				},
		{ "AddPowerObject.DB"		, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_AddPower() 			},
		{ "ArmorObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Armor()				},
		{ "BodyObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Body()				},
		{ "CampObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Camp()				},
		{ "CreateRoleObject.DB"		, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_CreateRole()			},
		{ "CardObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Card()				},
		{ "EqRefineAbilityObject.DB", EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_EqRefineAbility()	},
		{ "FlagObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Flag()				},
		{ "ItemObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Item()				},
		{ "KeyItemObject.DB"		, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_KeyItem()			},
		{ "LearnMagic.DB"			, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_LearnMagic()			},
		{ "MagicObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_MagicBase()			},
		{ "MagicCollectObject.DB"	, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_MagicCollect()		},
		{ "MineObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Mine()				},
		{ "NPCObject.DB"			, EM_ObjectDataLoadType_None			, GameObjDbStructEx::ObjReader_NPC()				},
		{ "PlayerObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Player()				},
		{ "QuestDetailObject.DB"	, EM_ObjectDataLoadType_None			, GameObjDbStructEx::ObjReader_QuestDetail()		},
		{ "QuestNPCObject.DB"		, EM_ObjectDataLoadType_None			, GameObjDbStructEx::ObjReader_QuestNPC()			},
		{ "RecipeObject.DB"			, EM_ObjectDataLoadType_None			, GameObjDbStructEx::ObjReader_Recipe()				},
		{ "RuneObject.DB"			, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_Rune()				},
		{ "ShopObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Shop()				},
		{ "SuitObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Suit()				},
		{ "TitleObject.DB"			, EM_ObjectDataLoadType_LoadAll_NoFree	, GameObjDbStructEx::ObjReader_Title()				},
		{ "TreasureObject.DB"		, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Treasure()			},
		{ "WeaponObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Weapon()				},
		{ "ZoneObject.DB"			, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Zone()				},
		{ "PhantomObject.DB"		, EM_ObjectDataLoadType_None 			, GameObjDbStructEx::ObjReader_Phantom()			},
		{ NULL						, EM_ObjectDataLoadType_None 			, NULL }
	};
#endif


	bool    Ret;
	int     i;
	char	Buf[4096];
	char	FilePath[4096];
	_OKFlag = true;

	g_ruResourceManager->LoadPackageFile("fdb\\data.fdb");

	for (i = 0; LoadFile[i].File != NULL; i++)
	{
		sprintf(FilePath, "data\\%s", LoadFile[i].File);


		_ObjectDB->SetOutPutFunc(Print);

		Ret = (g_ruResourceManager->FileExists(FilePath) != false);

		if (Ret != false)
		{
			if (_LoadAll != false)
				_ObjectDB->OpenFile(FilePath, EM_ObjectDataLoadType_LoadAll_NoFree, LoadFile[i].Reader->ReadInfo());
			else
				_ObjectDB->OpenFile(FilePath, LoadFile[i].CliLoadType, LoadFile[i].Reader->ReadInfo());
		}
		else
		{
			Print(LV5, "ObjectDataClass", "[Init] LoadFile failed = %s\n", FilePath); fflush(NULL);

			sprintf(Buf, "ObjectDataClass [Init] LoadFile failed = %s", FilePath);
#ifndef _CLIENT 
			PrintToController(true, Buf);
#else
			MessageBoxA(NULL, Buf, "", MB_OK);
#endif
			_OKFlag = false;
			_ErrorStr = Buf;
			return false;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool ObjectDataClass::LoadDisableTagFile()
{
	char	Buf[512];
	char	FilePath[512];
	int		StringCount = 0;
	//-------------------------------------------------------------------------------------------------------
	// 讀取字串表
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Loading Disable Tag File\n", Buf); fflush(NULL);

	sprintf(FilePath, "data\\DisableTag_%s.Ini", ObjectDataClass::QueryCountryString(GetDataLanguageID()).c_str());

	bool Ret = (TRUE == g_ruResourceManager->FileExists(FilePath));

	if (Ret == false)
		return false;


	IRuStream* pStream = g_ruResourceManager->LoadStream(FilePath);

	if (pStream == NULL)
		return false;

	char* pData = (char*)pStream->OpenStreamMapping();
	int		iStreamSize = pStream->GetStreamSize();

	char* pNewData = pData;

	_DisableTag.clear();
	for (int i = 0; i < iStreamSize; i++)
	{
		int ID = atol(pNewData);

		if (ID > 1000)
		{
			Print(LV5, "ObjectDataClass", "LoadDisableTagFile Error ID > 1000");
			return false;
		}

		while (ID >= _DisableTag.size())
		{
			_DisableTag.push_back(false);
		}
		if (ID != 0)
		{
			_DisableTag[ID] = true;
			char	Buf[256];
			sprintf(Buf, "DIR_TAGID_%03d", ID);
			Print(LV5, "LoadDisableTagFile", " TagID=%d %s", ID, Utf8ToChar(GetString(Buf)).c_str());
		}

		for (; i < iStreamSize; i++)
		{
			if (pData[i] == '\n')
			{
				pNewData = pData + i + 1;
				break;
			}
		}
	}

	delete pStream;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void ObjectDataClass::InitSysValue()
{
	_SysValue.Init();
	for (int i = 0; i < 10; i++)
	{
		char temp[256];
		sprintf(temp, "GlobalItemDrop%d", i);
		int GlobalDropItemID = GetSysKeyValue(temp);
		if (GlobalDropItemID == 0)
			break;

		_SysValue.GlobalItemDrop.push_back(GlobalDropItemID);
	}

	for (int i = 0; i < 10; i++)
	{
		char temp[256];
		sprintf(temp, "GlobalItemDrop_BigMonster_%d", i);
		int GlobalDropItemID = GetSysKeyValue(temp);
		if (GlobalDropItemID == 0)
			break;

		_SysValue.GlobalItemDrop_BigMonster.push_back(GlobalDropItemID);
	}

	for (int i = 0; i < 10; i++)
	{
		char temp[256];
		sprintf(temp, "GlobalItemDrop_King_%d", i);
		int GlobalDropItemID = GetSysKeyValue(temp);
		if (GlobalDropItemID == 0)
			break;

		_SysValue.GlobalItemDrop_King.push_back(GlobalDropItemID);
	}

	for (int i = 0; i < 10; i++)
	{
		char temp[256];
		sprintf(temp, "GlobalItemDrop_WorldKing_%d", i);
		int GlobalDropItemID = GetSysKeyValue(temp);
		if (GlobalDropItemID == 0)
			break;

		_SysValue.GlobalItemDrop_WorldKing.push_back(GlobalDropItemID);
	}

	for (int i = 0; i < 10; i++)
	{
		char temp[256];
		sprintf(temp, "GlobalItemDrop_DepartmentStore_%d", i);
		int GlobalDropItemID = GetSysKeyValue(temp);
		if (GlobalDropItemID == 0)
			break;

		_SysValue.GlobalItemDrop_DepartmentStore.push_back(GlobalDropItemID);
	}


	_SysValue.Money_GoldObjID = GetSysKeyValue("Money_GoldObjID");
	_SysValue.Money_AccountObjID = GetSysKeyValue("Money_AccountObjID");
	_SysValue.Money_BonusObjID = GetSysKeyValue("Money_BonusObjID");
	//設定金錢物件資料
	{
		GameObjDbStructEx* MoneyObj;
		MoneyObj = GetObj(_SysValue.Money_GoldObjID);
		if (MoneyObj != NULL)
		{
			MoneyObj->ACSearchType[0] = _DEF_ACSEARCHTYPE_MONEY;
			MoneyObj->ACSearchType[1] = _DEF_ACSEARCHTYPE_MONEY_GAMEMONEY;
			MoneyObj->ACSearchType[2] = _DEF_ACSEARCHTYPE_EMPTY;
		}
		MoneyObj = GetObj(_SysValue.Money_AccountObjID);
		if (MoneyObj != NULL)
		{
			MoneyObj->ACSearchType[0] = _DEF_ACSEARCHTYPE_MONEY;
			MoneyObj->ACSearchType[1] = _DEF_ACSEARCHTYPE_MONEY_ACCOUNTMONEY;
			MoneyObj->ACSearchType[2] = _DEF_ACSEARCHTYPE_EMPTY;
		}
		MoneyObj = GetObj(_SysValue.Money_BonusObjID);
		if (MoneyObj != NULL)
		{
			MoneyObj->ACSearchType[0] = _DEF_ACSEARCHTYPE_MONEY;
			MoneyObj->ACSearchType[1] = _DEF_ACSEARCHTYPE_MONEY_BONUSMONEY;
			MoneyObj->ACSearchType[2] = _DEF_ACSEARCHTYPE_EMPTY;
		}

	}


	_SysValue.TreasureBaseObjID = GetSysKeyValue("TreasureBaseObjID");
	_SysValue.PK_Revive_ZoneID = GetSysKeyValue("PK_Revive_ZoneID");
	_SysValue.PK_Revive_X = (float)GetSysKeyValue("PK_Revive_X");
	_SysValue.PK_Revive_Y = (float)GetSysKeyValue("PK_Revive_Y");
	_SysValue.PK_Revive_Z = (float)GetSysKeyValue("PK_Revive_Z");
	_SysValue.PK_ProtectEq_ItemID = GetSysKeyValue("PK_ProtectEq_ItemID");

	_SysValue.MagicStoneCount = GetSysKeyValue("MagicStoneCount");;			//魔石種類
	_SysValue.MagicStoneStart = GetSysKeyValue("MagicStoneStart");;			//魔石開始位置
	_SysValue.Teleport_PointObjID = GetSysKeyValue("Teleport_PointObjID");		//魔石開始位置
	_SysValue.HouseBuff = GetSysKeyValue("HouseBuff");				//魔石開始位置
	_SysValue.Pet_PopItemID = GetSysKeyValue("Pet_PopItemID");
	_SysValue.Pet_ReTrainItemID = GetSysKeyValue("Pet_ReTrainItemID");

	_SysValue.Plant.FruitsExp = GetSysKeyValue("PlantExp_Fruits");
	_SysValue.Plant.FeedPointDec = GetSysKeyValue("Plant_FeedPointDec");
	//	_SysValue.PlantExp.MagicWater	= GetSysKeyValue( "PlantExp_MagicWater" );
	//	_SysValue.PlantExp.Topdress		= GetSysKeyValue( "PlantExp_Topdress" );
	//	_SysValue.PlantExp.Water		= GetSysKeyValue( "PlantExp_Water" );


	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			char Buf[512];
			sprintf(Buf, "Dissolution_Item_Weapon_%d_%03d", i, j * 10 + 10);
			_SysValue.Dissolution.ItemWeapon[i][j] = GetSysKeyValue(Buf);
		}
	}

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			char Buf[512];
			sprintf(Buf, "Dissolution_Item_Armor_%d_%03d", i, j * 10 + 10);
			_SysValue.Dissolution.ItemArmor[i][j] = GetSysKeyValue(Buf);
		}
	}
	_SysValue.Dissolution.DLvClothes = GetSysKeyValue("Dissolution_DLv_Clothes");
	_SysValue.Dissolution.DLvPants = GetSysKeyValue("Dissolution_DLv_Other");
	_SysValue.Dissolution.DLvWeapon_OneHand = GetSysKeyValue("Dissolution_DLv_Weapon_One");
	_SysValue.Dissolution.DLvWeapon_TwoHand = GetSysKeyValue("Dissolution_DLv_Weapon_Two");
	_SysValue.Dissolution.DLvOther = GetSysKeyValue("Dissolution_DLv_Other");

	_SysValue.MagicBoxOpenKeyItemID = GetSysKeyValue("MagicBoxOpenKeyItemID");


	_SysValue.SysQuestColor_NPC = GetSysKeyValue("SYS_QUEST_COLORID_NPC") | 0xff000000;
	_SysValue.SysQuestColor_ITEM = GetSysKeyValue("SYS_QUEST_COLORID_ITEM") | 0xff000000;
	_SysValue.SysQuestColor_ZONE = GetSysKeyValue("SYS_QUEST_COLORID_ZONE") | 0xff000000;

	_SysValue.GoodEvilBuf[0] = GetSysKeyValue("SYS_GoodEvil_Buf_0");
	_SysValue.GoodEvilBuf[1] = GetSysKeyValue("SYS_GoodEvil_Buf_1");
	_SysValue.GoodEvilBuf[2] = GetSysKeyValue("SYS_GoodEvil_Buf_2");
	_SysValue.GoodEvilBuf[3] = GetSysKeyValue("SYS_GoodEvil_Buf_3");
	_SysValue.GoodEvilBuf[4] = GetSysKeyValue("SYS_GoodEvil_Buf_4");
	_SysValue.GoodEvilBuf[5] = GetSysKeyValue("SYS_GoodEvil_Buf_5");
	_SysValue.GoodEvilBuf[6] = GetSysKeyValue("SYS_GoodEvil_Buf_6");
	_SysValue.GoodEvilBuf[7] = GetSysKeyValue("SYS_GoodEvil_Buf_7");
	_SysValue.GoodEvilBuf[8] = GetSysKeyValue("SYS_GoodEvil_Buf_8");

	_SysValue.EarthPowerMagicID = GetSysKeyValue("EarthPowerMagicID");
	_SysValue.WaterPowerMagicID = GetSysKeyValue("WaterPowerMagicID");
	_SysValue.FirePowerMagicID = GetSysKeyValue("FirePowerMagicID");
	_SysValue.WindPowerMagicID = GetSysKeyValue("WindPowerMagicID");
	_SysValue.LightPowerMagicID = GetSysKeyValue("LightPowerMagicID");
	_SysValue.DarkPowerMagicID = GetSysKeyValue("DarkPowerMagicID");

	_SysValue.GamebleItemState_LockItemID = GetSysKeyValue("GamebleItemState_LockItemID");

	_SysValue.TombObjID = GetSysKeyValue("TombObjID");

	// 墓碑編號
	_tombID = GetSysKeyValue("TombObjID");


	RoleDataEx::FixArmorRate[EM_Armor_HeavyArmed] = GetSysKeyValue("FixArmorRate_HeavyArmed") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_LightArmed] = GetSysKeyValue("FixArmorRate_LightArmed") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Leather] = GetSysKeyValue("FixArmorRate_Leather") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Clothes] = GetSysKeyValue("FixArmorRate_Clothes") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Robe] = GetSysKeyValue("FixArmorRate_Robe") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Shield] = GetSysKeyValue("FixArmorRate_Shield") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Implement] = GetSysKeyValue("FixArmorRate_Other") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Ornament] = GetSysKeyValue("FixArmorRate_Other") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Wood] = GetSysKeyValue("FixArmorRate_Other") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Stone] = GetSysKeyValue("FixArmorRate_Other") / 100.0f;
	RoleDataEx::FixArmorRate[EM_Armor_Iron] = GetSysKeyValue("FixArmorRate_Other") / 100.0f;


	for (int i = 0; i < 100; i++)
	{
		char Buf[256];
		sprintf(Buf, "MagicStoneExchange_Ability_%02d", i);
		int AbilityID = GetSysKeyValue(Buf);
		if (AbilityID == 0)
			break;
		_SysValue.MagicStoneShopAbilityList.push_back(AbilityID);
	}

	_SysValue.MagicStoneShopCostType[0] = (PriceTypeENUM)GetSysKeyValue("MagicStoneExchange_MoneyType1");
	_SysValue.MagicStoneShopCostType[1] = (PriceTypeENUM)GetSysKeyValue("MagicStoneExchange_MoneyType2");

	InitNotColdownJob();

	_SysValue.TitleSys_FamouseDesc_Min = float(GetSysKeyValue("TitleSys_FamouseDesc_Day")) / (60 * 24);

}
//////////////////////////////////////////////////////////////////////////
ObjectDataClass::ObjectDataClass(const char* InitPath, const char* datalanguage, const char* stringlanguage)
{
	_Datalanguage = datalanguage;
	_Stringlanguage = stringlanguage;

	//	_Datalanguage = strlwr( (char*)datalanguage.c_str() );
	//	_Stringlanguage = strlwr( (char*)stringlanguage.c_str() );


	_ObjectDB = NEW ObjectDataFileClass<GameObjDbStructEx, RuStreamFile>;
	_ObjectDB->SetInitFunc(St_InitObjFunc, this);
	char	Buf[512];
	ObjectDataClass::SetLanguageID(QueryCountryID(datalanguage), QueryLanguageID(stringlanguage));


	if (LoadObjectFile() == false)
		return;
	if (LoadTableFile() == false)
		return;
	if (LoadStringFile() == false)
		return;

	LoadDisableTagFile();

	InitReader();
	InitSysValue();
	//-------------------------------------------------------------------------------------------------------
	Print(LV2, "ObjectDataClass", "[Init] Setting other link\n", Buf);

	SetMagicLink();
	SetLifeSillKeyItem();
	SetMagicJobInfo();
	SetRuneLink();
	SetTitleLink();

	//InitQuestDetailTable();

	//載入臉型髮型
	LoadBodyObject();


	Print(LV2, "ObjectDataClass", "[Init] Linked\n", Buf);
	//-------------------------------------------------------------------------------------------------------
	// 取得點數商品項目
	//-------------------------------------------------------------------------------------------------------
	{

		char Key[128];
		const char* pType;
		int count = 1;
		while (true)
		{
			sprintf(Key, "SYS_ACCOUNTSTORE_TYPE_%02d", count);

			pType = GetString(Key);

			if (pType[0] == '\0')
				break;

			_AccountStoreTypeName.push_back(pType);

			++count;
		}

	}

	// 取得TIPS數量
	_Tips.clear();
	{
		for (int i = 1; i < 100; i++)
		{
			char keyStr[256];
			sprintf_s(keyStr, 256, "TIPS_TEXT%02d", i);
			if (CheckedString(keyStr))
				_Tips.push_back(i);
		}

	}

#ifdef _CLIENT

	memset(_BaseEQ, 0, sizeof(_BaseEQ));
	for (int i = 0; i < 100; i++)
	{
		GameObjDbStructEx* obj = GetObj(i + Def_ObjectClass_CreateRole);
		if (obj)
		{
			// 特定國家顯示外型不同
			if (stricmp(_Datalanguage.c_str(), "tw") == 0 || stricmp(_Datalanguage.c_str(), "cn") == 0)
				obj->CreateRole.EQ = obj->CreateRole.EQPreview;

			for (int j = 0; j < EM_Max_Race; j++)
			{
				if (obj->CreateRole.RaceLimit._Race & (0x01 << j))
				{
					_BaseEQ[j][obj->CreateRole.Job] = i + Def_ObjectClass_CreateRole;
				}
			}
		}
	}

#endif

	Print(LV2, "ObjectDataClass", "[Init] ObjectDataClass Loaded\n", Buf); fflush(NULL);

	//載入測試資料
	//NS_TestData::Init( _ObjectDB );



}
//----------------------------------------------------------------------------------------------------
int	ObjectDataClass::GetMonetItemID(PriceTypeENUM type)
{
	int itemID = 0;
	switch (type)
	{
	case EM_ACPriceType_GameMoney:
		itemID = GetSysKeyValue("Money_GoldObjID");
		break;

	case EM_ACPriceType_AccountMoney:
		itemID = GetSysKeyValue("Money_AccountObjID");
		break;

	case EM_ACPriceType_BonusMoney:
		itemID = GetSysKeyValue("Money_BonusObjID");
		break;
	}
	return itemID;
}

//----------------------------------------------------------------------------------------------------
bool ObjectDataClass::GetItemFieldName(ItemFieldStruct* item, char* outStr, int outSize)
{
	if (item == NULL || outStr == NULL)
		return false;

	GameObjDbStructEx* itemDB = GetObj(item->OrgObjID);
	if (itemDB == NULL)
		return false;

	string tmp;

	// 金幣
	if (item->OrgObjID == GetMonetItemID(EM_ACPriceType_GameMoney))
	{
		char temp[512];

		// 加入分隔符號
		InsertComma(item->Money, temp, 512);

		// 回傳字串
		sprintf_s(outStr, outSize, "%s %s", temp, GetString("SYS_GOLD"));

		return true;
	}
	// 符文幣
	else if (item->OrgObjID == GetMonetItemID(EM_ACPriceType_AccountMoney))
	{
		char temp[512];

		// 加入分隔符號
		InsertComma(item->Money, temp, 512);

		// 回傳字串
		sprintf_s(outStr, outSize, "%s %s", temp, GetString("SYS_MONEY_RUNE"));

		return true;
	}
	else
	{
		/*
		if ( item->Inherent != 0 && itemDB->Item.IsNoTitle == false )
		{
		GameObjDbStruct* inherit = St_GetObjDB(item->Inherent);
		if ( inherit && inherit->Classification == EM_ObjectClass_Attribute )
		{
		tmp = inherit->Name;
		}
		}
		*/

		tmp += itemDB->GetName();
		MyStrcpy(outStr, tmp.c_str(), outSize - 1);
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------------------
bool ObjectDataClass::GetItemFieldTradeState(ItemFieldStruct* item)
{
	if (item == NULL || item->IsEmpty())
		return false;

	bool lock = false;
	bool trade = false;

	if (item->Mode._Mode == -1)
	{
		GameObjDbStructEx* OrgDB = g_ObjectData->GetObj(item->OrgObjID);
		if (OrgDB != NULL)
		{
			trade = !(OrgDB->Mode.PickupBound);
		}
	}
	else
	{
		lock = item->Mode.ItemLock;
		trade = item->Mode.Trade;
	}

	if (lock || !trade)
		return false;

	return true;
}


ObjectDataClass::~ObjectDataClass()
{
	delete _ObjectDB;
}
//----------------------------------------------------------------------------------------------------
GameObjDbStructEx* ObjectDataClass::GetObj(int ObjID)
{
	g_CritSect()->Enter();

	GameObjDbStructEx* Ret = _ObjectDB->GetObj(ObjID);

	if (Ret)
	{
		if (Ret->iTagID < _DisableTag.size() && _DisableTag[Ret->iTagID] != false)
		{
			Ret = NULL;
		}
		else if (ObjectDataClass::GetDataLanguageID() != EM_LanguageType_RW)
		{
			int iLanguageBit = 1 << ObjectDataClass::GetDataLanguageID();

			if ((Ret->LanguageSet & iLanguageBit) == 0)
				Ret = NULL;

		}
	}

	g_CritSect()->Leave();

	return Ret;
}
//----------------------------------------------------------------------------------------------------
#define DF_ZONESHAREMEMORY		"RoMZoneShareMemory"
bool    ObjectDataClass::InitShareMemory()
{
	bool	bInit = false;
	char* pszBuff = new char[268435456];
	//char	szBuff[268435456];
	memset(pszBuff, 1, 268435456);

	if (m_ghShareMemory == NULL)
	{
		m_ghShareMemory = CreateMutex(NULL, false, "RoMZoneShareMemoryMutex");

		if (m_ghShareMemory == NULL)
		{
#ifndef _CLIENT
			PrintToController(true, "InitShareMemory() CreateMutex() Error");
#else
			MessageBox(NULL, "InitShareMemory() CreateMutex() Error", "", MB_OK);
#endif
			return true; // before is return true, don't know why, so did not change it.
						 // "pszBuff" maybe leak if go this way, no one delete it.
		}
	}

	WaitForSingleObject(m_ghShareMemory, INFINITE);

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, DF_ZONESHAREMEMORY);

	if (hFileMapping == NULL)
	{
		bInit = true;
		hFileMapping = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 268435456, DF_ZONESHAREMEMORY);
	}

	char* ptr = (char*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (bInit != false)
	{
		memcpy(ptr, pszBuff, 268435456);
	}
	else
	{
		int Code = ptr[0] + 1;
		if (Code > 10)
			Code = 1;
		memset(ptr, Code, 268435456);
	}

	delete[] pszBuff;

	ReleaseMutex(m_ghShareMemory);

	return true; // not sure return false or true is correct - add by slime 2010/08/03
}
//----------------------------------------------------------------------------------------------------
bool    ObjectDataClass::Init(const char* InitPath, const char* datalanguage, const char* stringlanguage, bool LoadAll)
{
	_LoadAll = LoadAll;
	_LanguageNameIDMap.clear();
	_LanguageNameIDMap["RW"] = EM_LanguageType_RW;
	_LanguageNameIDMap["TW"] = EM_LanguageType_TW;
	_LanguageNameIDMap["CN"] = EM_LanguageType_CN;
	_LanguageNameIDMap["ENUS"] = EM_LanguageType_ENUS;
	_LanguageNameIDMap["DE"] = EM_LanguageType_DE;
	_LanguageNameIDMap["ENEU"] = EM_LanguageType_ENEU;
	_LanguageNameIDMap["JP"] = EM_LanguageType_JP;
	_LanguageNameIDMap["KR"] = EM_LanguageType_KR;
	_LanguageNameIDMap["BR"] = EM_LanguageType_BR;
	_LanguageNameIDMap["TR"] = EM_LanguageType_TR;
	_LanguageNameIDMap["RU"] = EM_LanguageType_RU;
	_LanguageNameIDMap["FR"] = EM_LanguageType_FR;
	_LanguageNameIDMap["VN"] = EM_LanguageType_VN;
	//	_LanguageNameIDMap[ "SG" ]		= EM_LanguageType_SG;
	_LanguageNameIDMap["ID"] = EM_LanguageType_ID;
	_LanguageNameIDMap["ES"] = EM_LanguageType_ES;
	//	_LanguageNameIDMap[ "NL" ]		= EM_LanguageType_NL;
	//	_LanguageNameIDMap[ "RO" ]		= EM_LanguageType_RO;
	_LanguageNameIDMap["PH"] = EM_LanguageType_PH;
	_LanguageNameIDMap["PL"] = EM_LanguageType_PL;
	_LanguageNameIDMap["TH"] = EM_LanguageType_TH;
	_LanguageNameIDMap["PT"] = EM_LanguageType_PT;
	_LanguageNameIDMap["AE"] = EM_LanguageType_AE;

	_LanguageNameIDMap["SAES"] = EM_LanguageType_SAES;
	_LanguageNameIDMap["ENAR"] = EM_LanguageType_ENAR;
	_LanguageNameIDMap["SAPT"] = EM_LanguageType_SAPT;
	_LanguageNameIDMap["IT"] = EM_LanguageType_IT;


	_CountryNameIDMap.clear();
	_CountryNameIDMap["RW"] = EM_CountryType_RW;
	_CountryNameIDMap["TW"] = EM_CountryType_TW;
	_CountryNameIDMap["CN"] = EM_CountryType_CN;
	_CountryNameIDMap["ENUS"] = EM_CountryType_ENUS;
	_CountryNameIDMap["DE"] = EM_CountryType_DE;
	_CountryNameIDMap["ENEU"] = EM_CountryType_ENEU;
	_CountryNameIDMap["JP"] = EM_CountryType_JP;
	_CountryNameIDMap["KR"] = EM_CountryType_KR;
	_CountryNameIDMap["SG"] = EM_CountryType_SG;
	_CountryNameIDMap["BR"] = EM_CountryType_BR;
	_CountryNameIDMap["TR"] = EM_CountryType_TR;
	_CountryNameIDMap["RU"] = EM_CountryType_RU;
	_CountryNameIDMap["FR"] = EM_CountryType_FR;
	_CountryNameIDMap["VN"] = EM_CountryType_VN;
	_CountryNameIDMap["SG"] = EM_CountryType_SG;
	_CountryNameIDMap["ID"] = EM_CountryType_ID;
	_CountryNameIDMap["ES"] = EM_CountryType_ES;
	_CountryNameIDMap["NL"] = EM_CountryType_NL;
	_CountryNameIDMap["RO"] = EM_CountryType_RO;
	_CountryNameIDMap["PH"] = EM_CountryType_PH;
	_CountryNameIDMap["PL"] = EM_CountryType_PL;
	_CountryNameIDMap["TH"] = EM_CountryType_TH;
	_CountryNameIDMap["PT"] = EM_CountryType_PT;
	_CountryNameIDMap["AE"] = EM_CountryType_AE;
	_CountryNameIDMap["SA"] = EM_CountryType_SA;
	_CountryNameIDMap["AUS"] = EM_CountryType_AUS;
	_CountryNameIDMap["IT"] = EM_CountryType_IT;

	ObjectDataClass* new_ObjectData = NEW ObjectDataClass(InitPath, datalanguage, stringlanguage);
	if (new_ObjectData->_OKFlag == false)
	{
		return false;
	}

	if (g_ObjectData != NULL)
	{
		delete g_ObjectData;
		g_ObjectData = NULL;
	}
	g_ObjectData = new_ObjectData;

	return true;
}
//----------------------------------------------------------------------------------------------------
bool    ObjectDataClass::Release()
{
	//	RelationCount--;
	//	if( RelationCount != 0 )
	//		return true;

	if (g_ObjectData == NULL)
		return false;

	delete g_ObjectData;
	g_ObjectData = NULL;

	return true;
}
//----------------------------------------------------------------------------------------------------
//物件資料
ObjectDataFileClass<GameObjDbStructEx, RuStreamFile>* ObjectDataClass::ObjectDB()
{
	return _ObjectDB;
}
//----------------------------------------------------------------------------------------------------
/*
void ObjectDataClass::SetImageObjLink( )
{
	static GameObjDbStruct	EmptyImageObj;
	GameObjDbStruct	*Obj;
	GameObjDbStruct	*ImageObj;

	memset( &EmptyImageObj , 0 , sizeof(EmptyImageObj) );

	_ObjectDB->GetDataByOrder( true );

	for( int i = 0 ; i < _ObjectDB->TotalInfoCount() ; i++ )
	{
		Obj = _ObjectDB->GetDataByOrder();

		ImageObj = (GameObjDbStruct*)_ObjectDB->GetObj( Obj->ImageID );
		if( ImageObj == NULL )
			Obj->ImageObj = &EmptyImageObj.Image;
		else
		{
			if ( ImageObj->Classification != EM_ObjectClass_Image ) {
#ifdef _CLIENT
				char buf[512];
				sprintf(buf, "[%d][%d]圖形資料錯誤", Obj->GUID, Obj->ImageID);
				MessageBoxA(NULL, "", buf, MB_OK);
#endif
				Obj->ImageObj = &EmptyImageObj.Image;
			} else {
				Obj->ImageObj = &ImageObj->Image;
			}

		}
	}
}
*/

//----------------------------------------------------------------------------------------------------
//void ObjectDataClass::InitQuestDetailTable()
//{
//	//_QuestDetailTable.clear();
//
//	GameObjDbStructEx* Obj;
//	_ObjectDB->GetDataByOrder( true );
//
//	int count = _ObjectDB->TotalInfoCount();
//	for( int i = 0 ; i < count ; ++i )
//	{
//		Obj = _ObjectDB->GetDataByOrder();
//
//		if( !Obj->IsQuestDetail() )
//			continue;
//
//		_QuestDetailTable.push_back( Obj );
//	}
//}

//----------------------------------------------------------------------------------------------------
void ObjectDataClass::SetMagicLink()
{
	/*
		GameObjDbStructEx	*Obj;

		//資料檢查
		_ObjectDB->GetDataByOrder( true );
		for( int i = 0 ; i < _ObjectDB->TotalInfoCount() ; i++ )
		{
			Obj = _ObjectDB->GetDataByOrder();

			if( !Obj->IsMagicCollect() )
				continue;
			for( int GroupID = 0 ; GroupID < _MAX_MAGICCOLLECT_GROUP_COUNT_ ; GroupID++ )
				for( int i = 0 ; i < _MAX_MAGICCOLLECT_COUNT_ ; i++ )
				{
					Obj->MagicCol.Magic[GroupID][i].MagicBaseDB = _ObjectDB->GetObj( Obj->MagicCol.Magic[GroupID][i].MagicBaseID );
				}
		}
	*/
}

//----------------------------------------------------------------------------------------------------
void ObjectDataClass::SetTitleLink()
{

	GameObjDbStructEx* Obj;

	_KillCountTitleInfo.clear();
	//資料檢查
	_ObjectDB->GetDataByOrder(true);
	for (int i = 0; i < _ObjectDB->TotalInfoCount(); i++)
	{
		Obj = _ObjectDB->GetDataByOrder();

		if (!Obj->IsTitle())
			continue;

		if (Obj->Title.Type != EM_TitleNameType_KillCount)
			continue;

		if (Obj->Title.KillCount.KillGroupID == -1)
			continue;

		_KillCountTitleInfo[Obj->Title.KillCount.KillGroupID].push_back(Obj);
	}

}
//----------------------------------------------------------------------------------------------------
void ObjectDataClass::SetRecipeLink()
{
	//
	//01.武器（歸於武器表）
	//02.防具（歸於防具表）
	//03.食物（歸於物品表）
	//04.甜食（歸於物品表）
	//05.藥水（歸於物品表）
	//------------------------------------------------------------------------------
	/*
	GameObjDbStructEx	*Obj;
	GameObjDbStructEx	*ProduceObj;
	_ObjectDB->GetDataByOrder( true );
	for( int i = 0 ; i < _ObjectDB->TotalInfoCount() ; i++ )
	{
	Obj = _ObjectDB->GetDataByOrder();

	if( !Obj->IsRecipe() )
	continue;

	GameObjectRecipeStruct& Recipe = Obj->Recipe;

	// 取代原本物品名稱
	char szOrgName[256];
	sprintf( szOrgName, "Sys%d_name", Obj->GUID )


	char szReplaceString[256];
	ProduceObj = _ObjectDB->GetObj( Recipe.Item1_Normal );
	if( ProduceObj != NULL )
	{
	sprintf( szReplaceString, "%s%s", GetString("SYS_RECIPE_TITLE"), ProduceObj->Name )

	_StringMap[ szOrgName ] = szReplaceString;
	string* pString = &( _StringMap[ szOrgName ] );
	Obj->Name = pString->c_str();
	}

	*/


	/*


	for( int j = 0 ; j < 9 ; j++ )
	{
	ProduceObj = _ObjectDB->GetObj( Recipe.ItemAll[j].OrgObjID );
	if( ProduceObj == NULL )
	continue;
	if( ProduceObj->IsItem() == false )
	continue;
	if( ProduceObj->Item.SrcRecipe != 0 )
	continue;

	if(		ProduceObj->IsArmor()
	||	ProduceObj->IsWeapon() )
	{
	ProduceObj->Item.SrcRecipe = Obj->GUID;
	}
	else if( ProduceObj->IsItem_Pure() ) //純物品類
	{
	if(		ProduceObj->Item.ItemType == EM_ItemType_Food
	||	ProduceObj->Item.ItemType == EM_ItemType_SweetFood
	||	ProduceObj->Item.ItemType == EM_ItemType_Water           	)
	{
	ProduceObj->Item.SrcRecipe = Obj->GUID;
	}
	}
	}
	}
	*/

}
//----------------------------------------------------------------------------------------------------
/*
void ObjectDataClass::SetEqAttributeLink( )
{

//------------------------------------------------------------------------------
for( int i = 0 ; i < _MAX_LEVEL_ ; i++ )
{
for( int j = 0 ; j < EM_AddonPos_WeaponType_Max ; j++ )
{
for( int k = 0 ; k < 2 ; k++ )
{
_WeaponAttribute[k][j][i].clear();
}
}
}

for( int i = 0 ; i < _MAX_LEVEL_ ; i++ )
{
for( int j = 0 ; j < EM_AddonPos_ArmorType_Max ; j++ )
{
for( int k = 0 ; k < EM_AddonPos_ArmorPos_Max ; k++ )
{
_ArmorAttribute[k][j][i].clear();
}
}
}

for( int i = 0 ; i < _MAX_LEVEL_ ; i++ )
{
for( int j = 0 ; j < EM_AddonPos_Treasure_Max ; j++ )
{
_TreasureAttribute[j][i].clear();
}
}

const int Def_MaxRate = 50;
GameObjDbStructEx	*Obj;
_ObjectDB->GetDataByOrder( true );
for( int i = 0 ; i < _ObjectDB->TotalInfoCount() ; i++ )
{
Obj = _ObjectDB->GetDataByOrder();

if( !Obj->IsAttribute() )
continue;

int LV = Obj->Rare;
if( LV > Def_MaxRate )
LV = Def_MaxRate -1;
if( LV < 0 )
continue;

//防具
for( int j = 0 ; j < EM_AddonPos_ArmorType_Max ; j++ )
{
if( TestBit( Obj->Attirbute.AddPos._ArmorType , j ) == false )
continue;

for( int k = 0 ; k < EM_AddonPos_ArmorPos_Max ; k++ )
{

if( TestBit( Obj->Attirbute.AddPos._ArmorPos , k ) == false )
continue;

_ArmorAttribute[j][k][ LV ].push_back( Obj );
}
}

//武器
for( int j = 0 ; j < EM_AddonPos_WeaponType_Max ; j++ )
{
if( TestBit( Obj->Attirbute.AddPos._WeaponType , j ) == false )
continue;

switch( Obj->Attirbute.AddPos.WeaponPosEnum )
{
case EM_AddonPos_WeaponLimitPos_None:
_WeaponAttribute[0][j][ LV ].push_back( Obj );
_WeaponAttribute[1][j][ LV ].push_back( Obj );
break;
case EM_AddonPos_WeaponLimitPos_MainHand:
_WeaponAttribute[0][j][ LV ].push_back( Obj );
break;
case EM_AddonPos_WeaponLimitPos_OffHand:
_WeaponAttribute[1][j][ LV ].push_back( Obj );
break;
}
}
//其它
for( int j = 0 ; j < EM_AddonPos_Treasure_Max ; j++ )
{
if( TestBit( Obj->Attirbute.AddPos._Treasure , j ) == false )
continue;

_TreasureAttribute[j][ LV ].push_back( Obj );
}

}
}
*/

//----------------------------------------------------------------------------------------------------
int ObjectDataClass::GetItemQuality(GameObjDbStruct* itemDB, ItemFieldStruct* itemField)
{
	int quality = 0;

	if (itemField)
	{
		if (itemDB == NULL)
		{
			itemDB = GetObj(itemField->OrgObjID);
		}
	}

	if (itemDB)
	{
		quality = itemDB->Rare;

		if (itemField)
			quality += itemField->Rare;

		/*
		if ( itemDB->Rare == 0 && itemField && itemField->Inherent != 0 )
		quality = 1;
		else
		quality = itemDB->Rare;

		// 衝等大於1為綠色
		if ( quality < 1 && itemField && itemField->Level > 0 )
		quality = 1;
		*/
	}
	return quality;
}

//----------------------------------------------------------------------------------------------------
int ObjectDataClass::GetItemQualityColor(int quality)
{
	switch (quality)
	{
	case 1:
		return 0xFF00FF00;
	case 2:
		return 0xFF0072BC;
	case 3:
		return 0xFFC805F8;
	case 4:
		return 0xFFF68E56;
	case 5:
		return 0xFFA37D50;
	case 6:
		return 0xFF6DCFF6;
	case 7:
		return 0xFFED145B;
	case 8:
		return 0xFFA864A8;
	case 9:
		return 0xFFF7941D;
	}
	return 0xFFFFFFFF;
}
//----------------------------------------------------------------------------------------------------
const char* ObjectDataClass::GetItemQualityTextColor(int quality)
{
	switch (quality)
	{
	case 1:
		return "|cFF00FF00";
	case 2:
		return "|cFF0072BC";
	case 3:
		return "|cFFC805F8";
	case 4:
		return "|cFFF68E56";
	case 5:
		return "|cFFA37D50";
	case 6:
		return "|cFF6DCFF6";
	case 7:
		return "|cFFED145B";
	case 8:
		return "|cFFA864A8";
	case 9:
		return "|cFFF7941D";
	}
	return "|cFFFFFFFF";
}
//----------------------------------------------------------------------------------------------------
string ObjectDataClass::GetItemColorName(ItemFieldStruct* itemField, GameObjDbStructEx* itemDB)
{
	if (itemField && itemDB == NULL) {
		itemDB = GetObj(itemField->OrgObjID);
	}

	if (itemDB == NULL)
		return "";

	char itemName[256];
	// 取得物品名稱
	if (!RoleDataEx::GetItemFieldName(itemField, itemName, 256))
		MyStrcpy(itemName, itemDB->GetName(), sizeof(itemName));

	char tmp[256];
	// 取得物品顏色
	sprintf(tmp, "%s[%s]|r", GetItemQualityTextColor(GetItemQuality(itemDB, itemField)), itemName);

	return tmp;
}

//----------------------------------------------------------------------------------------------------
int	ObjectDataClass::GetGoodEvilColor(GoodEvilTypeENUM type)
{
	int color = 0xFFFFFFFF;
	switch (type)
	{
	case EM_GoodEvil_Demon:
		color = 0xFFEC008C;
		break;
	case EM_GoodEvil_Evil3:
		color = 0xFFFF0000;
		break;
	case EM_GoodEvil_Evil2:
		color = 0xFFF26522;
		break;
	case EM_GoodEvil_Evil1:
		color = 0xFFFFF568;
		break;
	case EM_GoodEvil_Normal:
		color = 0xFFFFFFFF;
		break;
	case EM_GoodEvil_Good1:
		color = 0xFF6DCFF6;
		break;
	case EM_GoodEvil_Good2:
		color = 0xFF0090BC;
		break;
	case EM_GoodEvil_Good3:
		color = 0xFFBD8CBF;
		break;
	case EM_GoodEvil_Hero:
		color = 0xFF92278F;
		break;
	}
	return color;
}
//----------------------------------------------------------------------------------------------------
/*
void ObjectDataClass::SetQuestLink()
{
	GameObjDbStructEx		*pObj;
	GameObjDbStructEx		*pQuestObj;
	GameObjectQuestDetail	*pQuest;
	GameObjectNpcStruct		*pNPC;

	_ObjectDB->GetDataByOrder( true );
	for( int i = 0 ; i < _ObjectDB->TotalInfoCount() ; i++ )
	{
		pQuestObj = _ObjectDB->GetDataByOrder();

		if( pQuestObj == NULL )
			continue;

		int iGuid = pQuestObj->GUID;

		if( pQuestObj->IsNPC() != false )
		{
			ZeroMemory( pQuestObj->NPC.iQuestLinkObj, sizeof( pQuestObj->NPC.iQuestLinkObj ) );
		}

		if( !pQuestObj->IsQuest() )
		{
			if( pQuestObj->IsQuestNPC() )
			{
				if( pQuestObj->NPC.iQuestNPCRefID != 0 )
				{

					GameObjectNpcStruct RefObj;
					pObj	= _ObjectDB->GetObj( pQuestObj->NPC.iQuestNPCRefID );

					if( pObj )
					{
						pNPC	= &pQuestObj->NPC;

						RefObj	= *pNPC;
						memcpy(  RefObj.iQuestID, pNPC->iQuestID, (sizeof(int)*20) );

						*pNPC	= pObj->NPC;

						pNPC->iQuestMode		= RefObj.iQuestMode;
						pNPC->szQuestNpcText	= RefObj.szQuestNpcText;
						pNPC->szQuestDoingText	= RefObj.szQuestDoingText;
						//						pNPC->szLuaScript		= RefObj.szLuaScript;
						pNPC->iQuestNPCRefID	= RefObj.iQuestNPCRefID;
						//pNPC->KeepDistance		= RefObj.KeepDistance;
						//pNPC->OrgWeaponType		= RefObj.OrgWeaponType;
						//pNPC->OrgArmorType		= RefObj.OrgArmorType;
						//pNPC->FollowRange		= RefObj.FollowRange;
						pNPC->TitleName			= RefObj.TitleName;
						pNPC->ZoneID			= RefObj.ZoneID;
						pNPC->X					= RefObj.X;
						pNPC->Y					= RefObj.Y;
						pNPC->Z					= RefObj.Z;

						memcpy( pNPC->szLuaScript,		RefObj.szLuaScript,	(sizeof(RefObj.szLuaScript)) );
						memcpy( pNPC->szLuaInitScript,		RefObj.szLuaInitScript,	(sizeof(RefObj.szLuaInitScript)) );
						memcpy( pNPC->iQuestID,		RefObj.iQuestID,	(sizeof(int)*20) );
						memcpy( pNPC->MainHandEQ,	RefObj.MainHandEQ,	( sizeof(int)*_MAX_NPC_EQSEL_COUNT_) );
						memcpy( pNPC->SecHandEQ,	RefObj.SecHandEQ,	( sizeof(int)*_MAX_NPC_EQSEL_COUNT_) );
						memcpy( pNPC->szLuaDisplayScript,		RefObj.szLuaDisplayScript,	(sizeof(RefObj.szLuaDisplayScript)) );

					}
					else
					{
						int a = 3;
					}
				}
				else
				{
					pNPC	= &pQuestObj->NPC;
					pNPC->Init();

					pNPC->Race				= 0;
					pNPC->Vocation			= 1;
					pNPC->Sex				= 0;
					pNPC->ExpRate			= 0;
					pNPC->Level				= 1;
					pNPC->LevelRange		= 1;
					pNPC->STR				= 10;
					pNPC->STA				= 10;
					pNPC->INT				= 10;
					pNPC->MND				= 10;
					pNPC->AGI				= 10;
					pNPC->MoveSpeed			= 100;
					pNPC->SearchRange		= 15;
					pNPC->DropMoney_Base	= 0;
					pNPC->DropMoney_Rand	= 0;
					pNPC->Brave				= 0;
					pNPC->LinkType			= 0;
					pNPC->EscapeType		= 0;
					pNPC->OrgWeaponType		= EM_Weapon_Unarmed;
					pNPC->OrgArmorType		= EM_Armor_HeavyArmed;
					pNPC->FollowRange		= 100;
					pNPC->Effect._Value[0]	= 0;
					pNPC->Effect._Value[1]	= 0;
				}
			}

			continue;
		}

		pQuest = &pQuestObj->QuestDetail;

		// 檢查任務中有無需要宰殺怪物, 若有則取出該 NPC 資料, 並設入任務關聯

		for( int i = 0; i < _MAX_SIZE_QUEST_ITEM; i++ )
		{
			// 指定打倒目標計數

			int iObjID = pQuest->iRequest_KillID[ i ];

			if( iObjID != 0 )
			{
				pObj = _ObjectDB->GetObj( iObjID );

				if( pObj )
				{
					bool bLinked = false;
					pNPC = &( pObj->NPC );

					// 先檢查該 NPC 是否已有建立與該任務的關連
					for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
					{
						if( pNPC->pQuestLink[ i ] == pQuestObj )
						{
							bLinked = true;
							break;
						}
					}

					if( bLinked == false )
					{
						for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
						{
							if( pNPC->pQuestLink[ i ] == 0 )
							{
								bLinked = true;
								pNPC->pQuestLink[ i ] = pQuestObj;
								break;
							}
						}

						if( bLinked == false )
						{
							Print( LV5 , "SetQuestLink" , "NPC[ %d ][ %s ] Correlation with too many quest  ", pObj->GUID, pObj->GetName() );
						}
					}
				}
			}
			else
				break;
		}


		for( int i = 0; i < _MAX_SIZE_QUEST_CONDITION; i++ )
		{
			int iObjID = 0;

			// 打倒目標後有機會得到任務物品

			iObjID = pQuest->iProcess_KilltoGetItem_ObjID[ i ];

			if( iObjID != 0 )
			{
				pObj = _ObjectDB->GetObj( iObjID );

				if( pObj )
				{
					pNPC = &( pObj->NPC );

					bool bLinked = false;

					// 先檢查該 NPC 是否已有建立與該任務的關連
					for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
					{
						if( pNPC->pQuestLink[ i ] == pQuestObj )
						{
							bLinked = true;
							break;
						}
					}

					if( bLinked == false )
					{
						for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
						{
							if( pNPC->pQuestLink[ i ] == 0 )
							{
								bLinked = true;
								pNPC->pQuestLink[ i ] = pQuestObj;
								break;
							}
						}

						if( bLinked == false )
						{
							Print( LV5 , "SetQuestLink" , "NPC[ %d ][ %s ]Correlation with too many quest ", pObj->GUID, pObj->GetName() );
						}
					}
				}
			}



			// 點擊物件關連

			iObjID = pQuest->iProcess_ClickToGetItem_ObjID[ i ];

			if( iObjID != 0 )
			{
				pObj = _ObjectDB->GetObj( iObjID );

				if( pObj )
				{
					bool bLinked = false;
					pNPC = &( pObj->NPC );

					// 先檢查該 NPC 是否已有建立與該任務的關連
					for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
					{
						if( pNPC->pQuestLink[ i ] == pQuestObj )
						{
							bLinked = true;
							break;
						}
					}

					if( bLinked == false )
					{
						for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
						{
							if( pNPC->pQuestLink[ i ] == 0 )
							{
								bLinked = true;
								pNPC->pQuestLink[ i ] = pQuestObj;
								break;
							}
						}

						if( bLinked == false )
						{
							Print( LV5 , "SetQuestLink" , "NPC[ %d ][ %s ]Correlation with too many quest ", pObj->GUID, pObj->GetName() );
						}
					}
				}
			}
		}
	}
}
*/

//----------------------------------------------------------------------------------------------------
bool ObjectDataClass::CheckedString(const char* Key)
{
	return _StringMap.find(Key) != _StringMap.end();
}

//----------------------------------------------------------------------------------------------------
const char* ObjectDataClass::GetString(const char* Key)
{
	/*
#ifdef _DEBUG

	hash_map< string , string >::iterator IterHash;
	map< string , string >::iterator Iter;

	IterHash = _DBStringMap.find( Key );
	if( IterHash != _DBStringMap.end() )
		return IterHash->second.c_str();

	Iter = _StringMap.find( Key );
	if( Iter == _StringMap.end( ) )
		return "";

#else

	hash_map< string , string >::iterator Iter;
	Iter = _DBStringMap.find( Key );
	if( Iter != _DBStringMap.end() )
		return Iter->second.c_str();

	Iter = _StringMap.find( Key );
	if( Iter == _StringMap.end( ) )
		return "";

#endif
	return Iter->second.c_str();
	*/
	map< string, string >::iterator IterDB;
	map< const char*, const char*, LessCharStr>::iterator Iter;

	IterDB = _DBStringMap.find(Key);
	if (IterDB != _DBStringMap.end())
		return IterDB->second.c_str();

	Iter = _StringMap.find(Key);
	if (Iter == _StringMap.end())
		return "";

	return Iter->second;
}
//----------------------------------------------------------------------------------------------------
const char* ObjectDataClass::GetSpecialString(const char* Key)
{
	const char* buf = GetString(Key);
	if (strlen(buf) > 0)
		return buf;
	return Key;
}
//----------------------------------------------------------------------------------------------------
int	ObjectDataClass::GetClassColorByID(Voc_ENUM voc)
{
	switch (voc)
	{
	case EM_Vocation_GameMaster:			// GM
		return 0xFFFFFFFF;
	case EM_Vocation_Warrior:				// 戰士
		return 0xFFFA4106;
	case EM_Vocation_Ranger:				// 遊俠
		return 0xFFA4D502;
	case EM_Vocation_Rogue:					// 盜賊
		return 0xFF00D5C5;
	case EM_Vocation_Wizard:				// 法師
		return 0xFFFB7111;
	case EM_Vocation_Priest:				// 僧侶
		return 0xFF288BEB;
	case EM_Vocation_Knight:				// 騎士
		return 0xFFDFE14A;
	case EM_Vocation_RuneDancer:			// 言靈使
		return 0xFFC149D0;
	case EM_Vocation_Druid:					// 德魯伊
		return 0xFF60D060;
	case EM_Vocation_Duelist:				// 鬥棋士
		return 0xFF6060D0;
	}
	return 0xFFFFFFFF;
}
//----------------------------------------------------------------------------------------------------
const char* ObjectDataClass::GetClassTokenByID(Voc_ENUM voc)
{
	switch (voc)
	{
	case EM_Vocation_GameMaster:
		return "GM";

	case EM_Vocation_Warrior:
		return "WARRIOR";

	case EM_Vocation_Ranger:
		return "RANGER";

	case EM_Vocation_Rogue:
		return "THIEF";

	case EM_Vocation_Wizard:
		return "MAGE";

	case EM_Vocation_Priest:
		return "AUGUR";

	case EM_Vocation_Knight:
		return "KNIGHT";

	case EM_Vocation_RuneDancer:
		return "WARDEN";

	case EM_Vocation_Druid:
		return "DRUID";

	case EM_Vocation_Harpsyn:
		return "HARPSYN";

	case EM_Vocation_Psyron:
		return "PSYRON";

	case EM_Vocation_Duelist:
		return "DUELIST";
	}

	return "";
}
//----------------------------------------------------------------------------------------------------
const char* ObjectDataClass::GetClassNameByID(Voc_ENUM voc)
{
	char tmp[64];
	sprintf_s(tmp, 64, "SYS_CLASSNAME_%s", GetClassTokenByID(voc));
	return GetString(tmp);
}
//-----------------------------------------------------------------------------
Voc_ENUM		ObjectDataClass::GetClassEnumByToken(const char* name)
{
	if (name == NULL)
	{
		if (strcmp(name, "GM") == 0)
			return EM_Vocation_GameMaster;
		else if (strcmp(name, "WARRIOR") == 0)
			return EM_Vocation_Warrior;
		else if (strcmp(name, "RANGER") == 0)
			return EM_Vocation_Ranger;
		else if (strcmp(name, "THIEF") == 0)
			return EM_Vocation_Rogue;
		else if (strcmp(name, "MAGE") == 0)
			return EM_Vocation_Wizard;
		else if (strcmp(name, "AUGUR") == 0)
			return EM_Vocation_Priest;
		else if (strcmp(name, "KNIGHT") == 0)
			return EM_Vocation_Knight;
		else if (strcmp(name, "WARDEN") == 0)
			return EM_Vocation_RuneDancer;
		else if (strcmp(name, "DRUID") == 0)
			return EM_Vocation_Druid;
		else if (strcmp(name, "HARPSYN") == 0)
			return EM_Vocation_Harpsyn;
		else if (strcmp(name, "PSYRON") == 0)
			return EM_Vocation_Psyron;
		else if (strcmp(name, "DUELIST") == 0)
			return EM_Vocation_Duelist;
	}
	return EM_Vocation_None;
}
//----------------------------------------------------------------------------------------------------
/*
int ObjectDataClass::AccountStoreSearchItem(int type, const char* keyStr)
{
_AccountStoreTable.clear();

int count = 0;
for ( vector<AccountShopInfoStruct>::iterator iter = _AccountShop.begin(); iter != _AccountShop.end(); iter++, count++ )
{
AccountShopInfoStruct& accountShop = *iter;
for ( int i = 0; i < 4; i++ )
{
if ( type > 0 && type == accountShop.Type[i] )
{
// 如果必要可以加入名稱判斷

AccountStoreItemTable itemTable;
itemTable.index		= count;
itemTable.cost		= accountShop.Cost;
itemTable.count		= accountShop.Count;
itemTable.itemID	= accountShop.ItemObjID;

_AccountStoreTable.push_back(itemTable);
}
}
}
return (int)_AccountStoreTable.size();
}
*/

//----------------------------------------------------------------------------------------------------
void ObjectDataClass::InitWorldMapInfo()
{
	if (_IsWorldMapInfoInited)
	{
		return;
	}

	_IsWorldMapInfoInited = true;

	WorldMapInfoStruct* pWorldMapInfo;
	for (int i = 0; i < (int)_WorldMapInfo.size(); i++)
	{
		pWorldMapInfo = &_WorldMapInfo[i];

		if (pWorldMapInfo->Type == EM_WorldMapIType_World ||
			pWorldMapInfo->Type == EM_WorldMapIType_City ||
			GetObj(pWorldMapInfo->MapID + Def_ObjectClass_Zone))
		{

		}
		else
		{
			continue;
		}


		SWorldMapInfoTable& rWorldZoneStruct = GetWorldMapInfoTableByID(pWorldMapInfo->WorldID);

		if (pWorldMapInfo->Type <= 0)
		{
			continue;
		}
		else if (pWorldMapInfo->Type == EM_WorldMapIType_World) //大陸
		{
			rWorldZoneStruct.WorldZone = pWorldMapInfo;
		}
		else
		{
			rWorldZoneStruct.MapTables[pWorldMapInfo->Type].push_back(pWorldMapInfo);
		}

		_WofldMapNameMap[pWorldMapInfo->MapName] = pWorldMapInfo;
		_WofldMapIDMap[pWorldMapInfo->MapID] = pWorldMapInfo;
	}
}

//----------------------------------------------------------------------------------------------------
const WorldMapInfoStruct* ObjectDataClass::GetWorldMapInfoByName(const char* szName)
{
	InitWorldMapInfo();

	map<string, const WorldMapInfoStruct*>::iterator it = _WofldMapNameMap.find(szName);

	if (it == _WofldMapNameMap.end())
		return NULL;

	return it->second;
}

//----------------------------------------------------------------------------------------------------
const WorldMapInfoStruct* ObjectDataClass::GetWorldMapInfoByID(int mapID)
{
	InitWorldMapInfo();

	map<int, const WorldMapInfoStruct*>::iterator it = _WofldMapIDMap.find(mapID);

	if (it == _WofldMapIDMap.end())
		return NULL;

	return it->second;
}

//----------------------------------------------------------------------------------------------------
SWorldMapInfoTable& ObjectDataClass::GetWorldMapInfoTableByID(int WorldID)
{
	InitWorldMapInfo();

	int count = (int)_WorldMapInfoTables.size();
	for (int i = 0; i < count; ++i)
	{
		SWorldMapInfoTable& r = _WorldMapInfoTables[i];
		if (r.WorldID == WorldID)
			return r;
	}

	_WorldMapInfoTables.push_back(WorldID);

	return _WorldMapInfoTables[_WorldMapInfoTables.size() - 1];
}

//----------------------------------------------------------------------------------------------------
int ObjectDataClass::GetWorldMapInfoTableCount()
{
	InitWorldMapInfo();

	return (int)_WorldMapInfoTables.size();
}

//----------------------------------------------------------------------------------------------------
SWorldMapInfoTable& ObjectDataClass::GetWorldMapInfoTableByIndex(int index)
{
	InitWorldMapInfo();

	if (_WorldMapInfoTables.empty())
	{
		//輸出錯誤
		return GetWorldMapInfoTableByID(0);
	}

	if (index < 0 || index >= (int)_WorldMapInfoTables.size())
		index = 0;

	return _WorldMapInfoTables[index];
}

//----------------------------------------------------------------------------------------------------
int ObjectDataClass::GetCreateBaseEQ(Race_ENUM race, Voc_ENUM voc, int index, int* outColor)
{
	if (_BaseEQ[race][voc] == 0)
		return NULL;

	GameObjDbStructEx* createObj = GetObj(_BaseEQ[race][voc]);
	if (createObj == NULL)
		return NULL;

	if (outColor)
	{
		GameObjDbStructEx* item = GetObj(createObj->CreateRole.EQ.ID[index]);
		if (item)
		{
			GameObjDbStructEx* ImageDB = GetObj(item->ImageID);
			if (ImageDB != NULL)
			{
				outColor[0] = ImageDB->Image.ArmorMainColor.Color;
				outColor[1] = ImageDB->Image.ArmorOffColor.Color;
			}
		}
	}

	return createObj->CreateRole.EQ.ID[index];
}
//----------------------------------------------------------------------------------------------------
CreateCharacterColor& ObjectDataClass::GetCreateSkinColor(Race_ENUM race)
{
	return _SkinColor[race];
}

//----------------------------------------------------------------------------------------------------
CreateCharacterColor& ObjectDataClass::GetCreateHairColor(Race_ENUM race)
{
	return _HairColor[race];
}
//----------------------------------------------------------------------------------------------------
void ObjectDataClass::SetStringLink()
{
	_ObjectDB->GetDataByOrder(true);

	for (int i = 0; i < _ObjectDB->TotalInfoCount(); i++)
	{
		SetStringLink(_ObjectDB->GetDataByOrder());
	}

}

void ObjectDataClass::SetRuneLink()
{
	_ObjectDB->GetDataByOrder(true);
	for (int i = 0; i < _ObjectDB->TotalInfoCount(); i++)
	{
		GameObjDbStructEx* Obj = _ObjectDB->GetDataByOrder();
		if (Obj->IsRune() == false)
			continue;

		if (ObjectDataClass::GetDataLanguageID() != -1)
		{
			int iLanguageBit = 1 << ObjectDataClass::GetDataLanguageID();
			if ((Obj->LanguageSet & iLanguageBit) == 0)
				continue;
		}

		if (Obj->Item.RuneGroup >= 200 || Obj->Item.RuneGroup < 0)
			continue;

		while ((int)_RuneTable[Obj->Item.RuneGroup].size() <= Obj->Item.Level)
			_RuneTable[Obj->Item.RuneGroup].push_back(0);
		if (_RuneTable[Obj->Item.RuneGroup][Obj->Item.Level] == 0)
			_RuneTable[Obj->Item.RuneGroup][Obj->Item.Level] = Obj->GUID;
	}
}

void ObjectDataClass::SetLifeSillKeyItem()
{
	_ObjectDB->GetDataByOrder(true);
	for (int i = 0; i < _ObjectDB->TotalInfoCount(); i++)
	{
		GameObjDbStructEx* Obj = _ObjectDB->GetDataByOrder();
		if (Obj->IsKeyItem() == false)
			continue;
		if (Obj->KeyItem.Type != EM_KeyItemType_LifeSkillUnLock)
			continue;

		RoleDataEx::LifeSkillLearnKey[Obj->KeyItem.LifeSkillType].push_back(Obj->GUID);
	}
}

bool ObjectDataClass::CheckInLearnMagic(int IgnoreID, int MagicID)
{
	GameObjDbStructEx* Obj;
	for (int i = 1; i < 20; i++)
	{
		Voc_ENUM Job = (Voc_ENUM)i;
		Obj = GetObj(i + Def_ObjectClass_LearnMagic);
		if (Obj->IsLearnMagic() == false || Obj->GUID == IgnoreID)
			continue;

		for (int i = 0; i < Obj->LearnMagic.NormalSkillCount; i++)
		{
			if (Obj->LearnMagic.NormalSkill[i].MagicID == MagicID)
				return true;
		}
		for (int i = 0; i < Obj->LearnMagic.SPSkillCount; i++)
		{
			if (Obj->LearnMagic.SPSkill[i].MagicID == MagicID)
				return true;
		}
	}

	return false;
}

void ObjectDataClass::SetMagicJobInfo()
{
	GameObjDbStructEx* Obj;
	for (int i = 1; i < 1000; i++)
	{
		Voc_ENUM Job = (Voc_ENUM)i;
		Obj = GetObj(i + Def_ObjectClass_LearnMagic);
		if (Obj->IsLearnMagic() == false)
			continue;

		for (int i = 0; i < Obj->LearnMagic.NormalSkillCount; i++)
		{
			GameObjDbStructEx* MagicDB = GetObj(Obj->LearnMagic.NormalSkill[i].MagicID);
			if (MagicDB == NULL)
				continue;

			if (MagicDB->IsMagicCollect())
			{
				if (CheckInLearnMagic(Obj->GUID, Obj->LearnMagic.NormalSkill[i].MagicID))
				{
					MagicDB->MagicCol.JobInfo.IsJobLimit = false;
					MagicDB->MagicCol.JobInfo.Job = EM_Vocation_None;
				}
				else
				{
					MagicDB->MagicCol.JobInfo.IsJobLimit = false;
					MagicDB->MagicCol.JobInfo.Job = Job;
				}
			}
		}
		for (int i = 0; i < Obj->LearnMagic.SPSkillCount; i++)
		{
			GameObjDbStructEx* MagicDB = GetObj(Obj->LearnMagic.SPSkill[i].MagicID);
			if (MagicDB == NULL)
				continue;

			if (MagicDB->IsMagicCollect())
			{
				if (CheckInLearnMagic(Obj->GUID, Obj->LearnMagic.SPSkill[i].MagicID))
				{
					MagicDB->MagicCol.JobInfo.IsJobLimit = false;
					MagicDB->MagicCol.JobInfo.Job = EM_Vocation_None;
				}
				else
				{
					MagicDB->MagicCol.JobInfo.IsJobLimit = true;
					MagicDB->MagicCol.JobInfo.Job = Job;
				}
			}
		}
	}
}


/*
//取得某技能上限物件
int		ObjectDataClass::GetLifeSkillMax( RoleDataEx* Owner , SkillValueTypeENUM Type )
{
if( (unsigned)Type >= EM_SkillValueType_MaxCount )
return 0;

int MaxLv = 0;
GameObjDbStructEx	*Obj;
for( int i = 0 ; i < (int)_LifeSkillLearnKey[Type].size() ; i++ )
{
int KeyItemID = _LifeSkillLearnKey[Type][i];
if( Owner->CheckKeyItem( KeyItemID ) == false )
continue;
Obj = GetObj( KeyItemID );
if( Obj = NULL )
continue;
if( MaxLv < Obj->KeyItem.LifeSkillMaxLv )
MaxLv = Obj->KeyItem.LifeSkillMaxLv;
}
return MaxLv;
}*/

//取得Emote資訊
vector<MotionTableStruct>& ObjectDataClass::GetEmoteTable()
{
	return _MotionInfo;
};

int ObjectDataClass::GetEmoteInfoCount()
{
	return (int)_MotionInfo.size();
}

MotionTableStruct* ObjectDataClass::GetEmoteInfoByIndex(int index)
{
	if (index < 0 || index >= (int)_MotionInfo.size())
		return NULL;

	return &_MotionInfo[index];
}

MotionTableStruct* ObjectDataClass::GetEmoteInfoByID(int ID)
{
	int count = (int)_MotionInfo.size();

	for (int i = 0; i < count; ++i)
	{
		if (ID == _MotionInfo[i].ID)
		{
			return &_MotionInfo[i];
		}
	}
	return NULL;
}

vector<TeachTableStruct>& ObjectDataClass::GetTeachInfo()
{
	return _TeachInfo;
}

ColoringShopStruct* ObjectDataClass::GetColoringShop(int ID)
{
	if ((unsigned int)ID >= _ColoringShop.size())
		return NULL;

	return &_ColoringShop[ID];
}
int ObjectDataClass::SysKeyFunction(int KeyID)
{
	map<int, SysKeyFunctionStruct>::iterator Iter;

	Iter = _SysKeyFunction.find(KeyID);
	if (Iter == _SysKeyFunction.end())
		return 0;

	return Iter->second.MagicID;

}

int	ObjectDataClass::GetGuildFlagInsigniaCount()
{
	return (int)_GuildFlagInsigniaTable.size();
}

int ObjectDataClass::GuildFlagBannerCount()
{
	return (int)_GuildFlagBannerTable.size();
}

const char* ObjectDataClass::GetGuildFlagInsigniaType(int index)
{
	if (index < 0 || index >= (int)_GuildFlagInsigniaTable.size())
		return NULL;

	return _GuildFlagInsigniaTable[index].c_str();

}

const char* ObjectDataClass::GuildFlagBannerType(int index)
{
	if (index < 0 || index >= (int)_GuildFlagBannerTable.size())
		return NULL;

	return _GuildFlagBannerTable[index].c_str();
}

LotteryInfoTableStruct* ObjectDataClass::LotteryInfoTable(int SelectCount)
{
	if (unsigned(SelectCount - 1) >= _LotteryInfoTable.size())
		return NULL;

	return &_LotteryInfoTable[SelectCount - 1];
}

int		ObjectDataClass::GetSysKeyValueArg(char* keystr, int arg)
{
	char buf[256];
	sprintf(buf, keystr, arg);
	return GetSysKeyValue(buf);
}

int		ObjectDataClass::GetSysKeyValue(string Key)
{
	map<string, int>::iterator Iter;
	Iter = _SysKeyValeMap.find(Key);

	if (Iter == _SysKeyValeMap.end())
		return 0;

	return Iter->second;
}

int ObjectDataClass::GetKeyMagicID(int iID)
{
	map<int, SysKeyFunctionStruct>::iterator it = _SysKeyFunction.find(iID);

	if (it != _SysKeyFunction.end())
	{
		SysKeyFunctionStruct Val = it->second;
		return Val.MagicID;
	}

	return 0;
}

//取得賦加能力稀有度等級
int		ObjectDataClass::GetInherentRate(int MonsterLv)
{
	MonsterLv = (MonsterLv - 1) / 3;
	if (MonsterLv < 0)
		MonsterLv = 0;
	if (MonsterLv >= (int)_InherentRateTable.size())
		MonsterLv = (int)_InherentRateTable.size() - 1;

	InherentRateTableStruct& InherentRate = _InherentRateTable[MonsterLv];

	int	RandValue = rand() % 10000;

	for (int i = 0; i < 50; i++)
	{
		RandValue -= InherentRate.AllRateLv[i];
		if (RandValue <= 0)
			return i - 1;
	}
	return -1;
}

void	ObjectDataClass::SecondsToTimeAbbrev(float second, char* outText)
{
	outText[0] = 0;
	if (second > 86400.0f) {
		sprintf(outText, "%d %s", (int)(second / 86400), g_ObjectData->GetString("SYS_DAY"));
	}
	else if (second > 3600.0f) {
		sprintf(outText, "%d %s", (int)(second / 3600), g_ObjectData->GetString("SYS_HOUR"));
	}
	else if (second > 60.0f) {
		sprintf(outText, "%d %s", (int)(second / 60), g_ObjectData->GetString("SYS_MINUTE"));
	}
	else {
		double y, pi;
		y = modf(second, &pi);
		if (y < 0.1)
		{
			sprintf(outText, "%.1f %s", second, g_ObjectData->GetString("SYS_SECOND"));
		}
		else
		{
			sprintf(outText, "%d %s", (int)second, g_ObjectData->GetString("SYS_SECOND"));
		}

	}
}

void ObjectDataClass::GetMoneyText(int money, char* outStr, int outSize)
{
	char temp[512];

	if (outStr == NULL)
		return;

	// 加入分隔符號
	InsertComma(money, temp, 512);

	sprintf_s(outStr, 512, "%s %s", temp, g_ObjectData->GetString("SYS_GOLD"));
}

const char* ObjectDataClass::GetEquipRefineFilename(int level)
{
	if (level > 19) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv20.ros";
	}
	else if (level > 18) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv19.ros";
	}
	else if (level > 17) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv18.ros";
	}
	else if (level > 16) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv17.ros";
	}
	else if (level > 15) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv16.ros";
	}
	else if (level > 14) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv15.ros";
	}
	else if (level > 13) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv14.ros";
	}
	else if (level > 12) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv13.ros";
	}
	else if (level > 11) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv12.ros";
	}
	else if (level > 10) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv11.ros";
	}
	else if (level > 9) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv10.ros";
	}
	else if (level > 8) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv9.ros";
	}
	else if (level > 7) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv8.ros";
	}
	else if (level > 6) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv7.ros";
	}
	else if (level > 5) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv6.ros";
	}
	else if (level > 4) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv5.ros";
	}
	else if (level > 3) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv4.ros";
	}
	else if (level > 2) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv3.ros";
	}
	else if (level > 1) {
		return "model\\fx\\weapon_powerup\\act_powerup_generic_lv2.ros";
	}
	return "";
}

//取得威力轉換倍率值
float	ObjectDataClass::PowerRate(int Power, PowerRateTypeENUM Type)
{
	float	LRate = 0;
	float	LPower = 0;
	if (_PowerRate.size() == 0)
		return 0;

	for (unsigned i = 0; i < _PowerRate.size(); i++)
	{
		if (_PowerRate[i].Power[Type] > Power)
		{
			float NRate = _PowerRate[i].PowerRate[Type];
			float NPower = (float)_PowerRate[i].Power[Type];

			int	TempValue = int((NPower - LPower));
			if (TempValue == 0)
				return LRate;


			float Ret = LRate + ((NRate - LRate) / TempValue) * (Power - int(LPower));
			//	return _PowerRate[i-1].PowerRate[Type];
			return Ret;
		}
		LRate = _PowerRate[i].PowerRate[Type];
		LPower = float(_PowerRate[i].Power[Type]);
	}

	return _PowerRate.back().PowerRate[Type];
}

bool ObjectDataClass::GetWearEqTypeString(WearEqTypeENUM type, float value, char* inbuf, int insize)
{
	char temp[256];
	sprintf(temp, "SYS_WEAREQTYPE_%d", type);

	const char* typeStr = g_ObjectData->GetString(temp);
	if (typeStr != NULL) {
		/*
		float offset = value - (float)(int(value));
		if ( offset < 0.1f && offset > -0.1f )
		{
		if ( value > 0.0f ) {
		sprintf_s(inbuf, insize, "+%d %s", (int)value, typeStr);
		} else if ( value < 0.0f ) {
		sprintf_s(inbuf, insize, "-%d %s", -(int)value, typeStr);
		} else {
		memset(inbuf, 0, sizeof(char) * insize);
		}
		}
		else
		*/
		{
			IntegerString(value, temp, 256);

			if (value > 0.0f) {
				sprintf_s(inbuf, insize, "+%s %s", temp, typeStr);
			}
			else if (value < 0.0f) {
				sprintf_s(inbuf, insize, "%s %s", temp, typeStr);
			}
			else {
				memset(inbuf, 0, sizeof(char) * insize);
			}
		}
		return true;
	}
	return false;
}
void	ObjectDataClass::GetGuildWarDeclareTypeInfo(int Type, int& outTime, int& outMoney)
{
	char Buf[256];

	sprintf(Buf, "GuildPK_Time%d", Type);
	outTime = GetSysKeyValue(Buf);
	sprintf(Buf, "GuildPK_Money%d", Type);
	outMoney = GetSysKeyValue(Buf);
}

const char* ObjectDataClass::GetTipText(int index)
{
	/*
	if ( index < 0 ) {
		index = rand() % (max(_MaxTips, 1)) + 1;
	} else if ( index >= _MaxTips ) {
		return NULL;
	}
	*/

	char temp[256];

	if (_Tips.empty() || index > (int)_Tips.size())
		return NULL;
	else if (index < 1)
		index = int(rand() % _Tips.size() + 1);

	sprintf_s(temp, 256, "TIPS_TEXT%02d", index);
	return g_ObjectData->GetString(temp);
}

vector<GameObjDbStructEx*>& ObjectDataClass::KillCountTitleInfo(int KillGroupID)
{
	static vector<GameObjDbStructEx*> EmptyVector;

	map< int, vector<GameObjDbStructEx*> >::iterator Iter;
	Iter = _KillCountTitleInfo.find(KillGroupID);
	if (Iter == _KillCountTitleInfo.end())
		return EmptyVector;

	return Iter->second;
}

const char* ObjectDataClass::GetZoneNameByID(int zoneID)
{
	// Zone Name
	const char* zoneName = "";
	if (zoneID >= 0) {
		GameObjDbStructEx* ZoneDB = g_ObjectData->GetObj(zoneID + Def_ObjectClass_Zone);
		if (ZoneDB) {
			zoneName = ZoneDB->GetName();
		}
	}
	return zoneName;
}

// 比對物品排列順序
// [0]相同 [<0]src較大 [>0]dest較大
int ObjectDataClass::CompareItemOrder(GameObjDbStruct* src, GameObjDbStruct* dest)
{
	if (dest == NULL)
		return 1;
	else if (src == NULL)
		return -1;
	else
	{
		if (src->Classification != dest->Classification)
			return src->Classification - dest->Classification;

		switch (src->Classification)
		{
		case EM_ObjectClass_Item:
			if (src->Item.Class != dest->Item.Class)
				return src->Item.Class - dest->Item.Class;

			switch (src->Item.Class)
			{
				// 一般物品
			case EM_Item_Normal:
				if (src->Item.ItemType != dest->Item.ItemType)
					return src->Item.ItemType - dest->Item.ItemType;

				// 稀有度
				if (src->Rare != dest->Rare)
					return src->Rare - dest->Rare;

				// 等級
				if (src->Item.Level != dest->Item.Level)
					return src->Item.Level - dest->Item.Level;

				// 編號
				if (src->GUID != dest->GUID)
					return src->GUID - dest->GUID;

				break;

				// 金錢
			case EM_Item_Money:
				break;

				// 武器
			case EM_Item_Weapon:
				// 武器類型
				if (src->Item.WeaponType != dest->Item.WeaponType)
					return src->Item.WeaponType - dest->Item.WeaponType;

				// 稀有度
				if (src->Rare != dest->Rare)
					return src->Rare - dest->Rare;

				// 等級
				if (src->Item.Level != dest->Item.Level)
					return src->Item.Level - dest->Item.Level;

				// 編號
				if (src->GUID != dest->GUID)
					return src->GUID - dest->GUID;

				break;

			case EM_Item_Armor:
				// 防具部位
				if (src->Item.ArmorPos != dest->Item.ArmorPos)
					return src->Item.ArmorPos - dest->Item.ArmorPos;

				// 防具類型
				if (src->Item.ArmorType != dest->Item.ArmorType)
					return src->Item.ArmorType - dest->Item.ArmorType;

				// 稀有度
				if (src->Rare != dest->Rare)
					return src->Rare - dest->Rare;

				// 等級
				if (src->Item.Level != dest->Item.Level)
					return src->Item.Level - dest->Item.Level;

				// 編號
				if (src->GUID != dest->GUID)
					return src->GUID - dest->GUID;

				break;

			case EM_Item_Rune:
				// 符文類型
				if (src->Item.RuneType != dest->Item.RuneType)
					return src->Item.RuneType - dest->Item.RuneType;

				// 稀有度
				if (src->Rare != dest->Rare)
					return src->Rare - dest->Rare;

				// 等級
				if (src->Item.Level != dest->Item.Level)
					return src->Item.Level - dest->Item.Level;

				// 編號
				if (src->GUID != dest->GUID)
					return src->GUID - dest->GUID;
				break;

			case EM_Item_Card:
				// 稀有度
				if (src->Rare != dest->Rare)
					return src->Rare - dest->Rare;

				// 取得卡片來源資料
				{
					GameObjDbStructEx* srcCard = GetObj(src->Item.CardOrgObjID);
					GameObjDbStructEx* destCard = GetObj(dest->Item.CardOrgObjID);
					if (destCard == NULL)
						return 1;
					else if (srcCard == NULL)
						return -1;

					// 種族
					if (srcCard->NPC.Race != destCard->NPC.Race)
						return srcCard->NPC.Race - destCard->NPC.Race;

					// 等級
					if (srcCard->NPC.Level != destCard->NPC.Level)
						return srcCard->NPC.Level - destCard->NPC.Level;
				}

				// 編號
				if (src->GUID != dest->GUID)
					return src->GUID - dest->GUID;

				break;
			}
			break;
		}
	}
	return 0;
}
//取得關係法術
RelationMagicStruct* ObjectDataClass::RelationMagicInfo(RelationTypeENUM Type, int Lv)
{
	//_RelationMagic[ EM_RelationType_MaxRelation ]
	if ((unsigned)Type >= EM_RelationType_MaxRelation)
		return NULL;
	if ((unsigned)Lv >= _RelationMagic[Type].size())
		return NULL;

	return &(_RelationMagic[Type][Lv]);
}

int				ObjectDataClass::RuneExchange_Type(int Type1, int Type2)	//符文兌換表
{
	if (Type1 > Type2)
		swap(Type1, Type2);

	int key = Type2 * 10000 + Type1;
	map< int, int >::iterator iter;
	iter = _RuneExchangeTable.find(key);
	if (iter == _RuneExchangeTable.end())
		return -1;
	return iter->second;
	/*
	if( unsigned( Type1 ) >= 50  || unsigned( Type2 ) >= 50  )
		return -1;

	return _RuneExchangeTable[Type1][Type2];
	*/
}

//vector< int >&	ObjectDataClass::RuneList( int Type )					//符文表
int 	ObjectDataClass::RuneID(int Type, int Lv)
{
	if (unsigned(Type + 1) >= 200)
		return -1;

	if ((int)_RuneTable[Type].size() <= Lv)
		return -1;

	return _RuneTable[Type][Lv];
}

GameObjDbStructEx* ObjectDataClass::RuneExchange(GameObjDbStructEx* RuneDB1, GameObjDbStructEx* RuneDB2)
{
	if (RuneDB1->IsRune() == false || RuneDB2->IsRune() == false)
		return NULL;


	if (RuneDB1->Item.Level != RuneDB2->Item.Level)
		return NULL;

	int NewType = g_ObjectData->RuneExchange_Type(RuneDB1->Item.RuneGroup, RuneDB2->Item.RuneGroup);
	if (NewType == -1)
		return NULL;


	int NewItemID = g_ObjectData->RuneID(NewType, RuneDB2->Item.Level);

	GameObjDbStructEx* NewRuneDB = g_ObjectData->GetObj(NewItemID);

	if (NewRuneDB->IsRune() == false)
		return false;

	return NewRuneDB;
}

GameObjDbStructEx* ObjectDataClass::NextRuneLv(GameObjDbStructEx* RuneDB)
{
	if (RuneDB->IsRune() == false)
		return NULL;

	GameObjDbStructEx* NewRuneDB = g_ObjectData->GetObj(g_ObjectData->RuneID(RuneDB->Item.RuneGroup, RuneDB->Item.Level + 1));
	return NewRuneDB;

}
int		ObjectDataClass::St_GetSysKeyValue(const char* Str)
{
	return g_ObjectData->GetSysKeyValue(Str);
}

SysKeyValueStruct* ObjectDataClass::St_SysValue()
{
	return &g_ObjectData->SysValue();
}

bool	ObjectDataClass::SetExhcnageMoneyKeyValue(int Count, int DataSize, char* Data)
{
	if (Count <= 0)
	{
		return false;
	}

	MoneyExchangeKeyValueStruct* TempData = NEW(MoneyExchangeKeyValueStruct[Count]);
	MyLzoClass myZip;
	int RetSize = myZip.Decode(Data, DataSize, Count * sizeof(MoneyExchangeKeyValueStruct), (char*)TempData);

	if (RetSize == 0)
	{
		Print(LV2, "ObjectDataClass", "SetExhcnageMoneyKeyValue RetSize = 0 DataError!!\n");
		return false;
	}
	else
	{
		_MoneyExchangeKeyValue.clear();

		for (int i = 0; i < Count; i++)
		{
			_MoneyExchangeKeyValue[TempData[i].KeyStr] = TempData[i];
		}
	}

	delete TempData;

	//讀取房屋能量
	{
		_HouseEnergyCost.clear();

		HouseEnergyCostStruct temp;
		char Key[256];
		int index = 0;
		while (true)
		{
			sprintf(Key, "HouseEnergy%02d", index);
			temp.HouseEnergy = g_ObjectData->GetMoneyKeyValue(Key, 0);
			temp.AccountMoneyCost = g_ObjectData->GetMoneyKeyValue(Key, 1);

			if (temp.HouseEnergy == 0 || temp.AccountMoneyCost == 0)
			{
				break;
			}

			_HouseEnergyCost.push_back(temp);

			++index;
		}
	}

	return true;
}


bool	ObjectDataClass::SetDBStringTable(int Count, int DataSize, char* Data)
{
	if (Count <= 0)
	{
		return false;
	}
	_DBStringMap.clear();

	SysKeyStrChangeStruct* TempData = NEW(SysKeyStrChangeStruct[Count]);
	MyLzoClass myZip;
	int RetSize = myZip.Decode(Data, DataSize, Count * sizeof(SysKeyStrChangeStruct), (char*)TempData);

	if (RetSize == 0)
	{
		Print(LV2, "ObjectDataClass", "SetDBStringTable RetSize = 0 DataError!!\n");
		return false;
	}
	else
	{
		for (int i = 0; i < Count; i++)
		{
			// Key String 不能為空白
			if (TempData[i].KeyStr.Size() > 0)
			{
				_DBStringMap[TempData[i].KeyStr.Begin()] = TempData[i].Content.Begin();
			}
		}
	}

	delete TempData;


	//-------------------------------------------------------------------------------------------------------
	// 取得點數商品項目
	//-------------------------------------------------------------------------------------------------------
	{
		char Key[128];
		const char* pType;
		int count = 1;
		while (true)
		{
			sprintf(Key, "SYS_ACCOUNTSTORE_TYPE_%02d", count);

			pType = g_ObjectData->GetString(Key);

			if (pType[0] == '\0')
				break;

			g_ObjectData->_AccountStoreTypeName.push_back(pType);

			++count;
		}
	}

	return true;
}

int	ObjectDataClass::GetMoneyKeyValue(const char* Key, int Pos)
{
	if ((unsigned)Pos >= 4)
		return 0;

	map<string, MoneyExchangeKeyValueStruct>::iterator Iter;
	Iter = _MoneyExchangeKeyValue.find(Key);
	if (Iter == _MoneyExchangeKeyValue.end())
		return 0;

	return Iter->second.Value[Pos];
}

GuildBuildingInfoTableStruct* ObjectDataClass::GuildBuilding(int ID)
{
	map< int, GuildBuildingInfoTableStruct >::iterator Iter;
	Iter = _GuildBuildingTable.find(ID);
	if (Iter == _GuildBuildingTable.end())
		return NULL;
	return &(Iter->second);
	//	if( unsigned(ID) >= _GuildBuildingTable.size() )
	//		return NULL;

	//	return &_GuildBuildingTable[ID];
}

bool ObjectDataClass::LoadSpecialCharacterName(const char* filename)
{
	if (filename == NULL)
		return false;

	// 關鍵字串比對
	FILE* fptr = fopen(filename, "rt");
	if (fptr == NULL)
		return false;

	char buf[512];
	bool UTF8_BOM = true;
	while (!feof(fptr)) {
		if (fgets(buf, 512, fptr) != 0) {
			char pattern[512];
			int patternLength = 0;

			int len = 0;
			if (UTF8_BOM) {
				UTF8_BOM = false;
				if ((unsigned char)buf[0] == 0xEF && (unsigned char)buf[1] == 0xBB && (unsigned char)buf[2] == 0xBF) {
					len = 3;
				}
			}

			// 跳過空白區域				
			while ((unsigned char)buf[len] < ' ') {
				len++;
			}

			// 取得合法字元
			while (buf[len]) {
				if ((unsigned char)buf[len] > ' ') {
					pattern[patternLength] = ::tolower(buf[len]);
					patternLength++;
				}
				len++;
			}
			pattern[patternLength] = 0;

			if (patternLength > 0)
			{
				// 新增一筆字串
				_SpecialName.push_back(Utf8ToWchar(pattern));
			}
		}
	}
	fclose(fptr);
	return true;
}

// 轉換大寫
wchar_t Func_ToUpper(wchar_t ch)
{
	if (ch >= L'a' && ch <= L'z')
		return ch - 32;
	else if (ch >= 0xE0 && ch <= 0xF6)
		return ch - 32;
	else if (ch >= 0xF8 && ch <= 0xFE)
		return ch - 32;
	else if (ch == 0xFF)
		return 0x0178;

	// 土耳其
	else if (ch == 0x011F)
		return 0x011E;
	else if (ch == 0x015F)
		return 0x015E;

	// 越南字元
	else if (ch == 0x0103)
		ch = 0x0102;
	else if (ch == 0x0111)
		ch = 0x0110;
	else if (ch == 0x01a1)
		ch = 0x01a0;
	else if (ch == 0x01b0)
		ch = 0x01af;
	return ch;
}

// 轉換小寫
wchar_t Func_ToLower(wchar_t ch)
{
	if (ch >= L'A' && ch <= L'Z')
		return ch + 32;
	else if (ch >= 0xC0 && ch <= 0xD6)
		return ch + 32;
	else if (ch >= 0xD8 && ch <= 0xDE)
		return ch + 32;
	else if (ch == 0x0178)
		return 0xFF;

	// 土耳其
	else if (ch == 0x011E)
		return 0x011F;
	else if (ch == 0x015E)
		return 0x015F;

	// 越南字元
	else if (ch == 0x0102)
		ch = 0x0103;
	else if (ch == 0x0110)
		ch = 0x0111;
	else if (ch == 0x01a0)
		ch = 0x01a1;
	else if (ch == 0x01af)
		ch = 0x01b0;

	return ch;
}

void Func_RemoveSpaceWord(wstring& inString)
{
	wchar_t temp[512];
	int first = 0;
	int end = (int)inString.length() - 1;
	int size = 0;

	// 找到第一個非空白
	while (inString[first] == L' ')
		first++;

	// 由後方找到第一個非空白位置
	while (inString[end] == L' ' && end > first)
		end--;

	size = end - first + 1;
	memcpy(temp, inString.c_str() + first, sizeof(wchar_t) * size);
	temp[size] = 0;

	inString = temp;
}

CharacterNameRulesENUM ObjectDataClass::CheckGuildNameRules(const char* name, CountryTypeENUM countryType, std::wstring& outString)
{
	if (name == NULL || name[0] == 0)
		return EM_CharacterNameRules_Short;

	switch (countryType)
	{
	case EM_CountryType_ENUS:
	case EM_CountryType_ENEU:
	case EM_CountryType_AUS:
		//case EM_CountryType_TR:
	case EM_CountryType_FR:
	case EM_CountryType_DE:
	case EM_CountryType_ES:
	case EM_CountryType_NL:
	case EM_CountryType_RO:
		//case EM_CountryType_PH:
	case EM_CountryType_ID:
	case EM_CountryType_IT:
		break;

	default:
		return CheckCharacterNameRules(name, countryType, outString);
	}

	wstring wtName = Utf8ToWchar(name);

	// 去除空白
	Func_RemoveSpaceWord(wtName);

	// 字串長度
	int nameLength = (int)wtName.length();
	if (nameLength == 0)
		return EM_CharacterNameRules_EndLetter;
	// 貝份原始資料
	wstring wtNameBack = wtName;

	wchar_t wch;

	// 轉換小寫
	for (int i = 0; i < wtName.length(); i++)
		wtName[i] = tolowerUnic(wtName[i]);


	// 特別字符串結尾不能使用
	switch (wtName[nameLength - 1])
	{
	case '\'':
	case '\"':
		return EM_CharacterNameRules_EndLetter;			// 不合法字元
	}

	/*
	// 判斷特別字元
	for ( unsigned int i = 0; i < wtName.length(); i++ )
	{
		if ( wtName[i] < 128 )
		{
			// 數字 英文字母
			if ( (wtName[i] >= L'0' && wtName[i] <= L'9') || (wtName[i] >= L'a' && wtName[i] <= L'z') )
				continue;

			return EM_CharacterNameRules_Wrongful;			// 不合法字元
		}
	}
	*/

	// 判斷語言
	switch (countryType)
	{
	case EM_CountryType_ENUS:
	case EM_CountryType_ENEU:
	case EM_CountryType_AUS:
		//case EM_CountryType_TR:
	case EM_CountryType_FR:
	case EM_CountryType_DE:
	case EM_CountryType_ES:
	case EM_CountryType_NL:
	case EM_CountryType_RO:
		//case EM_CountryType_PH:
	case EM_CountryType_ID:
	case EM_CountryType_IT:

		// 檢查字串長度
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;
		if (nameLength > 24)
			return EM_CharacterNameRules_Long;

		// 檢查合符字元
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if ((wch >= L'a' && wtName[i] <= L'z') || wch == ' ')		// 英文字元區間
				continue;
			else if (wch == 0x178 || (wch >= 0xC0 && wch <= 0xFF))	// 合法歐系字母
				continue;

			return EM_CharacterNameRules_Wrongful;						// 有錯誤的符號或字元
		}

		// 檢查是否含有禁用字串
		for (vector<wstring>::iterator iter = _SpecialName.begin(); iter != _SpecialName.end(); iter++)
		{
			if (wcscmp(iter->c_str(), wtName.c_str()) == 0)
				return EM_CharacterNameRules_Special;
		}

		wtName[0] = toupperUnic(wtName[0]);
		break;
	}

	outString = wtName;

	return EM_CharacterNameRules_Rightful;
}

CharacterNameRulesENUM ObjectDataClass::CheckCharacterNameRules(const char* name, CountryTypeENUM countryType, std::wstring& outString)
{
	if (name == NULL || name[0] == 0)
		return EM_CharacterNameRules_Short;

	wstring wtName = Utf8ToWchar(name);
	wstring wtNameBack = wtName;
	wchar_t wch;
	int nameLength = (int)wtName.length();
	wchar_t	wch_bak = wtName[0];
	bool transLetter = false;						// 轉換字母大小寫

	if (nameLength == 0)
		return EM_CharacterNameRules_Short;

	// 判斷語言
	switch (countryType)
	{
	case EM_CountryType_TW:
	case EM_CountryType_CN:
	case EM_CountryType_JP:
	case EM_CountryType_KR:
	case EM_CountryType_RU:
	case EM_CountryType_TH:
		transLetter = false;
		break;

	case EM_CountryType_ENUS:
	case EM_CountryType_ENEU:
	case EM_CountryType_BR:
	case EM_CountryType_TR:
	case EM_CountryType_SA:
	case EM_CountryType_FR:
	case EM_CountryType_PT:
	case EM_CountryType_PL:
	case EM_CountryType_DE:
	case EM_CountryType_SG:
	case EM_CountryType_NL:
	case EM_CountryType_ES:
	case EM_CountryType_RO:
	case EM_CountryType_PH:
	case EM_CountryType_ID:
	case EM_CountryType_VN:
	case EM_CountryType_AE:
	case EM_CountryType_AUS:
	case EM_CountryType_IT:
		transLetter = true;
		break;
	}

	// 轉換小寫
	for (int i = 0; i < nameLength; i++)
		wtName[i] = tolowerUnic(wtName[i]);

	// 特別字符串結尾不能使用
	switch (wtName[nameLength - 1])
	{
	case '\'':
	case '\"':
		return EM_CharacterNameRules_EndLetter;			// 不合法字元
	}

	// 判斷特別字元
	for (unsigned int i = 0; i < wtName.length(); i++)
	{
		if (wtName[i] < 128)
		{
			// 數字 英文字母 '`^
			if ((wtName[i] >= L'0' && wtName[i] <= L'9') || (wtName[i] >= L'a' && wtName[i] <= L'z') || wtName[i] == '\'' || wtName[i] == '`' || wtName[i] == '^')
				continue;

			return EM_CharacterNameRules_Wrongful;			// 不合法字元
		}
	}

	// 判斷語言
	switch (countryType)
	{
	case EM_CountryType_TW:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (wch > 0x80)
			{
				// 非中文字元
				if (wch < 0x4e00 || wch > 0x9fa5)
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				nameLength += 2;
			}
			else
			{
				if (wch >= L'0' && wch <= L'9' || wch >= L'a' && wch <= L'z') {
					nameLength++;
				}
				else {
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				}
			}
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;
		break;

	case EM_CountryType_CN:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (wch > 0x80)
			{
				// 非中文字元
				if (wch < 0x4e00 || wch > 0x9fa5)
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				nameLength += 2;
			}
			else
			{
				if (wch >= L'0' && wch <= L'9' || wch >= L'a' && wch <= L'z') {
					nameLength++;
				}
				else {
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				}
			}
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;
		break;

		// 越南
	case EM_CountryType_VN:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];

			// 數字合法
			if (wch >= L'0' && wch <= L'9')
			{
			}
			// 英文合法
			if (wch >= L'a' && wch <= L'z')
			{
			}
			// 越南文字
			else if (wch == 0x0103 || wch == 0x00e2 || wch == 0x0111 || wch == 0x00ea || wch == 0x00f4 || wch == 0x01a1 || wch == 0x01b0)
			{
			}
			else
				return EM_CharacterNameRules_Wrongful;			// 不合法字元

			// 第一個字元不能為數字
			if (i == 0 && wch >= L'0' && wch <= L'9')
			{
				return EM_CharacterNameRules_Wrongful;			// 不合法字元
			}

			nameLength++;
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;
		break;

	case EM_CountryType_TR:		// 土耳其
	case EM_CountryType_SA:		// 中南美洲
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			// 合法拉丁字母
			if (wch >= L'a' && wch <= L'z')
			{
			}
			// 合法歐系字母
			else if (wch >= 0xC0 && wch <= 0xFF)
			{
			}
			else
			{
				switch (wch)
				{
				case 0x011E:
				case 0x011F:
				case 0x015E:
				case 0x015F:
				case 0x0130:
				case 0x0131:
					break;

				default:
					return EM_CharacterNameRules_Wrongful;				// 有錯誤的符號或字元
				}
			}

			nameLength++;
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;

		break;

	case EM_CountryType_ENUS:
	case EM_CountryType_ENEU:
	case EM_CountryType_BR:
	case EM_CountryType_FR:
	case EM_CountryType_PT:
	case EM_CountryType_PL:
	case EM_CountryType_DE:
	case EM_CountryType_NL:
	case EM_CountryType_ES:
	case EM_CountryType_RO:
	case EM_CountryType_ID:
	case EM_CountryType_AUS:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (i == 0 && wch == L'\'')
				return EM_CharacterNameRules_Wrongful;				// 有錯誤的符號或字元
			// 特別符號
			else if (wch == 0xF7 || wch == 0xD7)
				return EM_CharacterNameRules_Wrongful;				// 有錯誤的符號或字元
			// 合法歐系字母
			else if (wch == 0x178 || wch == L'\'' || (wch >= 0xC0 && wch <= 0xFF))
			{
			}
			else if (wch < L'a' || wch > L'z')
				return EM_CharacterNameRules_Wrongful;				// 有錯誤的符號或字元

			nameLength++;
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;

		break;

	case EM_CountryType_IT:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];

			// 合法拉丁字母
			if (wch >= L'a' && wch <= L'z')
			{
			}
			else if (wch >= L'0' && wch <= L'9')
			{
				if (i == 0)
					return EM_CharacterNameRules_Wrongful;				// 第一個字不能使用符號
			}
			// 合法歐系字母
			else if (wch >= 0xC0 && wch <= 0xFF)
			{
			}
			else
			{
				switch (wch)
				{
					// 合法字元
				case 0x005E:
				case 0x0060:
				case 0x00B4:
				case 0x00DF:
				case 0x0105:
				case 0x010D:
				case 0x0119:
				case 0x0141:
				case 0x0142:
				case 0x0161:
				case 0x017E:
				case 0x0131:
				case 0x015F:
					if (i == 0)
						return EM_CharacterNameRules_Wrongful;			// 第一個字不能使用符號
					break;

				default:
					return EM_CharacterNameRules_Wrongful;				// 有錯誤的符號或字元
				}
			}

			nameLength++;
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;

		break;

	case EM_CountryType_PH:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (wch < L'a' || wch > L'z')
				return EM_CharacterNameRules_Wrongful;				// 有錯誤的符號或字元

			nameLength++;
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;

		break;

	case EM_CountryType_JP:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (wch > 0x80)
			{
				// 特別不能使用字元
				if (wch == 0x3000 || wch == 0xff3b || wch == 0xff3c || wch == 0xff3d || wch == 0xff40)
					return EM_CharacterNameRules_Wrongful;			// 不合法字元

				// 非中文 日文
				if (wch < 0x0800 || wch > 0x9fa5)
				{
					// 半型片假
					if (wch >= 0xff66 && wch <= 0xff9f)
					{
					}
					// 全型英數字
					else if (wch >= 0xff11 && wch <= 0xff5a)
					{
					}
					// 不合法字元
					else
					{
						// 特別字元可以使用
						switch (wch)
						{
						case 0x03c6:
						case 0xff01:
						case 0xff03:
						case 0xff04:
						case 0xff05:
						case 0xff06:
						case 0xff08:
						case 0xff09:
						case 0xff0a:
						case 0xff0b:
						case 0xff0d:
						case 0xff1a:
						case 0xff1b:
						case 0xff1c:
						case 0xff1d:
						case 0xff1e:
						case 0xff1f:
						case 0xff20:
						case 0xff3e:
						case 0xff3f:
						case 0xff5b:
						case 0xff5c:
						case 0xff5d:
						case 0xff5e:
						case 0xffe5:
							break;

						default:
							return EM_CharacterNameRules_Wrongful;
							break;
						}
					}
				}

				nameLength += 2;
			}
			else
			{
				// 第一個字元不能為數字
				if (i == 0 && wch >= L'0' && wch <= L'9')
				{
					return EM_CharacterNameRules_Wrongful;			// 不合法字元					
				}

				if (wch >= L'0' && wch <= L'9' || wch >= L'a' && wch <= L'z') {
					nameLength++;
				}
				else {
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				}
			}
		}

		// 名稱太短
		if (nameLength < 1)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;
		break;

	case EM_CountryType_KR:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (wch > 0x80)
			{
				// 正常可以使用韓文字元
				if (wch < 0xac00 || wch > 0xd7af)
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				nameLength += 2;
			}
			else
			{
				// 第一個字元不能為數字
				if (i == 0 && wch >= L'0' && wch <= L'9')
				{
					return EM_CharacterNameRules_Wrongful;			// 不合法字元					
				}

				if (wch >= L'0' && wch <= L'9' || wch >= L'a' && wch <= L'z') {
					nameLength++;
				}
				else {
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				}
			}
		}

		// 名稱太短
		if (nameLength < 1)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;
		break;

	case EM_CountryType_SG:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (wch < L'a' || wch > L'z')
				return EM_CharacterNameRules_Wrongful;				// 有錯誤的符號或字元

			nameLength++;
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;
		break;

	case EM_CountryType_RU:
		break;

	case EM_CountryType_AE:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (wch >= 0x600 && wch <= 0x6FF)
				nameLength += 2;
			else if (wch < L'a' || wch > L'z')
				return EM_CharacterNameRules_Wrongful;				// 有錯誤的符號或字元
			else
				nameLength++;
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 16)
			return EM_CharacterNameRules_Long;
		break;

	case EM_CountryType_TH:
		nameLength = 0;
		for (unsigned int i = 0; i < wtName.length(); i++)
		{
			wch = wtName[i];
			if (wch >= 0x0E00 && wch <= 0x0E7F)
			{
				// 泰文字元
				nameLength += 2;
			}
			else if (wch < 0x80)
			{
				// 第一個字元不能為數字
				if (i == 0 && wch >= L'0' && wch <= L'9')
				{
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				}

				if (wch >= L'0' && wch <= L'9' || wch >= L'a' && wch <= L'z') {
					nameLength++;
				}
				else {
					return EM_CharacterNameRules_Wrongful;			// 不合法字元
				}
			}
			else
				return EM_CharacterNameRules_Wrongful;				// 不合法字元
		}

		// 名稱太短
		if (nameLength < 4)
			return EM_CharacterNameRules_Short;

		// 名稱太長
		if (nameLength > 20)
			return EM_CharacterNameRules_Long;
		break;
	}

	size_t begin;
	switch (countryType)
	{
	case EM_CountryType_TW:
	case EM_CountryType_CN:
	case EM_CountryType_JP:
	case EM_CountryType_KR:
	case EM_CountryType_TH:
		for (vector<wstring>::iterator iter = _SpecialName.begin(); iter != _SpecialName.end(); iter++)
		{
			if ((begin = wtName.find(*iter)) != wstring::npos)
				return EM_CharacterNameRules_Special;
		}
		wtName = wtNameBack;
		break;

	case EM_CountryType_SG:
	case EM_CountryType_RU:
	case EM_CountryType_ENUS:
	case EM_CountryType_ENEU:
	case EM_CountryType_BR:
	case EM_CountryType_TR:
	case EM_CountryType_SA:
	case EM_CountryType_FR:
	case EM_CountryType_PT:
	case EM_CountryType_PL:
	case EM_CountryType_DE:
	case EM_CountryType_NL:
	case EM_CountryType_ES:
	case EM_CountryType_RO:
	case EM_CountryType_PH:
	case EM_CountryType_ID:
	case EM_CountryType_AUS:
	case EM_CountryType_IT:
		// 土耳其
		if (countryType == EM_CountryType_TR)
		{
			// 不能含有下列特殊字元
			size_t begin;
			if ((begin = wtName.find(L"k2")) != string::npos)
				return EM_CharacterNameRules_Special;
			if ((begin = wtName.find(L"g1")) != string::npos)
				return EM_CharacterNameRules_Special;
			if ((begin = wtName.find(L"gm")) != string::npos)
				return EM_CharacterNameRules_Special;
		}

		for (vector<wstring>::iterator iter = _SpecialName.begin(); iter != _SpecialName.end(); iter++)
		{
			if (wcscmp(iter->c_str(), wtName.c_str()) == 0)
				return EM_CharacterNameRules_Special;
		}
		break;

	case EM_CountryType_VN:
		for (vector<wstring>::iterator iter = _SpecialName.begin(); iter != _SpecialName.end(); iter++)
		{
			if (wcscmp(iter->c_str(), wtName.c_str()) == 0)
				return EM_CharacterNameRules_Special;
		}
		break;
	}

	if (transLetter)
	{
		for (int i = 0; i < (int)wtName.size(); i++)
		{
			if (i == 0)
				wtName[i] = toupperUnic(wtName[i]);
			else
				wtName[i] = tolowerUnic(wtName[i]);
		}
	}

	outString = wtName;

	//outString[0] = wch_bak;
	return EM_CharacterNameRules_Rightful;
}
//-----------------------------------------------------------------------------------
int ObjectDataClass::QueryCountryID(const char* pszRegionString)
{
	if (pszRegionString == NULL)
		return 0;

	string sRegionString = pszRegionString;

	std::transform(sRegionString.begin(), sRegionString.end(), sRegionString.begin(), ::toupper);

	map< string, int >::iterator it = _CountryNameIDMap.find(sRegionString);

	if (it != _CountryNameIDMap.end())
		return it->second;
	else
		return 0;
}
int ObjectDataClass::QueryLanguageID(const char* pszRegionString)
{
	if (pszRegionString == NULL)
		return 0;

	string sRegionString = pszRegionString;

	std::transform(sRegionString.begin(), sRegionString.end(), sRegionString.begin(), ::toupper);

	map< string, int >::iterator it = _LanguageNameIDMap.find(sRegionString);

	if (it != _LanguageNameIDMap.end())
		return it->second;
	else
		return 0;
}
//-----------------------------------------------------------------------------------
string ObjectDataClass::QueryLanguageString(int iID)
{
	string sReturnString = "TW";

	for (map< string, int >::iterator it = _LanguageNameIDMap.begin(); it != _LanguageNameIDMap.end(); it++)
	{
		if (it->second == iID)
		{
			sReturnString = it->first;
			break;
		}
	}

	return sReturnString;
}
string ObjectDataClass::QueryCountryString(int iID)
{
	string sReturnString = "TW";

	for (map< string, int >::iterator it = _CountryNameIDMap.begin(); it != _CountryNameIDMap.end(); it++)
	{
		if (it->second == iID)
		{
			sReturnString = it->first;
			break;
		}
	}

	return sReturnString;
}
//-----------------------------------------------------------------------------------
void ObjectDataClass::SetLanguageID(int iDataID, int iStringID)
{
	m_iDataLanguageID = iDataID;
	m_iStringLanguageID = iStringID;
}
//-----------------------------------------------------------------------------------
int ObjectDataClass::GetDataLanguageID()
{
	return m_iDataLanguageID;
}
//-----------------------------------------------------------------------------------
int ObjectDataClass::GetStringLanguageID()
{
	return m_iStringLanguageID;
}
//-----------------------------------------------------------------------------------
bool  ObjectDataClass::InitRuneEngine(const char* FilePath)
{
	bool	Ret = true;

	Print(LV5, "ObjectDataClass::InitRuneEngine", "RuInitialize_Core()");
	if (RuInitialize_Core() == FALSE)
	{
		Print(LV2, "ObjectDataClass::InitRuneEngine", "RuInitialize_Core() == FALSE ");
		Ret = false;
	}
	Print(LV5, "ObjectDataClass::InitRuneEngine", "RuInitialize_NULL()");
	// Initialize NULL engine
	if (RuInitialize_NULL() == FALSE)
	{
		Print(LV2, "ObjectDataClass::InitRuneEngine", "RuInitialize_NULL() == FALSE ");
		Ret = false;
	}

	// Setup resource manager's root path
	g_ruResourceManager->SetRootDirectory(FilePath);

	return Ret;
}
//-----------------------------------------------------------------------------------

int ObjectDataClass::GetLanguageType(const char* LanguageStr)
{
	int LanguageType = EM_LanguageType_TW;

	LanguageType = (LanguageTypeENUM)QueryLanguageID(LanguageStr);

	return LanguageType;
}

void ObjectDataClass::SetCharacterEquipColor(GameObjDbStruct* itemDB, GameObjectImageArmorPosENUM armorPos, int color[2])
{
	if (itemDB)
	{
		GameObjDbStruct* imageDB = g_ObjectData->GetObj(itemDB->ImageID);
		if (imageDB && imageDB->Image.ArmorPos == armorPos)
		{
			if (color[0] == 0)
				color[0] = imageDB->Image.ArmorMainColor.Color;
			if (color[1] == 0)
				color[1] = imageDB->Image.ArmorOffColor.Color;
		}
	}
}

Sex_ENUM ObjectDataClass::GetModelSex(const char* filename)
{
	//Sex_ENUM sex = EM_Sex_Boy;

	// 找尋最後一個'\' or '/' 符號
	//const char* lastflash = strrchr(filename, '\\');
	//if ( lastflash == NULL )
	//	lastflash = strrchr(filename, '/');

	//if ( lastflash )
	//{
	//	lastflash = lastflash + 1;

	//	// 判斷是否為女性
	//	if ( stricmp( lastflash, "act_dummy_female.ros") == 0 || stricmp( lastflash, "act_hm_female.ros") == 0 )
	//		sex = EM_Sex_Girl;
	//}
	//return sex;

	//"act_dummy_female.ros"
	//"act_hm_female.ros"
	//"act_elf_female.ros"

	const char* szFind = strstr(filename, "female");

	if (szFind)
		return EM_Sex_Girl;

	return EM_Sex_Boy;
}

/*
// 取得某類種子的成長階段能執行的是件
PlantGrowActionTableStruct&	ObjectDataClass::GetPlantGrowAction( int Type , int GrowRate )
{
	static PlantGrowActionTableStruct EmptyTable;
	memset( &EmptyTable , 0 , sizeof(EmptyTable) );

	if( Type >= (int)_PlantGrowActionTable.size() )
		return EmptyTable;

	vector< PlantGrowActionTableStruct >& Vec = _PlantGrowActionTable[Type];
	for( int i = 0 ; i < (int)Vec.size() ;i++ )
	{
		if( Vec[i].GrowRate >= GrowRate )
			return Vec[i];
	}
	return EmptyTable;
}
*/

PlantProductTableStruct& ObjectDataClass::GetPlantProduct(int SeedObjID, int Env_GrowPoint, int Env_RarePoint)
{
	//map< int , vector<PlantProductTableStruct> >	_PlantProductTable;
	vector<PlantProductTableStruct>& ProductList = _PlantProductTable[SeedObjID];

	for (unsigned i = 0; i < ProductList.size(); i++)
	{
		if (ProductList[i].Env_GrowPoint <= Env_GrowPoint && ProductList[i].Env_RarePoint <= Env_RarePoint)
		{
			return ProductList[i];
		}
	}

	static PlantProductTableStruct EmptyTable;
	memset(&EmptyTable, 0, sizeof(EmptyTable));
	return EmptyTable;
}
//////////////////////////////////////////////////////////////////////////
//取得寵物學習法術表
CultivatePetLearnMagicTableStruct* ObjectDataClass::GetCultivatePetLearnMagic(int MagicID, int MagicLv)
{
	for (unsigned int i = 0; i < _CultivatePetLearnMagicTable.size(); i++)
	{
		if (_CultivatePetLearnMagicTable[i].MagicID == MagicID
			&& _CultivatePetLearnMagicTable[i].PowerLv == MagicLv)
			return &_CultivatePetLearnMagicTable[i];
	}
	return NULL;
}

//取得寵物學習法術表
CultivatePetLearnMagicTableStruct* ObjectDataClass::GetNextCultivatePetLearnMagic(int MagicID, int PreMagicLv)
{

	for (unsigned int i = 0; i < _CultivatePetLearnMagicTable.size(); i++)
	{
		if (_CultivatePetLearnMagicTable[i].MagicID == MagicID
			&& _CultivatePetLearnMagicTable[i].PowerLv > PreMagicLv)
			return &_CultivatePetLearnMagicTable[i];
	}

	return NULL;
}

//取得所有語系物件明稱字串 
void	ObjectDataClass::GetNameAllLanguage(map<int, NameAllLanguageStruct>& Result)
{
	Result.clear();
	char	Buf[1024];
	char	FilePath[1024];

	Print(LV2, "ObjectDataClass", "GetNameAllLanguage \n", Buf); fflush(NULL);

	for (int i = 0; i < EM_LanguageType_Count; i++)
	{
		fstream file;
		sprintf(FilePath, "data\\string_%s.db", ObjectDataClass::QueryLanguageString(i).c_str());
		BOOL Ret = g_ruResourceManager->FileExists(FilePath);
		if (Ret == FALSE)
			continue;

		StaticString<64>	KeyStr;
		int					ContentSize;
		char				Content[8192];

		int					iStreamSize = 0;
		char* ptr = NULL;

		IRuStream* pStream = g_ruResourceManager->LoadStream(FilePath);

		if (pStream == NULL)
			continue;

		iStreamSize = pStream->GetStreamSize();
		ptr = (char*)pStream->OpenStreamMapping();

		while (iStreamSize > 0)
		{
			memcpy((char*)KeyStr.Begin(), ptr, sizeof(KeyStr));
			ptr += sizeof(KeyStr);
			iStreamSize -= sizeof(KeyStr);

			memcpy((char*)&ContentSize, ptr, sizeof(ContentSize));
			ptr += sizeof(ContentSize);
			iStreamSize -= sizeof(ContentSize);

			memcpy((char*)Content, ptr, ContentSize);
			ptr += ContentSize;
			iStreamSize -= ContentSize;

			char* temp = Content;
			char* end = Content + ContentSize - 1;
			while (true)
			{
				char* space = (char*)strstr(temp, "\\32");
				if (space)
				{
					space[0] = ' ';
					memmove(space + 1, space + 3, end - space);
					temp = space + 1;
					ContentSize -= 2;
				}
				else
					break;
			}

			/////////////////////////////////////////////////
			//分析KeyStr
			int GUID = atol(KeyStr.Begin() + 3);
			if (stricmp(KeyStr.Begin() + 10, "name") != 0)
				continue;

			if (ContentSize == 0)
				continue;

			//物品 符文 
			if ((GUID >= Def_ObjectClass_Item && GUID < Def_ObjectClass_Table)	//物品
				|| (GUID >= Def_ObjectClass_Rune && GUID < Def_ObjectClass_Title)	//符文
				|| (GUID >= Def_ObjectClass_Recipe && GUID < Def_ObjectClass_Mine)	//配方表
				|| (GUID >= 770000))
			{
				NameAllLanguageStruct& Info = Result[GUID];
				Info.GUID = GUID;
				Info.Language[i] = Content;
			}
		}

		delete pStream;
	}
}

CultivatePetLifeSkillTableStruct* ObjectDataClass::GetCultivatePetLifeSkill(int index)
{
	if (index >= 0 && index < (int)_CultivatePetLifeSkillTable.size())
		return &_CultivatePetLifeSkillTable[index];
	return NULL;
}

//把物件資料初始化
void		ObjectDataClass::St_InitObjFunc(void* ClassObj, void* _ObjDB)
{
	ObjectDataClass* This = (ObjectDataClass*)ClassObj;
	GameObjDbStructEx* ObjDB = (GameObjDbStructEx*)_ObjDB;
	This->SetStringLink(ObjDB);
	This->SetQuestLink(ObjDB);

}
//////////////////////////////////////////////////////////////////////////
void ObjectDataClass::SetStringLink(GameObjDbStructEx* ObjDB)
{
	DB_GameObjDbStruct* TempDBObj = NULL;
	ReaderClass< DB_GameObjDbStruct >* Reader;
	char	KeyStr[256];
	char** OutStr;

	// 取代配方表名稱
	if (ObjDB == NULL)
		return;

	UINT64 ObjectOffest = UINT64(&TempDBObj->GameData);

	if ((unsigned)ObjDB->Classification >= EM_ObjectClass_Max)
		return;


	if (ObjDB->Classification == EM_ObjectClass_Item)
	{
		if ((unsigned)ObjDB->Item.Class >= EM_Item_Max)
			return;
		Reader = _Reader_Item[ObjDB->Item.Class];
	}
	else
		Reader = _Reader_Base[ObjDB->Classification];

	if (Reader == NULL)
		return;

	vector< ReadInfoStruct >& ReadInfoList1 = Reader->ListByType(ENUM_Type_CharStringPoint);
	for (int i = 0; i < (int)ReadInfoList1.size(); i++)
	{
		ReadInfoStruct* IDData = &ReadInfoList1[i];
		OutStr = (char**)(IDData->Point + INT64(ObjDB) - ObjectOffest);
		sprintf(KeyStr, "Sys%d_%s", ObjDB->GUID, IDData->ValueName.c_str());
		*OutStr = (char*)GetString(KeyStr);
	}
	vector< ReadInfoStruct >& ReadInfoList2 = Reader->ListByType(ENUM_Type_TextDataPoint);
	for (int i = 0; i < (int)ReadInfoList2.size(); i++)
	{
		ReadInfoStruct* IDData = &ReadInfoList2[i];
		OutStr = (char**)(IDData->Point + INT64(ObjDB) - ObjectOffest);
		sprintf(KeyStr, "Sys%d_%s", ObjDB->GUID, IDData->ValueName.c_str());
		*OutStr = (char*)GetString(KeyStr);
	}

}
//////////////////////////////////////////////////////////////////////////
void ObjectDataClass::SetQuestLink(GameObjDbStructEx* pQuestObj)
{
	GameObjDbStructEx* pObj;
	//GameObjectQuestDetail	*pQuest;
	GameObjectNpcStruct* pNPC;


	int iGuid = pQuestObj->GUID;

	/*
	if( pQuestObj->IsNPC() != false )
	{
		ZeroMemory( pQuestObj->NPC.iQuestLinkObj, sizeof( pQuestObj->NPC.iQuestLinkObj ) );
	}
	*/

	if (!pQuestObj->IsQuest())
	{
		if (pQuestObj->IsQuestNPC())
		{
			if (pQuestObj->NPC.iQuestNPCRefID != 0)
			{
				GameObjectNpcStruct RefObj;
				pObj = _ObjectDB->GetObj(pQuestObj->NPC.iQuestNPCRefID);

				if (pObj)
				{
					pNPC = &pQuestObj->NPC;

					RefObj = *pNPC;
					memcpy(RefObj.iQuestID, pNPC->iQuestID, (sizeof(int) * 20));

					*pNPC = pObj->NPC;

					pNPC->iQuestMode = RefObj.iQuestMode;
					pNPC->szQuestNpcText = RefObj.szQuestNpcText;
					pNPC->szQuestDoingText = RefObj.szQuestDoingText;
					//						pNPC->szLuaScript		= RefObj.szLuaScript;
					pNPC->iQuestNPCRefID = RefObj.iQuestNPCRefID;
					//pNPC->KeepDistance		= RefObj.KeepDistance;
					//pNPC->OrgWeaponType		= RefObj.OrgWeaponType;
					//pNPC->OrgArmorType		= RefObj.OrgArmorType;
					//pNPC->FollowRange		= RefObj.FollowRange;
					pNPC->TitleName = RefObj.TitleName;
					pNPC->ZoneID = RefObj.ZoneID;
					pNPC->X = RefObj.X;
					pNPC->Y = RefObj.Y;
					pNPC->Z = RefObj.Z;
					pNPC->_NpcMode = RefObj._NpcMode;
					pNPC->OtherFlag = RefObj.OtherFlag;

					memcpy(pNPC->szLuaScript, RefObj.szLuaScript, (sizeof(RefObj.szLuaScript)));
					memcpy(pNPC->szLuaInitScript, RefObj.szLuaInitScript, (sizeof(RefObj.szLuaInitScript)));
					memcpy(pNPC->iQuestID, RefObj.iQuestID, (sizeof(int) * 20));
					memcpy(pNPC->MainHandEQ, RefObj.MainHandEQ, (sizeof(int) * _MAX_NPC_EQSEL_COUNT_));
					memcpy(pNPC->SecHandEQ, RefObj.SecHandEQ, (sizeof(int) * _MAX_NPC_EQSEL_COUNT_));
					memcpy(pNPC->szLuaDisplayScript, RefObj.szLuaDisplayScript, (sizeof(RefObj.szLuaDisplayScript)));
					pNPC->SearchGroupID = RefObj.SearchGroupID;
				}
			}
			else
			{
				pNPC = &pQuestObj->NPC;
				pNPC->Init();

				pNPC->Race = 0;
				pNPC->Vocation = 1;
				pNPC->Sex = 0;
				pNPC->ExpRate = 0;
				pNPC->Level = 1;
				pNPC->LevelRange = 1;
				pNPC->STR = 10;
				pNPC->STA = 10;
				pNPC->INT = 10;
				pNPC->MND = 10;
				pNPC->AGI = 10;
				pNPC->MoveSpeed = 100;
				pNPC->SearchRange = 15;
				pNPC->DropMoney_Base = 0;
				pNPC->DropMoney_Rand = 0;
				pNPC->Brave = 0;
				pNPC->LinkType = 0;
				pNPC->EscapeType = 0;
				pNPC->OrgWeaponType = EM_Weapon_Unarmed;
				pNPC->OrgArmorType = EM_Armor_HeavyArmed;
				pNPC->FollowRange = 100;
				pNPC->Effect._Value[0] = 0;
				pNPC->Effect._Value[1] = 0;
			}
		}
	}
	else
	{
		/*
		pQuest = &pQuestObj->QuestDetail;

		// 檢查任務中有無需要宰殺怪物, 若有則取出該 NPC 資料, 並設入任務關聯

		for( int i = 0; i < _MAX_SIZE_QUEST_ITEM; i++ )
		{
			// 指定打倒目標計數

			int iObjID = pQuest->iRequest_KillID[ i ];

			if( iObjID != 0 )
			{
				pObj = _ObjectDB->GetObj( iObjID );

				if( pObj )
				{
					bool bLinked = false;
					pNPC = &( pObj->NPC );

					// 先檢查該 NPC 是否已有建立與該任務的關連
					for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
					{
						if( pNPC->iQuestLinkObj[ i ] == pQuestObj->GUID )
						{
							bLinked = true;
							break;
						}
					}

					if( bLinked == false )
					{
						for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
						{
							if( pNPC->iQuestLinkObj[ i ] == 0 )
							{
								bLinked = true;
								pNPC->iQuestLinkObj[ i ] = pQuestObj->GUID;
								break;
							}
						}

						if( bLinked == false )
						{
							Print( LV5 , "SetQuestLink" , "NPC[ %d ][ %s ] Correlation with too many quest  ", pObj->GUID, pObj->GetName() );
						}
					}
				}
			}
			else
				break;
		}


		for( int i = 0; i < _MAX_SIZE_QUEST_CONDITION; i++ )
		{
			int iObjID = 0;

			// 打倒目標後有機會得到任務物品

			iObjID = pQuest->iProcess_KilltoGetItem_ObjID[ i ];

			if( iObjID != 0 )
			{
				pObj = _ObjectDB->GetObj( iObjID );

				if( pObj )
				{
					pNPC = &( pObj->NPC );

					bool bLinked = false;

					// 先檢查該 NPC 是否已有建立與該任務的關連
					for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
					{
						if( pNPC->iQuestLinkObj[ i ] == pQuestObj->GUID )
						{
							bLinked = true;
							break;
						}
					}

					if( bLinked == false )
					{
						for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
						{
							if( pNPC->iQuestLinkObj[ i ] == 0 )
							{
								bLinked = true;
								pNPC->iQuestLinkObj[ i ] = pQuestObj->GUID;
								break;
							}
						}

						if( bLinked == false )
						{
							Print( LV5 , "SetQuestLink" , "NPC[ %d ][ %s ]Correlation with too many quest ", pObj->GUID, pObj->GetName() );
						}
					}
				}
			}



			// 點擊物件關連

			iObjID = pQuest->iProcess_ClickToGetItem_ObjID[ i ];

			if( iObjID != 0 )
			{
				pObj = _ObjectDB->GetObj( iObjID );

				if( pObj )
				{
					bool bLinked = false;
					pNPC = &( pObj->NPC );

					// 先檢查該 NPC 是否已有建立與該任務的關連
					for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
					{
						if( pNPC->iQuestLinkObj[ i ] == pQuestObj->GUID )
						{
							bLinked = true;
							break;
						}
					}

					if( bLinked == false )
					{
						for( int i = 0; i < _MAX_QUESTLINK_COUNT; i++ )
						{
							if( pNPC->iQuestLinkObj[ i ] == 0 )
							{
								bLinked = true;
								pNPC->iQuestLinkObj[ i ] = pQuestObj->GUID;
								break;
							}
						}

						if( bLinked == false )
						{
							Print( LV5 , "SetQuestLink" , "NPC[ %d ][ %s ]Correlation with too many quest ", pObj->GUID, pObj->GetName() );
						}
					}
				}
			}
		}
		*/
	}



}
//////////////////////////////////////////////////////////////////////////
//定時處理
void	ObjectDataClass::OnTimeProc()
{
#ifdef _CLIENT
	_ObjectDB->Process();
#endif
}
//////////////////////////////////////////////////////////////////////////
const char* ObjectDataClass::GetSkillValueString(SkillValueTypeENUM value)
{
	switch (value)
	{
	case EM_SkillValueType_None:
		return "";
	case EM_SkillValueType_Unarmed:			//Unarmed空手
		return "UNARMED";
	case EM_SkillValueType_Blade:			// 劍
		return "BLADE";
	case EM_SkillValueType_Dagger:			// 匕首
		return "DAGGER";
	case EM_SkillValueType_Wand:			// 權杖
		return "WAND";
	case EM_SkillValueType_Axe:				// 斧
		return "AXE";
	case EM_SkillValueType_Bludgeon:		// 鎚棍棒
		return "BLUDGEON";
	case EM_SkillValueType_Claymore:		// 雙手劍
		return "CLAYMORE";
	case EM_SkillValueType_Staff:			// 杖
		return "STAFF";
	case EM_SkillValueType_2H_Axe:			// 雙手斧
		return "AXE2H";
	case EM_SkillValueType_2H_Hammer:		// 雙手鎚
		return "HAMMER";
	case EM_SkillValueType_Polearm:			// 槍(長矛)
		return "POLEARM";
	case EM_SkillValueType_Bow:				// 遠程武器 弓
		return "BOW";
	case EM_SkillValueType_CossBow:			// 遠程武器 十字弓
		return "COSSBOW";
	case EM_SkillValueType_Gun:				// 遠程武器 槍
		return "GUN";
	case EM_SkillValueType_Define:			// 防禦
		return "DEFENCE";
	case EM_SkillValueType_BlackSmith:		// 打鐵
		return "BLACKSMITH";
	case EM_SkillValueType_Carpenter:		// 木工
		return "CARPENTER";
	case EM_SkillValueType_MakeArmor:		// 製甲
		return "MAKEARMOR";
	case EM_SkillValueType_Tailor:			// 裁縫
		return "TAILOR";
	case EM_SkillValueType_Cook:			// 烹飪
		return "COOK";
	case EM_SkillValueType_Alchemy:			// 煉金
		return "ALCHEMY";
	case EM_SkillValueType_Mining:			// 挖礦
		return "MINING";
	case EM_SkillValueType_Lumbering:		// 砍材
		return "LUMBERING";
	case EM_SkillValueType_Herblism:		// 採藥
		return "HERBLISM";
	case EM_SkillValueType_Fishing:			// 釣魚
		return "FISH";
	case EM_SkillValueType_Plant:			// 種植
		return "PLANT";
	case EM_SkillValueType_CatchPet:		// 補捉寵物
		return "CATCH";
	}
	return "";
}
//////////////////////////////////////////////////////////////////////////
SkillValueTypeENUM ObjectDataClass::GetSkillValueTypeENUM(const char* keyStr)
{
	static std::map<std::string, SkillValueTypeENUM> SkillTypes;

	if (SkillTypes.empty())
	{
		const char* KeyStr;
		for (int i = EM_SkillValueType_Unarmed; i < EM_SkillValueType_MaxCount; i++)
		{
			KeyStr = GetSkillValueString((SkillValueTypeENUM)i);
			if (KeyStr && KeyStr[0])
			{
				SkillTypes.insert(make_pair(KeyStr, (SkillValueTypeENUM)i));
			}
		}
	}

	std::map<std::string, SkillValueTypeENUM>::iterator pos;
	pos = SkillTypes.find(keyStr);
	if (pos == SkillTypes.end())
		return EM_SkillValueType_None;
	return pos->second;
}
//////////////////////////////////////////////////////////////////////////
//計算物品賭博金額
void	ObjectDataClass::GetGamebleItemStateCost(int Rare, int ItemStateAvgLv, int EqLv, int& OutCost, int& OutCostDiscount)
{
	int		AbilityLvArg = g_ObjectData->GetSysKeyValue("GamebleItemState_AbilityLvArg");
	int		RareArg = g_ObjectData->GetSysKeyValue("GamebleItemState_RareArg");
	int		LvArg = g_ObjectData->GetSysKeyValue("GamebleItemState_LvArg");
	float	DiscountArg = g_ObjectData->GetSysKeyValue("GamebleItemState_Discount") / 100.0f;

	OutCost = (AbilityLvArg * ItemStateAvgLv + RareArg * Rare + LvArg * EqLv + 99) / 100 * 100;
	OutCostDiscount = int(OutCost * DiscountArg);
}
//////////////////////////////////////////////////////////////////////////
int GetBodyObjectIndex(Race_ENUM race, Sex_ENUM sex)
{
	if (race == EM_Race1 && sex == EM_Sex_Boy)
		return 0;
	if (race == EM_Race1 && sex == EM_Sex_Girl)
		return 1;
	if (race == EM_Race2 && sex == EM_Sex_Boy)
		return 2;
	if (race == EM_Race2 && sex == EM_Sex_Girl)
		return 3;
	if (race == EM_Race3 && (sex == EM_Sex_Boy || sex == EM_Sex_Girl))
		return 4;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
vector<int>& ObjectDataClass::GetCreateRoleHair(Race_ENUM race, Sex_ENUM sex)
{
	int id = GetBodyObjectIndex(race, sex);
	return _CreateRoleHair[id];
}
//////////////////////////////////////////////////////////////////////////
vector<int>& ObjectDataClass::GetCreateRoleFace(Race_ENUM race, Sex_ENUM sex)
{
	int id = GetBodyObjectIndex(race, sex);
	return _CreateRoleFace[id];
}
//////////////////////////////////////////////////////////////////////////
vector<int>& ObjectDataClass::GetBeautyShopHair(Race_ENUM race, Sex_ENUM sex)
{
	int id = GetBodyObjectIndex(race, sex);
	return _BeautyShopHair[id];
}
//////////////////////////////////////////////////////////////////////////
vector<int>& ObjectDataClass::GetBeautyShopFace(Race_ENUM race, Sex_ENUM sex)
{
	int id = GetBodyObjectIndex(race, sex);
	return _BeautyShopFace[id];
}
//////////////////////////////////////////////////////////////////////////
void ObjectDataClass::IntegerString(float value, char* inStr, int size)
{
	if (inStr == NULL || size <= 0)
		return;

	double ip;
	float y = (float)(modf(value, &ip));
	if (y < 0.1f)
	{
		sprintf_s(inStr, size, "%d", (int)value);
	}
	else
	{
		sprintf_s(inStr, size, "%.1f", value);
	}
}
//////////////////////////////////////////////////////////////////////////
void ObjectDataClass::SetCommaSymbol(const char* comma)
{
	if (comma == NULL)
		return;

	_comma = comma;
}
//////////////////////////////////////////////////////////////////////////
void ObjectDataClass::InsertComma(int value, char* outStr, int outSize)
{
	if (outStr == NULL)
		return;

	string commaStr;
	while (value > 0 || commaStr.empty())
	{
		char remainderStr[256];
		int quotient = value / 1000;
		int remainder = value % 1000;

		if (quotient > 0)
			sprintf_s(remainderStr, 256, "%03d", remainder);
		else
			sprintf_s(remainderStr, 256, "%d", remainder);

		if (commaStr.empty())
		{
			commaStr = remainderStr;
		}
		else
		{
			char temp[512];
			sprintf_s(temp, 512, "%s%s%s", remainderStr, _comma.c_str(), commaStr.c_str());
			commaStr = temp;
		}

		value = quotient;
	}

	sprintf_s(outStr, outSize, "%s", commaStr.c_str());
}
void ObjectDataClass::InitNotColdownJob()
{
	map<string, int>						_SysKeyValeMap;
	char buf[32];
	int serial;
	ZeroMemory(_NotColdownMagicJob, sizeof(bool) * DEF_MAX_COLDOWN_COUNT_JOB_);
	for (int i = 1; i <= DEF_MAX_COLDOWN_COUNT_JOB_; i++)
	{
		sprintf(buf, "NotColdDown_%d", i);
		serial = GetSysKeyValue(buf);
		if (serial == 0) break;
		if (serial <= 155)
		{//有偏差值的問題要修正
			serial += 5;
		}
		else
		{
			serial += 4;
		}
		_NotColdownMagicJob[serial] = 1;
	}
}
bool ObjectDataClass::GetNotColdownJob(int magicserial)
{
	if (magicserial < 0 || magicserial >= DEF_MAX_COLDOWN_COUNT_JOB_)
	{
		return false;
	}
	return _NotColdownMagicJob[magicserial];
}
/*
vector< ItemCombineStruct >*	ObjectDataClass::GetItemCombineInfo( int hashCode )
{
	map< int , vector< ItemCombineStruct > >::iterator iter;
	iter = _ItemCombineList.find( hashCode );
	if( iter == _ItemCombineList.end() )
		return NULL;
	return &(iter->second);
}
*/

int	ObjectDataClass::GetLifeSkillRareExID(int ItemID, int AddRare)
{
	AddRare--;
	if (unsigned(AddRare) >= 3)
		return 0;
	map< int, LifeSkillRareExStruct >::iterator iter;
	iter = _LifeSkillRareExMap.find(ItemID);
	if (iter == _LifeSkillRareExMap.end())
		return 0;
	return iter->second.RareItemID[AddRare];
}
int	ObjectDataClass::GetGuildHouseReplaceNpc(int type, int orgNpcID)
{
	map< int, map< int, int > >::iterator iter = _GuildHouseNpcReplace.find(type);
	if (iter == _GuildHouseNpcReplace.end())
		return orgNpcID;

	map< int, int >::iterator iterA = iter->second.find(orgNpcID);
	if (iterA == iter->second.end())
		return orgNpcID;

	return iterA->second;
}
GuildHouseNpcReplaceCostTableStruct* ObjectDataClass::GetGuildHouseNpcReplaceCost(int type)
{
	map< int, GuildHouseNpcReplaceCostTableStruct >::iterator iter = _GuildHouseNpcReplaceCost.find(type);
	if (iter == _GuildHouseNpcReplaceCost.end())
		return NULL;
	return &iter->second;
}

//回存寵物卡對應ID 如果回傳-1 表是沒找到
int			ObjectDataClass::GetPetCardID(int orgObjID)
{
	for (unsigned i = 0; i < _PetCardList.size(); i++)
	{
		if (_PetCardList[i] == orgObjID)
		{
			return i;
		}
	}
	return -1;
}
#pragma managed

