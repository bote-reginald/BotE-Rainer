#if !defined(AFX_SHOWSTATSDLG_H__0DE9408D_2B35_4530_BD77_BAC3D0F1E725__INCLUDED_)
#define AFX_SHOWSTATSDLG_H__0DE9408D_2B35_4530_BD77_BAC3D0F1E725__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowStatsDlg.h : Header-Datei
//
#include "ShipInfo.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CShowStatsDlg 

class CShowStatsDlg : public CDialog
{
// Konstruktion
public:
	CShowStatsDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	void SetShowStatsShip(CShipInfo* shipinfo);
// Dialogfelddaten
	//{{AFX_DATA(CShowStatsDlg)
	enum { IDD = IDD_SHOWSTATS_DIALOG };
	UINT	m_iOverallBeamDamage;
	UINT	m_iAverageTorpedoDamage;
	UINT	m_iMaxHull;
	UINT	m_iMaxShield;
	UINT	m_iShieldRecharge;
	UINT	m_iCrystal;
	UINT	m_iDeuterium;
	UINT	m_iDilithium;
	UINT	m_iDuranium;
	UINT	m_iIndustry;
	UINT	m_iIridium;
	UINT	m_iTitan;
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CShowStatsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CShowStatsDlg)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_SHOWSTATSDLG_H__0DE9408D_2B35_4530_BD77_BAC3D0F1E725__INCLUDED_
