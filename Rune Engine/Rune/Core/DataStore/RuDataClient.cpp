#include "../../Core/DataStore/RuDataClient.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuDataClient, CRuDataStore_Base)
ruHEAP_DEFINE_SUBCLASS(CRuDataClient, "CRuDataClient", "CRuDataStore_Base")
ruCLASSGUID_DEFINE(CRuDataClient, 0xFF1C93C5, 0x0EEB4668, 0xAAD085A8, 0x2E75F511)

// ************************************************************************************************************************************************************

CRuDataClient::CRuDataClient()
:	m_ioBufferSize(4096),
	m_ioBuffer(NULL)
{
	m_ioBuffer = ruNEW BYTE [m_ioBufferSize];
}

CRuDataClient::~CRuDataClient()
{
	delete[] m_ioBuffer;
}

BOOL CRuDataClient::Connect(const char *serverName, INT32 port)
{
	WSADATA wsaData;

	// Initialize Winsock
	if(WSAStartup(MAKEWORD(2, 0), &wsaData) != NO_ERROR)
	{
		return FALSE;
	}

	// Check winsock version.... unnecessary?

	// Construct socket
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(m_socket == INVALID_SOCKET)
	{
		WSACleanup();

		return FALSE;
	}

	// Get remote host information
	hostent *remoteHost = gethostbyname(serverName);

	if(remoteHost && remoteHost->h_addr_list[0])
	{
		// Connect to server
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = *((u_long *) remoteHost->h_addr_list[0]);
		service.sin_port = htons(port);

		if(connect(m_socket, (SOCKADDR *) &service, sizeof(service)) != NO_ERROR)
		{
			closesocket(m_socket);
			WSACleanup();

			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuDataClient::CreateDataStore(const char *dataStoreName)
{
	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_CREATEDATASTORE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));

	INT32 dataStoreNameLength = (INT32)strlen(dataStoreName) + 1;
	m_packetStream.Write(&dataStoreNameLength, sizeof(INT32));
	m_packetStream.Write(dataStoreName, sizeof(char) * dataStoreNameLength);

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		return FALSE;
	}

	// Interpret packet
	INT32 returnValue = 0;
	if(GetReturnPacket_Integer(&returnValue))
	{
		return (returnValue != 0);
	}

	return FALSE;
}

BOOL CRuDataClient::OpenDataStore(const char *dataStoreName)
{
	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_OPENDATASTORE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));

	INT32 dataStoreNameLength = (INT32)strlen(dataStoreName) + 1;
	m_packetStream.Write(&dataStoreNameLength, sizeof(INT32));
	m_packetStream.Write(dataStoreName, sizeof(char) * dataStoreNameLength);

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		return FALSE;
	}

	// Interpret packet
	INT32 returnValue = 0;
	if(GetReturnPacket_Integer(&returnValue))
	{
		return (returnValue != 0);
	}

	return FALSE;
}

BOOL CRuDataClient::DeleteDataStore(const char *dataStoreName)
{
	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_DELETEDATASTORE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));

	INT32 dataStoreNameLength = (INT32)strlen(dataStoreName) + 1;
	m_packetStream.Write(&dataStoreNameLength, sizeof(INT32));
	m_packetStream.Write(dataStoreName, sizeof(char) * dataStoreNameLength);

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		return FALSE;
	}

	// Interpret packet
	INT32 returnValue = 0;
	if(GetReturnPacket_Integer(&returnValue))
	{
		return (returnValue != 0);
	}

	return FALSE;
}

BOOL CRuDataClient::EnumerateDataStores(CRuArrayList<char *> &dataStoresOut)
{
	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_ENUMERATEDATASTORES;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		return FALSE;
	}

	// Interpret packet
	IRuStream *stream = GetReturnPacket_Stream();
	if(stream)
	{
		// Read number of data stores
		INT32 numDataStores = 0;
		stream->Read(&numDataStores, sizeof(INT32));

		// Iterate and read every data store name
		for(INT32 i = 0; i < numDataStores; ++i)
		{
			// Read data store name length including NULL
			INT32 dsNameLength = 0;
			stream->Read(&dsNameLength, sizeof(INT32));

			// Read data store name and append to list
			if(dsNameLength > 0)
			{
				char *dsName = ruNEW char [dsNameLength];
				stream->Read(dsName, dsNameLength);

				dataStoresOut.Add(dsName);
			}
		}

		delete stream;

		return TRUE;
	}

	return FALSE;
}

INT32 CRuDataClient::CreateTable(const char *tableName, INT32 keyLength)
{
	m_cs.Enter();

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_CREATETABLE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));

	INT32 tableNameLength = (INT32)strlen(tableName) + 1;
	m_packetStream.Write(&tableNameLength, sizeof(INT32));
	m_packetStream.Write(tableName, sizeof(char) * tableNameLength);

	m_packetStream.Write(&keyLength, sizeof(INT32));

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return -1;
	}

	// Interpret packet
	INT32 returnValue = -1;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return returnValue;
	}

	m_cs.Leave();
	return -1;
}

BOOL CRuDataClient::DeleteTable(const char *tableName)
{
	m_cs.Enter();

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_DELETETABLE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));

	INT32 tableNameLength = (INT32)strlen(tableName) + 1;
	m_packetStream.Write(&tableNameLength, sizeof(INT32));
	m_packetStream.Write(tableName, sizeof(char) * tableNameLength);

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return FALSE;
	}

	// Interpret packet
	INT32 returnValue = FALSE;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return (returnValue != 0);
	}

	m_cs.Leave();
	return FALSE;
}

INT32 CRuDataClient::GetTableHandle(const char *tableName)
{
	m_cs.Enter();

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_GETTABLEHANDLE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));

	INT32 tableNameLength = (INT32)strlen(tableName) + 1;
	m_packetStream.Write(&tableNameLength, sizeof(INT32));
	m_packetStream.Write(tableName, sizeof(char) * tableNameLength);

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return -1;
	}

	// Interpret packet
	INT32 returnValue = -1;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return returnValue;
	}

	m_cs.Leave();
	return -1;
}

INT32 CRuDataClient::GetTableKeyLength(INT32 tableHandle)
{
	m_cs.Enter();

	// If we already have the table key length stored, return it
	if(tableHandle >= 0 && tableHandle < m_tableKeyLengths.Count() && m_tableKeyLengths[tableHandle] > 0)
	{
		INT32 keyLength = m_tableKeyLengths[tableHandle];
		m_cs.Leave();
		return keyLength;
	}

	// Extend the key length table if necessary
	while(tableHandle >= m_tableKeyLengths.Count())
	{
		m_tableKeyLengths.Add(0);
	}

	// Make sure connection is established

	// Build packet
	INT32 packetSize = sizeof(INT32) * 3;
	INT32 commandCode = ruDATACLIENTCMD_GETTABLEKEYLENGTH;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&tableHandle, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return 0;
	}

	// Interpret packet
	INT32 returnValue = 0;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_tableKeyLengths[tableHandle] = returnValue;
		m_cs.Leave();
		return returnValue;
	}

	m_cs.Leave();
	return 0;
}

INT32 CRuDataClient::GetNumTables()
{
	m_cs.Enter();

	// Build packet
	INT32 packetSize = sizeof(INT32) * 2;
	INT32 commandCode = ruDATACLIENTCMD_GETNUMTABLES;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return 0;
	}

	// Interpret packet
	INT32 returnValue = 0;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return returnValue;
	}

	m_cs.Leave();
	return 0;
}

const char *CRuDataClient::GetTableNameByIndex(INT32 index)
{
	m_cs.Enter();

	// Build packet
	INT32 packetSize = sizeof(INT32) * 3;
	INT32 commandCode = ruDATACLIENTCMD_GETTABLENAMEBYINDEX;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&index, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return 0;
	}

	// Interpret packet
	INT32 returnValue = 0;

	if(GetReturnPacket_ByteStream(m_tempBufferSpace))
	{
		// TODO: THIS IS NOT THREAD-SAFE!!!!!
		m_cs.Leave();
		return (const char *) m_tempBufferSpace;
	}

	m_cs.Leave();
	return NULL;
}

INT32 CRuDataClient::GetTableKeyLengthByIndex(INT32 index)
{
	m_cs.Enter();

	// Build packet
	INT32 packetSize = sizeof(INT32) * 3;
	INT32 commandCode = ruDATACLIENTCMD_GETTABLEKEYLENGTHBYINDEX;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&index, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return 0;
	}

	// Interpret packet
	INT32 returnValue = 0;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return returnValue;
	}

	m_cs.Leave();
	return 0;
}

INT32 CRuDataClient::GetNumRecords(INT32 tableHandle)
{
	m_cs.Enter();

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_GETNUMRECORDS;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&tableHandle, sizeof(INT32));

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return 0;
	}

	// Interpret packet
	INT32 returnValue = 0;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return returnValue;
	}

	m_cs.Leave();
	return 0;
}

IRuStream *CRuDataClient::Retrieve(INT32 tableHandle, const BYTE *key)
{
	m_cs.Enter();

	INT32 keyLength = GetTableKeyLength(tableHandle);

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_RETRIEVE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&tableHandle, sizeof(INT32));

	m_packetStream.Write(key, keyLength);

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return NULL;
	}

	// Interpret packet
	IRuStream *returnStream = GetReturnPacket_Stream();
	m_cs.Leave();
	return returnStream;
}

IRuStream *CRuDataClient::RetrieveByIndex(INT32 tableHandle, INT32 index)
{
	m_cs.Enter();

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_RETRIEVEBYINDEX;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&tableHandle, sizeof(INT32));
	m_packetStream.Write(&index, sizeof(INT32));

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return NULL;
	}

	// Interpret packet
	IRuStream *returnStream = GetReturnPacket_Stream();
	m_cs.Leave();
	return returnStream;
}

BOOL CRuDataClient::RetrieveKeyByIndex(INT32 tableHandle, INT32 index, BYTE *keyOut)
{
	m_cs.Enter();

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_RETRIEVEKEYBYINDEX;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&tableHandle, sizeof(INT32));
	m_packetStream.Write(&index, sizeof(INT32));

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return NULL;
	}

	// Interpret packet
	INT32 returnValue = 0;
	if(GetReturnPacket_ByteStream(keyOut))
	{
		m_cs.Leave();
		return TRUE;
	}

	m_cs.Leave();
	return FALSE;
}

BOOL CRuDataClient::Store(INT32 tableHandle, const BYTE *key, IRuStream *stream)
{
	m_cs.Enter();

	INT32 keyLength = GetTableKeyLength(tableHandle);

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_STORE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&tableHandle, sizeof(INT32));

	m_packetStream.Write(key, keyLength);

	INT32 streamLength = stream->GetStreamSize();
	m_packetStream.Write(&streamLength, sizeof(INT32));

	stream->Seek(0, ruSSM_Begin);
	while(stream->EndOfStream() == FALSE)
	{
		INT32 bytesRead = stream->Read(m_ioBuffer, m_ioBufferSize);

		if(bytesRead > 0)
		{
			m_packetStream.Write(m_ioBuffer, bytesRead);
		}
	}

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	int sendCode = send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0);

	if(sendCode == SOCKET_ERROR || sendCode != m_packetStream.GetStreamSize())
	{
		m_cs.Leave();
		return FALSE;
	}

	// Interpret packet
	INT32 returnValue = FALSE;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return (returnValue != 0);
	}

	m_cs.Leave();
	return FALSE;
}

BOOL CRuDataClient::Delete(INT32 tableHandle, const BYTE *key)
{
	m_cs.Enter();

	INT32 keyLength = GetTableKeyLength(tableHandle);

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_DELETE;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&tableHandle, sizeof(INT32));

	m_packetStream.Write(key, keyLength);

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return FALSE;
	}

	// Interpret packet
	INT32 returnValue = FALSE;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return (returnValue != 0);
	}

	m_cs.Leave();
	return FALSE;
}

BOOL CRuDataClient::DeleteAllRecords(INT32 tableHandle)
{
	m_cs.Enter();

	// Make sure connection is established

	// Build packet
	INT32 packetSize = 0;
	INT32 commandCode = ruDATACLIENTCMD_DELETEALL;

	m_packetStream.SetStreamSize(0);
	m_packetStream.Write(&packetSize, sizeof(INT32));
	m_packetStream.Write(&commandCode, sizeof(INT32));
	m_packetStream.Write(&tableHandle, sizeof(INT32));

	packetSize = m_packetStream.GetStreamSize();
	m_packetStream.Seek(0, ruSSM_Begin);
	m_packetStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_socket, (const char *) m_packetStream.GetBuffer(), m_packetStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		m_cs.Leave();
		return FALSE;
	}

	// Interpret packet
	INT32 returnValue = FALSE;
	if(GetReturnPacket_Integer(&returnValue))
	{
		m_cs.Leave();
		return (returnValue != 0);
	}

	m_cs.Leave();
	return FALSE;
}

BOOL CRuDataClient::ReceivePacket(CRuMemoryStream *packetStream)
{
	// Reset output stream size to zero
	packetStream->SetStreamSize(0);

	// Read four bytes
	while(packetStream->GetStreamSize() < sizeof(INT32))
	{
		int prefixResult = recv(m_socket, reinterpret_cast<char *>(packetStream->GetEndOfStreamPointer()), sizeof(INT32) - packetStream->GetStreamSize(), 0);

		// Connection has terminated
		if(prefixResult == 0 || prefixResult == SOCKET_ERROR)
		{
			// Reset connection
			return FALSE;
		}
		else
		{
			// Set stream size
			packetStream->SetStreamSize(packetStream->GetStreamSize() + prefixResult);
		}
	}

	// Read packet size and resize buffer if necessary
	INT32 packetSize = 0;
	packetStream->Seek(0, ruSSM_Begin);
	packetStream->Read(&packetSize, sizeof(INT32));

	if(packetStream->GetBufferSize() < (UINT32) packetSize)
	{
		packetStream->SetBufferSize(packetSize);
	}

	// Verify packet size (packet must at least be big enough to contain the packet size DWORD itself!)
	if(packetSize < sizeof(INT32))
	{
		// Reset connection
		return FALSE;
	}

	while(packetStream->GetStreamSize() < (UINT32) packetSize)
	{
		int result = recv(m_socket, reinterpret_cast<char *>(packetStream->GetEndOfStreamPointer()), packetSize - packetStream->GetStreamSize(), 0);

		// Connection has terminated
		if(result == 0 || result == SOCKET_ERROR)
		{
			// Reset connection
			return FALSE;
		}
		else
		{
			// Set stream size
			packetStream->SetStreamSize(packetStream->GetStreamSize() + result);
		}
	}

	return TRUE;
}

BOOL CRuDataClient::GetReturnPacket_Integer(INT32 *returnValueOut)
{
	if(ReceivePacket(&m_packetStream))
	{
		// Rewind packet
		m_packetStream.Seek(0, ruSSM_Begin);

		// Read packet size
		INT32 packetSize = 0;
		m_packetStream.Read(&packetSize, sizeof(INT32));

		if(packetSize == 8)
		{
			m_packetStream.Read(returnValueOut, sizeof(INT32));
			return TRUE;
		}
	}

	return FALSE;
}

IRuStream *CRuDataClient::GetReturnPacket_Stream()
{
	if(ReceivePacket(&m_packetStream))
	{
		// Rewind packet
		m_packetStream.Seek(0, ruSSM_Begin);

		// Read packet size
		INT32 packetSize = 0;
		m_packetStream.Read(&packetSize, sizeof(INT32));

		if(packetSize > 4)
		{
			// Read stream length
			INT32 streamLength;
			m_packetStream.Read(&streamLength, sizeof(INT32));

			if(streamLength >= 0)
			{
				// Read stream
				CRuMemoryStream *outputStream = ruNEW CRuMemoryStream();
				outputStream->SetBufferSize(streamLength);
				outputStream->SetStreamSize(streamLength);
				m_packetStream.Read(outputStream->GetBuffer(), streamLength);
				
				return outputStream;
			}
		}
	}

	return NULL;
}

BOOL CRuDataClient::GetReturnPacket_ByteStream(BYTE *byteStreamOut)
{
	if(ReceivePacket(&m_packetStream))
	{
		// Rewind packet
		m_packetStream.Seek(0, ruSSM_Begin);

		// Read packet size
		INT32 packetSize = 0;
		m_packetStream.Read(&packetSize, sizeof(INT32));

		if(packetSize > 4)
		{
			// Read stream length
			INT32 streamLength;
			m_packetStream.Read(&streamLength, sizeof(INT32));

			// Read stream
			m_packetStream.Read(byteStreamOut, streamLength);

			return TRUE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
