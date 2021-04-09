#ifndef _RUANIMTRACK_H_
#define _RUANIMTRACK_H_

// Boost.Variant
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>

// Rune system core
#include "../../Engine/Base/RuCore.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

template <typename DataType>
DataType Interpolate(DataType &key0, DataType &key1, REAL t)
{
	return static_cast<DataType>(((key0 * (1.0f - t)) + (key1 * t)));
}

// ************************************************************************************************************************************************************

class CRuAnimTrackBase
{
protected:
	char*							m_trackCategory;			//!< Category of the track
	char*							m_trackName;				//!< Name of the track

public:
									CRuAnimTrackBase()
									:	m_trackName(NULL),
										m_trackCategory(NULL)
									{
									}

	virtual							~CRuAnimTrackBase()
									{
										delete[] m_trackName;
										delete[] m_trackCategory;
									}

	/************************************************************************/
	/* Base-provided functionality                                          */
	/************************************************************************/

	const char*						GetTrackCategory() const
									{
										if(m_trackCategory)
											return m_trackCategory;

										return "";
									}

	void							SetTrackCategory(const char *trackCategory)
									{
										delete[] m_trackCategory;

										m_trackCategory = ruNEW char [strlen(trackCategory) + 1];
										strcpy(m_trackCategory, trackCategory);
									}

	const char*						GetTrackName() const
									{
										if(m_trackName)
											return m_trackName;

										return "";
									}

	void							SetTrackName(const char *trackName)
									{
										delete[] m_trackName;

										m_trackName = ruNEW char [strlen(trackName) + 1];
										strcpy(m_trackName, trackName);
									}

	/************************************************************************/
	/* Base interface definition                                            */
	/************************************************************************/

	virtual BOOL					CopyKeyByIndex(INT32 index, REAL time) = 0;
	virtual BOOL					DeleteKey(REAL time) = 0;
	virtual BOOL					DeleteKeyByIndex(INT32 index) = 0;
	virtual INT32					GetKeyIndex(REAL time) const = 0;
	virtual BOOL					GetKeyTimeByIndex(INT32 index, REAL *keyTimeOut) const = 0;
	virtual INT32					GetNumKeys() const = 0;
	virtual REAL					GetTrackLength() const = 0;
	virtual INT32					InsertKey(REAL time) = 0;
	virtual BOOL					MoveKey(REAL time, REAL newTime) = 0;
	virtual INT32					MoveKeyByIndex(INT32 index, REAL time) = 0;
	virtual BOOL					ScaleKeys(REAL scale) = 0;
	virtual BOOL					SerializeFrom(IRuStream *stream) = 0;
	virtual BOOL					SerializeTo(IRuStream *stream) = 0;
};

// ************************************************************************************************************************************************************

template <typename KeyType>
class CRuAnimTrack : public CRuAnimTrackBase
{
protected:
	struct KeyDesc
	{
		REAL						m_time;
		KeyType						m_key;

									KeyDesc()
									{
									}

									KeyDesc(REAL time)
									:	m_time(time)
									{
									}

									KeyDesc(REAL time, KeyType key)
									:	m_time(time), m_key(key)
									{
									}
	};

	CRuArrayList<KeyDesc>			m_keys;						//!< Array of keys

public:
									CRuAnimTrack()
									{
									}

	virtual							~CRuAnimTrack()
									{
									}

	/************************************************************************/
	/* Base interface implementation                                        */
	/************************************************************************/

	virtual BOOL					CopyKeyByIndex(INT32 index, REAL time)
									{
										if(index >= 0 && index < m_keys.Count())
										{
											KeyDesc newKeyDesc(time, m_keys[index].m_key);

											// Insert new key
											return InsertKey(newKeyDesc.m_time, newKeyDesc.m_key);
										}

										return FALSE;
									}

	virtual BOOL					DeleteKey(REAL time)
									{
										return DeleteKeyByIndex(GetKeyIndex(time));
									}

	virtual BOOL					DeleteKeyByIndex(INT32 index)
									{
										if(index >= 0 && index < m_keys.Count())
										{
											m_keys.RemoveAt(index);
											return TRUE;
										}

										return FALSE;
									}

	virtual INT32					GetKeyIndex(REAL time) const
									{
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i].m_time == time)
											{
												return i;
											}
										}

										return -1;
									}

	virtual BOOL					GetKeyTimeByIndex(INT32 index, REAL *keyTimeOut) const
									{
										if(index >= 0 && index < m_keys.Count())
										{
											*keyTimeOut = m_keys[index].m_time;
											return TRUE;
										}

										return FALSE;
									}

	virtual INT32					GetNumKeys() const
									{
										return m_keys.Count();
									}

	virtual REAL					GetTrackLength() const
									{
										if(m_keys.Count() > 0)
										{
											return m_keys[m_keys.Count() - 1].m_time;
										}

										return 0.0f;
									}

	virtual INT32					InsertKey(REAL time)
									{
										KeyType defaultKeyValue;

										// Find position to insert the key
										INT32 insertPosition = m_keys.Count();
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i].m_time > time)
											{
												insertPosition = i;
												break;
											}

											// Set default key value to the value of the previous key
											defaultKeyValue = m_keys[i].m_key;
										}

										// Insert key
										m_keys.Insert(insertPosition, KeyDesc(time, defaultKeyValue));

										return insertPosition;
									}

	virtual BOOL					MoveKey(REAL time, REAL newTime)
									{
										return MoveKeyByIndex(GetKeyIndex(time), newTime);
									}

	virtual INT32					MoveKeyByIndex(INT32 index, REAL time)
									{
										if(index >= 0 && index < m_keys.Count())
										{
											KeyDesc newKeyDesc(time, m_keys[index].m_key);

											// Remove old key
											m_keys.RemoveAt(index);

											// Insert new key
											return InsertKey(newKeyDesc.m_time, newKeyDesc.m_key);
										}

										return -1;
									}

	virtual BOOL					ScaleKeys(REAL scale)
									{
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											m_keys[i].m_time *= scale;
										}

										return TRUE;
									}

	virtual BOOL					SerializeFrom(IRuStream *stream)
									{
										// Read number of keys
										INT32 numKeys = m_keys.Count();
										stream->Read(&numKeys, sizeof(INT32));

										// Resize key array
										m_keys.SetArraySize(numKeys);
										m_keys.SetNumEntries(numKeys);

										// Read each time/key pair
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											// Read time
											stream->Read(&m_keys[i].m_time, sizeof(REAL));

											// Read key
											if(::SerializeFrom(stream, &m_keys[i].m_key) == FALSE)
											{
												return FALSE;
											}
										}

										return TRUE;
									}

	virtual BOOL					SerializeTo(IRuStream *stream)
									{
										// Write number of keys
										INT32 numKeys = m_keys.Count();
										stream->Write(&numKeys, sizeof(INT32));

										// Write each time/key pair
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											// Write time
											stream->Write(&m_keys[i].m_time, sizeof(REAL));

											// Write key
											if(::SerializeTo(stream, &m_keys[i].m_key) == FALSE)
											{
												return FALSE;
											}
										}

										return TRUE;
									}

	/************************************************************************/
	/* Additional interfaces                                                */
	/************************************************************************/

	BOOL							CopyTo(CRuAnimTrack<KeyType> *targetAnimTrack)
									{
										targetAnimTrack->m_keys.CloneFrom(m_keys);
										return TRUE;
									}

	INT32							AppendKey(KeyType key)
									{
										REAL insertTime = (m_keys.Count() > 0) ? m_keys[m_keys.Count() - 1].m_time : 0.0f;
										m_keys.Insert(m_keys.Count(), KeyDesc(insertTime, key));

										return m_keys.Count() - 1;
									}

	INT32							InsertKey(REAL time, KeyType &key)
									{
										// Find position to insert the key
										INT32 insertPosition = m_keys.Count();
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i].m_time > time)
											{
												insertPosition = i;
												break;
											}
										}

										// Insert key
										m_keys.Insert(insertPosition, KeyDesc(time, key));

										return insertPosition;
									}

	BOOL							GetKey(REAL time, KeyType *keyOut) const
									{
										return GetKeyByIndex(GetKeyIndex(time), keyOut);
									}

	BOOL							SetKey(REAL time, const KeyType &key)
									{
										return SetKeyByIndex(GetKeyIndex(time), key);
									}

	BOOL							GetKeyByIndex(INT32 index, KeyType *keyOut) const
									{
										if(index >= 0 && index < m_keys.Count())
										{
											*keyOut = m_keys[index].m_key;
											return TRUE;
										}

										return FALSE;
									}

	BOOL							SetKeyByIndex(INT32 index, const KeyType &key)
									{
										if(index >= 0 && index < m_keys.Count())
										{
											m_keys[index].m_key = key;
											return TRUE;
										}

										return FALSE;
									}

	KeyType							GetDiscreteKey(REAL time) const
									{
										INT32 keyIndex = m_keys.Count() - 1;

										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i].m_time > time)
											{
												keyIndex = (i > 0) ? i - 1 : 0;
												break;
											}
										}

										if(keyIndex >= 0)
											return m_keys[keyIndex].m_key;

										KeyType dummyKey;
										return dummyKey;
									}

	KeyType							GetInterpolatedKey(REAL time) const
									{
										INT32 key0Index = 0;
										INT32 key1Index = m_keys.Count();
										REAL t = 0.0f;

										if(m_keys.Count() == 0)
										{
											KeyType dummyKey = KeyType();
											return dummyKey;
										}

										// Find key1
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i].m_time >= time)
											{
												key1Index = i;
												break;
											}
										}

										// Calculate key0
										if(key1Index >= m_keys.Count())
										{
											key0Index = m_keys.Count() - 1;
											key1Index = m_keys.Count() - 1;
											t = 1.0f;
										}
										else if(key1Index == 0)
										{
											key0Index = key1Index;
										}
										else
										{
											key0Index = key1Index - 1;
											t = (time - m_keys[key0Index].m_time) / (m_keys[key1Index].m_time - m_keys[key0Index].m_time);
										}

										// Interpolate
										return Interpolate(m_keys[key0Index].m_key, m_keys[key1Index].m_key, t);
									}

};

// ************************************************************************************************************************************************************

template<>
class CRuAnimTrack<char *> : public CRuAnimTrackBase
{
protected:
	struct KeyDesc
	{
		REAL						m_time;
		char*						m_key;

									KeyDesc()
									:	m_time(0.0f), m_key(NULL)
									{
									}

									KeyDesc(REAL time)
									:	m_time(time)
									{
										INT32 keyLength = static_cast<INT32>(1);
										m_key = ruNEW char [keyLength];
										memset(m_key, 0, keyLength);
									}

									KeyDesc(REAL time, char *key)
									:	m_time(time)
									{
										INT32 keyLength = static_cast<INT32>(strlen(key) + 1);
										m_key = ruNEW char [keyLength];
										memcpy(m_key, key, keyLength);
									}

									~KeyDesc()
									{
										delete[] m_key;
									}

		void						SetKey(const char *key)
									{
										delete[] m_key;

										INT32 keyLength = static_cast<INT32>(strlen(key) + 1);
										m_key = ruNEW char [keyLength];
										memcpy(m_key, key, keyLength);
									}
	};

	CRuArrayList<KeyDesc *>			m_keys;						//!< Array of keys

public:
									CRuAnimTrack()
									{
									}

	virtual							~CRuAnimTrack()
									{
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											delete m_keys[i];
										}
									}

	/************************************************************************/
	/* Base interface implementation                                        */
	/************************************************************************/

	virtual BOOL					CopyKeyByIndex(INT32 index, REAL time)
									{
										if(index >= 0 && index < m_keys.Count())
										{
											KeyDesc *sourceKey = m_keys[index];

											// Insert new key
											BOOL retVal = InsertKey(sourceKey->m_time, sourceKey->m_key);

											return retVal;
										}

										return FALSE;
									}

	virtual BOOL					DeleteKey(REAL time)
									{
										return DeleteKeyByIndex(GetKeyIndex(time));
									}

	virtual BOOL					DeleteKeyByIndex(INT32 index)
									{
										if(index >= 0 && index < m_keys.Count())
										{
											delete m_keys[index];
											m_keys.RemoveAt(index);
											return TRUE;
										}

										return FALSE;
									}

	virtual INT32					GetKeyIndex(REAL time) const
									{
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i]->m_time == time)
											{
												return i;
											}
										}

										return -1;
									}

	virtual BOOL					GetKeyTimeByIndex(INT32 index, REAL *keyTimeOut) const
									{
										if(index >= 0 && index < m_keys.Count())
										{
											*keyTimeOut = m_keys[index]->m_time;
											return TRUE;
										}

										return FALSE;
									}

	virtual INT32					GetNumKeys() const
									{
										return m_keys.Count();
									}

	virtual REAL					GetTrackLength() const
									{
										if(m_keys.Count() > 0)
										{
											return m_keys[m_keys.Count() - 1]->m_time;
										}

										return 0.0f;
									}

	virtual INT32					InsertKey(REAL time)
									{
										// Find position to insert the key
										INT32 insertPosition = m_keys.Count();
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i]->m_time > time)
											{
												insertPosition = i;
												break;
											}
										}

										// Insert key
										m_keys.Insert(insertPosition, ruNEW KeyDesc(time));

										return insertPosition;
									}

	virtual BOOL					MoveKey(REAL time, REAL newTime)
									{
										return MoveKeyByIndex(GetKeyIndex(time), newTime);
									}

	virtual INT32					MoveKeyByIndex(INT32 index, REAL time)
									{
										if(index >= 0 && index < m_keys.Count())
										{
											KeyDesc *oldKey = m_keys[index];

											// Remove old key
											m_keys.RemoveAt(index);

											// Insert new key
											BOOL retVal = InsertKey(time, oldKey->m_key);

											delete oldKey;

											return retVal;
										}

										return -1;
									}

	virtual BOOL					ScaleKeys(REAL scale)
									{
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											m_keys[i]->m_time *= scale;
										}

										return TRUE;
									}

	virtual BOOL					SerializeFrom(IRuStream *stream)
									{
										// Delete old stuff
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											delete m_keys[i];
										}
										m_keys.Clear();

										// Read number of keys
										INT32 numKeys = m_keys.Count();
										stream->Read(&numKeys, sizeof(INT32));

										// Resize key array
										m_keys.SetArraySize(numKeys);

										// Read each time/key pair
										for(INT32 i = 0; i < numKeys; ++i)
										{
											REAL time;
											char *key;

											// Read time
											stream->Read(&time, sizeof(REAL));

											// Read key
											if(::SerializeFrom(stream, &key) == FALSE)
											{
												return FALSE;
											}

											InsertKey(time, key);

											delete[] key;
										}

										return TRUE;
									}

	virtual BOOL					SerializeTo(IRuStream *stream)
									{
										// Write number of keys
										INT32 numKeys = m_keys.Count();
										stream->Write(&numKeys, sizeof(INT32));

										// Write each time/key pair
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											// Write time
											stream->Write(&m_keys[i]->m_time, sizeof(REAL));

											// Write key
											if(::SerializeTo(stream, &m_keys[i]->m_key) == FALSE)
											{
												return FALSE;
											}
										}

										return TRUE;
									}

	/************************************************************************/
	/* Additional interfaces                                                */
	/************************************************************************/

	BOOL							CopyTo(CRuAnimTrack<char *> *targetAnimTrack)
									{
										for(INT32 i = 0; i < targetAnimTrack->m_keys.Count(); ++i)
										{
											delete targetAnimTrack->m_keys[i];
										}

										targetAnimTrack->m_keys.Clear();

										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											targetAnimTrack->m_keys.Add(ruNEW KeyDesc(m_keys[i]->m_time, m_keys[i]->m_key));
										}

										return TRUE;
									}

	INT32							InsertKey(REAL time, char *key)
									{
										// Find position to insert the key
										INT32 insertPosition = m_keys.Count();
										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i]->m_time > time)
											{
												insertPosition = i;
												break;
											}
										}

										// Insert key
										m_keys.Insert(insertPosition, ruNEW KeyDesc(time, key));

										return insertPosition;
									}

	BOOL							GetKey(REAL time, char **keyOut) const
									{
										return GetKeyByIndex(GetKeyIndex(time), keyOut);
									}

	BOOL							SetKey(REAL time, const char *key)
									{
										return SetKeyByIndex(GetKeyIndex(time), key);
									}

	BOOL							GetKeyByIndex(INT32 index, char **keyOut) const
									{
										if(index >= 0 && index < m_keys.Count())
										{
											*keyOut = m_keys[index]->m_key;
											return TRUE;
										}

										return FALSE;
									}

	BOOL							SetKeyByIndex(INT32 index, const char *key)
									{
										if(index >= 0 && index < m_keys.Count())
										{
											m_keys[index]->SetKey(key);
											return TRUE;
										}

										return FALSE;
									}

	char*							GetDiscreteKey(REAL time) const
									{
										INT32 keyIndex = m_keys.Count() - 1;

										for(INT32 i = 0; i < m_keys.Count(); ++i)
										{
											if(m_keys[i]->m_time > time)
											{
												keyIndex = (i > 0) ? i - 1 : 0;
												break;
											}
										}

										if(keyIndex >= 0)
											return m_keys[keyIndex]->m_key;

										return "";
									}

	char*							GetInterpolatedKey(REAL time) const
									{
										return GetDiscreteKey(time);
									}

};

// ************************************************************************************************************************************************************

#pragma managed

#endif
