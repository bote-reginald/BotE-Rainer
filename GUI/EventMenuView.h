/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "MainBaseView.h"

// forward declaration
class CEventScreen;

// CEventMenuView view
class CEventMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CEventMenuView)

protected:
	CEventMenuView();           // protected constructor used by dynamic creation
	virtual ~CEventMenuView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

	/// Funktion schlie�t das derzeit angezeigte Event.
	void CloseScreen(CEventScreen* eventScreen);

	/// Funktion l�dt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics() {};

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


