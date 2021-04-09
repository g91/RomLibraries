#include <windows.h>
#include "directinput.h"

// --------------------------------------------------------------------------------------
CInputCOM::CInputCOM()
{
	m_pDI = NULL;
	m_hWnd = NULL;
}

// --------------------------------------------------------------------------------------
CInputCOM::~CInputCOM()
{
	Shutdown();
}

// --------------------------------------------------------------------------------------
bool CInputCOM::Init( HWND hWnd, HINSTANCE hInst )
{
	Shutdown();

	// Record parent Window handle
	m_hWnd = hWnd;
	
	// Create a DirectInput interface
	if( FAILED( DirectInput8Create( hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL) ) )
		return false;

	// Return a success
	return true;
}

// --------------------------------------------------------------------------------------
void CInputCOM::Shutdown()
{
	ReleaseCOM(m_pDI);

	m_hWnd = NULL;
}

// --------------------------------------------------------------------------------------
CInputDevice::CInputDevice()
{
	m_pInput			= NULL;
	m_pDIDevice			= NULL;
	m_type				= eID_None;

	m_mouseState		= (DIMOUSESTATE*)&m_state;
	m_joystickState		= (DIJOYSTATE*)&m_state;

	Clear();
}

// --------------------------------------------------------------------------------------
CInputDevice::~CInputDevice()
{
	Free();
}

// --------------------------------------------------------------------------------------
bool CInputDevice::Create( CInputCOM* pInput, int type )
{
	DIDATAFORMAT *DataFormat;

	Free();

	if ( (m_pInput = pInput) == NULL )
		return false;

	switch ( type )
	{
	case eID_Keyboard:
		if( FAILED( m_pInput->GetDirectInputCOM()->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, NULL) ) )
			return false;
		DataFormat = (DIDATAFORMAT*)&c_dfDIKeyboard;
		break;

	case eID_Mouse:
		if( FAILED( m_pInput->GetDirectInputCOM()->CreateDevice(GUID_SysMouse, &m_pDIDevice, NULL) ) )
			return false;
		DataFormat = (DIDATAFORMAT*)&c_dfDIMouse;
		break;

	case eID_Joystick:
		return false;
		break;

	default:
		return false;
	}

	// Set the data format of keyboard
	if( FAILED( m_pDIDevice->SetDataFormat(DataFormat) ) )
		return false;

	// Set the cooperative level - Foreground & Nonexclusive
#ifndef KALYDO
	// can't get foreground with Kalydo
	if( FAILED( m_pDIDevice->SetCooperativeLevel(m_pInput->GetWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) ) )
		return false;
#endif // KALYDO

	// Acquire the device for use
	m_pDIDevice->Acquire();

	// Set the device type
	m_type = type;

	// Clear the device information
	Clear();	

	// Return a success
	return true;
}

// --------------------------------------------------------------------------------------
bool CInputDevice::Free()
{
	// Unacquire and release the object
	if ( m_pDIDevice != NULL )
	{
		m_pDIDevice->Unacquire();
		ReleaseCOM( m_pDIDevice );
	}	

	// Set to no device installed
	m_type = eID_None;

	// Clear the data  
	Clear();

	return TRUE;
}

// --------------------------------------------------------------------------------------
void CInputDevice::Clear()
{
	memset( m_state, 0, sizeof(m_state) );
}

// --------------------------------------------------------------------------------------
bool CInputDevice::Update()
{
	HRESULT hr;
	int bufferSize[3] = { 256, sizeof(DIMOUSESTATE), sizeof(DIJOYSTATE) };
	int index;

	// Make sure to have a valid IDirectInputDevice8 object
	if ( m_pDIDevice == NULL )
		return false;

	switch ( m_type )
	{
	case eID_Keyboard:
		index = 0;
		break;

	case eID_Mouse:
		index = 1;
		break;

	case eID_Joystick:
		return false;
		break;

	default:
		return false;
	}

	// Loop polling and reading until succeeded or unknown error
	// Also take care of lost-focus problems
	while (true)
	{
		// Poll
		m_pDIDevice->Poll();

		// Read in state
		if( SUCCEEDED( hr = m_pDIDevice->GetDeviceState(bufferSize[index], (LPVOID)&m_state) ) )
			break;

		// Return on an unknown error  
		if( hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED )
			return false;

		// Reacquire and try again
		if( FAILED( m_pDIDevice->Acquire() ) )
			return false;
	}

	// return a success
	return true;
}

// --------------------------------------------------------------------------------------
long CInputDevice::GetXDelta()
{
	switch ( m_type )
	{
	case eID_Mouse:
		return m_mouseState->lX;

	case eID_Joystick:
		return m_joystickState->lX;

	default:
		return 0;
	}
}

// --------------------------------------------------------------------------------------
long CInputDevice::GetYDelta()
{
	switch ( m_type )
	{
	case eID_Mouse:
		return m_mouseState->lY;

	case eID_Joystick:
		return m_joystickState->lY;

	default:
		return 0;
	}
}

// --------------------------------------------------------------------------------------
long CInputDevice::GetZDelta()
{
	switch ( m_type )
	{
	case eID_Mouse:
		return m_mouseState->lZ;

	case eID_Joystick:
		return m_joystickState->lZ;

	default:
		return 0;
	}
}

// --------------------------------------------------------------------------------------
bool CInputDevice::GetKeyState( BYTE num )
{
	return (m_state[num] & 0x80) ? true : false;
}

// --------------------------------------------------------------------------------------
void CInputDevice::SetKeyState( BYTE num, bool state )
{
	if ( state )
		m_state[num] |= 0x80;
	else
		m_state[num] &= ~0x80;
}

// --------------------------------------------------------------------------------------
bool CInputDevice::GetButtonState( BYTE num )
{
	if ( m_type == eID_Mouse )
		return (m_mouseState->rgbButtons[num] & 0x80) ? true : false;
	else if ( m_type == eID_Joystick )
		return (m_joystickState->rgbButtons[num] & 0x80) ? true : false;
	return false;
}

// --------------------------------------------------------------------------------------
void CInputDevice::SetButtonState( BYTE num, bool state )
{
	switch ( m_type )
	{
	case eID_Mouse:
		if ( state )
			m_mouseState->rgbButtons[num] |= 0x80;
		else
			m_mouseState->rgbButtons[num] &= ~0x80;
		break;

	case eID_Joystick:
		if ( state )
			m_joystickState->rgbButtons[num] |= 0x80;
		else
			m_joystickState->rgbButtons[num] &= ~0x80;
		break;
	}
}