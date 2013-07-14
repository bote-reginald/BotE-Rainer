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
#include "OverlayBanner.h"

// CSystemMenuView view

class CSystemMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CSystemMenuView)

protected:
	CSystemMenuView();           // protected constructor used by dynamic creation
	virtual ~CSystemMenuView();

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

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

	void SetSubMenu(BYTE menuID) {m_bySubMenu = menuID;}

	BYTE GetSubMenu() const {return m_bySubMenu;}

	static void SetMarkedBuildListEntry(short entry) {m_iClickedOn = entry;}

	static BYTE& GetResourceRouteRes() {return m_byResourceRouteRes;}

private:
	// Funktionen
	/// Funkion legt alle Buttons f�r die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen der Baumen�ansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawBuildMenue(Graphics* g);

	/// Funktion zum Zeichnen des Arbeiterzuweisungsmenues
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawWorkersMenue(Graphics* g);

	/// Funktion zum Zeichnen des Energiezuweisungsmenues
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawEnergyMenue(Graphics* g);

	// Funktion zum zeichnen der Bauwerkeansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawBuildingsOverviewMenue(Graphics* g);

	/// Funktion zum zeichnen der Systemhandelsansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawSystemTradeMenue(Graphics* g);

	/// Funktion zeichnet die gro�en Buttons (Energy, Arbeiter...) unten in der Systemansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawButtonsUnderSystemView(Graphics* g);

	/// Funktion zeichnet die Informationen rechts in der Systemansicht (Nahrung, Industrie usw.)
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawSystemProduction(Graphics* g);

	/// Fkt. berechnet und zeichnet die Produktion eines Geb�udes (z.B. es macht 50 Nahrung)
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawBuildingProduction(Graphics* g);

	/// Funktion zeichnet die Bauliste in der Baumen�- und Arbeitermen�ansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawBuildList(Graphics* g);

	// Attribute

	// Grafiken
	Bitmap* bg_buildmenu;			// Baumen�
	Bitmap* bg_workmenu;			// Arbeitermen�
	Bitmap* bg_energymenu;			// Energiemen�
	Bitmap* bg_overviewmenu;		// Geb�ude�bersichtsmen�
	Bitmap* bg_systrademenu;		// Handelsmen� im System

	// Buttons
	CArray<CMyButton*> m_BuildMenueMainButtons;		///< die unteren Buttons in der Baumen�ansicht (also Baumen�, Arbeiter...)
	CArray<CMyButton*> m_WorkerButtons;				///< die Buttons in der Arbeitermen�ansicht zum �ndern der zugewiesenen Arbeiter
	CArray<CMyButton*> m_SystemTradeButtons;		///< die Buttons in der Systemmen�handelsansicht zum �ndern der Mengen im stellaren Lager

	// sonstige Variablen
	// Hier die Rechtecke zum Klicken �n der Baumen�ansicht
	// Daf�r eine Struktur, die das Rechteck und die RunningNumber des Geb�udes/Updates beinhaltet
	CArray<int> m_vBuildlist;
	static short m_iClickedOn;			// Hilfsvariable, die mir sagt, auf welches Geb�ude ich geklickt habe
	CRect BuildingDescription;			// Rechteck, wo die Beschreibung des Geb�udes steht
	CRect BuildingInfo;					// Rechteck, wo Informationen (Kosten) zu dem Geb�ude stehen
	CRect AssemblyListRect;				// Rechteck, wo die Bauliste steht
	USHORT m_iWhichSubMenu;				// in welcher Liste sind wir 0 = Geb�ude 1 = Schiffe 2 = Truppen
	BYTE m_byStartList;					// ab welchem Eintrag der Bauliste werden die Auftr�ge angezeigt
	BYTE m_byEndList;					// bis zu welchem Eintrag in der Bauliste werden die Auftr�ge angezeigt

	// Hier die Variablen in der Bauwerks�bersicht
	struct BuildingOverviewStruct {CRect rect; USHORT runningNumber;};
	CArray<BuildingOverviewStruct> m_BuildingOverview;	// Feld, in dem die Anzahl der Geb�ude in einem System abgelegt werden
	USHORT m_iBOPage;									// aktuelle Seite in der Geb�ude�bersichtsansicht

	// Hier die Variablen in der Energie�bersicht
	struct ENERGYSTRUCT {CRect rect; USHORT index; BOOLEAN status;};
	CArray<ENERGYSTRUCT> m_EnergyList;		// Feld, in dem die Anzahl der Geb�ude, welche Energie ben�tigen, abgelegt werden
	USHORT m_iELPage;						// aktuelle Seite im Energiemen�

	// Hier die Variablen in der Systemhandelsansicht
	USHORT m_iSTPage;						// aktuelle Seite f�r die Anzeige der Handelsrouten

	// Ab hier die Rechtecke f�r die ganzen Buttons
	CRect ShipyardListButton;				// Rechteck f�r den Werftbutton
	CRect TroopListButton;					// Rechteck f�r den Truppenbutton
	CRect BuildingListButton;				// Rechteck f�r den Geb�ude/Updateliste Button
	CRect ShipdesignButton;					// rainer Rechteck f�r den Shipdesign-Button
	CRect BuyButton;						// Rechteck f�r den Kaufenbutton
	CRect DeleteButton;						// Rechteck f�r den Abbruchbutton
	CRect OkayButton;						// Rechteck f�r den Okaybutton
	CRect CancelButton;						// Rechteck f�r den Cancelbutton
	CRect BuildingInfoButton;				// Rechteck f�r den Geb�udeinfobutton
	CRect BuildingDescriptionButton;		// Rechteck f�r den Geb�udebeschreibungsbutton
	CRect ChangeWorkersButton;				// Rechteck f�r den Button f�r Umschaltung zwischen "normalen" und RES-Arbeitern
	CRect Timber[5][200];					// Die Balken die die Arbeiterzuweisung anzeigen 5 St�ck a max. 200 Arbeiter

	BYTE m_bySubMenu;						// Welcher Button wurde gedr�ckt (0 f�r Baumen�, 1 f�r Arbeiter usw.)
	BOOLEAN m_bClickedOnBuyButton;			// Wurde auf den "kaufen" Button in der Bauansicht geklickt
	BOOLEAN m_bClickedOnDeleteButton;		// Wurde auf den "Bau_abbrechen" Button geklickt in der Bauansicht
	BOOLEAN m_bClickedOnBuildingInfoButton;	// Wurde auf den "Info" Button f�r ein Geb�ude geklickt
	BOOLEAN m_bClickedOnBuildingDescriptionButton;	// Wurde auf den "Beschreibung" Button f�r ein Geb�ude geklickt
	// Hier die Variablen, wenn wir beim Globalen Lager sind
	UINT m_iGlobalStoreageQuantity;			// Wieviel Ressourcen werden pro Klick verschoben
	// Hier Variablen, wenn wir eine Ressourcenroute ziehen wollen
	static BYTE m_byResourceRouteRes;		// welche Ressource soll f�r die Ressourcenroute gelten

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
};


