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

// CFleetMenuView view

class CFleetMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CFleetMenuView)

protected:
	CFleetMenuView();           // protected constructor used by dynamic creation
	virtual ~CFleetMenuView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion f�hrt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgef�hrt werden m�ssen.
	virtual void OnNewRound(void);

	/// Funktion l�dt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

private:
	// Funktionen
	/// Funkion legt alle Buttons f�r die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen des Flottenzusammenstellungsmen�s
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawFleetMenue(Graphics* g);

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

	// Attribute

	// Grafiken
	Bitmap* bg_fleetmenu;			// Flottenzusammenstellungsmen�

	// Buttons

	// Hier die Rechtecke und Variablen, die wir ben�tigen wenn wir in der Flottenansicht sind
	USHORT m_iFleetPage;						///< die aktuelle Seite f�r die Flottenansicht
	BOOLEAN m_bShowNextButton;					///< soll der next-Button angezeigt werden, falls wir nicht alles auf eine Seite bekommen
	BOOLEAN m_bShowBackButton;					///< wenn wir nicht mehr auf der ersten Seite sind kommt der back-Button

	vector<pair<CRect, CShips*> > m_vShipRects;	///< alle angezeigten Schiffe mit ihren Rechtecken zu draufklicken
	CRect m_rLastMarkedRect;					///< zuletzt markiertes Rechteck

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


