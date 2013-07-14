/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "IntelAssignment.h"
#include "IntelReports.h"
#include "IntelInfo.h"
#include <map>

using namespace std;
/**
 * Diese Klasse abstrahiert den Geheimdienst einer Hauptrasse. Sie beinhaltet alle Funktionen zur Manipulation
 * von Geheimdienstwerten.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */
class CIntelligence : public CObject
{
public:
	DECLARE_SERIAL (CIntelligence)

	/// Standardkonstruktor
	CIntelligence(void);

	/// Destruktor
	~CIntelligence(void);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die Nummer der Rasse zur�ck, welche wir f�r unsere Geheimdienstaten verantworlich machen wollen.
	const CString& GetResponsibleRace() const {return m_sResponsibleRace;}

	/// Funktion gibt die Anzahl der produzierten Geheimdienstpunkte zur�ck.
	UINT GetSecurityPoints() const {return m_iSecurityPoints;}

	/// Funktion gibt die gesammelten Punkte f�r die innere Sicherheit zur�ck.
	UINT GetInnerSecurityStorage() const {return m_iInnerStorage;}

	/// Funktion gibt die angesammelten Punkte bei der Spionage oder der Sabotage bei einer bestimmten Rasse
	/// zur�ck.
	/// @param type Spionage == 0, Sabotage == 1
	/// @param sRace Rasse
	UINT GetSPStorage(BOOLEAN type, const CString& sRace) {return m_iSPStorage[type][sRace];}

	/// Funktion gibt einen Bonus auf ein Geheimdienstressort zur�ck.
	/// @param bonus Wirtschaft == 0, Forschung == 1, Milit�r == 2, Diplomatie == 3, innere Sicherheit == 4
	/// @param type Spionage == 0, Sabotage == 1
	short GetBonus(BYTE bonus, BOOLEAN type) const;

	/// Funktion gibt den Bonus der inneren Sicherheit zur�ck.
	short GetInnerSecurityBoni() const {return (m_nInnerSecurityBoni < -100) ? -100 : m_nInnerSecurityBoni;}

	/// Funktion gibt den Bonus f�r Wirtschaft zur�ck.
	/// @param type Spionage == 0, Sabotage == 1
	short GetEconomyBonus(BOOLEAN type) const {return (m_nEconomyBoni[type] < -100) ? -100 : m_nEconomyBoni[type];}

	/// Funktion gibt den Bonus f�r Wissenschaft zur�ck.
	/// @param type Spionage == 0, Sabotage == 1
	short GetScienceBonus(BOOLEAN type) const {return (m_nScienceBoni[type] < -100) ? -100 : m_nScienceBoni[type];}

	/// Funktion gibt den Bonus f�r Milit�r zur�ck.
	/// @param type Spionage == 0, Sabotage == 1
	short GetMilitaryBonus(BOOLEAN type) const {return (m_nMilitaryBoni[type] < -100) ? -100 : m_nMilitaryBoni[type];}

	/// Funktion gibt die Aggressivit�t bei Geheimdienstaktionen zur�ck.
	/// @param type Spionage == 0, Sabotage == 1
	/// @param sRace Rasse
	BYTE GetAggressiveness(BOOLEAN type, const CString& sRace) {return m_byAggressiveness[type][sRace];}

	/// Funktion gibt einen Zeiger auf die Geheimdienstreporte zur�ck.
	CIntelReports* GetIntelReports() {return &m_Reports;}

	/// Funktion gibt einen Zeiger auf die Geheimdienstinformationen eines Imperiums zur�ck.
	CIntelInfo* GetIntelInfo() {return &m_IntelInfo;}

	/// Funktion gibt einen Zeiger auf die Zuteilung der einzelnen Geheimdienstressorts zur�ck.
	CIntelAssignment* GetAssignment() {return &m_Assignment;}

	/// Funktion gibt einen Zeiger auf die Zuteilung der einzelnen Geheimdienstressorts zur�ck.
	CIntelAssignment* SetAssignment() {return &m_Assignment;}

	/// Funktion legt die Rassennummer fest.
	/// @param sRace Nummer der zugeh�rigen Rasse/Imperium
	void SetRaceID(const CString& sRace) {m_sRace = sRace; SetResponsibleRace(sRace);}

	/// Funktion legt die f�r unsere Geheimdiensttaten verantworlich machbare Rasse fest. Somit l��t sich die Sache
	/// auf eigentlich unbeteiligte Rassen abw�lzen.
	/// @param responibleRace neue verantworlichmachbare Rasse
	void SetResponsibleRace(const CString& responsibleRace) {m_sResponsibleRace = responsibleRace;}

	/// Funktion setzt die Aggressivit�t bei Geheimdienstaktionen fest.
	/// @param type Spionage == 0, Sabotage == 1
	/// @param sRace Rasse
	/// @param value vorsichtig == 0, normal == 1, aggressiv = 2
	void SetAggressiveness(BOOLEAN type, const CString& sRace, BYTE value) {m_byAggressiveness[type][sRace] = value;}

	/// Funktion addiert einen Wert zum vorhandenen Wert der inneren Sicherheit. Funktion �berpr�ft auch, ob der neue
	/// Wert im richtigen Bereich liegt.
	/// @param add der zu addierende Wert
	void AddInnerSecurityPoints(int add);

	/// Funktion addiert die �bergebenen Punkte zum jeweiligen Lager einer Rasse. Gleichzeitig wird �berpr�ft, dass
	/// ihr neuer Wert im richtigen Bereich liegt.
	/// @param type Spionage == 0, Sabotage == 1
	/// @param sRace Rasse
	/// @param add der zu addierende Wert
	void AddSPStoragePoints(BOOLEAN type, const CString& sRace, int add);

	/// Funktion f�gt den im Parameter �bergebenen Wert dem Bonus f�r die innere Sicherheit hinzu.
	/// @param add hinzuzuf�gender Bonus
	void AddInnerSecurityBonus(short add) {m_nInnerSecurityBoni += add;}

	/// Funktion f�gt den im Parameter �bergebenen Wert dem Bonus f�r die Wirtschaft hinzu.
	/// @param add hinzuzuf�gender Bonus
	/// @param type Spionage == 0, Sabotage == 1
	void AddEconomyBonus(short add, BOOLEAN type) {m_nEconomyBoni[type] += add;}

	/// Funktion f�gt den im Parameter �bergebenen Wert dem Bonus f�r die Wissenschaft hinzu.
	/// @param add hinzuzuf�gender Bonus
	/// @param type Spionage == 0, Sabotage == 1
	void AddScienceBonus(short add, BOOLEAN type) {m_nScienceBoni[type] += add;}

	/// Funktion f�gt den im Parameter �bergebenen Wert dem Bonus f�r die Milit�r hinzu.
	/// @param add hinzuzuf�gender Bonus
	/// @param type Spionage == 0, Sabotage == 1
	void AddMilitaryBonus(short add, BOOLEAN type) {m_nMilitaryBoni[type] += add;}

	// sonstige Funktionen
	/// Funktion l�scht die produzierten Geheimdienstpunkte.
	void ClearSecurityPoints() {m_iSecurityPoints = 0;}

	/// Funktion l�scht alle Geheimdienstboni.
	void ClearBoni();

	/// Funktion addiert den im Paramter �bergebenen Wert zu den aktuell produzierten Geheimdienstpunkten.
	/// @param add Anzahl der addierten Geheimdienstpunkte
	void AddSecurityPoints(int add);

	/// Resetfunktion f�r das CIntelligence-Objekt.
	void Reset();

private:
	// Attribute
	CString m_sRace;					///< Rassen-ID des Imperiums zu welchem dieses Geheimdienstobjekt geh�rt

	CString m_sResponsibleRace;			///< Rasse welche f�r unsere Geheimdiensttaten verantwortlich gemacht werden kann

	UINT m_iSecurityPoints;				///< globale Anzahl der produzierten Geheimdienstpunkte

	UINT m_iInnerStorage;				///< Lager der angesammelten Geheimdienstpunkte f�r die innere Sicherheit

	map<CString, UINT> m_iSPStorage[2];	///< Lager der angesammelten Spionage- und Sabotagepunkte bei einer bestimmten Rasse

	short m_nInnerSecurityBoni;			///< Bonus auf die innere Sicherheit

	short m_nEconomyBoni[2];			///< Wirtschaftsbonus f�r Spionage und Sabotage;

	short m_nScienceBoni[2];			///< Wissenschaftsbonus f�r Spionage und Sabotage;

	short m_nMilitaryBoni[2];			///< Milit�rbonus f�r Spionage und Sabotage;

	map<CString, BYTE> m_byAggressiveness[2];	///< Aggressivit�t, mit der bei Spionage und Sabotage vorgegangen wird

	CIntelAssignment m_Assignment;		///< Zuteilungen auf die einzelnen Geheimdienstressorts

	CIntelReports m_Reports;			///< alle Geheimdienstreports (Aktionen)

	CIntelInfo m_IntelInfo;				///< Geheimdienstinformationen (Datensammlung �ber andere Rassen)
};
