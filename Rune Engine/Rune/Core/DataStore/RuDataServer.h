#ifndef _RUDATASERVER_
#define _RUDATASERVER_

// Base types
#include "../../Core/DataStore/RuDataStore_Disk.h"
#include "../../Core/DataStore/RuDataClient.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuDataServer : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct DataStoreDescriptor
	{
		char*									m_dataStoreName;
		CRuDataStore_Disk*						m_dataStore;
	};

	struct ClientDescriptor
	{
		SOCKET									m_socket;

		char*									m_dataStoreName;
		CRuDataStore_Disk*						m_dataStore;

		BOOL									m_packetInProgress;
		INT32									m_packetSize;
		CRuMemoryStream*						m_packetStream;
	};

	char*										m_dataStoreRootPath;
	SOCKET										m_listeningSocket;					//!< Service socket

	CRuArrayList<DataStoreDescriptor>			m_dataStores;						//!< List of opened data stores
	CRuArrayList<ClientDescriptor>				m_clients;							//!< List of connected clients

	// Workspace
	INT32										m_ioBufferSize;
	BYTE*										m_ioBuffer;
	CRuMemoryStream								m_tempStream;
	CRuMemoryStream								m_outputPacketStream;

	INT32										m_requestNumber;

public:
												CRuDataServer();
	virtual										~CRuDataServer();

	// Metrics
	BOOL										ClearRequestCount();
	INT32										GetRequestCount();
	INT32										GetClientCount();

	// Service functions
	BOOL										StartServer(const char *dataStoreRootPath, INT32 port);
	BOOL										ServiceLoop();
	BOOL										Flush();

protected:
	BOOL										DropClient(INT32 clientIndex);

	CRuDataStore_Disk*							FindDataStore(const char *dataStoreName);
	BOOL										ProcessPacket(INT32 clientIndex);

	BOOL										PacketReturn_Integer(INT32 clientIndex, INT32 value);
	BOOL										PacketReturn_Stream(INT32 clientIndex, IRuStream *stream);
	BOOL										PacketReturn_ByteStream(INT32 clientIndex, BYTE *byteStream, INT32 streamLength);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
