/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__A9311EEF_D13E_46E6_BEB6_62C2ADEB9F30__INCLUDED_)
#define AFX_MAINFRM_H__A9311EEF_D13E_46E6_BEB6_62C2ADEB9F30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Verh�ltnis der Splitterviews zur Gesamtview, so dass auf allen Aufl�sung das Spiel gleich aussieht.
// Eine feste Gr��e der Splitters kann dann nicht vorgegeben werden
#define HORZ_PROPORTION 0.15625
#define VERT_PROPORTION	0.732421875

#include "MultiSplitterView.h"
#include "PPTooltip.h"

class CMainFrame : public CFrameWnd
{
protected: // Nur aus Serialisierung erzeugen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Operationen
public:
	/// Funktion liefert die ID der gerade angezeigten View in einem bestimmten SplitterWindows.
	/// @param paneRow Nummer der Splitter-Zeile
	/// @param paneRow Nummer der Splitter-Spalte
	/// @return ID der aktiven View (<code>-1</code> wenn keine View aktive oder sonstiger Fehler)
	short GetActiveView(int paneRow, int paneCol) const;

	/// Funktion gibt die Klasse einer View zur�ck.
	/// @param className Klassenname der neuzuzeichnenden View
	/// @return Zeiger auf Viewobjekt
	CView* GetView(const CRuntimeClass* className) const;

	/// Funktion veranlasst eine View neu zu zeichnen.
	/// @param className Klassenname der neuzuzeichnenden View
	void InvalidateView(const CRuntimeClass* className);

	/// Funktion veranlasst eine View neu zu zeichnen.
	/// @param viewID ID der neuzuzeichnenden View
	void InvalidateView(USHORT viewID);

	/// Funktion setzt das Untermen� einer bestimmten View
	/// @param viewClassName Klassenname der View
	/// @param menuID Untermen�-ID der View
	void SetSubMenu(const CRuntimeClass* viewClassName, BYTE menuID);

	/// Funktion gibt das gerade aktive Untermen� einer View zur�ck
	/// @param viewClassName Klassenname der View
	/// @return ID des gerade aktiven Untermen�s
	BYTE GetSubMenu(const CRuntimeClass* viewClassName) const;

	/// Funktion legt die View f�r jede Rasse fest, also in welchem Men� man sich gerade befindet.
	/// @param whichView Nummer (ID) der View
	/// @param sRace auf welche Hauptrasse bezieht sich die �nderung
	void SelectMainView(USHORT whichView, const CString& sRace);

	/// Funktion legt die View fest, also in welchem Men� man sich gerade befindet.
	/// @param whichView Nummer (ID) der View
	void SelectMainView(USHORT whichView);

	/// Funktion w�hlt eine View f�r den unteren View-Bereich aus.
	/// @param viewID View-ID der gew�nschten View
	void SelectBottomView(USHORT viewID);

	/// Funktion setzt das Splitterwindow mit der MainView auf die volle Fenstergr��e.
	/// @param fullScrenn <code>true</code> f�r Fullscreenmodus, <code>false</code> f�r normalen Modus.
	void FullScreenMainView(bool fullScreen);

	/// Funktion gibt das Splitterwindow zur�ck
	/// @return Zeiger auf Splitterwindow
	CMultiSplitterView* GetSplitterWindow() {return &m_wndSplitter;}

	/// Funktion registriert eine View oder ein Control beim Tooltip. Erst nachdem die View registriert wurde
	/// kann sie auch ToolTips anzeigen.
	/// @param pWnd Fenster
	/// @return <code>true</code> wenn View registriert werden konnte, sonst <code>false</code>
	bool AddToTooltip(CWnd* pWnd, const CString& sTip = _T(""));

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CDialogBar  m_wndDlgBar;
	CMultiSplitterView m_wndSplitter;
	CPPToolTip	m_CPPToolTip;

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void NotifyCPPTooltip(NMHDR* pNMHDR, LRESULT* result);
	/// Funktion veranlasst die Views die rassenspezifischen Grafiken zu laden.
	afx_msg LRESULT InitViews(WPARAM, LPARAM);
	afx_msg LRESULT UpdateViews(WPARAM, LPARAM);
	afx_msg LRESULT ShowCombatView(WPARAM, LPARAM);
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_MAINFRM_H__A9311EEF_D13E_46E6_BEB6_62C2ADEB9F30__INCLUDED_)
