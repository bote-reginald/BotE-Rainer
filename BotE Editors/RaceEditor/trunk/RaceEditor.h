// MinorRaceEditor.h : Haupt-Header-Datei f�r die Anwendung MINORRACEEDITOR
//

#if !defined(AFX_MINORRACEEDITOR_H__F1234864_537A_4071_B946_BBE64F8EF06E__INCLUDED_)
#define AFX_MINORRACEEDITOR_H__F1234864_537A_4071_B946_BBE64F8EF06E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole
#include <Time.h>

/////////////////////////////////////////////////////////////////////////////
// CRaceEditorApp:
// Siehe MinorRaceEditor.cpp f�r die Implementierung dieser Klasse
//

class CRaceEditorApp : public CWinApp
{
public:
	CRaceEditorApp();

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CRaceEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CRaceEditorApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_MINORRACEEDITOR_H__F1234864_537A_4071_B946_BBE64F8EF06E__INCLUDED_)
