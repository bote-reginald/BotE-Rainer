/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Constants.h"
#include <map>
#include "Ships/ShipMap.h"

using namespace std;

// forward declaration
class CBotEDoc;
class CSectorAI;
class CMajor;
class CShips;

class CShipAI
{
public:
	/// Konstruktor
	CShipAI(CBotEDoc* pDoc);

	/// Destruktor
	~CShipAI(void);

	/// Diese Funktion erteilt allen Schiffen aller computergesteuerten Rassen Befehle.
	void CalculateShipOrders(CSectorAI* SectorAI);

private:
	//gives ship orders to alien ships
	void CalculateAlienShipOrders(CShips& ship);

	/// Funktion erteilt einen Terraformbefehl, sofern dies auch m�glich ist.
	/// @param pShip Zeiger des Schiffes
	/// @return <code>true</code> wenn ein Terraformbefehl gegeben werden k�nnte
	bool DoTerraform(CShips* pShip);

	/// Funktion erteilt einen Kolonisierungsbefehl, sofern dies auch m�glich ist.
	/// @param pShip Zeiger des Schiffes
	/// @return <code>true</code> wenn ein Kolonisierungsbefehl gegeben werden k�nnte
	bool DoColonize(CShips* pShip);

	/// Funktion schickt Kriegsschiffe zu einem m�glichen Offensivziel. Unter Umst�nden wird auch der Befehl zum
	/// Angriff automatisch erteilt. Auch kann es passieren, das diese Funktion die Kriegsschiffe zu einem
	/// gemeinsamen Sammelpunkt schickt.
	/// @param pShip Zeiger des Schiffes
	/// @param pMajor Zeiger auf den Besitzer des Schiffes
	/// @return <code>true</code> wenn ein Bewegungsbefehl gegeben werden k�nnte
	bool DoAttackMove(CShips* pShip, const CMajor* pMajor);

	/// Funktion schickt Kriegsschiffe zu einem m�glichen System, welches Bombardiert werden k�nnte.
	/// @param pShip Zeiger des Schiffes
	/// @return <code>true</code> wenn ein Bewegungsbefehl gegeben werden konnte, eine Bombardierung befohlen wurde oder das Schiff auf Verst�rkung zur Bombardierung im System wartet
	bool DoBombardSystem(CShips* pShip);

	/// Funktion erteilt einen Tarnbefehl oder Enttarnbefehl.
	/// @param pShip Zeiger des Schiffes
	/// @param bCamouflage <code>true</code> f�r Tarnen, <code>false</code> f�r Enttarnen
	/// @return <code>true</code> wenn der Befehl gegeben wurde
	static bool DoCamouflage(CShips* pShip, bool bCamouflage = true);

	/// Funktion erteilt einen Au�enpostenbaubefehl, sofern dies auch m�glich ist.
	/// @param pShip Zeiger des Schiffes
	/// @return <code>true</code> wenn ein Au�enpostenbaubefehl gegeben werden k�nnte
	bool DoStationBuild(CShips* pShip);

	/// Funktion erstellt eine Flotte. Schiffe werden der Flotte nur hinzugef�gt, wenn diese bestimmte Voraussetzungen erf�llen.
	/// So muss der ungef�hre Schiffstyp �bereinstimmen (Combat <-> NonCombat) sowie die Geschwindigkeit des Schiffes.
	/// @param pShip Zeiger des Schiffes
	/// @param nIndex Index des aktuellen Schiffes im Array.
	void DoMakeFleet(const CShipMap::iterator& pShip);

	/// Funkion berechnet einen m�glichen Angriffssektor, welcher sp�ter gesammelt angegriffen werden kann.
	void CalcAttackSector(void);

	/// Funktion berechnet einen m�glich besten Sektor f�r eine Bombardierung. Wurde solch ein Sektor ermittelt hat dieser
	/// die allerh�chste Priorit�t.
	void CalcBombardSector(void);

	/// Ein Zeiger auf das Document.
	CBotEDoc* m_pDoc;

	/// Ein Zeiger auf das SectorAI-Objekt
	CSectorAI* m_pSectorAI;

	map<CString, CPoint> m_AttackSector;	///< der globale Angriffssektor der einzelnen Rassen

	map<CString, CPoint> m_BombardSector;	///< der globale Bombardierungssektor der einzelnen Rassen

	map<CString, int> m_iAverageMoral;		///< die durchschnittliche Moral in allen Systemen der einzelnen Rassen
};
