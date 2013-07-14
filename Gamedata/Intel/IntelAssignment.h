/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "Constants.h"
#include <map>

/**
 * Diese Klasse beinhaltet die genauen Zuweisungen auf die einzelnen Geheimdienstressorts und stellt Methoden
 * zur Manipulation und f�r den Zugriff dieser Informationen bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

using namespace std;
// forward declaration
class CMajor;

class CIntelAssignment : public CObject
{
	// ein CIntelligence Objekt bekommt vollen Zugriff auf die Attribute eines CIntelAssignment Objekts
	friend class CIntelligence;
public:
	DECLARE_SERIAL (CIntelAssignment)

	/// Konstruktor
	CIntelAssignment(void);

	/// Destruktor
	~CIntelAssignment(void);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die globale prozentuale Zuteilung auf die innere Sicherheit zur�ck.
	BYTE GetInnerSecurityPercentage() const;

	/// Funktion gibt die globale prozentuale Zuteilung der Spionageabteilung zur�ck.
	/// @param sRace Rasse
	BYTE GetGlobalSpyPercentage(const CString& sRace) {return m_byPercentage[0][sRace];}

	/// Funktion gibt die globale prozentuale Zuteilung der Sabotageabteilung zur�ck.
	BYTE GetGlobalSabotagePercentage(const CString& sRace) {return m_byPercentage[1][sRace];}

	/// Funktion gibt die einzelnen Spionagezuteilungen f�r eine bestimmte Rasse zur�ck.
	/// @param sRace Rasse
	/// @param type Wirtschaft == 0, Wissenschaft == 1, Milit�r == 2, Diplomatie == 3, Depot == 4
	BYTE GetSpyPercentages(const CString& sRace, BYTE type);

	/// Funktion gibt die einzelnen Sabotagezuteilungen f�r eine bestimmte Rasse zur�ck.
	/// @param sRace Rasse
	/// @param type Wirtschaft == 0, Wissenschaft == 1, Milit�r == 2, Diplomatie == 3, Depot == 4
	BYTE GetSabotagePercentages(const CString& sRace, BYTE type);

	/// Funktion gibt das Feld der globalen Zuteilung zur�ck.
	/// @param sRace Rasse
	/// @param nType (0 f�r Spionage, 1 f�r Sabotage
	BYTE GetGlobalPercentage(const CString& sRace, BYTE nType) {ASSERT(nType <= 1); return m_byPercentage[nType][sRace];}

	/// Funktion �ndert die globale prozentuale Zuteilung der einzelnen Geheimdienstressorts. Dabei wird wenn
	/// n�tig die Zuteilung der anderen Ressorts ver�ndert.
	/// @param type Ressort (0 == Spionage, 1 == Sabotage, 2 == innere Sicherheit)
	/// @param perc neue prozentuale Zuteilung
	/// @param pMajor unsere Majorrace
	/// @param sRace Rasse, welche von der �nderung betroffen sein soll
	/// @param pmMajors Map mit allen Majors im Spiel
	void SetGlobalPercentage(BYTE type, BYTE perc, CMajor* pMajor, const CString& sRace, const map<CString, CMajor*>* pmMajors);

	/// Funktion �ndert die genauen Zuteilungen bei der Spionage. Also wieviel Prozent der Spionageabteilung
	/// gehen z.B. in die Wirtschaftsspionage. Nicht vergebene Prozentpunkte befinden sich automatisch im
	/// Spionagedepot f�r die jeweilige Rasse.
	/// @param type Wirtschaft == 0, Wissenschaft == 1, Milit�r == 2, Diplomatie == 3, Spionagedepot == 4
	/// @param perc neue prozentuale Zuteilung
	/// @param sRace Rasse, welche von der �nderung betroffen sein soll
	void SetSpyPercentage(BYTE type, BYTE perc, const CString& sRace);

	/// Funktion �ndert die genauen Zuteilungen bei der Sabotage. Also wieviel Prozent der Sabotageabteilung
	/// gehen z.B. in die Wirtschaftssabotage. Nicht vergebene Prozentpunkte befinden sich automatisch im
	/// Sabotagedepot f�r die jeweilige Rasse.
	/// @param type Wirtschaft == 0, Wissenschaft == 1, Milit�r == 2, Diplomatie == 3, Sabotagedepot == 4
	/// @param perc neue prozentuale Zuteilung
	/// @param sRace Rasse, welche von der �nderung betroffen sein soll
	void SetSabotagePercentage(BYTE type, BYTE perc, const CString& sRace);

	/// Funktion entfernt eine ausgeschiedene Rasse aus allen Geheimdienstzuweisungen.
	/// @param sRace Rasse, welche entfernt werden soll
	void RemoveRaceFromAssignments(const CString& sRace);

	/// Resetfunktion f�r das CIntelAssignment-Objekt.
	void Reset();

private:
	// Attribute
	map<CString, BYTE> m_byPercentage[2];	///< prozentuale Zuteilung zwischen Spionage und Sabotage

	map<CString, BYTE> m_bySpyPercentage[4];///< prozentuale Zuteilung bei den einzelnen Spionagefeldern

	map<CString, BYTE> m_bySabPercentage[4];///< prozentuale Zuteilung bei den einzelnen Sabotagefeldern
};
