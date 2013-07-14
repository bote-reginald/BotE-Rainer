/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include <map>
#include <list>

class CGlobalBuildings : public CObject
{
public:
	DECLARE_SERIAL (CGlobalBuildings)
	// Standardkonstruktor
	CGlobalBuildings(void);

	// Destruktor
	~CGlobalBuildings(void);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/// Funktion gibt die Anzahl eines bestimmtes Geb�udes f�r eine bestimmte Major-Rasse zur�ck.
	/// @param sRaceID Rassen-ID
	/// @param nID Geb�ude-ID
	/// @return Anzahl der Geb�ude
	int GetCountGlobalBuilding(const CString& sRaceID, USHORT nID) const;

	// zum Schreiben der Membervariablen
	/// Funktion f�gt ein Geb�ude f�r eine bestimmte Rasse den globalen Geb�uden hinzu.
	/// @param sRaceID Rassen-ID
	/// @param nID Geb�ude-ID
	void AddGlobalBuilding(const CString& sRaceID, USHORT nID) {m_mGlobalBuildings[sRaceID].push_back(nID);}

	// sonstige Funktionen
	/// Funktion l�scht ein globales Geb�ude aus dem Feld der globalen Geb�ude. Diese Funktion sollte aufgerufen
	/// werden, wenn ein solches Geb�ude abgerissen oder verloren wird.
	/// @param sRaceID Rassen-ID
	/// @param nID Geb�ude-ID
	void DeleteGlobalBuilding(const CString& sRaceID, USHORT nID);

	// Resetfunktion f�r die Klasse CGlobalBuildings
	void Reset();

private:
	// Dieses Feld beinhaltet die Geb�ude, welche ein imperienweites Attribut haben und in einer Bauliste stehen, sowei
	// alle Geb�ude die in allen Systemen stehen. Kommen mehrere gleiche Geb�ude vor, so ist deren ID auch mehrmals
	// hier in dem Feld vorhanden.
	std::map<CString, std::list<USHORT> > m_mGlobalBuildings;
};
