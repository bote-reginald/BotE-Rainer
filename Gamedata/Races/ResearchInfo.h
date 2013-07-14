/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "ResearchComplex.h"
#include <math.h>

class CResearchInfo : public CObject
{
	friend class CResearch; // damit wir in der Klasse CResearch (und nur dieser) auf die private Attribute zugreifen k�nnen
public:

	/// Faktor der die ben�tigten Forschungspunkte modifiziert
	static double m_dResearchSpeedFactor;

	DECLARE_SERIAL (CResearchInfo)
	/// Konstruktor
	CResearchInfo(void);

	/// Destruktor
	~CResearchInfo();

	/// Kopierkonstruktor
	CResearchInfo(const CResearchInfo & rhs);
	/// Zuweisungsoperator
	CResearchInfo & operator=(const CResearchInfo &);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/**
	 * Diese Funktionen geben die ben�tigten FP f�r ein jeweiliges Techgebiet zur�ck. Diese werden hier dynamisch
	 * berechnet. Der "Startwert" f�r die einzelnen Gebiete ist der Multiplikator (z.B. 125 oder 150). An die einzelnen
	 * Funktionen wird das Techlevel <code>techLevel</code> �bergeben, f�r welches man die ben�tigten FP haben will.
	 */
	ULONG GetBio(USHORT nTechLevel) const;
	ULONG GetEnergy(USHORT nTechLevel) const;
	ULONG GetComp(USHORT nTechLevel) const;
	ULONG GetPropulsion(USHORT nTechLevel) const;
	ULONG GetConstruction(USHORT nTechLevel) const;
	ULONG GetWeapon(USHORT nTechLevel) const;

	/**
	 * Diese Funktion gibt einen Wahrheitswert zur�ck, der angibt, ob der Spieler die Wahl einer der 3 M�glichkeiten
	 * bei der Spezialforschung schon getroffen hat.
	 */
	bool GetChoiceTaken(void) const {return m_bChoiceTaken;}

	/**
	 * Diese Funktion gibt einen Zeiger auf den aktuellen Komplex der Spezialforschung zur�ck.
	 */
	const CResearchComplex* GetCurrentResearchComplex() const {return &m_ResearchComplex[m_nCurrentComplex];}

	/**
	 * Diese Funktion gibt einen Zeiger auf einen bestimmten Komplex der Spezialforschung zur�ck.
	 * Welcher Komplex zur�ckgegeben wird, wird durch den Parameter <code>nComplex</code> bestimmt.
	 */
	const CResearchComplex* GetResearchComplex(RESEARCH_COMPLEX::Typ nComplex) const {return &m_ResearchComplex[nComplex];}

	/**
	 * Funktion gibt den Namen einer gew�nschten Technologie zur�ck. Als Parameter muss eine Nummer <code>tech</code>
	 * �bergeben werden, die die Technologie identifiziert.
	 */
	const CString& GetTechName(BYTE tech) const {return m_strTechName[tech];}

	/**
	 * Funktion gibt die Beschreibung einer bestimmten Technologie zur�ck. Als Parameter muss eine Nummer
	 * <code>tech</code> �bergeben werden, die die Technologie identifiziert.
	 */
	const CString& GetTechDescription(BYTE tech) const {return m_strTechDescription[tech];}

	// sonstige Funktionen
	/**
	 * Diese Funktion w�hlt zuf�llig ein Unique-Themengebiet aus den noch nicht erforschten Komplexen aus.
	 * Vor Aufruf der Funktion sollte �berpr�ft werden, dass nicht schon alle Komplexe erforscht wurden, da
	 * es sonst zum Absturz des Programms kommen k�nnte.
	 */
	void ChooseUniqueResearch(void);

	/**
	 * Diese Funktion �ndert den Status des aktuellen Komplexes. Dabei �ndert sie gleichzeitig auch den Status
	 * der zuvor gew�hlten Wahlm�glichkeit. Als Parameter wird dabei ein neuer Status <code>nNewStatus</code>
	 * �bergeben.
	 */
	void ChangeStatusOfComplex(RESEARCH_STATUS::Typ nNewStatus);

	/**
	 * Diese Funktion w�hlt eine der drei M�glichkeiten der Uniqueforschung aus. Daf�r muss man das Gebiet, welches
	 * erforscht werden soll mit dem Parameter <code>possibility</code> �bergeben. Genaueres steht in der Definition
	 * dieser Funktion.
	 */
	void SetUniqueResearchChoosePossibility(BYTE possibility);

	/**
	 * Diese Funktion ermittelt den Namen und die Beschreibung einer bestimmten Technologie, an der gerade geforscht
	 * wird. Dies wird in den Attributen <code>m_strTechName</code> und <code>m_strTechDescription</code> gespeichert.
	 * Als Parameter m�ssen daf�r die jeweilige Technologie <code>tech</code> und die Stufe <code>level</code>, die
	 * aktuell erforscht wird �bergeben werden.
	 */
	void SetTechInfos(BYTE tech, BYTE level);


	/**
	 * Diese Funktion ermittelt den Namen und die Beschreibung einer bestimmten Technologie
	 * Dies wird in den Parametern <code>m_sTechName</code> und <code>m_sTechDesc</code> gespeichert.
	 * Als Parameter m�ssen daf�r die jeweilige Technologie <code>tech</code> und die Stufe <code>level</code>
	 * �bergeben werden.
	 */
	static void GetTechInfos(BYTE tech, BYTE level, CString& sTechName, CString& sTechDesc);

private:
	/// Die derzeit 12 Objekte f�r die einzelnen Komplexe der Spezialforschung
	CResearchComplex m_ResearchComplex[NoUC];

	/// Der aktuell gew�hlter Komplex
	RESEARCH_COMPLEX::Typ m_nCurrentComplex;

	/// Wurde eine der drei Wahlm�glichkeiten getroffen
	bool m_bChoiceTaken;

	/// Der Name der Technologie, an der gerade geforscht wird.
	CString m_strTechName[6];

	/// Die Beschreibung der Technologie, an der gerade geforscht wird.
	CString m_strTechDescription[6];
};
