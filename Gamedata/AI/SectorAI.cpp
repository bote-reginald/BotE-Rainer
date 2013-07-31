#include "stdafx.h"
#include "SectorAI.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "Galaxy\Anomaly.h"
#include "Ships/Ships.h"
#include <algorithm>
#include "MyTrace.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSectorAI::CSectorAI(CBotEDoc* pDoc)
{
	ASSERT(pDoc);
	m_pDoc = pDoc;

	Clear();
}

CSectorAI::~CSectorAI(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion gibt das gesamte Gefahrenpotenzial aller Rassen in einem Sektor <code>sector</code> zur�ck.
/// Das Gefahrenpotential der eigenen Rasse <code>sOwnRaceID</code> wird dabei aber nicht mit eingerechnet.
UINT CSectorAI::GetCompleteDanger(const CString& sOwnRaceID, const CPoint& sector) const
{
	UINT danger = 0;

	for (map<CString, map<pair<int, int>, UINT> >::const_iterator it = m_iDangers.begin(); it != m_iDangers.end(); ++it)
		if (it->first != sOwnRaceID)
			for (map<pair<int, int>, UINT>::const_iterator itt = it->second.begin(); itt != it->second.end(); ++itt)
				if (CPoint(itt->first.first, itt->first.second) == sector)
				{
					danger += itt->second;
					//MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "SectorAI.cpp: EnemyDanger: %i, new Summary: %i\n", itt->second, danger);
				}
				//MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "SectorAI.cpp: EnemyDanger: %i, new Summary: %i\n", itt->second, danger);
	return danger;
}

/// Diese Funktion tr�gt die ganzen Gefahrenpotenziale der Schiffe und Stationen in die Variable
/// <code>m_iDangers</code> ein. Au�erdem wird hier auch gleich die Anzahl der verschiedenen Schiffe
/// der einzelnen Rassen ermittelt.
void CSectorAI::CalculateDangers()
{
	for(CShipMap::const_iterator i =  m_pDoc->m_ShipMap.begin(); i !=  m_pDoc->m_ShipMap.end(); ++i)
	{
		AddDanger(i->second);
		// F�hrt das Schiff eine Flotte an, so muss dies alles auch f�r die Schiffe in der Flotte getan werden
		for(CShips::const_iterator j =  i->second->begin(); j !=  i->second->end(); ++j)
			AddDanger(j->second);
	}
}

/// Diese Funktion berechnet die ganzen Wichtigkeiten f�r die einzelnen Sektoren. Also wo lohnt es sich zum
/// Terraformen, wo lernt man neue Minorraces kennen usw. Vorher sollte die Funktion <code>CalculateDangers()
/// </code> aufgerufen werden.
void CSectorAI::CalcualteSectorPriorities()
{
	map<CString, CRace*>* mRaces = m_pDoc->GetRaceCtrl()->GetRaces();
	map<CString, UINT> highestCombatShipDanger;

	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		{
			// Gibt es ein Sonnensystem im Sektor?
			if (m_pDoc->GetSector(x, y).GetSunSystem())
			{
				CalculateTerraformSectors(x,y);
				CalculateMinorraceSectors(x,y);

			}
			// Offensivziele in diesem Feld gelten nur, wenn nicht eine gef�hrliche Anomalie einen
			// Einflug in den Sektor sinnlos machen w�rde. Also bei gef�hrlicher Anomalie wird kein Offensivziel berechnet, sonst immer
			if (m_pDoc->GetSector(x, y).GetAnomaly() == false || m_pDoc->GetSector(x, y).GetAnomaly()->GetWaySearchWeight() < 10.0)
				CalculateOffensiveTargets(x,y);

			for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); ++it)
				if (GetDangerOnlyFromCombatShips(it->first, CPoint(x,y)) > highestCombatShipDanger[it->first])
				{
					highestCombatShipDanger[it->first] = GetDangerOnlyFromCombatShips(it->first, CPoint(x,y));
					m_HighestShipDanger[it->first] = CPoint(x,y);
				}
		}

	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); ++it)
		if (it->second->IsMajor())
		{
			// Feld der am ehesten zu terraformenden Systeme der Gr��e nach Sortieren. Der h�chste Eintrag steht an erster Stelle.
			std::sort(m_vSectorsToTerraform[it->first].begin(), m_vSectorsToTerraform[it->first].end());
			std::reverse(m_vSectorsToTerraform[it->first].begin(), m_vSectorsToTerraform[it->first].end());

			// Sektor f�r den Bau eines Au�enpostens berechnen
			CalculateStationTargets(it->first);
			if(MT::CMyTrace::IsLoggingEnabledFor("ai"))
			{
				if (m_vSectorsToTerraform[it->first].size())
				{
					MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "---------- sectors to terraform or colonize -------------\n");
					MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "Race-ID: %s\n",it->first);
					for (UINT j = 0; j < m_vSectorsToTerraform[it->first].size(); j++)
						MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "POP: %d - KO: %d/%d\n",m_vSectorsToTerraform[it->first][j].pop, m_vSectorsToTerraform[it->first][j].p.x, m_vSectorsToTerraform[it->first][j].p.y);
				}
				if (m_vOffensiveTargets[it->first].size())
				{
					MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "---------- sectors to attack -------------\n");
					MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "offensive targets for race %s\n",it->first);
					for (UINT j = 0; j < m_vOffensiveTargets[it->first].size(); j++)
						MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "offensive target in sector %d/%d\n", m_vOffensiveTargets[it->first][j].x, m_vOffensiveTargets[it->first][j].y);
				}
				if (m_vBombardTargets[it->first].size())
				{
					MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "---------- sectors to bombard -------------\n");
					MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "bombard target for race %s\n",it->first);
					for (UINT j = 0; j < m_vBombardTargets[it->first].size(); j++)
						MYTRACE_CHECKED("ai")(MT::LEVEL_DEBUG, "bombard target in sector %d/%d\n",m_vBombardTargets[it->first][j].x, m_vBombardTargets[it->first][j].y);
				}
			}
		}
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion addiert die Offensiv- und Defensivst�rke eines Schiffes einer Rasse zum jeweiligen
/// Sektor.
void CSectorAI::AddDanger(const CShips* ship)
{
	CString race = ship->GetOwnerOfShip();

	UINT offensive = ship->GetCompleteOffensivePower();
	UINT defensive = ship->GetCompleteDefensivePower() / 2;
	m_iDangers[race][pair<int, int>(ship->GetKO().x, ship->GetKO().y)] += (offensive + defensive);

	if (ship->GetShipType() > SHIP_TYPE::COLONYSHIP && ship->GetShipType() < SHIP_TYPE::OUTPOST)
	{
		m_iCompleteDanger[race] += (offensive + defensive);
		m_iCombatShipDangers[race][pair<int, int>(ship->GetKO().x, ship->GetKO().y)] += (offensive + defensive);
		MYTRACE_CHECKED("shipdetails")(MT::LEVEL_DEBUG, "SectorAI.cpp: %s (%s), Off:%i + Def.(half):%i = CompleteDanger:%i\n", 
				ship->GetShipName(),ship->GetShipTypeAsString(),offensive,defensive,m_iCompleteDanger[race]);

	}
	// (not necassary) MYTRACE_CHECKED("shipdetails")(MT::LEVEL_DEBUG, "SectorAI.cpp: %s, Off:%i + Def.(half):%i = Compl.DangerOutOfShips:%i\n", 
	//			race, m_iCompleteDanger[race],offensive,defensive, m_iCompleteDanger[race]);

	// Hier wird die Anzahl an Kolonieschiffen f�r die Rassen hochgez�hlt.
	if (ship->GetShipType() == SHIP_TYPE::COLONYSHIP)
		m_iColoShips[race] += 1;
	else if (ship->GetShipType() == SHIP_TYPE::TRANSPORTER)
		m_iTransportShips[race] += 1;
	// (not good/neccessary) MYTRACE_CHECKED("shipdetails")(MT::LEVEL_DEBUG, "SectorAI.cpp: %s, Compl.DangerOutOfShips:%i, Off:%i,Def.(half):%i, further Ships: Colo:%d, Transp.:%d\n", 
	//			race, m_iCompleteDanger[race], offensive, defensive, m_iColoShips[race], m_iTransportShips[race]);
}

/// Diese Funktion ermittelt die Sektoren, welche sich am ehesten zum Terraformen f�r eine bestimmte Rasse eignen.
/// Die Eintr�ge werden dann im Array <code>m_vSectorsToTerraform</code> gemacht.
void CSectorAI::CalculateTerraformSectors(int x, int y)
{
	BYTE pop = 0;
	// wieviel Bev�lkerung kann man noch ins System bringen
	for (int j = 0; j < m_pDoc->GetSector(x, y).GetNumberOfPlanets(); j++)
		if (m_pDoc->GetSector(x, y).GetPlanet(j)->GetHabitable() == TRUE
			&& m_pDoc->GetSector(x, y).GetPlanet(j)->GetColonized() == FALSE)
			pop += (BYTE)m_pDoc->GetSector(x, y).GetPlanet(j)->GetMaxHabitant();

	if (pop > 5)
	{
		// Eintrag f�r die jeweilige Rasse machen.
		map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			if (it->second->GetStarmap()->GetRange(CPoint(x,y)) != 3)
				if (m_pDoc->GetSector(x, y).GetOwnerOfSector().IsEmpty() || m_pDoc->GetSector(x, y).GetOwnerOfSector() == it->first)
				{
					SectorToTerraform stt(pop,CPoint(x,y));
					m_vSectorsToTerraform[it->first].push_back(stt);
				}
	}
}

/// Funktion berechnet die Sektoren, in denen eine einem Imperium unbekannte Minorrace lebt, zu deren Sektor
/// aber theoretisch geflogen werden kann. Das Ergebnis wird im Array <code>m_vMinorraceSectors</code> gespeichert.
void CSectorAI::CalculateMinorraceSectors(int x, int y)
{
	// Geh�rt der Sektor aktuell auch einer Minorrace
		// Wenn die Minorrace einem anderen Imperium beigetreten ist, so tritt folgende Bediengnung nicht ein!.
		// Dann fliegt die KI diesen Sektor nicht bevorzugt an, was so auch realistischer ist.
	CString sOwner	= m_pDoc->GetSector(x, y).GetOwnerOfSector();
	if (sOwner.IsEmpty())
		return;

	CRace* pOwner	= m_pDoc->GetRaceCtrl()->GetRace(sOwner);
	if (!pOwner || !pOwner->IsMinor())
		return;

	CMinor* pMinor = dynamic_cast<CMinor*>(pOwner);
	ASSERT(pMinor);

	// Eintrag f�r die jeweilige Rasse machen.
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			if (it->second->GetStarmap()->GetRange(CPoint(x,y)) != 3)
				if (it->second->IsRaceContacted(sOwner) == false)
					if (pMinor->GetRaceKO() != CPoint(-1,-1))
						m_vMinorraceSectors[it->first].push_back(pMinor->GetRaceKO());
}

/// Diese Funktion berechnet alle m�glichen offensiven Ziele f�r eine bestimmte Rasse. Das Ergebnis wird im Array
/// <code>m_vOffensiveTargets</code> gespeichert.
void CSectorAI::CalculateOffensiveTargets(int x, int y)
{
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		// Wenn unsere Rasse dieses Feld �berhaupt erreichen kann.
		if (it->second->GetStarmap()->GetRange(CPoint(x,y)) != 3)
		{
			int danger = GetCompleteDanger(it->first, CPoint(x,y));
			// Die Gefahr wird nur hinzugef�gt, wenn diese auf MEINEM Gebiet liegt und wir mit der entsprechenden
			// Rasse Krieg oder unsere Beziehung zu ihr auf 50% gefallen ist. Die andere M�glichkeit
			// w�re, wenn diese nicht in unserem Gebiet liegt und wir Krieg haben oder die Beziehung auf
			// unter 30% gefallen ist.
			if (danger > 0)
			{
				// den st�rksten Gegner in diesem Sektor ermitteln
				CString sEnemy = 0;
				UINT max = 0;
				for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
					if (it->first != itt->first && GetDanger(itt->first, CPoint(x,y)) > 0)
						if (max < GetDanger(itt->first, CPoint(x,y)))
						{
							max = GetDanger(itt->first, CPoint(x,y));
							sEnemy = itt->first;
						}
				// kennen wir den Gegner?
				if (it->second->IsRaceContacted(sEnemy))
				{
					// pr�fen ob es auf unserem eigenen Gebiet ist
					if (m_pDoc->GetSector(x, y).GetOwnerOfSector() == it->first)
					{
						// jetzt wird �berpr�ft, ob obige Bedingungen gelten
						if (it->second->GetRelation(sEnemy) < 50 || it->second->GetAgreement(sEnemy) == DIPLOMATIC_AGREEMENT::WAR)
							// Gefahr wird dem Feld hinzuge�gt
							m_vOffensiveTargets[it->first].push_back(CPoint(x,y));
					}
					// wenn es nicht auf unserem Gebiet ist
					else
					{
						// jetzt wird �berpr�ft, ob obige Bedingungen gelten
						if (it->second->GetRelation(sEnemy) < 30 || it->second->GetAgreement(sEnemy) == DIPLOMATIC_AGREEMENT::WAR)
							// Gefahr wird dem Feld hinzuge�gt
							m_vOffensiveTargets[it->first].push_back(CPoint(x,y));
					}
				}
			}
			CalculateBombardTargets(it->first,x,y);
		}
}

/// Diese Funktion berechnet Systeme, welche im Kriegsfall wom�glich angegriffen werden k�nnen. Das Ergebnis wird
/// im Array <code>m_vBombardTargets</code> gespeichert.
void CSectorAI::CalculateBombardTargets(const CString& sRaceID, int x, int y)
{
	CString sOwner	= m_pDoc->GetSystem(x, y).GetOwnerOfSystem();
	if (sOwner.IsEmpty())
		return;
	CRace* pOwner	= m_pDoc->GetRaceCtrl()->GetRace(sOwner);
	// wenn das System nicht einem anderen Major geh�rt
	if (!pOwner || !pOwner->IsMajor())
		return;

	// geh�rt das System einer anderen Majorrace, au�er uns selbst?
	if (m_pDoc->GetSystem(x, y).GetOwnerOfSystem() != sRaceID)
	{
		CRace* pOurRace = m_pDoc->GetRaceCtrl()->GetRace(sRaceID);
		if (!pOurRace)
			return;
		// haben wir mit dieser anderen Majorrace Krieg?
		if (pOurRace->GetAgreement(sOwner) == DIPLOMATIC_AGREEMENT::WAR)
			// dann w�re dies ein lohnendes Ziel, welches angegriffen werden k�nnte
			m_vBombardTargets[sRaceID].push_back(CPoint(x,y));
	}
}

/// Diese Funktion berechnet einen Sektor, welcher sich zum Bau eines Au�enpostens eignet.
/// Funktion erst nach der Berechnung der Terraformsektoren aufrufen.
void CSectorAI::CalculateStationTargets(const CString& sRaceID)
{
	CMajor* pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sRaceID));
	if (pMajor)
	{
		m_mStationBuild[sRaceID] = pMajor->GetStarmap()->CalcAIBaseSector(0.0f);

		// Umso weniger Terraformsektoren zur Verf�gung stehen, umso h�her sind die Stationsbauprioirit�ten
		if (GetSectorsToTerraform(sRaceID)->size() == 0)
			m_mStationBuild[sRaceID].points *= 4.0f;
		else if (GetSectorsToTerraform(sRaceID)->size() == 1)
		{
			if (GetSectorsToTerraform(sRaceID)->front().pop < 10)
				m_mStationBuild[sRaceID].points *= 3.0f;
			else
				m_mStationBuild[sRaceID].points *= 2.0f;
		}
		else if (GetSectorsToTerraform(sRaceID)->size() == 2)
			m_mStationBuild[sRaceID].points *= 1.5f;
	}
}

/// Funktion l�scht alle vorher berechneten Priorit�ten.
void CSectorAI::Clear(void)
{
	map<CString, CRace*>* mRaces = m_pDoc->GetRaceCtrl()->GetRaces();

	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); ++it)
		m_HighestShipDanger[it->first] = CPoint(-1,-1);

	m_iDangers.clear();
	m_iCombatShipDangers.clear();
	m_iCompleteDanger.clear();
	m_mStationBuild.clear();
	m_vSectorsToTerraform.clear();
	m_vMinorraceSectors.clear();
	m_vOffensiveTargets.clear();
	m_vBombardTargets.clear();
	m_iColoShips.clear();
	m_iTransportShips.clear();
}
