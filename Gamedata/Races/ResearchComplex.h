/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Constants.h"

class CResearchComplex : public CObject
{
	friend class CResearchInfo;
public:
	DECLARE_SERIAL (CResearchComplex)
	/// Konstruktor
	CResearchComplex();

	/// Destruktor
	virtual ~CResearchComplex();

	/// Kopierkonstruktor
	CResearchComplex(const CResearchComplex & rhs);

	/// Zuweisungsoperator
	CResearchComplex & operator=(const CResearchComplex &);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/**
	 * Diese Funktion gibt den Status des Komplexes zur�ck. Der Status des Komplex kann entweder
	 * RESEARCHED, NOTRESEARCHED oder RESEARCHING sein.
	 */
	RESEARCH_STATUS::Typ GetComplexStatus(void) const {return m_nComplexStatus;}

	/**
	 * Diese Funktion gibt den Status eines speziellen Gebietes in dem Komplex zur�ck. Der Status des Gebietes
	 * kann entweder RESEARCHED, NOTRESEARCHED oder RESEARCHING sein. Als Paramter erwartet die Funktion das
	 * gew�nschte Gebiet <code>field</code>. Dieses beginnt bei 1 und nicht bei 0 (maximal 3)!
	 */
	RESEARCH_STATUS::Typ GetFieldStatus(BYTE field) const {return m_nFieldStatus[field-1];}

	/**
	 * Diese Funktion gibt den Namen (die Bezeichnung) des Komplexes zur�ck.
	 */
	const CString& GetComplexName(void) const {return m_strComplexName;}

	/**
	 * Diese Funktion gibt die Beschreibung des Komplex zur�ck.
	 */
	const CString& GetComplexDescription(void) const {return m_strComplexDescription;}

	/**
	 * Diese Funktion gibt den Namen eines speziellen Gebietes in dem Komplex zur�ck. Als Paramter erwartet die
	 * Funktion das gew�nschte Gebiet <code>field</code>. Dieses beginnt bei 1 und nicht bei 0 (maximal 3)!
	 */
	const CString& GetFieldName(BYTE field) const {return m_strFieldName[field-1];}

	/**
	 * Diese Funktion gibt die Beschreibung eines speziellen Gebietes in dem Komplex zur�ck. Als Paramter erwartet die
	 * Funktion das gew�nschte Gebiet <code>field</code>. Dieses beginnt bei 1 und nicht bei 0 (maximal 3)!
	 */
	const CString& GetFieldDescription(BYTE field) const {return m_strFieldDescription[field-1];}

	/**
	 * Diese Funktion gibt den Bonus eines speziellen Gebietes in dem Komplex zur�ck. Als Paramter erwartet die
	 * Funktion das gew�nschte Gebiet <code>field</code>. Dieses beginnt bei 1 und nicht bei 0 (maximal 3)!
	 */
	short GetBonus(BYTE field) const {return m_iBonus[field-1];}

	// sonstige Funktionen
	/**
	 * Diese Funktion generiert einen Komplex, setzt also erst alle Attribute des Objektes. Als einziger Parameter
	 * muss daf�r eine Komplexnummer <code>nComplex</code> �bergeben werden.
	 */
	void GenerateComplex(RESEARCH_COMPLEX::Typ nComplex);

	/**
	 * Diese Funktion setzt alle Variablen des CResearchComplex-Objektes wieder auf ihre Ausgangswerte
	 */
	void Reset(void);

private:
	/**
	 * Diese private Funktion liest die Beschreibungen zu den Spezialforschungen aus einer Datei ein und speichert sie
	 * auf die entsprechenden Variablen.
	 */
	void ReadSpecialTech(RESEARCH_COMPLEX::Typ nComplex);

	/// Status des Komplexes (erforscht, nicht erforscht, gerade am erforschen).
	RESEARCH_STATUS::Typ m_nComplexStatus;

	/// Der Status der einzelnen Wahlgebiete des Komplexes (erforscht, nicht erforscht, gerade am erforschen).
	RESEARCH_STATUS::Typ m_nFieldStatus[3];

	/// Der Name (Bezeichnung) des Komplexes (z.B. Schiffstechnik offensiv oder Wirtschaft...).
	CString m_strComplexName;

	/// Die Beschreibung des Komplexes.
	CString m_strComplexDescription;

	/// Der Name eines einzelnen Gebietes in dem Komplex.
	CString m_strFieldName[3];

	/// Die Beschreibung eines einzelnen Gebietes in dem Komplex.
	CString m_strFieldDescription[3];

	/// Den einzelnen Bonus, den das jeweilige Gebiet macht.
	short m_iBonus[3];
};
