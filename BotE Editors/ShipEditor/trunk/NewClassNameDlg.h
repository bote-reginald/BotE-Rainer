#if !defined(AFX_NEWCLASSNAMEDLG_H__A99935B9_04D0_4711_9C87_7081CC62B28B__INCLUDED_)
#define AFX_NEWCLASSNAMEDLG_H__A99935B9_04D0_4711_9C87_7081CC62B28B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewClassNameDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CNewClassNameDlg 

class CNewClassNameDlg : public CDialog
{
// Konstruktion
public:
	CNewClassNameDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CNewClassNameDlg)
	enum { IDD = IDD_NEWCLASS_DIALOG };
	CString	m_strNewClassName;
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CNewClassNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CNewClassNameDlg)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_NEWCLASSNAMEDLG_H__A99935B9_04D0_4711_9C87_7081CC62B28B__INCLUDED_
