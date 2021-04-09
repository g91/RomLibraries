#include "../../Core/Utility/RuUtility_Profile.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

CRuProfiler::CRuProfiler()
:	m_histogramSize(100)
{
	m_timer = ruNEW CRuPerformanceTimer();
}

CRuProfiler::~CRuProfiler()
{
	// Release timer module
	m_timer->Release();
	delete m_timer;

	// Release probes
	for(INT32 i = 0; i < m_probes.Count(); ++i)
	{
		delete m_probes[i];
	}
}

INT32 CRuProfiler::CreateProbe(const char *probeName, const char *groupName)
{
	INT32 probeIndex = GetProbeIndex(probeName, groupName);

	if(probeIndex == -1)
	{
		probeIndex = m_probes.Count();

		// Create a new probe
		Probe *probe = ruNEW Probe();

		probe->m_name = ruNEW char [strlen(probeName) + 1];
		strcpy(probe->m_name, probeName);

		probe->m_groupName = ruNEW char [strlen(groupName) + 1];
		strcpy(probe->m_groupName, groupName);

		probe->m_value = 0.0f;

		// Resize histogram and reset histogram tail
		probe->m_histogram = ruNEW float [m_histogramSize];
		probe->m_histogramTail = 0;

		m_probes.Add(probe);
	}

	return probeIndex;
}

void CRuProfiler::SetHistogramSize(INT32 histogramSize)
{
	// Ignore invalid sizes
	if(histogramSize <= 0)
	{
		return;
	}

	// Store new histogram size
	m_histogramSize = histogramSize;

	// Resize all existing histograms
	for(INT32 i = 0; i < m_probes.Count(); ++i)
	{
		// Delete original histogram
		delete[] m_probes[i]->m_histogram;

		// Resize histogram and reset histogram tail
		m_probes[i]->m_histogram = ruNEW float [m_histogramSize];
		m_probes[i]->m_histogramTail = 0;
	}
}

INT32 CRuProfiler::GetNumProbes() const
{
	return m_probes.Count();
}

INT32 CRuProfiler::GetProbeIndex(const char *probeName, const char *groupName) const
{
	// Attempt to locate a probe with the specified name
	for(INT32 i = 0; i < m_probes.Count(); ++i)
	{
		if(strcmp(m_probes[i]->m_name, probeName) == 0 && strcmp(m_probes[i]->m_groupName, groupName) == 0)
		{
			return i;
		}
	}

	return -1;
}

const char *CRuProfiler::GetProbeName(INT32 index) const
{
	if(index >= 0 && index < m_probes.Count())
	{
		// Return the profile probe name for the profile entry at the specified index
		return m_probes[index]->m_name;
	}

	return NULL;
}

const char *CRuProfiler::GetProbeGroup(INT32 index) const
{
	if(index >= 0 && index < m_probes.Count())
	{
		// Return the profile probe group name for the profile entry at the specified index
		return m_probes[index]->m_groupName;
	}

	return NULL;
}

void CRuProfiler::ClearProbeValues(BOOL saveToHistogram)
{
	for(INT32 i = 0; i < m_probes.Count(); ++i)
	{
		if(saveToHistogram)
		{
			// Save sample value into histogram
			m_probes[i]->m_histogram[m_probes[i]->m_histogramTail] = m_probes[i]->m_value;

			// Increment histogram tail, wrapping around if necessary
			++m_probes[i]->m_histogramTail;
			if(m_probes[i]->m_histogramTail >= m_histogramSize)
			{
				m_probes[i]->m_histogramTail = 0;
			}
		}

		// Reset sample value to zero
		m_probes[i]->m_value = 0.0f;
	}
}

BOOL CRuProfiler::AccumulateProbeValue(INT32 index, float value)
{
	if(index >= 0 && index < m_probes.Count())
	{
		// Accumulate value into the probe
		m_probes[index]->m_value += value;

		return TRUE;
	}

	return FALSE;
}

float CRuProfiler::GetProbeValue(INT32 index) const
{
	if(index >= 0 && index < m_probes.Count())
	{
		// Return the profile timing for the profile entry at the specified index
		return m_probes[index]->m_value;
	}

	return -1.0f;
}

float CRuProfiler::GetProbeMeanValue(INT32 index) const
{
	if(index >= 0 && index < m_probes.Count())
	{
		float totalValue = 0.0f;

		for(INT32 i = 0; i < m_histogramSize; ++i)
		{
			totalValue += m_probes[index]->m_histogram[i];
		}

		return totalValue / m_histogramSize;
	}

	return -1.0f;
}

BOOL CRuProfiler::SetProbeValue(INT32 index, float value)
{
	if(index >= 0 && index < m_probes.Count())
	{
		// Set value into the probe
		m_probes[index]->m_value = value;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuProfiler::AccumulateProbeValue(const char *probeName, const char *groupName, float value)
{
	return AccumulateProbeValue(GetProbeIndex(probeName, groupName), value);
}

float CRuProfiler::GetProbeValue(const char *probeName, const char *groupName) const
{
	return GetProbeValue(GetProbeIndex(probeName, groupName));
}

float CRuProfiler::GetProbeMeanValue(const char *probeName, const char *groupName) const
{
	return GetProbeMeanValue(GetProbeIndex(probeName, groupName));
}

BOOL CRuProfiler::SetProbeValue(const char *probeName, const char *groupName, float value)
{
	return SetProbeValue(GetProbeIndex(probeName, groupName), value);
}

BOOL CRuProfiler::BeginSample(INT32 index)
{
	if(index >= 0 && index < m_probes.Count())
	{
		// Save the sampling start time
		m_probes[index]->m_probeStartTime = m_timer->QueryPerformanceCounter();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuProfiler::EndSample(INT32 index)
{
	if(index >= 0 && index < m_probes.Count())
	{
		LARGE_INTEGER probeEndTime = m_timer->QueryPerformanceCounter();
		float timeLength = (probeEndTime.QuadPart - m_probes[index]->m_probeStartTime.QuadPart) / (m_timer->GetQPCFrequency().QuadPart / 1000.0f);
		m_probes[index]->m_value += timeLength;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuProfiler::BeginSample(const char *probeName, const char *groupName)
{
	return BeginSample(GetProbeIndex(probeName, groupName));
}

BOOL CRuProfiler::EndSample(const char *probeName, const char *groupName)
{
	return EndSample(GetProbeIndex(probeName, groupName));
}

void CRuProfiler::OutputProfileToDebug() const
{
	char outputString[2048];

	for(INT32 i = 0; i < m_probes.Count(); ++i)
	{
		sprintf(outputString, "%s: %f\n", m_probes[i]->m_name, m_probes[i]->m_value);
		OutputDebugString(outputString);
	}
}

// ************************************************************************************************************************************************************

CRuProfiler*		g_ruProfiler = NULL;
INT32				g_ruProfilingLevel = 0;

// ************************************************************************************************************************************************************

#pragma managed
