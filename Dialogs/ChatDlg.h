/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BotEClient.h"


// CChatDlg-Dialogfeld
class CBotEDoc;
class CChatDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(CBotEDoc *pDoc, CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CChatDlg();

	/// Nimmt einen String entgegen, welcher danach im Chat angezeigt werden kann.
	void AddChatMsg(CString userName, CString msg);

// Dialogfelddaten
	enum { IDD = IDD_CHATDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSend();

private:
	CBotEDoc *m_pDoc;
	afx_msg void OnBnClickedClose();
};
