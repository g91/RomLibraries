#ifndef _DIRECTINPUT_H
#define _DIRECTINPUT_H

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION			0x0800
#endif
#include <dinput.h>

#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

#define ReleaseCOM(p)	if(p) { p->Release(); p=NULL; }

// Enumerated list of of device types
enum InputDevices {
	eID_None			= 0,
	eID_Keyboard,
	eID_Mouse,
	eID_Joystick
};

/*
///////////////////////////////////////////////////////////////////////
// Mouse and Joystick definitions
///////////////////////////////////////////////////////////////////////
#define JOYSTICK_BUTTON0       0
#define JOYSTICK_BUTTON1       1
#define JOYSTICK_BUTTON2       2
#define JOYSTICK_BUTTON3       3
#define JOYSTICK_BUTTON4       4
#define JOYSTICK_BUTTON5       5
#define JOYSTICK_BUTTON6       6
#define JOYSTICK_BUTTON7       7
*/

///////////////////////////////////////////////////////////////////////
// Class definitions
///////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------------------
class CInputCOM
{
public:
	CInputCOM();
	~CInputCOM();

	IDirectInput8*			GetDirectInputCOM()					{ return m_pDI; }
	HWND					GetWnd()							{ return m_hWnd; }

	bool					Init( HWND hWnd, HINSTANCE hInst );
	void					Shutdown();

protected:
	IDirectInput8*			m_pDI;
	HWND					m_hWnd;
};

// --------------------------------------------------------------------------------------
class CInputDevice
{
public:	
	CInputDevice();
	~CInputDevice();

	bool					Create( CInputCOM* pInput, int type );
	bool					Free();	
	void					Clear();
	bool					Update();

	BYTE*					GetKeyState()					{ return m_state; }
	DIMOUSESTATE*			GetMouseState()					{ return m_mouseState; }
	DIJOYSTATE*				GetjoystickState()				{ return m_joystickState; }

	long					GetXDelta();
	long					GetYDelta();
	long					GetZDelta();

	// Keyboard specific functions
	bool					GetKeyState( BYTE num );
	void					SetKeyState( BYTE num, bool state );

	// Mouse/Joystick specific functions
	bool					GetButtonState( BYTE num );
	void					SetButtonState( BYTE num, bool state );

protected:
	CInputCOM*				m_pInput;
	IDirectInputDevice8*	m_pDIDevice;
	int						m_type;

	BYTE					m_state[256];
	DIMOUSESTATE			*m_mouseState;
	DIJOYSTATE				*m_joystickState;
};

#endif _DIRECTINPUT_H