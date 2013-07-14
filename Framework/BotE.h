/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole
#include "ChatDlg.h"

#define WM_INITVIEWS	(WM_APP+10)
#define WM_UPDATEVIEWS	(WM_APP+11)
#define WM_COMBATVIEW	(WM_APP+12)
#define WM_SHOWCHATDLG	(WM_APP+13)

/////////////////////////////////////////////////////////////////////////////
// CBotEApp:
// Siehe BotE.cpp f�r die Implementierung dieser Klasse
class CBotEApp : public CWinApp
{
private:
	CChatDlg *m_pChatDlg;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

public:
	/// Konstruktor
	CBotEApp();

	/// Destruktor
	virtual ~CBotEApp();

	/// Funktion gibt einen Zeiger auf den ChatDialog zur�ck.
	CChatDlg* GetChatDlg() {return m_pChatDlg;}

// �berschreibungen
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
// Implementierung
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
	afx_msg void InitViews(WPARAM, LPARAM);
	afx_msg void UpdateViews(WPARAM, LPARAM);
	afx_msg void ShowChatDlg(WPARAM, LPARAM);
	afx_msg void OnSettings();
	afx_msg void OnChat();
	afx_msg void OnOpenManual();
	afx_msg void OnHelpWiki();
	afx_msg void OnOpenReadme();
	afx_msg void OnOpenCredits();
};
