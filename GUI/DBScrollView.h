/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#if !defined(AFX_DBSCROLLVIEW_H__5990A9BB_C42A_476B_98CA_7FD49B4C160A__INCLUDED_)
#define AFX_DBSCROLLVIEW_H__5990A9BB_C42A_476B_98CA_7FD49B4C160A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBScrollView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDBScrollView view

/**
 * ScrollView, die Double Buffering unterst�tzt. Ist m_bUpdateOnly == TRUE wird der urspr�ngliche Fensterinhalt
 * nicht gel�scht; damit ist es m�glich nur die �nderungen zu zeichnen (ohne Double Buffering).
 */
class CDBScrollView : public CScrollView
{
protected:
	CDBScrollView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDBScrollView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBScrollView)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// TRUE, wenn nur �nderungen gezeichnet werden sollen (kein Double Buffering), sonst FALSE;
	// standardm��ig FALSE; wird nach dem Aufruf von OnDraw(CDC*) wieder auf FALSE gesetzt
	BOOL m_bUpdateOnly;
	// Hintergrundfarbe, mit der die View bei Double Buffering  vor OnDraw(CDC*) gef�llt werden soll
	COLORREF m_colorBackground;

	virtual void OnDraw(CDC* pDC);
	virtual ~CDBScrollView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDBScrollView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBSCROLLVIEW_H__5990A9BB_C42A_476B_98CA_7FD49B4C160A__INCLUDED_)
