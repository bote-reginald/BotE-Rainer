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

// Submen�s
#define EMPIREVIEW_NEWS			0
#define EMPIREVIEW_SYSTEMS		1
#define EMPIREVIEW_SHIPS		2
#define EMPIREVIEW_DEMOGRAPHICS 3
#define EMPIREVIEW_TOP5			4
#define EMPIREVIEW_VICTORY		5

#define EMPIREVIEW_SYSTEMS_NORMAL	0
#define EMPIREVIEW_SYSTEMS_RESOURCE	1
#define EMPIREVIEW_SYSTEMS_DEFENCE	2
#define EMPIREVIEW_SYSTEMS_TRADE	3
#define EMPIREVIEW_SYSTEMS_TUTORIAL	4
#define EMPIREVIEW_SYSTEMS_ENERGY	5

/// View zur Anzeige des kompletten Imperiumsmen�s
class CEmpireMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CEmpireMenuView)

protected:
	CEmpireMenuView();           // protected constructor used by dynamic creation
	virtual ~CEmpireMenuView();

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

	void SetSubMenu(BYTE menuID) {m_iSubMenu = menuID;}

	BYTE GetSubMenu() const {return m_iSubMenu;}

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

private:
	// Funktionen
	void DrawEmpireNewsMenue(Graphics* g);			///< Funktion zum Zeichnen der Nachrichtenansicht an das Imperium
	void DrawEmpireSystemMenue(Graphics* g);		///< Funktion zum Zeichnen der Ansicht aller Systeme des Imperiums
	void DrawEmpireShipMenue(Graphics* g);			///< Funktion zum Zeichnen der Ansicht aller Schiffe des Imperiums
	void DrawEmpireDemographicsMenue(Graphics* g);	///< Funktion zum Zeichnen der Demographieansicht
	void DrawEmpireTop5Menue(Graphics* g);			///< Funktion zum Zeichnen der Top-5 Systemeansicht
	void DrawEmpireVictoryMenue(Graphics* g);		///< Funktion zum Zeichnen der Ansicht der Siegbedingungen
	void DrawSunSystem(Gdiplus::Graphics *g, const CPoint& ptKO, int nPos);	///< Funktion zeichnet das Sonnensystem in der Top-5 Systemansicht
	void CreateButtons();

	//Attribute
	// Grafiken
	Bitmap* bg_newsovmenu;			// Nachrichten�bersichtsmen�
	Bitmap* bg_systemovmenu;		// System�bersichtsmen�
	Bitmap* bg_shipovmenu;			// Schiffs�bersichtsmen�
	Bitmap* bg_demographicsmenu;	// Demographiemen�
	Bitmap* bg_top5menu;			// Top-5 Systememen�
	Bitmap* bg_victorymenu;			// Siegbedingunsmen�

	// Buttons
	CArray<CMyButton*> m_EmpireNewsFilterButtons;	///< Buttons zum filtern der Nachrichten in der Imperiums�bersicht
	CArray<CMyButton*> m_EmpireNewsButtons;			///< Buttons am unteren Rand der Imperiums�bersicht
	CArray<CMyButton*> m_EmpireShipsFilterButtons;	///< Buttons um aktuelle oder verlorene Schiffe anzuzeigen
	CArray<CMyButton*> m_EmpireSystemFilterButtons;	///< Buttons um in der Imperiumssystemansicht zwischen Ressourcen- und normaler Ansicht zu wechseln

	// Hier die Rechtecke und Variablen, die wir ben�tigen wenn wir in der Nachrichten und Informationsansicht sind
	USHORT m_iSubMenu;						// in welchem Untermen� der Informationsansicht sind wir
	USHORT m_iSystemSubMenue;				// Untermen� in der Systemansicht (normal, Ressourcen, Verteidigung)
	EMPIRE_NEWS_TYPE::Typ m_iWhichNewsButtonIsPressed;	// wollen wir nur eine bestimme Nachtichtenart anzeigen lassen, z.b. Forschung
	short  m_iClickedNews;					// auf welche Nachticht oder Information in der Liste wurde geklickt
	short  m_iOldClickedNews;				// auf welche Nachricht wurde vorher geklickt, braucht man als Modifikator
	short  m_iClickedSystem;				// auf welches System wurde in der Liste geklickt, welches wurde markiert
	short  m_iOldClickedSystem;				// auf welches System wurde vorher geklickt, braucht man als Modifikator
	short  m_iClickedShip;					// auf welches Schiff wurde in der Liste geklickt, welches wurde markiert
	short  m_iOldClickedShip;				// auf welches Schiffe wurde vorher geklickt, braucht man als Modifikator
	short  m_iClickedShipIndex;				// Index des angeklickten Schiffes in der Schiffshistory

	BOOLEAN m_bShowAliveShips;				// sollen nur die noch vorhandenen Schiffe angezeigt werden, oder die verlorenen

};


