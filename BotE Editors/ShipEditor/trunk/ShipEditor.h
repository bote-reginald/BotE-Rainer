// ShipEditor.h : Haupt-Header-Datei f�r die Anwendung SHIPEDITOR
//

#if !defined(AFX_SHIPEDITOR_H__26BE7FAC_3C6F_4CDD_B008_F3466172AF28__INCLUDED_)
#define AFX_SHIPEDITOR_H__26BE7FAC_3C6F_4CDD_B008_F3466172AF28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CShipEditorApp:
// Siehe ShipEditor.cpp f�r die Implementierung dieser Klasse
//

class CShipEditorApp : public CWinApp
{
public:
	CShipEditorApp();

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CShipEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CShipEditorApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_SHIPEDITOR_H__26BE7FAC_3C6F_4CDD_B008_F3466172AF28__INCLUDED_)
