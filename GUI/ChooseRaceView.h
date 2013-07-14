/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include <vector>
#include "CRoundButton2/roundbuttonstyle.h"
#include "CRoundButton2/roundbutton2.h"

// CChooseRaceView-Formularansicht

class CChooseRaceView : public CFormView
{
	DECLARE_DYNCREATE(CChooseRaceView)

public:
	CChooseRaceView();           // Dynamische Erstellung verwendet gesch�tzten Konstruktor
	virtual ~CChooseRaceView();

	void SetServer(bool bIsServer = true) {m_bIsServer = bIsServer;}
	bool IsServer() const {return m_bIsServer;}
	void EnableRaceButtons();
	void AddChatMsg(const CString& name, const CString& msg);
	BOOL CheckValues();

	enum { IDD = IDD_CHOOSERACEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	/// Den Style f�r die Button-Controls setzen
	/// @pBtn Zeiger auf Button
	void SetButtonStyle(CRoundButton2* pBtn, double dFontHeight);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung

	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRaceButtonClicked(UINT nID);
	afx_msg void OnBnStartGameClicked();
	afx_msg void OnBnCancelClicked();
	afx_msg void OnEnChangeMsg();
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	// This object includes the project wide style of the buttons
	CRoundButtonStyle m_tButtonStyle;

	std::vector<std::pair<CRoundButton2*, CString> > m_vMajorBtns;	///< Rassenauswahlbuttons

	CEdit m_edtChat;					///< Chatfenster
	CEdit m_edtChatMsg;					///< Eingabefeld f�r Chatnachrichten

	CRoundButton2 m_btStartGame;				///< Button zum Starten des Spiels
	CRoundButton2 m_btCancel;					///< Button zum Abbrechen

	bool m_bIsServer;					///< <code>true</code>, wenn auf dem lokalen Rechner der Server l�uft, sonst <code>false</code>
	int m_nPlayerCount;					///< Anzahl der Spieler, die ein Volk gew�hlt haben

	Bitmap* m_pBkgndImg;				///< Hintergrundgrafik
	CSize	m_TotalSize;				///< Gr��e der View

	//prevent weird crashes when calling OnDraw for too long after start game has already been clicked
	//appears to happen under the debugger only
	bool m_bStopDrawing;
};


