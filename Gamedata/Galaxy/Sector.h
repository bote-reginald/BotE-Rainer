/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Sector.h: Schnittstelle f�r die Klasse CSector.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "Planet.h"
#include "GenSectorName.h"
#include <map>
#include <set>
#include <vector>

#include "resources.h"
#include "CommandLineParameters.h"
#include "BotE.h"

using namespace std;

// forward declaration
class CBotEDoc;
class CMajor;
class CAnomaly;
class CRace;

/// Liefert verschiedene Attributswerte der Sektorklasse.
enum SectorAttributes
{
	/// Ist ein Sonnensystem in diesem Sektor?
	SECTOR_SUNSYSTEM	= 1,
	/// Geh�rt der Sektor einer irgendeiner Rasse?
	SECTOR_OWNED		= 2,
	/// Gibt es eine kleine Rasse in dem Sector
	SECTOR_MINORRACE	= 4,
	/// Wurde der Sector milit�risch eingenommen, also keine eigene Kolonie oder Heimatsystem
	SECTOR_CONQUERED	= 8
};

class CSector : public CObject
{
public:

	enum DISCOVER_STATUS
	{
		DISCOVER_STATUS_NONE		= 0,
		DISCOVER_STATUS_SCANNED		= 1,
		DISCOVER_STATUS_KNOWN		= 2,
		DISCOVER_STATUS_FULL_KNOWN	= 3
	};

	// Klasse serialisierbar machen
	DECLARE_SERIAL (CSector)

	/// Konstruktor
	CSector(void);

	///Kopierkontruktor
	CSector(const CSector& other);

	CSector& operator=(const CSector&);

	/// Destruktor
	virtual ~CSector(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen zum Lesen der Membervariablen
	/// Diese Funktion gibt die Koordinaten des Sektors zur�ck.
	CPoint GetKO() const {return m_KO;}

	/**
	 * Funktion gibt den Namen des Sektors zur�ck. Wenn in ihm kein Sonnensystem ist, dann wird "" zur�ckgegeben.
	 * Wenn man aber den Parameter <code>longName<code> beim Aufruf der Funktion auf <code>TRUE<code> setzt, wird
	 * versucht ein genauerer Sektorname zu generieren.
	 */
	CString GetName(BOOLEAN longName = FALSE) const;

	/// Diese Funktion gibt zur�ck, ob sich in diesem Sektor ein Sonnensystem befindet.
	BOOLEAN GetSunSystem(void) const {return (m_Attributes & SECTOR_SUNSYSTEM) == SECTOR_SUNSYSTEM;}

	/// Diese Funktion gibt zur�ck, ob der Sektor irgendeiner Majorrasse geh�rt.
	BOOLEAN GetOwned(void) const {return (m_Attributes & SECTOR_OWNED) == SECTOR_OWNED;}

	/// Diese Funktion gibt zur�ck, ob sich in diesem Sektor eine Minorrace befindet.
	BOOLEAN GetMinorRace(void) const {return (m_Attributes & SECTOR_MINORRACE) == SECTOR_MINORRACE;}

	/// Diese Funktion gibt zur�ck, ob der Sektor milit�risch erobert wurde.
	BOOLEAN GetTakenSector(void) const {return (m_Attributes & SECTOR_CONQUERED) == SECTOR_CONQUERED;}

	/// Diese Funktion gibt zur�ck, wer im Besitz dieses Sektors ist.
	CString GetOwnerOfSector(void) const {return m_sOwnerOfSector;}

	/// Diese Funktion gibt zur�ck, wem dieser Sektor zuerst geh�rte. Aber nur, wenn es sich dabei
	/// um eine Kolonie oder um das Heimatsystem handelte.
	CString GetColonyOwner(void) const {return m_sColonyOwner;}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob dieser Sektor von der Majorrace
	/// <code>Race</code> gescannt wurde.
	bool GetScanned(const CString& sRace) const
	{
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap())
			return true;

		map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.find(sRace);
		if (it != m_Status.end())
			return it->second >= DISCOVER_STATUS_SCANNED;
		return false;
	}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob der Name dieses Sektor der
	/// Majorrace <code>Race</code> bekannt ist.
	bool GetKnown(const CString& sRace) const
	{
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap())
			return true;

		map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.find(sRace);
		if (it != m_Status.end())
			return it->second >= DISCOVER_STATUS_KNOWN;
		return false;
	}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob die Majorrace <code>Race</code>
	/// den kompletten Sektor (inkl. der Planeten) kennt.
	bool GetFullKnown(const CString& sRace) const
	{
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap())
			return true;

		map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.find(sRace);
		if (it != m_Status.end())
			return it->second >= DISCOVER_STATUS_FULL_KNOWN;
		return false;
	}

	const DISCOVER_STATUS GetDiscoverStatus(const CString& sRace) const
	{
		const CCommandLineParameters* const clp = resources::pClp;
		if(clp->SeeAllOfMap())
			return DISCOVER_STATUS_FULL_KNOWN;

		map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.find(sRace);
		if (it != m_Status.end())
			return it->second;
		return DISCOVER_STATUS_NONE;
	}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob die Majorrace <code>Race</code>
	/// eine online Werft (bzw. kann auch eine Station sein) in diesem Sektor besitzt.
	bool GetShipPort(const CString& sRace) const
	{
		if (m_bShipPort.find(sRace) != m_bShipPort.end())
			return true;
		return false;
	}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob die Majorrace <code>Race</code>
	/// einen Aussenposten in diesem Sektor besitzt.
	bool GetOutpost(const CString& sRace) const
	{
		return m_Outpost == sRace;
	}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob die Majorrace <code>Race</code>
	/// eine Sternbasis in diesem Sektor besitzt.
	bool GetStarbase(const CString& sRace) const
	{
		return m_Starbase == sRace;
	}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob die Rasse <code>Race</code> ein
	/// bzw. mehrere Schiffe in diesem Sektor hat.
	BOOLEAN GetOwnerOfShip(const CString& sRace) const
	{
		if (m_bWhoIsOwnerOfShip.find(sRace) != m_bWhoIsOwnerOfShip.end())
			return true;
		return false;
	}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob von irgendwem ein Schiff in diesem
	/// Sektor ist
	BOOLEAN GetIsShipInSector(void) const {return !m_bWhoIsOwnerOfShip.empty();}

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob von irgendwem eine Station in diesem
	/// Sektor ist
	BOOLEAN GetIsStationInSector(void) const {return HasOutpost() || HasStarbase();}

	bool HasOutpost() const { return !m_Outpost.IsEmpty(); }
	bool HasStarbase() const { return !m_Starbase.IsEmpty(); }

	/// Diese Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob die Majorrace <code>Race</code>
	/// gerade eine Station in diesem Sektor baut.
	BOOLEAN GetIsStationBuilding(const CString& sRace) const
	{
		if (m_bIsStationBuild.find(sRace) != m_bIsStationBuild.end())
			return true;
		return false;
	}

	//is station buildable in this sector by race according to whatever station exists ?
	bool IsStationBuildable(SHIP_TYPE::Typ station, const CString& race) const;

	/// Diese Funktion gibt die Scanpower zur�ck, die die Majorrace <code>Race</code> in diesem Sektor hat.
	short GetScanPower(const CString& sRace, bool bWith_ships = true) const;

	/// Diese Funktion gibt die Scanpower zur�ck, die man ben�tigt, um das Schiff/die Schiffe der
	/// Majorrace <code>Race</code> in diesem Sektor zu entdecken.
	short GetNeededScanPower(const CString& sRace) const
	{
		map<CString, short>::const_iterator it = m_iNeededScanPower.find(sRace);
		if (it != m_iNeededScanPower.end())
			return it->second;
		else
			return MAXSHORT;
	}

	/// Diese Funktion gibt die ben�tigten Punkte zum Stationenbau zur�ck, die die Majorrace
	/// <code>Race</code> in diesem Sektor noch zur Fertigstellung einer Station ben�tigt.
	short GetNeededStationPoints(const CString& sRace)
	{
		map<CString, short>::const_iterator it = m_iNeededStationPoints.find(sRace);
		if (it != m_iNeededStationPoints.end())
			return it->second;
		else
			return false;
	}

	/// Diese Funktion gibt die Punkte zum Stationenbau zur�ck, die die Majorrace
	/// <code>Race</code> zu Beginn des Stationenbaus ben�tigt hat.
	short GetStartStationPoints(const CString& sRace)
	{
		map<CString, short>::const_iterator it = m_iStartStationPoints.find(sRace);
		if (it != m_iStartStationPoints.end())
			return it->second;
		else
			return false;
	}

	/// Die Funktion gibt die Farbe der Sonne in diesem System zur�ck. Der R�ckgabewert ist ein BYTE-Wert, also nicht
	///die Farbe als Adjektiv.
	BYTE GetSunColor() const {return m_bySunColor;}

	/// Diese Funktion gibt die Anzahl der Planeten zur�ck, die in dem Sektor beheimatet sind.
	BYTE GetNumberOfPlanets(void) const {return m_Planets.size();}

	/// Diese Funktion gibt einen Zeiger auf einen Planeten in diesem System zur�ck.
	CPlanet* GetPlanet(BYTE nPlanetIndex) {return &m_Planets[nPlanetIndex];}

	/// Diese Funktion gibt einen Zeiger auf eine eventuell vorhandene Anomalie zur�ck (<code>NULL</code> wenn kein vorhanden)
	CAnomaly* GetAnomaly(void) const {return m_pAnomaly;}

	/// Diese Funktion gibt eine Referenz auf das Feld mit den Planeten in diesem System zur�ck.
	std::vector<CPlanet>& GetPlanets() {
		return m_Planets;
	}

	/// Funktion gibt alle Einwohner aller Planeten in dem Sektor zur�ck.
	float GetCurrentHabitants() const;

	/// Diese Funktion berechnet die vorhandenen Rohstoffe der Planeten im Sektor. �bergebn wird daf�r ein Feld f�r
	/// die Ressourcen <code>res</code>.
	void GetAvailableResources(BOOLEAN bResources[DERITIUM + 1], BOOLEAN bOnlyColonized = true);

// zum Schreiben der Membervariablen
	/// Diese Funktion setzt die Koordinaten des Sektors und �bernimmt dabei die beiden Koordinatenwerte.
	void SetKO(int x, int y) {m_KO = CPoint(x,y);}

	/// Funktion legt den Namen des Sektors fest.
	void SetSectorsName(const CString& nameOfSunSystem) {m_strSectorName = nameOfSunSystem;}

	/// Diese Funktion legt fest, ob sich ein Sonnensystem in diesem Sektor befindet.
	void SetSunSystem(BOOLEAN is) {SetAttributes(is, SECTOR_SUNSYSTEM, m_Attributes);}

	/// Funktion legt fest, ob der Sektor irgendwem geh�rt.
	void SetOwned(BOOLEAN is) {SetAttributes(is, SECTOR_OWNED, m_Attributes);}

	/// Funktion legt fest, ob sich eine Minorrace in diesem Sektor befindet.
	void SetMinorRace(BOOLEAN is) {SetAttributes(is, SECTOR_MINORRACE, m_Attributes);}

	/// Funktion legt fest, ob der Sektor milit�risch erobert wurde.
	void SetTakenSector(BOOLEAN is) {SetAttributes(is, SECTOR_CONQUERED, m_Attributes);}

	/// Funktion legt fest, wem der Sektor geh�rt.
	void SetOwnerOfSector(const CString& sOwner) {m_sOwnerOfSector = sOwner;}

	/// Funktion legt fest, welcher Majorrace der Sektor zuerst geh�rte. Das gilt aber nur f�r
	/// eigene Kolonien und dem Heimatsektor
	void SetColonyOwner(const CString& sOwner) {m_sColonyOwner = sOwner;}

	/// Diese Funktion legt fest, ob der Sektor von der Majorrace <code>Race</code> gescannt ist.
	void SetScanned(const CString& Race) {if (this->GetScanned(Race) == FALSE) m_Status[Race] = CSector::DISCOVER_STATUS_SCANNED;}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> den Namen des Sektor kennt.
	void SetKnown(const CString& Race) {if (this->GetKnown(Race) == FALSE) m_Status[Race] = CSector::DISCOVER_STATUS_KNOWN;}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> den Sektor kompletten Sektor
	/// (inkl. der ganzen Planeten) kennt.
	void SetFullKnown(const CString& Race) {m_Status[Race] = CSector::DISCOVER_STATUS_FULL_KNOWN;}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> eine online Werft (bzw. auch durch eine
	/// Station erhalten) in diesem Sektor besitzt.
	void SetShipPort(BOOLEAN is, const CString& Race)
	{
		if (is)
			m_bShipPort.insert(Race);
		else
			m_bShipPort.erase(Race);
	}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> einen Aussenposten in diesem Sektor unterh�lt.
	void SetOutpost(const CString& Race)
	{
		m_Outpost = Race;
	}
	void UnsetOutpost(const CString& Race)
	{
		if(m_Outpost == Race)
			m_Outpost.Empty();
	}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> eine Sternbasis in diesem Sektor unterh�lt.
	void SetStarbase(const CString& Race)
	{
		m_Starbase = Race;
	}
	void UnsetStarbase(const CString& Race)
	{
		if(m_Starbase == Race)
			m_Starbase.Empty();
	}

	/// Funktion legt fest, ob die Majorrace <code>Race</code> ein oder auch mehrer Schiffe in diesem Sektor hat.
	void SetOwnerOfShip(BOOLEAN is, const CString& sOwner)
	{
		if (is)
			m_bWhoIsOwnerOfShip.insert(sOwner);
		else
			m_bWhoIsOwnerOfShip.erase(sOwner);
	}

	//Add 1 to the number of race's ships in this sector
	void IncrementNumberOfShips(const CString& race);

	/// Funktion legt fest, ob die Majorrace <code>Race</code> gerade eine Station in diesem Sektor baut.
	void SetIsStationBuilding(BOOLEAN is, const CString& Race)
	{
		if (is)
			m_bIsStationBuild.insert(Race);
		else
			m_bIsStationBuild.erase(Race);
	}

	/// Funktion legt die Scanpower <code>scanpower</code>, welche die Majorrace <code>Race</code>
	/// in diesem Sektor hat, fest.
	void SetScanPower(short scanpower, const CString& Race);

	/// Function puts a scanned square over this sector in the middle, if range == 1, 9 sectors are affected
	/// for instance. Function calculates and sets the scan power values for each of these sectors.
	/// @param sector: The middle sector where the scan power affecting object is. This can be a ship,
	/// a scanning deteriorating anomaly, or a continuum scanner (or continuum scanner upgrade).
	/// @param range: range of the object
	/// @param power: scan power of the object
	/// @param race: race who gets these scan powers (can be a minor race)
	/// @param bBetterScanner: are we on a scanning improving anomaly (implies the scan power affecting object is a ship or base)
	/// @param patrolship: is this a patrolship (implies the scan power affecting object is a ship or base)
	/// @param anomaly: is the scan power affecting object a scanning deteriorating anomaly
	void PutScannedSquare(unsigned range, const int power,
		const CRace& race, bool bBetterScanner = false, bool patrolship = false, bool anomaly = false);

	/// Funktion legt die Scanpower <code>scanpower</code> fest, welche ben�tigt wird, um ein Schiff der
	/// Majorrace <code>Race</code> in diesem Sektor zu erkennen.
	void SetNeededScanPower(short scanpower, const CString& Race)
	{
		if (scanpower != MAXSHORT)
			m_iNeededScanPower[Race] = scanpower;
		else
			m_iNeededScanPower.erase(Race);
	}

	/// Diese Funktion zieht <code>sub</code> Punkte von den noch ben�tigten Stationbaupunkten der Majorrace
	/// <code>Race</code> ab.
	BOOLEAN SetNeededStationPoints(int sub, const CString& Race)
	{
		m_iNeededStationPoints[Race] -= sub;
		if (m_iNeededStationPoints[Race] <= 0)
		{
			m_iNeededStationPoints.erase(Race);
			return TRUE;
		}
		else return FALSE;
	}

	/// Diese Funktion legt die Stationsbaupunkte <code>value</code> der Majorrace <code>Race</code> fest,
	/// welche zu Beginn des Baus ben�tigt werden.
	void SetStartStationPoints(int value, const CString& Race)
	{
		if (value)
		{
			m_iStartStationPoints[Race] = value;
			m_iNeededStationPoints[Race] = value;
		}
		else
		{
			m_iStartStationPoints.erase(Race);
			m_iNeededStationPoints.erase(Race);
		}
	}

	/// Diese Funktion addiert Besitzerpunkte <code>ownerPoints</code> zu den Punkten des Besitzers <code>owner</owner>
	/// dazu.
	void AddOwnerPoints(BYTE ownerPoints, const CString& sOwner) {m_byOwnerPoints[sOwner] += ownerPoints;}

// sonstige Funktionen

	/**
	 * Funktion generiert den Sektor. Dabei wird als Parameter die Wahrscheinlichkeit, ob in dem Sektor ein
	 * Sonnensystem ist, im Paramter <code>sunProb</code> in Prozent �bergeben. Im Parameter <code>minorProb</code>
	 * wird die Wahrscheinlichkeit in Prozent �bergeben, dass sich in dem Sektor eine Minorrace befindet.
	 */
	void GenerateSector(int sunProb, int minorProb);

	/// Diese Funktion generiert die Planeten in dem Sektor.
	/// @param majorNumber Nummer der Majorrace, wenn deren Planeten im Sektor generiert werden soll. Wird kein Wert
	/// angegeben, so werden zuf�llige Planeten erstellt.
	void CreatePlanets(const CString& sMajorID = "");

	/// Funktion erzeugt eine zuf�llige Anomalie im Sektor.
	void CreateAnomaly(void);

	/// Diese Funktion f�hrt das Planetenwachstum f�r diesen Sektor durch.
	void LetPlanetsGrowth();

	//Sets whether to display planets as being terraformed based on all currently terraforming ships in this sector
	void RecalcPlanetsTerraformingStatus();

	/// Diese Funktion l�sst die Bev�lkerung auf allen Planeten zusammen um den �bergebenen Wert <code>Value</code>
	/// schrumpfen.
	void LetPlanetsShrink(float Value);

	/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
	/// machen, werden diese Variablen sp�ter ja wieder korrekt gesetzt. Au�erdem werden auch die Besitzerpunkte wieder
	/// gel�scht.
	void ClearAllPoints();

	/// Diese Funktion berechnet anhand der Besitzerpunkte und anderen Enfl�ssen, wem dieser Sektor schlussendlich
	/// geh�rt. �bergeben wird daf�r auch der m�gliche Besitzer des Systems in diesem Sektor.
	void CalculateOwner(const CString& sSystemOwner);

	/// Resetfunktion f�r die Klasse CSector
	void Reset();

// Zeichenfunktionen f�r diese Klasse
	/// Diese Funktion zeichnet den Namen des Sektors.
	void DrawSectorsName(CDC *pDC, CBotEDoc* pDoc, CMajor* pPlayer);

	//Should any their_race_id's ship in this sector be visible to our_race ?
	//not for outposts/starbases
	bool ShouldDrawShip(const CMajor& our_race, const CString& their_race_id) const;
	//Should any their_race_id's outpost/starbase (perhaps under construction) in this sector be visible to our_race ?
	//only for outposts/starbases
	bool ShouldDrawOutpost(const CMajor& our_race, const CString& their_race_id) const;
	/// Diese Funktion zeichnet die entsprechenden Schiffssymbole in den Sektor
	void DrawShipSymbolInSector(Graphics *g, CBotEDoc* pDoc, CMajor* pPlayer);

private:
	/// Die Koordinate des Sektors auf der Map
	CPoint m_KO;

	/// Der Name des Sectors
	CString m_strSectorName;

	/// Diese Variable beinhaltet alle Sektorattribute.
	int m_Attributes;

	/// Wem geh�rt der Sektor?
	CString m_sOwnerOfSector;

	/// Wem geh�rt/geh�rte der Sektor zuerst? Also eigene Kolonie und Heimatsystem
	CString m_sColonyOwner;

	/// Variable speichert den Status �ber diesen Sektor, wobei 0 -> nichts, 1 -> gescannt,
	/// 2 -> Name bekannt, 3 -> alles inkl. Planeten bekannt, bedeutet
	map<CString, DISCOVER_STATUS> m_Status;

	/// Gibts in diesem Sektor eine online Werft (bzw. auch Station)
	set<CString> m_bShipPort;

	/// Besitzt die jeweilige Rasse in dem Sektor einen Au�enposten?
	CString m_Outpost;

	/// Besitzt die jeweilige Rasse in dem Sektor eine Sternbasis?
	CString m_Starbase;

	/// Hat eine Majorrace ein Schiff in diesem Sektor?
	set<CString> m_bWhoIsOwnerOfShip;

	//race CString has this many ships in this sector
	std::map<CString, unsigned> m_mNumbersOfShips;

	/// Baut eine bestimmte Majorrasse gerade eine Station in dem Sektor?
	set<CString> m_bIsStationBuild;

	/// Scanst�rke der jeweiligen Major/Minorrace in dem Sektor
	map<CString, short> m_iScanPower;

	/// ben�tigte Scanst�rke der Majorrace, um ihr Schiff in diesem Sektor erkennen zu k�nnen
	map<CString, short> m_iNeededScanPower;

	/// Die aktuell ben�tigten Stationsbaupunkte bis zur Fertigstellung
	map<CString, short> m_iNeededStationPoints;

	/// Die anf�nglich ben�tigten Stationsbaupunkte (ben�tigt zur prozentualen Anzeige)
	map<CString, short> m_iStartStationPoints;

	/// Die Farbe der Sonne in diesem Sektor
	BYTE m_bySunColor;

	/// Punktem welche angeben, wer den gr��ten Einfluss auf diesen Sektor hat. Wer die meisten Punkte hat und kein
	/// anderer in diesem Sektor einen Au�enposten oder eine Kolonie besitzt, dem geh�rt der Sektor
	map<CString, BYTE> m_byOwnerPoints;

	/// Die Feld mit den einzelnen Planeten in dem Sektor
	std::vector<CPlanet> m_Planets;

	/// M�gliche Anomalie in dem Sektor (NULL wenn keine vorhanden)
	CAnomaly* m_pAnomaly;
};
