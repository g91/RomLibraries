#include "Rune Engine Audio.h"
#pragma unmanaged

BOOL RuSymphony_Initialize()
{
	if (g_ruSoundFMOD == NULL)
	{
		g_ruSoundFMOD = ruNEW CRuSoundFMOD();
		g_ruSoundFMOD->Initialize();
		g_ruSoundFMOD->Set3DSettings(1.0f, 10.0f, 0.35f);
	}

	if (g_ruAudible == NULL)
	{
		g_ruAudible = ruNEW CRuAudible();
		g_ruAudible->Initialize(g_ruSoundFMOD);
		g_ruAudible->SetListenerMode(CRuAudible::ruLISTENERMODE_STANDARD);
	}

	//if (g_ruSymphony == NULL)
	//{
	//	g_ruSymphony = ruNEW CRuSymphony();
	//	g_ruSymphony->Startup();
	//}

	if (g_ruAudioLibrary == NULL)
	{
		g_ruAudioLibrary = ruNEW CRuFusion_AudioLibrary();
	}
	return TRUE;
}

BOOL RuSymphony_Shutdown()
{
	//if (g_ruSymphony)
	//{
	//	g_ruSymphony->Shutdown();
	//	ruSAFE_RELEASE(g_ruSymphony);
	//}

	if (g_ruAudible)
	{
		ruSAFE_RELEASE(g_ruAudible);
	}

	//if (g_ruSoundFMOD)
	//{
	//	ruSAFE_RELEASE(g_ruSoundFMOD);
	//}

	if (g_ruAudioLibrary)
	{
		ruSAFE_RELEASE(g_ruAudioLibrary);
	}

	return TRUE;
}

#pragma managed
