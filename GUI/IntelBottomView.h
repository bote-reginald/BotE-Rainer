/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BottomBaseView.h"

// CIntelBottomView view

class CIntelBottomView : public CBottomBaseView
{
	DECLARE_DYNCREATE(CIntelBottomView)

protected:
	CIntelBottomView();           // protected constructor used by dynamic creation
	virtual ~CIntelBottomView();

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
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


