#include "stdafx.h"
#include "RaceDesign.h"

IMPLEMENT_SERIAL (CRaceDesign, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CRaceDesign::CRaceDesign(void)
{
	Reset();
}

CRaceDesign::~CRaceDesign(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CRaceDesign::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_clrSector;		// Sektormarkierung
		ar << m_clrSmallBtn;	// Schriftfarbe f�r kleinen Button
		ar << m_clrLargeBtn;	// Schriftfarbe f�r gro�en Button
		ar << m_clrSmallText;	// Schriftfarbe f�r kleinen Text
		ar << m_clrNormalText;	// Schriftfarbe f�r normalen Text
		ar << m_clrSecondText;	// Zweitfarbe f�r normalen Text

		ar << m_clrGalaxySectorText;// Farbe der Systemnamen auf der Galaxiemap

		ar << m_clrListMarkTextColor;	// Farbe f�r Text wenn dieser markiert ist
		ar << m_clrListMarkPenColor;	// Farbe f�r Umrandung bei Markierung eines Eintrags in einer Liste
		ar << m_clrRouteColor;			// Farbe der Handels- und Ressourcenrouten

		for (int i = 0; i < 6; i++)
		{
			ar << m_byGDIFontSize[i];		// Fontgr��en f�r GDI Darstellung
			ar << m_byGDIPlusFontSize[i];	// Fontgr��en f�r GDI+ Darstellung
		}
		ar << m_sFontName;		// Name der Schriftart
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_clrSector;		// Sektormarkierung
		ar >> m_clrSmallBtn;	// Schriftfarbe f�r kleinen Button
		ar >> m_clrLargeBtn;	// Schriftfarbe f�r gro�en Button
		ar >> m_clrSmallText;	// Schriftfarbe f�r kleinen Text
		ar >> m_clrNormalText;	// Schriftfarbe f�r normalen Text
		ar >> m_clrSecondText;	// Zweitfarbe f�r normalen Text

		ar >> m_clrGalaxySectorText; // Farbe der Systemnamen auf der Galaxiemap

		ar >> m_clrListMarkTextColor;	// Farbe f�r Text wenn dieser markiert ist
		ar >> m_clrListMarkPenColor;	// Farbe f�r Umrandung bei Markierung eines Eintrags in einer Liste
		ar >> m_clrRouteColor;			// Farbe der Handels- und Ressourcenrouten

		for (int i = 0; i < 6; i++)
		{
			ar >> m_byGDIFontSize[i];		// Fontgr��en f�r GDI Darstellung
			ar >> m_byGDIPlusFontSize[i];	// Fontgr��en f�r GDI+ Darstellung
		}
		ar >> m_sFontName;		// Name der Schriftart
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum zur�cksetzen aller Werte auf Ausgangswerte.
void CRaceDesign::Reset(void)
{
	m_clrSector		= RGB(200,200,200);	// Sektormarkierung
	m_clrSmallBtn	= 0;				// Schriftfarbe f�r kleinen Button
	m_clrLargeBtn	= 0;				// Schriftfarbe f�r gro�en Button
	m_clrSmallText	= 0;				// Schriftfarbe f�r kleinen Text
	m_clrNormalText	= 0;				// Schriftfarbe f�r normalen Text
	m_clrSecondText = 0;				// Zweitfarbe f�r normalen Text

	m_clrGalaxySectorText = RGB(255,255, 255);	// Farbe der Systemnamen auf der Galaxiemap

	m_clrListMarkTextColor	= RGB(220,220,220);		// Farbe f�r Text wenn dieser markiert ist
	m_clrListMarkPenColor	= RGB(140,196,203);		// Farbe f�r Umrandung bei Markierung eines Eintrags in einer Liste
	m_clrRouteColor			= RGB(255,255,255);		// Farbe der Handels- und Ressourcenrouten

	for (int i = 0; i < 6; i++)
	{
		m_byGDIFontSize[i]		= 12;	// Fontgr��en f�r GDI Darstellung
		m_byGDIPlusFontSize[i]	= 12;	// Fontgr��en f�r GDI+ Darstellung
	}
	m_sFontName		= "Arial";			// Name der Schriftart
}
