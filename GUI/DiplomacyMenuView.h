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
#include "AI\DiplomacyAI.h"
#include "Races\RaceController.h"

// CDiplomacyMenuView view

class CDiplomacyMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CDiplomacyMenuView)

protected:
	CDiplomacyMenuView();           // protected constructor used by dynamic creation
	virtual ~CDiplomacyMenuView();

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
	void OnNewRound(void);

	/// Funktion l�dt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

	void SetSubMenu(BYTE menuID) {m_bySubMenu = menuID;}

	BYTE GetSubMenu() const {return m_bySubMenu;}

	CRaceController* m_pRaceCtrl;		///< Rassencontroller f�r alle Rassen des Spiels

private:
	// Funktionen
	/// Funkion legt alle Buttons f�r die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen der Diplomatieansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawDiplomacyMenue(Graphics* g);

	/// Funktion zeichnet alles f�r die Majorraces in der Diplomatieansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawRaceDiplomacyMenue(Graphics* g);

	/// Funktion zeichnet das Informationsmen� in der Diplomatieansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param sWhichRace aktuell angeklickte Rasse
	void DrawDiplomacyInfoMenue(Graphics* g, const CString& sWhichRace);

		/// Funktion zeichnet das Informationsmen� in der Diplomatieansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param sWhichRace aktuell angeklickte Rasse
	void DrawDiplomacyIngameMenue(Graphics* g, const CString& sWhichRace);

		/// Funktion zeichnet das Informationsmen� in der Diplomatieansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param sWhichRace aktuell angeklickte Rasse
	void DrawDiplomacyDatabaseMenue(Graphics* g, const CString& sWhichRace);

	/// Funktion zeichnet das Angebotsmen� der Diplomatieansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param sWhichRace aktuell angeklickte Rasse
	void DrawDiplomacyOfferMenue(Graphics* g, const CString& sWhichRace);

	/// Funktion zeichnet die Buttons in den Diplomatiemen�s.
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param pMajor Spielerrasse
	void DrawDiplomacyButtons(Graphics* g, CMajor* pMajor, CArray<CMyButton*>* buttonArray, int counter);

	/// Funktion, die einen CString mit dem Status einer MinorRace gegen�ber einer anderen Rasse zur�ckgibt
	/// @param unsere Rasse
	/// @param sRace Majorrace
	/// @param color Referenz auf zu benutzende Farbe
	/// @return Diplomatischer Status als String
	CString PrintDiplomacyStatus(const CString& sOurRace, const CString& sRace, Gdiplus::Color& color);

	/// Funktion nimmt die Ressourcen und das Credits, welches f�r verschenken k�nnen aus den Lagern oder gibt es zur�ck
	/// @param bTake soll etwas genommen oder zur�ckgegeben werden?
	void TakeOrGetbackResLat(bool bTake);

	// Attribute

	// Grafiken
	Bitmap* bg_diploinfomenu;		// Diplomatieinformationsmen�
	Bitmap* bg_diploinmenu;			// Diplomatieeingangsmen�
	Bitmap* bg_diplooutmenu;		// Diplomatieausgangsmen�

	// Buttons
	CArray<CMyButton*> m_DiplomacyMainButtons;		///< die unteren Buttons in den Diplomatieansichten
	CArray<CMyButton*> m_DiplomacyMajorOfferButtons;///< die einzelnen Angebotsbuttons f�r die Majorraces
	CArray<CMyButton*> m_DiplomacyMinorOfferButtons;///< die einzelnen Angebotsbuttons f�r die Minorraces
	// Hier Variablen f�r die Diplomatieansicht
	USHORT	m_bySubMenu;						///< Welcher Button im Diplomatiemenue wurde gedr�ckt, 0 f�r Information, 1 f�r Angebote usw.
	CString m_sClickedOnRace;					///< Variable, die mir sagt auf welche Rasse ich geklickt habe
	BYTE	m_byWhichResourceIsChosen;			///< welche Ressource soll an die Rasse �bergeben werden
	CPoint	m_ptResourceFromSystem;				///< Aus welchem System werden die zu verschenkenden Ressourcen abgekn�pft

	bool m_bSortRaceList;					///< Soll die Racelist nach Vertr�gen sortiert werden?
	bool m_bDisplayAllRaces;				///< Display of all ingame races (ingame-database)
	bool m_bShowSendButton;					///< Soll der Abschicken Button in der Angebotssicht angezeigt werden?
	bool m_bShowDeclineButton;				///< Soll der Ablehnen Button in der Eingangsansicht angezeigt werden?
	bool m_bCanSend;						///< kann das Angebot gesendet werden?

	CDiplomacyInfo  m_OutgoingInfo;			///< Diplomatieobjekt, welches bei einem Angebot erstellt wird und abgeschickt werden kann
	CDiplomacyInfo* m_pIncomingInfo;		///< Zeiger auf Diplomatieobjekt, welches sich aktuell im Eingang befindet
	vector<CRace*>  m_vRaceList;			///< sortierte Liste aller bekannten Rassen, welche angezeigt werden sollen
	vector<CDiplomacyInfo*>	m_vIncomeList;	///< Liste aller eingehenden Nachrichten

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


