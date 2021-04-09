#include "../../Core/DataStore/RuDataServer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuDataServer, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuDataServer, "CRuDataServer", "IRuObject")
ruCLASSGUID_DEFINE(CRuDataServer, 0xD2CE5525, 0x60E84ab4, 0x84831C45, 0xECFFDEE3)

// ************************************************************************************************************************************************************

CRuDataServer::CRuDataServer()
:	m_dataStoreRootPath(NULL),
	m_ioBufferSize(4096),
	m_ioBuffer(NULL)
{
	m_ioBuffer = ruNEW BYTE [m_ioBufferSize];

	m_requestNumber = 0;
}

CRuDataServer::~CRuDataServer()
{
	delete[] m_dataStoreRootPath;
	delete[] m_ioBuffer;
}

BOOL CRuDataServer::ClearRequestCount()
{
	m_requestNumber = 0;
	return TRUE;
}

INT32 CRuDataServer::GetRequestCount()
{
	return m_requestNumber;
}

INT32 CRuDataServer::GetClientCount()
{
	return m_clients.Count();
}

BOOL CRuDataServer::StartServer(const char *dataStoreRootPath, INT32 port)
{
	WSADATA wsaData;

	// Initialize Winsock
	if(WSAStartup(MAKEWORD(2, 0), &wsaData) != NO_ERROR)
	{
		return FALSE;
	}

	// Check winsock version.... unnecessary?

	// Construct listening socket
	m_listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(m_listeningSocket == INVALID_SOCKET)
	{
		WSACleanup();

		return FALSE;
	}

	// Bind listening socket to the specified port on all IP addresses
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(port);

	if(bind(m_listeningSocket, (SOCKADDR *) &service, sizeof(service)) != NO_ERROR)
	{
		closesocket(m_listeningSocket);
		WSACleanup();

		return FALSE;
	}

	// Listen
	if(listen(m_listeningSocket, SOMAXCONN) != NO_ERROR)
	{
		closesocket(m_listeningSocket);
		WSACleanup();

		return FALSE;
	}

	// Save the root path
	m_dataStoreRootPath = ruNEW char [strlen(dataStoreRootPath) + 1];
	strcpy(m_dataStoreRootPath, dataStoreRootPath);

	return TRUE;
}

BOOL CRuDataServer::ServiceLoop()
{
	fd_set socketSet;

	FD_ZERO(&socketSet);

	// Append listening socket to the fd_set
	FD_SET(m_listeningSocket, &socketSet);

	// Append client sockets to the fd_set (up to 63 connections)
	for(INT32 i = 0; i < m_clients.Count(); ++i)
	{
		FD_SET(m_clients[i].m_socket, &socketSet);
	}

	// Perform non-blocking select
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	do
	{
		int numAffectedSockets = select(0, &socketSet, NULL, NULL, &timeVal);

		if(numAffectedSockets > 0)
		{
			// Incoming connection is available?
			if(FD_ISSET(m_listeningSocket, &socketSet))
			{
				SOCKET clientSocket = accept(m_listeningSocket, NULL, NULL);
				if(clientSocket != SOCKET_ERROR)
				{
					INT32 index = m_clients.Count();
					m_clients.Add(ClientDescriptor());
					m_clients[index].m_socket = clientSocket;
					m_clients[index].m_dataStoreName = NULL;
					m_clients[index].m_dataStore = NULL;
					m_clients[index].m_packetInProgress = FALSE;
					m_clients[index].m_packetStream = ruNEW CRuMemoryStream();
					m_clients[index].m_packetStream->SetBufferSize(4096);
				}
			}

			// Check client connections
			for(INT32 i = 0; i < m_clients.Count(); ++i)
			{
				// Incoming data from client?
				if(FD_ISSET(m_clients[i].m_socket, &socketSet))
				{
					// If the incoming data begins a packet, read initial packet information
					if(m_clients[i].m_packetInProgress == FALSE)
					{
						int result = recv(m_clients[i].m_socket, reinterpret_cast<char *>(m_clients[i].m_packetStream->GetEndOfStreamPointer()), sizeof(INT32) - m_clients[i].m_packetStream->GetStreamSize(), 0);

						// Connection has terminated
						if(result == 0 || result == SOCKET_ERROR)
						{
							// Remove client
							DropClient(i);
							--i;
							continue;
						}
						else
						{
							// Set stream size
							m_clients[i].m_packetStream->SetStreamSize(m_clients[i].m_packetStream->GetStreamSize() + result);
						}

						// Have we received the entire packet length prefix?
						if(m_clients[i].m_packetStream->GetStreamSize() == sizeof(INT32))
						{
							// Set packet reading status
							m_clients[i].m_packetInProgress = TRUE;

							// Read packet size and resize the packet buffer if necessary
							m_clients[i].m_packetStream->Seek(0, ruSSM_Begin);
							m_clients[i].m_packetStream->Read(&m_clients[i].m_packetSize, sizeof(INT32));

							if(m_clients[i].m_packetStream->GetBufferSize() < (UINT32) m_clients[i].m_packetSize)
							{
								m_clients[i].m_packetStream->SetBufferSize(m_clients[i].m_packetSize);
							}

							// Verify packet size (packet must at least be big enough to contain the packet size DWORD itself!)
							if(m_clients[i].m_packetSize < sizeof(INT32))
							{
								// Remove client
								DropClient(i);
								--i;
								continue;
							}
						}
					}

					// Is a packet incoming?
					if(m_clients[i].m_packetInProgress != false)
					{
						// If the packet is not completely received, continue to retrieve chunks of it
						if(m_clients[i].m_packetStream->GetStreamSize() < (UINT32) m_clients[i].m_packetSize)
						{
							int result = recv(m_clients[i].m_socket, reinterpret_cast<char *>(m_clients[i].m_packetStream->GetEndOfStreamPointer()), m_clients[i].m_packetSize - m_clients[i].m_packetStream->GetStreamSize(), 0);

							// Connection has terminated
							if(result == 0 || result == SOCKET_ERROR)
							{
								// Remove client
								DropClient(i);
								--i;
								continue;
							}
							else
							{
								// Set stream size
								m_clients[i].m_packetStream->SetStreamSize(m_clients[i].m_packetStream->GetStreamSize() + result);
							}
						}

						// If the packet is complete, process it
						if(m_clients[i].m_packetStream->GetStreamSize() == (UINT32) m_clients[i].m_packetSize)
						{
							// Reset packet status
							m_clients[i].m_packetInProgress = FALSE;

							// Process packet
							ProcessPacket(i);
						}
					}
				}
			}
		}
		else
		{
			break;
		}

	} while(TRUE);

	// Check for idle data stores and release them
	for(INT32 i = 0; i < m_dataStores.Count(); ++i)
	{
		if(m_dataStores[i].m_dataStore->GetRefCount() == 1)
		{
			delete[] m_dataStores[i].m_dataStoreName;
			m_dataStores[i].m_dataStore->Flush();
			m_dataStores[i].m_dataStore->Release();

			m_dataStores.RemoveAt(i);
			--i;
		}
	}

	return TRUE;
}

BOOL CRuDataServer::Flush()
{
	for(INT32 i = 0; i < m_dataStores.Count(); ++i)
	{
		m_dataStores[i].m_dataStore->Flush();
	}

	return TRUE;
}

BOOL CRuDataServer::DropClient(INT32 clientIndex)
{
	if(m_clients[clientIndex].m_packetInProgress)
		printf("\rDropped client...packet was in progress...\n");
	else
		printf("\rDropped client...\n");

	// Close client socket
	closesocket(m_clients[clientIndex].m_socket);

	// Release reference to the data store
	if(m_clients[clientIndex].m_dataStore)
		m_clients[clientIndex].m_dataStore->Release();

	delete m_clients[clientIndex].m_packetStream;

	// Remove client from list
	m_clients.RemoveAt(clientIndex);

	return TRUE;
}

CRuDataStore_Disk *CRuDataServer::FindDataStore(const char *dataStoreName)
{
	for(INT32 i = 0; i < m_dataStores.Count(); ++i)
	{
		if(stricmp(dataStoreName, m_dataStores[i].m_dataStoreName) == 0)
		{
			return m_dataStores[i].m_dataStore;
		}
	}

	return NULL;
}

BOOL CRuDataServer::ProcessPacket(INT32 clientIndex)
{
	++m_requestNumber;

	IRuStream *packetStream = m_clients[clientIndex].m_packetStream;

	// Rewind stream
	packetStream->Seek(0, ruSSM_Begin);

	INT32 packetSize;
	if(packetStream->Read(&packetSize, sizeof(INT32)) == sizeof(INT32))
	{
		// Make sure the packet is valid
		if(packetSize > sizeof(INT32))
		{
			INT32 commandCode;
			if(packetStream->Read(&commandCode, sizeof(INT32)) == sizeof(INT32))
			{
				switch(commandCode)
				{
					default:
						{
							printf("Unrecognized command code %d...\n", commandCode);
						}
						
						break;
					case ruDATACLIENTCMD_CREATEDATASTORE:
						{
							if(m_clients[clientIndex].m_dataStore)
								m_clients[clientIndex].m_dataStore->Release();

							INT32 returnValue;

							// Read data store name length
							INT32 dataStoreNameLength;
							packetStream->Read(&dataStoreNameLength, sizeof(INT32));

							if(dataStoreNameLength > 0 && dataStoreNameLength <= 256)
							{
								// Read data store name
								char *dataStoreName = ruNEW char [dataStoreNameLength];
								packetStream->Read(dataStoreName, sizeof(char) * dataStoreNameLength);

								// If the data store is already opened, the create operation fails
								if(FindDataStore(dataStoreName))
								{
									delete[] dataStoreName;

									returnValue = 0;
								}

								// If the data store already exists on-disk, the create operation fails

								// Create the data store
								else
								{
									char *dataStorePath = ruNEW char [dataStoreNameLength + strlen(m_dataStoreRootPath) + 4];
									sprintf(dataStorePath, "%s%s.dsf", m_dataStoreRootPath, dataStoreName);

									INT32 dsIndex = m_dataStores.Count();
									m_dataStores.Add(DataStoreDescriptor());
									m_dataStores[dsIndex].m_dataStoreName = dataStoreName;
									m_dataStores[dsIndex].m_dataStore = ruNEW CRuDataStore_Disk();
									m_dataStores[dsIndex].m_dataStore->Create(dataStorePath);

									m_clients[clientIndex].m_dataStore = m_dataStores[dsIndex].m_dataStore;
									m_clients[clientIndex].m_dataStore->AddRef();

									delete[] dataStorePath;

									returnValue = 1;
								}
							}
							else
							{
								returnValue = 0;
							}

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_OPENDATASTORE:
						{
							if(m_clients[clientIndex].m_dataStore)
								m_clients[clientIndex].m_dataStore->Release();

							INT32 returnValue;

							// Read data store name length
							INT32 dataStoreNameLength;
							packetStream->Read(&dataStoreNameLength, sizeof(INT32));

							if(dataStoreNameLength > 0 && dataStoreNameLength <= 256)
							{
								// Read data store name
								char *dataStoreName = ruNEW char [dataStoreNameLength];
								packetStream->Read(dataStoreName, sizeof(char) * dataStoreNameLength);

								m_clients[clientIndex].m_dataStore = FindDataStore(dataStoreName);
								if(m_clients[clientIndex].m_dataStore)
								{
									m_clients[clientIndex].m_dataStore->AddRef();

									returnValue = 1;

									delete[] dataStoreName;
								}
								else
								{
									char *dataStorePath = ruNEW char [dataStoreNameLength + strlen(m_dataStoreRootPath) + 4];
									sprintf(dataStorePath, "%s%s.dsf", m_dataStoreRootPath, dataStoreName);

									INT32 dsIndex = m_dataStores.Count();
									m_dataStores.Add(DataStoreDescriptor());
									m_dataStores[dsIndex].m_dataStoreName = dataStoreName;
									m_dataStores[dsIndex].m_dataStore = ruNEW CRuDataStore_Disk();
									if(m_dataStores[dsIndex].m_dataStore->Open(dataStorePath) == FALSE)
									{
										delete[] m_dataStores[dsIndex].m_dataStoreName;
										m_dataStores[dsIndex].m_dataStore->Release();
										m_dataStores.RemoveAt(dsIndex);

										m_clients[clientIndex].m_dataStore = NULL;

										returnValue = 0;
									}
									else
									{
										m_clients[clientIndex].m_dataStore = m_dataStores[dsIndex].m_dataStore;
										m_clients[clientIndex].m_dataStore->AddRef();

										returnValue = 1;
									}

									delete[] dataStorePath;
								}
							}
							else
							{
								returnValue = 0;
							}

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_DELETEDATASTORE:
						{
							INT32 returnValue;

							// Read data store name length
							INT32 dataStoreNameLength;
							packetStream->Read(&dataStoreNameLength, sizeof(INT32));

							if(dataStoreNameLength > 0 && dataStoreNameLength <= 256)
							{
								// Read data store name
								char *dataStoreName = ruNEW char [dataStoreNameLength];
								packetStream->Read(dataStoreName, sizeof(char) * dataStoreNameLength);

								// If the data store is open, the call fails
								if(FindDataStore(dataStoreName))
								{
									returnValue = 0;
								}
								else
								{
									char *dataStorePath = ruNEW char [dataStoreNameLength + strlen(m_dataStoreRootPath) + 4];
									sprintf(dataStorePath, "%s%s.dsf", m_dataStoreRootPath, dataStoreName);

									returnValue = (DeleteFile(dataStorePath) != 0);

									delete[] dataStorePath;
								}

								delete[] dataStoreName;
							}
							else
							{
								returnValue = 0;
							}

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_ENUMERATEDATASTORES:
						{
							// Clear temporary stream
							m_tempStream.SetStreamSize(0);

							// Reserve space for number of data stores
							INT32 numDataStores = 0;
							m_tempStream.Write(&numDataStores, sizeof(INT32));

							// Enumerate all .dsf files in the root directory
							char *searchPath = ruNEW char [strlen(m_dataStoreRootPath) + 6];
							sprintf(searchPath, "%s*.dsf", m_dataStoreRootPath);

							HANDLE searchHandle;
							WIN32_FIND_DATA findData;
							BOOL retValue;

							searchHandle = FindFirstFile(searchPath, &findData);
							retValue = TRUE;

							while(searchHandle != INVALID_HANDLE_VALUE && retValue)
							{
								CRuString *filePath = ruNEW CRuString(findData.cFileName);
								filePath->TruncateAt(filePath->LastIndexOf("."));

								INT32 lastSlashIndex = filePath->LastIndexOf("\\");
								if(lastSlashIndex >= 0)
								{
									filePath->TruncateBefore(lastSlashIndex + 1);
								}

								// Write name length
								INT32 dsNameLength = filePath->GetLength() + 1;
								m_tempStream.Write(&dsNameLength, sizeof(INT32));

								// Write name
								m_tempStream.Write((char *) *filePath, dsNameLength);

								++numDataStores;

								// Go to the next file
								retValue = FindNextFile(searchHandle, &findData);
							}

							delete[] searchPath;

							// Write number of data stores
							m_tempStream.Seek(0, ruSSM_Begin);
							m_tempStream.Write(&numDataStores, sizeof(INT32));

							// Return
							PacketReturn_Stream(clientIndex, &m_tempStream);
						}

						break;

					case ruDATACLIENTCMD_CREATETABLE:
						{
							INT32 returnValue;

							// Read table name length
							INT32 tableNameLength;
							packetStream->Read(&tableNameLength, sizeof(INT32));

							if(tableNameLength > 0 && tableNameLength <= 256)
							{
								// Read table name
								char *tableName = ruNEW char [tableNameLength];
								packetStream->Read(tableName, sizeof(char) * tableNameLength);

								// Read table key length
								INT32 keyLength;
								packetStream->Read(&keyLength, sizeof(INT32));

								returnValue = m_clients[clientIndex].m_dataStore->CreateTable(tableName, keyLength);

								delete[] tableName;
							}
							else
							{
								returnValue = -1;
							}

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_DELETETABLE:
						{
							INT32 returnValue;

							// Read table name length
							INT32 tableNameLength;
							packetStream->Read(&tableNameLength, sizeof(INT32));

							if(tableNameLength > 0 && tableNameLength <= 256)
							{
								// Read table name
								char *tableName = ruNEW char [tableNameLength];
								packetStream->Read(tableName, sizeof(char) * tableNameLength);

								returnValue = m_clients[clientIndex].m_dataStore->DeleteTable(tableName);

								delete[] tableName;
							}
							else
							{
								returnValue = -1;
							}

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_GETTABLEHANDLE:
						{
							INT32 returnValue;

							// Read table name length
							INT32 tableNameLength;
							packetStream->Read(&tableNameLength, sizeof(INT32));

							if(tableNameLength > 0 && tableNameLength <= 256)
							{
								// Read table name
								char *tableName = ruNEW char [tableNameLength];
								packetStream->Read(tableName, sizeof(char) * tableNameLength);

								returnValue = m_clients[clientIndex].m_dataStore->GetTableHandle(tableName);

								delete[] tableName;
							}
							else
							{
								returnValue = -1;
							}

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_GETTABLEKEYLENGTH:
						{
							INT32 returnValue;

							// Read table handle
							INT32 tableHandle;
							packetStream->Read(&tableHandle, sizeof(INT32));

							returnValue = m_clients[clientIndex].m_dataStore->GetTableKeyLength(tableHandle);

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_GETNUMTABLES:
						{
							INT32 returnValue;

							returnValue = m_clients[clientIndex].m_dataStore->GetNumTables();

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_GETTABLENAMEBYINDEX:
						{
							const char *returnValue;

							// Read table index
							INT32 tableIndex;
							packetStream->Read(&tableIndex, sizeof(INT32));

							returnValue = m_clients[clientIndex].m_dataStore->GetTableNameByIndex(tableIndex);

							char *valueCopy = ruNEW char [strlen(returnValue) + 1];
							strcpy(valueCopy, returnValue);

							// Return
							PacketReturn_ByteStream(clientIndex, (BYTE *) valueCopy, (INT32)strlen(valueCopy) + 1);

							delete[] valueCopy;
						}

						break;

					case ruDATACLIENTCMD_GETTABLEKEYLENGTHBYINDEX:
						{
							INT32 returnValue;

							// Read table index
							INT32 tableIndex;
							packetStream->Read(&tableIndex, sizeof(INT32));

							returnValue = m_clients[clientIndex].m_dataStore->GetTableKeyLengthByIndex(tableIndex);

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_GETNUMRECORDS:
						{
							INT32 returnValue;

							// Read table handle
							INT32 tableHandle;
							packetStream->Read(&tableHandle, sizeof(INT32));

							returnValue = m_clients[clientIndex].m_dataStore->GetNumRecords(tableHandle);

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;

					case ruDATACLIENTCMD_RETRIEVE:
						{
							// Read table handle
							INT32 tableHandle;
							packetStream->Read(&tableHandle, sizeof(INT32));

							// Read key
							INT32 keyLength = m_clients[clientIndex].m_dataStore->GetTableKeyLength(tableHandle);
							BYTE *key = ruNEW BYTE [keyLength];
							packetStream->Read(key, keyLength);

							IRuStream *stream = m_clients[clientIndex].m_dataStore->Retrieve(tableHandle, key);

							// Return
							PacketReturn_Stream(clientIndex, stream);

							delete[] key;
							delete stream;
						}

						break;

					case ruDATACLIENTCMD_RETRIEVEBYINDEX:
						{
							// Read table handle
							INT32 tableHandle;
							packetStream->Read(&tableHandle, sizeof(INT32));

							// Read index
							INT32 index;
							packetStream->Read(&index, sizeof(INT32));

							IRuStream *stream = m_clients[clientIndex].m_dataStore->RetrieveByIndex(tableHandle, index);

							// Return
							PacketReturn_Stream(clientIndex, stream);

							delete stream;
						}

						break;

					case ruDATACLIENTCMD_RETRIEVEKEYBYINDEX:
						{
							// Read table handle
							INT32 tableHandle;
							packetStream->Read(&tableHandle, sizeof(INT32));

							// Read index
							INT32 index;
							packetStream->Read(&index, sizeof(INT32));

							INT32 keyLength = m_clients[clientIndex].m_dataStore->GetTableKeyLength(tableHandle);
							BYTE *keyStream = ruNEW BYTE [keyLength];

							m_clients[clientIndex].m_dataStore->RetrieveKeyByIndex(tableHandle, index, keyStream);

							// Return
							PacketReturn_ByteStream(clientIndex, keyStream, keyLength);

							delete[] keyStream;
						}

						break;

					case ruDATACLIENTCMD_STORE:
						{
							INT32 returnValue;

							// Read table handle
							INT32 tableHandle;
							packetStream->Read(&tableHandle, sizeof(INT32));

							// Read key
							INT32 keyLength = m_clients[clientIndex].m_dataStore->GetTableKeyLength(tableHandle);
							BYTE *key = ruNEW BYTE [keyLength];
							packetStream->Read(key, keyLength);

							// Read stream length
							INT32 streamLength;
							packetStream->Read(&streamLength, sizeof(INT32));

							// Read stream
							m_tempStream.SetBufferSize(streamLength);
							m_tempStream.SetStreamSize(streamLength);
							packetStream->Read(m_tempStream.GetBuffer(), streamLength);

							returnValue = m_clients[clientIndex].m_dataStore->Store(tableHandle, key, &m_tempStream);

							// Return
							PacketReturn_Integer(clientIndex, returnValue);

							delete[] key;
						}

						break;

					case ruDATACLIENTCMD_DELETE:
						{
							INT32 returnValue;

							// Read table handle
							INT32 tableHandle;
							packetStream->Read(&tableHandle, sizeof(INT32));

							// Read key
							INT32 keyLength = m_clients[clientIndex].m_dataStore->GetTableKeyLength(tableHandle);
							BYTE *key = ruNEW BYTE [keyLength];
							packetStream->Read(key, keyLength);

							returnValue = m_clients[clientIndex].m_dataStore->Delete(tableHandle, key);

							// Return
							PacketReturn_Integer(clientIndex, returnValue);

							delete[] key;
						}

						break;

					case ruDATACLIENTCMD_DELETEALL:
						{
							INT32 returnValue;

							// Read table handle
							INT32 tableHandle;
							packetStream->Read(&tableHandle, sizeof(INT32));

							returnValue = m_clients[clientIndex].m_dataStore->DeleteAllRecords(tableHandle);

							// Return
							PacketReturn_Integer(clientIndex, returnValue);
						}

						break;
				}
			}
		}
	}

	// Clear packet stream
	m_clients[clientIndex].m_packetStream->SetStreamSize(0);

	return FALSE;
}

BOOL CRuDataServer::PacketReturn_Integer(INT32 clientIndex, INT32 value)
{
	// Build packet
	INT32 packetSize = 8;
	INT32 returnCode = value;

	m_outputPacketStream.SetStreamSize(0);
	m_outputPacketStream.Write(&packetSize, sizeof(INT32));
	m_outputPacketStream.Write(&returnCode, sizeof(INT32));

	// Send packet
	if(send(m_clients[clientIndex].m_socket, (const char *) m_outputPacketStream.GetBuffer(), m_outputPacketStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		printf("\rSocket error on return integer...\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CRuDataServer::PacketReturn_Stream(INT32 clientIndex, IRuStream *stream)
{
	// Build packet
	INT32 packetSize = 0;
	INT32 streamLength = -1;

	m_outputPacketStream.SetStreamSize(0);
	m_outputPacketStream.Write(&packetSize, sizeof(INT32));

	if(stream == NULL)
	{
		m_outputPacketStream.Write(&streamLength, sizeof(INT32));
	}
	else
	{
		streamLength = stream->GetStreamSize();
		m_outputPacketStream.Write(&streamLength, sizeof(INT32));

		stream->Seek(0, ruSSM_Begin);
		while(stream->EndOfStream() == FALSE)
		{
			INT32 bytesRead = stream->Read(m_ioBuffer, m_ioBufferSize);

			if(bytesRead > 0)
			{
				m_outputPacketStream.Write(m_ioBuffer, bytesRead);
			}
		}
	}

	// Upate packet size
	packetSize = m_outputPacketStream.GetStreamSize();
	m_outputPacketStream.Seek(0, ruSSM_Begin);
	m_outputPacketStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_clients[clientIndex].m_socket, (const char *) m_outputPacketStream.GetBuffer(), m_outputPacketStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		printf("\rSocket error on return stream...\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CRuDataServer::PacketReturn_ByteStream(INT32 clientIndex, BYTE *byteStream, INT32 streamLength)
{
	// Build packet
	INT32 packetSize = 0;

	m_outputPacketStream.SetStreamSize(0);
	m_outputPacketStream.Write(&packetSize, sizeof(INT32));

	m_outputPacketStream.Write(&streamLength, sizeof(INT32));
	m_outputPacketStream.Write(byteStream, streamLength);

	// Upate packet size
	packetSize = m_outputPacketStream.GetStreamSize();
	m_outputPacketStream.Seek(0, ruSSM_Begin);
	m_outputPacketStream.Write(&packetSize, sizeof(INT32));

	// Send packet
	if(send(m_clients[clientIndex].m_socket, (const char *) m_outputPacketStream.GetBuffer(), m_outputPacketStream.GetStreamSize(), 0) == SOCKET_ERROR)
	{
		printf("\rSocket error on return stream...\n");
		return FALSE;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
