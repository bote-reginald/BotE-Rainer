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
#include "Races\Starmap.h"
#include <map>
#include <vector>
#include <utility>

using namespace std;

// forward declaration
class CBotEDoc;
class CShips;

class CSectorAI
{
public:

	struct SectorToTerraform {
		BYTE pop;
		CPoint p;

		bool operator< (const SectorToTerraform& elem2) const { return pop < elem2.pop;}
		bool operator> (const SectorToTerraform& elem2) const { return pop > elem2.pop;}

		SectorToTerraform() : pop(0), p(-1, -1) {}
		SectorToTerraform(BYTE _pop, CPoint _p) : pop(_pop), p(_p) {}
	};

	/// Konstruktor
	CSectorAI(CBotEDoc* pDoc);

	/// Destruktor
	~CSectorAI(void);

	/// Diese Funktion gibt das Gefahrenpotenzial in einem Sektor <code>sector</code> durch eine bestimmte
	/// Rasse <code>sRaceID</code> zur�ck.
	UINT GetDanger(const CString& sRaceID, const CPoint& sector) {return m_iDangers[sRaceID][pair<int, int>(sector.x, sector.y)];}

	/// Diese Funktion gibt das Gefahrenpotenzial in einem Sektor <code>sector</code> durch eine bestimmte
	/// Rasse <code>sRaceID</code> zur�ck. Dabei werden aber nur Kriegsschiffe beachtet.
	UINT GetDangerOnlyFromCombatShips(const CString& sRaceID, const CPoint& sector) {return m_iCombatShipDangers[sRaceID][pair<int, int>(sector.x, sector.y)];}

	/// Diese Funktion gibt das gesamte Gefahrenpotenzial aller Rassen in einem Sektor <code>sector</code> zur�ck.
	/// Das Gefahrenpotential der eigenen Rasse <code>sOwnRaceID</code> wird dabei aber nicht mit eingerechnet.
	UINT GetCompleteDanger(const CString& sOwnRaceID, const CPoint& sector) const;

	/// Diese Funktion gibt das gesamte Gefahrenpotenzial durch Kriegsschiffe einer Rasse <code>sRaceID</code> aus
	/// allen Sektoren zur�ck.
	UINT GetCompleteDanger(const CString& sRaceID) {return m_iCompleteDanger[sRaceID];}

	/// Funktion gibt die Sektorkoordinate zur�ck, in welchem eine Rasse <code>sRaceID</code> die st�rkste Ansammlung
	/// von Kriegsschiffen hat.
	CPoint GetHighestShipDanger(const CString& sRaceID) {return m_HighestShipDanger[sRaceID];}

	/// Funktion gibt die Sektorkoordinate zur�ck, in welchem eine Rasse <code>sRaceID</code> einen Au�enposten
	/// bauen sollte.
	BaseSector GetStationBuildSector(const CString& sRaceID) {return m_mStationBuild[sRaceID];}

	/// Diese Funktion gibt einen Zeiger auf das geordnete Feld der zum Terraforming am geeignetesten Systeme zur�ck.
	/// Daf�r muss nur die jeweilige Rasse <code>sRaceID</code> �bergeben werden.
	vector<SectorToTerraform>* GetSectorsToTerraform(const CString& sRaceID) {return &(m_vSectorsToTerraform[sRaceID]);}

	/// Diese Funktion gibt einen Zeiger auf das Feld mit den Sektoren der unbekannten Minors zur�ck. �bergeben muss
	/// daf�r nur die jeweilige Rasse <code>sRaceID</code> werden.
	vector<CPoint>* GetMinorraceSectors(const CString& sRaceID) {return &(m_vMinorraceSectors[sRaceID]);}

	/// Diese Funktion gibt einen Zeiger auf das Feld mit den Sektoren f�r m�gliche Offensivziele zur�ck. �bergeben muss
	/// daf�r nur die jeweilige Rasse <code>sRaceID</code> werden.
	vector<CPoint>* GetOffensiveTargets(const CString& sRaceID) {return &(m_vOffensiveTargets[sRaceID]);}

	/// Diese Funktion gibt einen Zeiger auf das Feld mit den Sektoren f�r m�gliche Bombardierungsziele zur�ck.
	/// �bergeben muss daf�r nur die jeweilige Rasse <code>sRaceID</code> werden.
	vector<CPoint>* GetBombardTargets(const CString& sRaceID) {return &(m_vBombardTargets[sRaceID]);}

	/// Funktion gibt die Anzahl an Kolonieschiffen der Rasse <code>sRaceID</code> zur�ck.
	short GetNumberOfColoships(const CString& sRaceID) {return m_iColoShips[sRaceID];}

	/// Funktion gibt die Anzahl an Tranportschiffen der Rasse <code>sRaceID</code> zur�ck.
	short GetNumberOfTransportShips(const CString& sRaceID) {return m_iTransportShips[sRaceID];}

	/// Diese Funktion tr�gt die ganzen Gefahrenpotenziale der Schiffe und Stationen in die Variable
	/// <code>m_iDangers</code> ein. Au�erdem wird hier auch gleich die Anzahl der verschiedenen Schiffe
	/// der einzelnen Rassen ermittelt.
	void CalculateDangers();

	/// Diese Funktion berechnet die ganzen Wichtigkeiten f�r die einzelnen Sektoren. Also wo lohnt es sich zum
	/// Terraformen, wo lernt man neue Minorraces kennen usw. Vorher sollte die Funktion <code>CalculateDangers()
	/// </code> aufgerufen werden.
	void CalcualteSectorPriorities();

	/// Funktion l�scht alle vorher berechneten Priorit�ten.
	void Clear(void);

private:
	/// Diese Funktion addiert die Offensiv- und Defensivst�rke eines Schiffes einer Rasse zum jeweiligen
	/// Sektor.
	void AddDanger(const CShips* ship);

	/// Diese Funktion ermittelt die Sektoren, welche sich am ehesten zum Terraformen f�r eine bestimmte Rasse eignen.
	/// Die Eintr�ge werden dann im Array <code>m_SectorsToTerraform</code> gemacht.
	void CalculateTerraformSectors(int x, int y);

	/// Funktion berechnet die Sektoren, in denen eine einem Imperium unbekannte Minorrace lebt, zu deren Sektor
	/// aber theoretisch geflogen werden kann. Das Ergebnis wird im Array <code>m_MinorraceSectors</code> gespeichert.
	void CalculateMinorraceSectors(int x, int y);

	/// Diese Funktion berechnet alle m�glichen offensiven Ziele f�r eine bestimmte Rasse. Das Ergebnis wird im Array
	/// <code>m_OffensiveTargets</code> gespeichert.
	void CalculateOffensiveTargets(int x, int y);

	/// Diese Funktion berechnet Systeme, welche im Kriegsfall wom�glich angegriffen werden k�nnen. Das Ergebnis wird
	/// im Array <code>m_BombardTargets</code> gespeichert.
	void CalculateBombardTargets(const CString& sRaceID, int x, int y);

	/// Diese Funktion berechnet einen Sektor, welcher sich zum Bau eines Au�enpostens eignet.
	/// Funktion erst nach der Berechnung der Terraformsektoren aufrufen.
	void CalculateStationTargets(const CString& sRaceID);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Variablen

	/// Ein Zeiger auf das Document.
	CBotEDoc* m_pDoc;

	/// In dieser Variable stehen die addierten Offensiv- und Defensivwerte der einzelnen Rassen f�r jeden Sektor. Diese
	/// Werte spiegeln im Prinzip die St�rke der Schiffe wieder.
	/// CString -> Rassen-ID; pair<int,int> -> Sektor-KO; UINT -> St�rkewert
	map<CString, map<pair<int, int>, UINT> > m_iDangers;

	/// In dieser Variable stehen die addierten Offensiv- und Defensivwerte der einzelnen Rassen f�r jeden Sektor.
	/// Doch befinden sich in dieser Variable nur die St�rken von Kriegsschiffen.
	/// CString -> Rassen-ID; pair<int,int> -> Sektor-KO; UINT -> St�rkewert
	map<CString, map<pair<int, int>, UINT> > m_iCombatShipDangers;

	/// Variable beinhaltet die komplette St�rke aller Kriegsschiffe einer Rasse.
	map<CString, UINT> m_iCompleteDanger;

	/// Variable beinhaltet den Sektor, in dem die jeweilige Rasse die st�rkste Schiffsansammlung hat. Stationen
	/// werden dabei nicht mit beachtet.
	map<CString, CPoint> m_HighestShipDanger;

	/// Varibale beinhaltet den Sektor, in welchem die jewelige Rasse einen Au�enposten bauen sollte.
	map<CString, BaseSector> m_mStationBuild;

	/// In diesem Feld werden die Sektoren f�r die jeweilige Rasse gespeichert, welche sich am meisten zum
	/// Terraformen lohnen.
	map<CString, vector<SectorToTerraform> > m_vSectorsToTerraform;

	/// In diesem Feld werden die Sektoren von Minorraces gespeichert, die die entsprechende Rasse noch nicht
	/// kennt, zu denen sie aber fliegen kann, um sie kennenzulernen.
	map<CString, vector<CPoint> > m_vMinorraceSectors;

	/// In diesem Feld stehen m�gliche Offensivziele einer bestimmten Rasse drin. Dabei kann es sich um eine
	/// gegnerische Flotte aber auch um einen Aussenposten handeln.
	map<CString, vector<CPoint> > m_vOffensiveTargets;

	/// In diesem Feld stehen m�gliche Systeme, welche im Kriegsfall bombardiert werden k�nnten.
	map<CString, vector<CPoint> > m_vBombardTargets;

	/// Variable beinhaltet die Anzahl an Kolonieschiffen einer bestimmten Rasse
	map<CString, short> m_iColoShips;

	/// Variable beinhaltet die Anzahl an Truppentransportern einer bestimmten Rasse
	map<CString, short> m_iTransportShips;
};
