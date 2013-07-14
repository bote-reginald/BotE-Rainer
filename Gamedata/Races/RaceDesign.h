/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "afx.h"

/// Klasse beinhaltet Informationen zu Farben, Schriftfarben, Schriftart und Schriftgr��en
/// f�r bestimmte grafische Elemente einer Rasse.
class CRaceDesign :	public CObject
{
	/// CMajor-Klasse, welche ein Objekt dieser Klasse verwendet hat uneingeschr�nkten Zugriff
	friend class CMajor;
	friend class CFontLoader;

	DECLARE_SERIAL(CRaceDesign)

public:
	/// Standardkonstruktor
	CRaceDesign(void);

	/// Standarddestruktor
	~CRaceDesign(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen

	/// Funktion zum zur�cksetzen aller Werte auf Ausgangswerte.
	void Reset(void);

	/// Diese Funktion erstellt aus einem RGB String eine Farbe.
	/// @param sRGB RGB-Farbwerte als String
	/// @param sToken Trennzeichen zwischen den einzelnen Farbwerten im String
	/// @return Farbe
	static COLORREF StringToColor(const CString& sRGB, const CString& sToken = ",")
	{
		int iStart = 0;

		return RGB(
			atoi(sRGB.Tokenize(sToken, iStart)),
			atoi(sRGB.Tokenize(sToken, iStart)),
			atoi(sRGB.Tokenize(sToken, iStart)));
	}

	static COLORREF MakeDarker(const COLORREF& color, int nValue)
	{
		int r	= GetRValue(color) - nValue;
		int g	= GetGValue(color) - nValue;
		int b	= GetBValue(color) - nValue;

		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;

		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;

		return RGB(r,g,b);
	}

public:
	// Attribute
	COLORREF			m_clrSector;					///<!!! Sektormarkierung
	COLORREF			m_clrSmallBtn;					///<!!! Schriftfarbe f�r kleinen Button
	COLORREF			m_clrLargeBtn;					///<!!! Schriftfarbe f�r gro�en Button
	COLORREF			m_clrSmallText;					///<!!! Schriftfarbe f�r kleinen Text
	COLORREF			m_clrNormalText;				///<!!! Schriftfarbe f�r normalen Text
	COLORREF			m_clrSecondText;				///<!!! Zweitfarbe f�r normalen Text

	COLORREF			m_clrGalaxySectorText;			///<!!! Farbe der Systemnamen auf der Galaxiemap

	COLORREF			m_clrListMarkTextColor;			///<!!! Farbe f�r Text wenn dieser markiert ist
	COLORREF			m_clrListMarkPenColor;			///<!!! Farbe f�r Umrandung bei Markierung eines Eintrags in einer Liste

	COLORREF			m_clrRouteColor;				///<!!! Farbe der Handels- und Ressourcenrouten

	BYTE				m_byGDIFontSize[6];				///<!!! Fontgr��en f�r GDI Darstellung
	BYTE				m_byGDIPlusFontSize[6];			///<!!!	Fontgr��en f�r GDI+ Darstellung
	CString				m_sFontName;					///<!!!	Name der Schriftart
};
