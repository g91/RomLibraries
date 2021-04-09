/*!
	@file RuUtility_Profile.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUUTILITY_PROFILE_H_
#define _RUUTILITY_PROFILE_H_

// Base Includes
#include "../../Core/Type/RuType_Object.h"
#include "../../Core/Utility/RuUtility.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	A container class for profile reports. A profile report is a set of string-to-float
	relations that specify profile timings for any number of user-defined entries.

	@author John Tang
	@version 2005/01/10
*/
class CRuProfiler
{
protected:
	struct Probe
	{
		char*								m_name;											//!< Probe name
		char*								m_groupName;									//!< Probe group name
		float								m_value;										//!< Probe value
		LARGE_INTEGER						m_probeStartTime;								//!< Marks beginning of sampling period
		float*								m_histogram;									//!< Histogram of probe samples
		INT32								m_histogramTail;								//!< Tail of the histogram

											Probe()
											:	m_name(NULL),
												m_groupName(NULL),
												m_value(0.0f),
												m_histogram(NULL),
												m_histogramTail(0)
											{
											}

											~Probe()
											{
												delete[] m_name;
												delete[] m_groupName;
												delete[] m_histogram;
											}
	};

	CRuPerformanceTimer*					m_timer;										//!< Timer used for profiling
	CRuArrayList<Probe *>					m_probes;										//!< List of probes
	INT32									m_histogramSize;

public:
											CRuProfiler();
	virtual									~CRuProfiler();

	INT32									CreateProbe(const char *probeName, const char *groupName = "default");
	void									SetHistogramSize(INT32 histogramSize);

	INT32									GetNumProbes() const;
	INT32									GetProbeIndex(const char *probeName, const char *groupName) const;
	const char*								GetProbeName(INT32 index) const;
	const char*								GetProbeGroup(INT32 index) const;

	void									ClearProbeValues(BOOL saveToHistogram = TRUE);

	BOOL									AccumulateProbeValue(INT32 index, float value);
	float									GetProbeValue(INT32 index) const;
	float									GetProbeMeanValue(INT32 index) const;
	BOOL									SetProbeValue(INT32 index, float value);

	BOOL									AccumulateProbeValue(const char *probeName, const char *groupName, float value);
	float									GetProbeValue(const char *probeName, const char *groupName) const;
	float									GetProbeMeanValue(const char *probeName, const char *groupName) const;
	BOOL									SetProbeValue(const char *probeName, const char *groupName, float value);

	BOOL									BeginSample(INT32 index);
	BOOL									EndSample(INT32 index);

	BOOL									BeginSample(const char *probeName, const char *groupName);
	BOOL									EndSample(const char *probeName, const char *groupName);

	void									OutputProfileToDebug() const;
};

// ************************************************************************************************************************************************************

extern CRuProfiler*			g_ruProfiler;
extern INT32				g_ruProfilingLevel;

// ************************************************************************************************************************************************************

#pragma managed

#endif
