// CombatSimulator.h : Hauptheaderdatei f�r die CombatSimulator-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error "\"stdafx.h\" vor dieser Datei f�r PCH einschlie�en"
#endif

#include "resource.h"       // Hauptsymbole


// CCombatSimulatorApp:
// Siehe CombatSimulator.cpp f�r die Implementierung dieser Klasse
//

class CCombatSimulatorApp : public CWinApp
{
public:
	CCombatSimulatorApp();


// �berschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCombatSimulatorApp theApp;