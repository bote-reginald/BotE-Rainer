/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BotEDoc.h"
#include "MainBaseView.h"


// CResearchMenuView view

class CResearchMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CResearchMenuView)

protected:
	CResearchMenuView();           // protected constructor used by dynamic creation
	virtual ~CResearchMenuView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion l�dt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

	void SetSubMenu(BYTE menuID) {m_bySubMenu = menuID;}

	BYTE GetSubMenu() const {return m_bySubMenu;}

private:
	// Funktionen
	/// Funkion legt alle Buttons f�r die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktio zum Zeichnen des Forschungsmen�s
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawResearchMenue(Graphics* g);

	/// Funktion zum Zeichnen des Spezialforschungsmen�s
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawUniqueResearchMenue(Graphics* g);

	// Attribute

	// Buttons
	CArray<CMyButton*> m_ResearchMainButtons;		///< Buttons an der rechten Seite der Forschungsansichten

	// Grafiken
	Bitmap* bg_researchmenu;				///< normales Forschungsmen�
	Bitmap* bg_urmenu;						///< Spezialforschungsmen�
	Bitmap* bg_emptyur;						///< wenn keine Spezialforschung zur Auswahl steht

	// Hier die Rechtecke zum Klicken in der Forschungs�bersicht
	CRect LockStatusRect[7];				///< Rechteck zeigt den Lockstatus des jeweiligen Forschungsgebietes an
	CRect ResearchTimber[7][101];			///< 7 Balken f�r die ganzen Foschungsgebiete � 100 kleinen Rechtecke + 1
	BYTE m_bySubMenu;						///< Welcher Button im Forschungsmenue wurde gedr�ckt, 0 f�r normal, 1 f�r Unique

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


