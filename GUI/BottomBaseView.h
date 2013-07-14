/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "FontLoader.h"
#include "OverlayBanner.h"
#include "BotEDoc.h"

// CBottomBaseView view

class CBottomBaseView : public CView
{
	DECLARE_DYNCREATE(CBottomBaseView)

protected:
	CBottomBaseView();           // protected constructor used by dynamic creation
	virtual ~CBottomBaseView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion setzt ein Untermen�. Diese Funktion sollte von der abgeleiteten Klasse �berschrieben werden.
	/// @param menuID Nummer des Untermen�s
	virtual void SetSubMenu(BYTE) {};

	/// Funktion gibt das akutuelle Untermen� zur�ck. Diese Funktion sollte von der abgeleiteten Klasse �berschrieben werden.
	/// @return <code>NULL</code>
	virtual BYTE GetSubMenu() const {return NULL;}

	/// Funktion f�hrt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgef�hrt werden m�ssen. Sollte von jeder
	/// abgeleiteten Klasse abgeleitet werden.
	virtual void OnNewRound() {};

	/// Funktion l�dt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics() {};

	/// Funktion zum Festlegen der Spielerrasse in der View
	/// @pPlayer Zeiger auf Spielerrasse
	static void SetPlayersRace(CMajor* pPlayer) {m_pPlayersRace = pPlayer;}

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void) { return ""; }

protected:
	// Funktionen
	/// Funktion rechnet einen �bergebenen Punkt in logische Koordinaten um.
	/// @param point Referenz auf umzurechnenden Punkt
	void CalcLogicalPoint(CPoint &point);

	/// Funktion rechnet ein �bergebenes Rechteck in logische Koordinaten um.
	/// @param point Referenz auf umzurechnendes Rechteck
	void CalcDeviceRect(CRect &rect);

	/// Funktion liefert die rassenspezifische Schriftfarbe f�r die kleinen Buttons.
	/// @return Farbe
	Gdiplus::Color GetFontColorForSmallButton(void);

	// Attribute
	CSize m_TotalSize;					///< Gr��e der View in logischen Koordinaten

	static CMajor* m_pPlayersRace;		///< Zeiger auf Spielerrasse

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


