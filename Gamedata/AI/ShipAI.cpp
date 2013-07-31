#include "stdafx.h"
#include "ShipAI.h"
#include "BotEDoc.h"
#include "SectorAI.h"
#include "Races\RaceController.h"
#include "Ships/Ships.h"

#include <cassert>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShipAI::CShipAI(CBotEDoc* pDoc) : m_pSectorAI()
{
	ASSERT(pDoc);
	m_pDoc = pDoc;

	map<CString, CRace*>* mRaces = m_pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(mRaces);

	// Durchschnittsmoral berechnen
	map<CString, int> moralAll;
	map<CString, int> systems;
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_pDoc->GetSystem(x, y).GetOwnerOfSystem() != "")
			{
				moralAll[m_pDoc->GetSystem(x, y).GetOwnerOfSystem()] += m_pDoc->GetSystem(x, y).GetMoral();
				systems[m_pDoc->GetSystem(x, y).GetOwnerOfSystem()] += 1;
			}

	// alles initial initialisieren
	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); ++it)
	{
		m_AttackSector[it->first] = CPoint(-1,-1);
		m_BombardSector[it->first] = CPoint(-1,-1);

		if (systems[it->first] > 0)
			m_iAverageMoral[it->first] = moralAll[it->first] / systems[it->first];
		else
			m_iAverageMoral[it->first] = 0;
	}
}

CShipAI::~CShipAI(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion erteilt allen Schiffen aller computergesteuerten Rassen Befehle.
void CShipAI::CalculateShipOrders(CSectorAI* SectorAI)
{
	ASSERT(SectorAI);
	m_pSectorAI = SectorAI;

	// einen m�glichen Angriffssektor berechnen lassen
	CalcAttackSector();
	// danach einen m�glichen Bombardierungssektor finden
	CalcBombardSector();

	for(CShipMap::iterator i = m_pDoc->m_ShipMap.begin(); i != m_pDoc->m_ShipMap.end(); ++i)
	{
		if(i->second->IsAlien()) {
			CalculateAlienShipOrders(*i->second);
			continue;
		}
		const CString& sOwner	= i->second->GetOwnerOfShip();
		CMajor* pOwner	= dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sOwner));

		// gilt erstmal nur f�r Majors
		if (!pOwner || !pOwner->IsMajor())
			continue;

		// gilt nicht f�r menschliche Spieler
		if (pOwner->AHumanPlays())
			continue;

		// Flotte versuchen zu erstellen
		DoMakeFleet(i);

		// Vielleicht haben unsere Schiffe ein Ziel, welches sie angreifen m�ssen/k�nnen
		if (DoAttackMove(i->second, pOwner))
		{
			DoCamouflage(i->second);
			continue;
		}

		// haben Kolonieschiffe einen Sektor zum Terraformen als Ziel, welcher kurz zuvor aber von einer
		// anderen Rasse weggeschnappt wurde, so wird ihr Kurs gel�scht
		if (i->second->GetShipType() == SHIP_TYPE::COLONYSHIP)
		{
			CPoint ptKO = i->second->GetKO();
			// hat das Kolonieschiff den Befehl zum Terraformen, so wird dieser r�ckg�ngig gemacht, wenn der Sektor
			// schon einer anderen Rasse geh�rt
			if (i->second->GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
			{
				if (m_pDoc->GetSector(ptKO.x, ptKO.y).GetOwnerOfSector() != "" && m_pDoc->GetSector(ptKO.x, ptKO.y).GetOwnerOfSector() != sOwner)
				{
					// Terraforming abbrechen
					i->second->UnsetCurrentOrder();
					i->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
				}
			}

			CPoint ptTarget = i->second->GetTargetKO();
			// nur wenn der Sektor noch niemandem geh�rt bzw. uns selbst ist, sollen Planeten terraformt werden
			if (ptTarget != CPoint(-1,-1) && m_pDoc->GetSector(ptTarget.x, ptTarget.y).GetOwnerOfSector() != "" && m_pDoc->GetSector(ptTarget.x, ptTarget.y).GetOwnerOfSector() != sOwner)
			{
				// nicht weiter fliegen und Kurs l�schen
				i->second->SetTargetKO(CPoint(-1, -1));
				i->second->GetPath()->RemoveAll();
			}
		}

		// exisitiert kein aktueller Kurs, so wird dieser hier versucht dem Schiff zu erteilen
		if (i->second->GetPath()->GetSize() == 0)
		{
			// Scouts und Kriegsschiffe fliegen zuerst einmal zu den Minorracesystemen
			if (i->second->GetShipType() > SHIP_TYPE::COLONYSHIP)
			{
				// Zeiger auf Vektor mit Minorracessektoren holen
				vector<CPoint>* vMinorraceSectors = m_pSectorAI->GetMinorraceSectors(sOwner);

				bool bSet = false;
				int nCount = vMinorraceSectors->size() * 2;
				while (vMinorraceSectors->size() && nCount--)
				{
					int j = rand()%vMinorraceSectors->size();
					CPoint ko = vMinorraceSectors->at(j);
					// Wenn Gefahr der anderen Rassen kleiner als die der meinen ist
					if (m_pSectorAI->GetCompleteDanger(sOwner, ko) == NULL ||
						(m_pSectorAI->GetCompleteDanger(sOwner, ko) <= m_pSectorAI->GetDangerOnlyFromCombatShips(sOwner, i->second->GetKO())))
						if (pOwner->GetStarmap()->GetRange(ko) <= i->second->GetRange(false))
						{
							// Zielkoordinate f�r das Schiff setzen
							i->second->SetTargetKO(ko);
							MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: Ship to Minor: %s (%s) - Target: %d,%d, MinorDanger:%i\n",
								sOwner, i->second->GetShipName(), i->second->GetShipTypeAsString(), ko.x,ko.y, m_pSectorAI->GetCompleteDanger(sOwner, ko));
							MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: Ship to Minor: %s, MinorDanger:%i, DangerCombatShips:%i\n",
								sOwner, m_pSectorAI->GetCompleteDanger(sOwner, ko), m_pSectorAI->GetDangerOnlyFromCombatShips(sOwner, i->second->GetKO()));

							vMinorraceSectors->erase(vMinorraceSectors->begin() + j--);
							bSet = true;
							break;
						}
				}

				if (bSet)
				{
					DoCamouflage(i->second);
					continue;
				}
			}

			// Kolonieschiffe zum Terraformen schicken. Andere Schiffe fliegen manchmal auch dort hin, wenn
			// sie gerade keinen anderen Flugauftrag haben.
			if (i->second->GetShipType() >= SHIP_TYPE::COLONYSHIP && i->second->GetCurrentOrder() != SHIP_ORDER::TERRAFORM)
			{
				// Zeiger auf Vektor mit Terraformsektoren holen
				vector<CSectorAI::SectorToTerraform>* vSectorsToTerrform = m_pSectorAI->GetSectorsToTerraform(sOwner);

				for (UINT j = 0; j < vSectorsToTerrform->size(); j++)
				{
					CPoint ko = vSectorsToTerrform->at(j).p;
					// Wenn das Kolonieschiff schon auf einem Sektor f�r unser Terraforming steht, so fliegt es nicht weiter
					if (i->second->GetShipType() == SHIP_TYPE::COLONYSHIP && i->second->GetKO() == ko)
						break;

					// Wenn Gefahr der anderen Rassen kleiner als die der meinen ist
					if (m_pSectorAI->GetCompleteDanger(sOwner, ko) == NULL || (m_pSectorAI->GetCompleteDanger(sOwner, ko) < m_pSectorAI->GetDanger(sOwner, i->second->GetKO())))
					{
						if (pOwner->GetStarmap()->GetRange(ko) <= i->second->GetRange(false))
						{
							// Zielkoordinate f�r das Schiff setzen
							i->second->SetTargetKO(ko == i->second->GetKO() ? CPoint(-1, -1) : ko);
							MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: Ship %s (%s) has terraforming target: %d,%d\n",
								sOwner, i->second->GetShipName(), i->second->GetShipTypeAsString(), ko.x,ko.y);
							MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: Ship has terraforming target: EnemyDanger:%i, DangerCombatShips:%i\n",
								sOwner, m_pSectorAI->GetCompleteDanger(sOwner, ko), m_pSectorAI->GetDanger(sOwner, i->second->GetKO()));
								//sOwner, m_pSectorAI->GetCompleteDanger(sOwner, ko), m_pSectorAI->m_pSectorAI->GetDanger(sOwner, i->second->GetKO()));

							break;
						}
					}
				}
			}
			// Truppentransporter zu einem m�glichen Sektor fliegen lassen um dort einen Au�enposten bauen zu k�nnen
			if (m_pSectorAI->GetStationBuildSector(sOwner).points > MINBASEPOINTS && i->second->GetCurrentOrder() != SHIP_ORDER::BUILD_OUTPOST)
			{
				// nur Truppentransporter oder andere Schiffe ohne Ziel fliegen zu diesem Punkt, niemals aber
				// Kolonieschiffe
				if (i->second->GetShipType() == SHIP_TYPE::TRANSPORTER	|| (i->second->GetShipType() != SHIP_TYPE::COLONYSHIP && !i->second->HasTarget()))
				{
					CPoint ko(m_pSectorAI->GetStationBuildSector(sOwner).position.x, m_pSectorAI->GetStationBuildSector(sOwner).position.y);
					// Wenn Gefahr der anderen Rassen kleiner als die der meinen ist
					if (m_pSectorAI->GetCompleteDanger(sOwner, ko) == 0 || (m_pSectorAI->GetCompleteDanger(sOwner, ko) < m_pSectorAI->GetDanger(sOwner, i->second->GetKO())))
					{
						if (pOwner->GetStarmap()->GetRange(ko) <= i->second->GetRange(false))
						{
							// Zielkoordinate f�r das Schiff setzen
							i->second->SetTargetKO(ko == i->second->GetKO() ? CPoint(-1, -1) : ko);
							MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: Ship %s (%s) has stationbuild target: %d,%d\n",
								sOwner, i->second->GetShipName(), i->second->GetShipTypeAsString(), ko.x,ko.y);
							MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: stationbuild target: EnemyDanger:%i, DangerCombatShips:%i\n",
								sOwner, m_pSectorAI->GetCompleteDanger(sOwner, ko), m_pSectorAI->GetDanger(sOwner, i->second->GetKO()));
						}
					}
				}
			}

			DoCamouflage(i->second);
			if (m_pDoc->GetSector(i->second->GetKO().x, i->second->GetKO().y).GetSunSystem())
			{
				if (!DoTerraform(i->second))
					DoColonize(i->second);
			}

			DoStationBuild(i->second);
		}
		else
		{
			DoCamouflage(i->second);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
void CShipAI::CalculateAlienShipOrders(CShips& ship)
{
	if (ship.GetOwnerOfShip() == MIDWAY_ZEITREISENDE)
	{
		const CPoint& co = ship.GetKO();
		const CSector& sector = m_pDoc->GetSector(co.x, co.y);
		if (sector.GetSunSystem())
		{
			const CSystem& system = m_pDoc->GetSystemForSector(sector);
			const CRace* owner = m_pDoc->GetRaceCtrl()->GetRace(system.GetOwnerOfSystem());
			if (owner && owner->IsMajor() && owner->GetAgreement(MIDWAY_ZEITREISENDE) == DIPLOMATIC_AGREEMENT::WAR)
			{
				// Damit irgendwann auch einmal der Angriffsbegehl zur�ckgenommen wird und das Midwayschiff nicht
				// ewig ein und dasselbe System bombardiert, hier etwas Zufall (80% wird bombardiert)
				if (ship.GetCurrentOrder() != SHIP_ORDER::ATTACK_SYSTEM || rand()%100 > 20)
				{
					// evtl. vorhandenen Zielkurs r�ckg�ngig machen
					ship.SetTargetKO(CPoint(-1, -1));
					// Angriffsbefehl geben und fertig
					ship.SetCurrentOrder(SHIP_ORDER::ATTACK_SYSTEM);
					return;
				}
			}
			MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race MIDWAY:%s Ship %s, Sector: %d,%d, Order:%s\n",
								owner, ship.GetShipName(), co.x,co.y, ship.GetCurrentOrderAsString());
		}

		// in allen anderen F�llen keinen Systemangriffsbefehl geben bzw. diesen zur�cksetzen
		ship.UnsetCurrentOrder();
		return;
	}
}

/// Funktion erteilt einen Terraformbefehl, sofern dies auch m�glich ist.
/// @param pShip Zeiger des Schiffes
/// @return <code>true</code> wenn ein Terraformbefehl gegeben werden k�nnte
bool CShipAI::DoTerraform(CShips* pShip)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	// Es wird jeder Planet sofort genommen, welcher weniger als 8 Runden zum Terraformen
	// ben�tigt. Planeten welche mehr Runden zum Terraformen ben�tigen werden nur ausgew�hlt,
	// wenn in dem Sektor kein Planet zum sofortigen Kolonisieren zur Verf�gung steht

	int nTerraPoints = pShip->GetColonizePoints();
	if (nTerraPoints <= 0)
		return false;

	CSector* pSector = &m_pDoc->GetSector(pShip->GetKO().x, pShip->GetKO().y);
	// nur wenn der Sektor noch niemandem geh�rt bzw. uns selbst ist, sollen Planeten terraformt werden
	if (pSector->GetOwnerOfSector() != "" && pSector->GetOwnerOfSector() != pShip->GetOwnerOfShip())
		return false;

	int nMinTerraPoints = INT_MAX;
	short nPlanet = -1;

	bool bColonizable = false;
	for (int j = 0; j < pSector->GetNumberOfPlanets(); j++)
	{
		const CPlanet* pPlanet = pSector->GetPlanet(j);
		// Planet �berhaupt bewohnbar?
		if (!pPlanet->GetHabitable())
			continue;

		// Planet schon terraformt?
		if (pPlanet->GetTerraformed())
		{
			// und noch nicht kolonisiert?
			if (!pPlanet->GetColonized())
				bColonizable = true;
		}
		else if (pPlanet->GetNeededTerraformPoints() < nMinTerraPoints)
		{
			nMinTerraPoints = pPlanet->GetNeededTerraformPoints();
			nPlanet = j;
		}
	}

	// Wurde ein zu terraformender Planet gefunden und w�rden weniger als 6 Runden
	// zum Terraformen ben�tigt werden oder es gibt keinen Planeten, der
	// sofort kolonisiert werden k�nnte, dann den gefundenen Planeten terraformen
	if (nPlanet != -1 && (!bColonizable || nMinTerraPoints / nTerraPoints < 8))
	{
		const CPlanet* pPlanet = pSector->GetPlanet(nPlanet);
		// Hier muss als erstes ein m�glicher neuer Kurs gel�scht werden
		pShip->SetTargetKO(CPoint(-1, -1));
		pShip->SetTerraform(nPlanet);
		MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: Sector: %d,%d: Planet:%d will be terraformed (needed Points:%d)\n",
								pShip->GetOwnerOfShip(), pShip->GetKO().x,pShip->GetKO().y, nPlanet, pPlanet->GetNeededTerraformPoints());


		return true;
	}

	return false;
}

/// Funktion erteilt einen Kolonisierungsbefehl, sofern dies auch m�glich ist.
/// @param pShip Zeiger des Schiffes
/// @return <code>true</code> wenn ein Kolonisierungsbefehl gegeben werden k�nnte
bool CShipAI::DoColonize(CShips* pShip)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	// Kolonieschiffe eine Kolonisierung vorschlagen
	if (pShip->GetShipType() != SHIP_TYPE::COLONYSHIP)
		return false;

	CSector* pSector = &m_pDoc->GetSector(pShip->GetKO().x, pShip->GetKO().y);
	// Geh�rt der Sektor aktuell auch keiner Minorrace (also niemanden oder uns selbst)
	if (pSector->GetOwnerOfSector() != "" && pSector->GetOwnerOfSector() != pShip->GetOwnerOfShip())
		return false;

	// Kolonisierungsbefehl geben
	for (int i = 0; i < pSector->GetNumberOfPlanets(); i++)
	{
		if (pSector->GetPlanet(i)->GetTerraformed() && !pSector->GetPlanet(i)->GetColonized())
		{
			// Hier muss als erstes ein m�glicher neuer Kurs gel�scht werden
			pShip->SetTargetKO(CPoint(-1, -1));
			pShip->SetCurrentOrder(SHIP_ORDER::COLONIZE);
			MYTRACE("shipai")(MT::LEVEL_DEBUG, "Sector: %d,%d: Planet:%d will be colonized\n",
								pShip->GetKO().x,pShip->GetKO().y, i);
			return true;
		}
	}

	return false;
}

/// Funktion schickt Kriegsschiffe zu einem m�glichen Offensivziel. Unter Umst�nden wird auch der Befehl zum
/// Angriff automatisch erteilt. Auch kann es passieren, das diese Funktion die Kriegsschiffe zu einem
/// gemeinsamen Sammelpunkt schickt.
/// @param pShip Zeiger des Schiffes
/// @param pMajor Zeiger auf den Besitzer des Schiffes
/// @return <code>true</code> wenn ein Bewegungsbefehl gegeben werden k�nnte
bool CShipAI::DoAttackMove(CShips* pShip, const CMajor* pMajor)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	if (!pMajor)
	{
		ASSERT(pMajor);
		return false;
	}

	if (pShip->GetShipType() <= SHIP_TYPE::COLONYSHIP || pShip->GetShipType() >= SHIP_TYPE::OUTPOST)
		return false;

	if (DoBombardSystem(pShip))
		return true;

	CString sRace = pMajor->GetRaceID();
	UINT nOurDanger = m_pSectorAI->GetDangerOnlyFromCombatShips(sRace, pShip->GetKO());

	// Gibt es ein Bombardierungsziel, so wird dieses mit der h�chsten Priorit�t angeflogen
	if (m_BombardSector[sRace] != CPoint(-1,-1))
	{
		// pr�fen ob wir im Bombardierungssektor st�rker w�ren
		if (nOurDanger > m_pSectorAI->GetCompleteDanger(sRace, m_BombardSector[sRace]))
		{
			// hier �berpr�fen, ob der Sektor erreicht werden kann
			if (pMajor->GetStarmap()->GetRange(m_BombardSector[sRace]) <= pShip->GetRange(false))
			{
				pShip->SetTargetKO(m_BombardSector[sRace] == pShip->GetKO() ? CPoint(-1, -1) : m_BombardSector[sRace]);
				MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: BOMBARDTARGET in sector: %d/%d\n",sRace,m_BombardSector[sRace].x,m_BombardSector[sRace].y);
				return true;
			}
		}
	}

	// wenn es kein Ziel in der N�he gibt, oder wenn das Ziel in der N�he nicht f�r den Angriff geeignet war, dann
	// wenn m�glich die Schiffe zu dem Punkt schicken, welcher als generelles Angriffsziel auserkoren wurde
	if (m_AttackSector[sRace] != CPoint(-1,-1))
	{
		// pr�fen ob wir st�rker sind
		UINT nEnemyDanger = m_pSectorAI->GetCompleteDanger(sRace, m_AttackSector[sRace]);
		bool bAttack = nOurDanger > nEnemyDanger;
		// falls nicht, pr�fen ob schon 75% bis 90% unserer Flotte hier stationiert haben
		if (!bAttack)
		{
			// nur wenn wir mindestens halb so stark sind wir der Gegner (sonst macht ein Angriff keinen Sinn)
			if (nOurDanger > nEnemyDanger / 2)
				bAttack = nOurDanger > (m_pSectorAI->GetCompleteDanger(sRace) * (rand()%16+75) / 100);
		}

		if (bAttack)
		{
			// hier noch �berpr�fen, ob der Sektor erreicht werden kann
			if (pMajor->GetStarmap()->GetRange(m_AttackSector[sRace]) <= pShip->GetRange(false))
			{
				pShip->SetTargetKO(m_AttackSector[sRace] == pShip->GetKO() ? CPoint(-1, -1) : m_AttackSector[sRace]);
				if(MT::CMyTrace::IsLoggingEnabledFor("shipai"))
				{
					MYTRACE_CHECKED("shipai")(MT::LEVEL_DEBUG, "Race %s: GLOBAL ATTACK in sector: %d/%d\n",sRace,m_AttackSector[sRace].x,m_AttackSector[sRace].y);
					MYTRACE_CHECKED("shipai")(MT::LEVEL_DEBUG, "Ship: %s\n",pShip->GetShipName());
					MYTRACE_CHECKED("shipai")(MT::LEVEL_DEBUG, "OnlyDangerFromShips: %d in Sector: %d/%d\n",nOurDanger,pShip->GetKO().x,pShip->GetKO().y);
					MYTRACE_CHECKED("shipai")(MT::LEVEL_DEBUG, "CompleteDangerInTargetSector: %d\n",nEnemyDanger);
					MYTRACE_CHECKED("shipai")(MT::LEVEL_DEBUG, "Our Complete Danger overall: %d\n",(m_pSectorAI->GetCompleteDanger(sRace) * 100 / (rand()%16+75)));
				}
				return true;
			}
		}
		// falls wir nicht st�rker sind, so fliegen wir zu dem Feld, wo unsere st�rkste Schiffsansammlung ist
		else if (m_pSectorAI->GetHighestShipDanger(sRace) != CPoint(-1,-1))
		{
			CPoint p = m_pSectorAI->GetHighestShipDanger(sRace);
			// hier noch �berpr�fen, ob der Sektor erreicht werden kann
			if (pMajor->GetStarmap()->GetRange(p) <= pShip->GetRange(false))
			{
				pShip->SetTargetKO(p == pShip->GetKO() ? CPoint(-1, -1) : p);
				MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: COLLECT ships in sector: %d/%d\n", sRace,p.x,p.y);
				return true;
			}
		}
	}
	return false;
}

/// Funktion schickt Kriegsschiffe zu einem m�glichen System, welches Bombardiert werden k�nnte.
/// @param pShip Zeiger des Schiffes
/// @return <code>true</code> wenn ein Bewegungsbefehl gegeben werden konnte, eine Bombardierung befohlen wurde oder das Schiff auf Verst�rkung zur Bombardierung im System wartet
bool CShipAI::DoBombardSystem(CShips* pShip)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	if (pShip->GetKO() == m_BombardSector[pShip->GetOwnerOfShip()])
	{
		// Hier muss als erstes ein m�glicher neuer Kurs gel�scht werden und ein alter Systemangriffsbefehl aufgehoben werden
		pShip->SetTargetKO(CPoint(-1, -1));

		// Wenn das Schiff bzw. Schiffe aus der Flotte getarnt sind, dann m�ssen diese erst enttarnt werden
		if (!pShip->CanHaveOrder(SHIP_ORDER::ATTACK_SYSTEM, false))
		{
			// Schiff enttarnen
			pShip->SetCurrentOrder(SHIP_ORDER::DECLOAK);
			return true;
		}

		// Wenn die defensive Schiffsst�rke im System des Angreifers ungef�hr doppelt so gro� als der Systemverteidigungswert
		// des Verteidigers ist, wird bombardiert!
		int nShipDefend = m_pDoc->GetSystem(pShip->GetKO().x, pShip->GetKO().y).GetProduction()->GetShipDefend();
		nShipDefend *= 1.25;

		int nShipValue = 0;

		for(CShipMap::iterator i = m_pDoc->m_ShipMap.begin(); i != m_pDoc->m_ShipMap.end(); ++i)
		{
			if (i->second->GetOwnerOfShip() != pShip->GetOwnerOfShip())
				continue;

			if (i->second->GetKO() != pShip->GetKO())
				continue;

			nShipValue += i->second->GetHull()->GetCurrentHull();
			for (CShips::iterator m = i->second->begin(); m != i->second->end(); ++m)
				nShipValue += m->second->GetHull()->GetCurrentHull();

			if (nShipValue > nShipDefend)
				break;
		}

		//CString s;
		//s.Format("shipValue = %d\nshipDefend = %d\nSektor = %s", shipValue, shipDefend, m_pDoc->GetSector(pShip->GetKO().x, pShip->GetKO().y).GetName(true));
		//AfxMessageBox(s);
		if (nShipValue > nShipDefend)
		{
			MYTRACE("shipai")(MT::LEVEL_INFO, "Race %s: Ship %s (%s) is bombarding system: %d,%d\n",pShip->GetOwnerOfShip(), pShip->GetShipName(), pShip->GetShipTypeAsString(), pShip->GetKO().x,pShip->GetKO().y);
			pShip->SetCurrentOrder(SHIP_ORDER::ATTACK_SYSTEM);
			return true;
		}
	}

	return false;
}

/// Funktion erteilt einen Tarnbefehl oder Enttarnbefehl.
/// @param pShip Zeiger des Schiffes
/// @param bCamouflage <code>true</code> f�r Tarnen, <code>false</code> f�r Enttarnen
/// @return <code>true</code> wenn der Befehl gegeben wurde
bool CShipAI::DoCamouflage(CShips* pShip, bool bCamouflage/* = true*/)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	// Schiff ist schon getarnt/enttarnt
	if (bCamouflage == pShip->GetCloak())
		return false;

	// Nur wenn das Schiff sich tarnen kann und nicht gerade dabei ist ein System zu bombardieren soll es sich tarnen
	if (pShip->CanCloak(false) && pShip->GetCurrentOrder() != SHIP_ORDER::ATTACK_SYSTEM)
	{
		pShip->SetCurrentOrder(bCamouflage ? SHIP_ORDER::ENCLOAK : SHIP_ORDER::DECLOAK);
		return true;
	}

	return false;
}

/// Funktion erstellt eine Flotte. Schiffe werden der Flotte nur hinzugef�gt, wenn diese bestimmte Voraussetzungen erf�llen.
/// So muss der ungef�hre Schiffstyp �bereinstimmen (Combat <-> NonCombat) sowie die Reichweite des Schiffes.
void CShipAI::DoMakeFleet(const CShipMap::iterator& pShip)
{
	if (pShip->second->IsStation())
		return;

	bool increment = true;
	CShipMap::iterator i = pShip;
	for(;;)
	{
		if(increment)
			++i;
		increment = true;
		if(i == m_pDoc->m_ShipMap.end())
			break;

		// Schiffe m�ssen von der selben Rasse sein
		if (pShip->second->GetOwnerOfShip() != i->second->GetOwnerOfShip())
			continue;

		// Schiffe m�ssen sich im selben Sektor befinden
		if (pShip->second->GetKO() != i->second->GetKO())
			continue;

		// beide Schiffe m�ssen die selbe Reichweite haben
		if (pShip->second->GetRange(false) != i->second->GetRange(false))
			continue;

		// das hinzuzuf�gende Schiff darf kein Au�enposten oder Sternbasis sein
		if (i->second->IsStation())
			continue;

		// das hinzuzuf�gende Schiff darf kein individuelles Ziel haben
		if (i->second->GetPath()->GetSize() > 0)
			continue;

		// der Tarnstatus muss gleich sein (also keine getarnten und ungetarnte Schiffe in eine Flotte)
		if (pShip->second->GetCloak() != i->second->GetCloak())
			continue;

		// wenn sich das F�hrungsschiff tarnen kann, dann muss das hinzuzuf�gende Schiff sich auch tarnen k�nnen
		if(pShip->second->CanCloak(false) != i->second->CanCloak(false))
			continue;

		// es muss sich bei beiden Schiffen um Kriegsschiffe handeln oder bei beiden Schiffen um Transporter oder bei beiden Schiffen um Kolonieschiffe
		if ((!pShip->second->IsNonCombat() && !i->second->IsNonCombat())
			||(pShip->second->GetShipType() == SHIP_TYPE::TRANSPORTER && i->second->GetShipType() == SHIP_TYPE::TRANSPORTER && i->second->GetCurrentOrder() < SHIP_ORDER::BUILD_OUTPOST)
			||(pShip->second->GetShipType() == SHIP_TYPE::COLONYSHIP && i->second->GetShipType() == SHIP_TYPE::COLONYSHIP && i->second->GetCurrentOrder() < SHIP_ORDER::COLONIZE))
		{
			pShip->second->AddShipToFleet(i->second);
			m_pDoc->m_ShipMap.EraseAt(i, false);
			increment = false;
		}
	}
}

/// Funktion erteilt einen Au�enpostenbaubefehl, sofern dies auch m�glich ist.
/// @param pShip Zeiger des Schiffes
/// @return <code>true</code> wenn ein Au�enpostenbaubefehl gegeben werden k�nnte
bool CShipAI::DoStationBuild(CShips* pShip)
{
	if (!pShip)
	{
		ASSERT(pShip);
		return false;
	}

	if (pShip->GetStationBuildPoints() <= 0 || pShip->GetCurrentOrder() == SHIP_ORDER::BUILD_OUTPOST)
		return false;

	const CString& sRace = pShip->GetOwnerOfShip();
	if (m_pSectorAI->GetStationBuildSector(sRace).points <= MINBASEPOINTS)
		return false;

	CPoint ptKO(m_pSectorAI->GetStationBuildSector(sRace).position.x, m_pSectorAI->GetStationBuildSector(sRace).position.y);
	if (pShip->GetKO() != ptKO)
		return false;

	// Nur wenn der Sektor uns bzw. niemanden geh�rt
	if (m_pDoc->GetSector(ptKO.x, ptKO.y).GetOwnerOfSector() == "" || m_pDoc->GetSector(ptKO.x, ptKO.y).GetOwnerOfSector() == sRace)
	{
		pShip->SetTargetKO(CPoint(-1, -1));
		pShip->SetCurrentOrder(SHIP_ORDER::BUILD_OUTPOST);
		return true;
	}

	return false;
}

/// Funkion berechnet einen m�glichen Angriffssektor, welcher sp�ter gesammelt angegriffen werden kann.
void CShipAI::CalcAttackSector(void)
{
	// f�r alle Majors den Angriffssektor berechnen
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		int nearestSector = MAXSHORT;
		// beinhaltet die Sektorkoordinate mit unserem Angriffsziel
		CPoint sector = CPoint(-1,-1);
		for (UINT j = 0; j < m_pSectorAI->GetOffensiveTargets(it->first)->size(); j++)
		{
			int space = max(abs(m_pSectorAI->GetHighestShipDanger(it->first).x - m_pSectorAI->GetOffensiveTargets(it->first)->at(j).x),
				abs(m_pSectorAI->GetHighestShipDanger(it->first).y - m_pSectorAI->GetOffensiveTargets(it->first)->at(j).y));
			if (space < nearestSector)
			{
				// Wenn dieses Ziel eine kleinere Gefahr darstellt als unsere gr��te Flottenansammlung oder wir
				// in schon mindst 50% unserer Flotte hier versammelt haben, so wird es als Ziel aufgenommen
				if ((m_pSectorAI->GetDangerOnlyFromCombatShips(it->first, m_pSectorAI->GetHighestShipDanger(it->first)) > m_pSectorAI->GetCompleteDanger(it->first, m_pSectorAI->GetOffensiveTargets(it->first)->at(j)))
					|| (m_pSectorAI->GetDangerOnlyFromCombatShips(it->first, m_pSectorAI->GetHighestShipDanger(it->first)) > (m_pSectorAI->GetCompleteDanger(it->first) * 50 / 100)))
				{
					nearestSector = space;
					sector = m_pSectorAI->GetOffensiveTargets(it->first)->at(j);

				}
			}
		}
		m_AttackSector[it->first] = sector;
		MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: global attack sector is %d/%d\n",it->first, m_AttackSector[it->first].x, m_AttackSector[it->first].y);
	}
}

/// Funktion berechnet einen m�glich besten Sektor f�r eine Bombardierung. Wurde solch ein Sektor ermittelt hat dieser
/// die allerh�chste Priorit�t.
void CShipAI::CalcBombardSector(void)
{
	// f�r alle Majors den Angriffssektor berechnen
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// pazifistische Rassen bombardieren nie
		if (it->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			continue;

		// die Hauptrassen bombardieren nicht alle gleichoft und gleich lang. Die F�deration z.B. kann wegen den
		// Moralabzu�gen nicht so lang bombardieren.
		// Moralwert f�r Bombardierungen holen
		short nMoralValue = CMoralObserver::GetMoralValue(it->second->GetRaceMoralNumber(), 19);
		if (nMoralValue < 0)
		{
			nMoralValue = abs(nMoralValue);
			if (m_iAverageMoral[it->first] < 80 + nMoralValue * 5)
				continue;
		}

		// ist der globale Angriffssektor auch bei den Bombardierungzielen vorhanden, so wird dieser benutzt und kein
		// am n�chsten befindlicher Bombardierungssektor
		for (UINT j = 0; j < m_pSectorAI->GetBombardTargets(it->first)->size(); j++)
		{
			if (m_pSectorAI->GetBombardTargets(it->first)->at(j) == m_AttackSector[it->first])
			{
				m_BombardSector[it->first] = m_pSectorAI->GetBombardTargets(it->first)->at(j);
				break;
			}
		}

		// Wurde kein Bombardierungziel mit dem gleichen globalen Attackziel gefunden, so wird versucht ein n�chstes
		// zu unserer gr��ten Flottenansammlung zu finden
		if (m_BombardSector[it->first] == CPoint(-1,-1))
		{
			int nearestSector = INT_MAX;
			// beinhaltet die Sektorkoordinate mit unserem Bombardierungsziel
			CPoint sector = CPoint(-1,-1);
			for (UINT j = 0; j < m_pSectorAI->GetBombardTargets(it->first)->size(); j++)
			{
				// Es wird das Ziel ausgew�hlt, welches am n�chsten von unserer gr��ten Flottensammlung entfernt ist
				int space = max(abs(m_pSectorAI->GetHighestShipDanger(it->first).x - m_pSectorAI->GetBombardTargets(it->first)->at(j).x),
					abs(m_pSectorAI->GetHighestShipDanger(it->first).y - m_pSectorAI->GetBombardTargets(it->first)->at(j).y));
				if (space < nearestSector)
				{
					// Wenn dieses Ziel eine kleinere Gefahr darstellt als unsere gr��te Flottenansammlung, so wird
					// es als Ziel aufgenommen
					if (m_pSectorAI->GetDangerOnlyFromCombatShips(it->first, m_pSectorAI->GetHighestShipDanger(it->first)) > m_pSectorAI->GetCompleteDanger(it->first, m_pSectorAI->GetBombardTargets(it->first)->at(j)))
					{
						nearestSector = space;
						sector = m_pSectorAI->GetBombardTargets(it->first)->at(j);
					}
				}
			}
			m_BombardSector[it->first] = sector;
		}
		MYTRACE("shipai")(MT::LEVEL_DEBUG, "Race %s: global bombard sector is %d/%d\n",it->first, m_BombardSector[it->first].x, m_BombardSector[it->first].y);
	}
}
