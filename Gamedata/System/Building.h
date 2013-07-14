/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Building.h: Schnittstelle f�r die Klasse CBuilding.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "Constants.h"

class CBuilding : CObject
{
public:
	DECLARE_SERIAL (CBuilding)		// Klasse serialisierbar machen

	// Standardkonstruktor
	CBuilding(void);

	// Konstruktor mit Parameter�bergabe
	CBuilding(USHORT id);

	// Destruktor
	~CBuilding(void);

	// Kopierkonstruktor
	CBuilding(const CBuilding & rhs);

	// Zuweisungsoperatur
	CBuilding & operator=(const CBuilding &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Operator�berschreibung, der zum Sortieren des Feldes benuzt werden kann
	bool operator< (const CBuilding& elem2) const { return m_iRunningNumber < elem2.m_iRunningNumber;}
	bool operator> (const CBuilding& elem2) const { return m_iRunningNumber > elem2.m_iRunningNumber;}

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	USHORT GetRunningNumber() const { return m_iRunningNumber; }

	BOOLEAN GetIsBuildingOnline() const { return m_bOnline; }

	// zum Schreiben der Membervariablen
	void SetIsBuildingOnline(BOOLEAN TrueOrFalse) { m_bOnline = TrueOrFalse; }

private:
	// Die laufende Nummer (ID) des Geb�udes
	USHORT m_iRunningNumber;

	// Ist das Geb�ude online?
	BOOLEAN m_bOnline;
};

typedef CArray<CBuilding> BuildingArray;	// Das dynamische Feld wird vereinfacht als BuildingArray angegeben
