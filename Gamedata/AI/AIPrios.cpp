#include "stdafx.h"
#include "AIPrios.h"
#include "BotEDoc.h"
#include "SectorAI.h"
#include "Races\RaceController.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CAIPrios::CAIPrios(CBotEDoc* pDoc)
{
	ASSERT(pDoc);
	m_pDoc = pDoc;
	Clear();
}

CAIPrios::~CAIPrios(void)
{
}

//////////////////////////////////////////////////////////////////////
// Zugriffsfunktionen
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion berechnet die Priorit�ten der einzelnen Majorrassen, wann sie ein Kolonieschiff in Auftrag
/// geben sollen.
void CAIPrios::CalcShipPrios(CSectorAI* sectorAI)
{
	MYTRACE("ai")(MT::LEVEL_DEBUG, "CAIPrios::CalcShipPrios() begin... \n");

	ASSERT(sectorAI);
	Clear();
	int max = 0;
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		ASSERT(pMajor);

		BYTE researchLevels[6] =
		{
			pMajor->GetEmpire()->GetResearch()->GetBioTech(),
			pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
			pMajor->GetEmpire()->GetResearch()->GetCompTech(),
			pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
			pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
			pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
		};

		SHIP_RANGE::Typ range = SHIP_RANGE::SHORT;
		// berechnen welche max. Reichweite das Kolonieschiff der Rasse hat
		for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
			if (m_pDoc->m_ShipInfoArray.GetAt(j).GetRace() == pMajor->GetRaceShipNumber())
				if (m_pDoc->m_ShipInfoArray.GetAt(j).GetShipType() == SHIP_TYPE::COLONYSHIP
					&& m_pDoc->m_ShipInfoArray.GetAt(j).GetRange() > range
					&& m_pDoc->m_ShipInfoArray.GetAt(j).IsThisShipBuildableNow(researchLevels))
				{
					range = m_pDoc->m_ShipInfoArray.GetAt(j).GetRange();
					MYTRACE("ai")(MT::LEVEL_DEBUG, "CAIPrios::CalcShipPrios() range COLONYSHIP = %d\n", range);
					if (range == SHIP_RANGE::LONG)
						break;
				}
		// Jetzt die zum Terraformen ausgesuchten Sektoren durchgehen und schauen das diese innerhalb der Reichweite
		// des Schiffes liegen.
		for (UINT j = 0; j < sectorAI->GetSectorsToTerraform(it->first)->size(); j++)
			if (pMajor->GetStarmap()->GetRange(sectorAI->GetSectorsToTerraform(it->first)->at(j).p) <= range)
				m_mColoShipPrio[it->first] += 1;
		// Bei 0 oder einem Kolonieschiff bleibt die Priorit�t gleich der Anzahl der zu terraformenden Sektoren.
		// Bei zwei Kolonieschiffen ist die maximale Priori�t noch 7, bei drei 8, bei vier 9 bei f�nf 10.
		// Bei mehr als 5 Kolonieschiffen wird die Priorit�t halbiert.
		// Wenn die Priorit�t + 2 kleiner der Anzahl der Kolonischiffe ist, so ist die Priorit�t 0
		if ((m_mColoShipPrio[it->first] + 1) < sectorAI->GetNumberOfColoships(it->first))
			m_mColoShipPrio[it->first] = 0;
		else if (sectorAI->GetNumberOfColoships(it->first) > 1 && sectorAI->GetNumberOfColoships(it->first) < 6 && m_mColoShipPrio[it->first] > sectorAI->GetNumberOfColoships(it->first) + 5)
			m_mColoShipPrio[it->first] = sectorAI->GetNumberOfColoships(it->first) + 5;
		else if (sectorAI->GetNumberOfColoships(it->first) > 12)
			m_mColoShipPrio[it->first] = rand()%2;
		else if (sectorAI->GetNumberOfColoships(it->first) > 5)
			m_mColoShipPrio[it->first] /= 4;
	}

	// Priorit�ten f�r Truppentransporter berechnen
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		ASSERT(pMajor);

		if (sectorAI->GetStationBuildSector(it->first).points > MINBASEPOINTS)
		{
			BOOLEAN buildableStation = FALSE;
			const BYTE researchLevels[6] =
			{
				pMajor->GetEmpire()->GetResearch()->GetBioTech(),
				pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
				pMajor->GetEmpire()->GetResearch()->GetCompTech(),
				pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
				pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
				pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
			};
			for (int j = 0; j < m_pDoc->m_ShipInfoArray.GetSize(); j++)
				if (m_pDoc->m_ShipInfoArray.GetAt(j).GetRace() == pMajor->GetRaceShipNumber())
					if (m_pDoc->m_ShipInfoArray.GetAt(j).IsStation())
						if (m_pDoc->m_ShipInfoArray.GetAt(j).IsThisShipBuildableNow(researchLevels))
						{
							buildableStation = TRUE;
							break;
						}
			if (buildableStation)
			{
				if (sectorAI->GetNumberOfTransportShips(it->first) == NULL)
					m_mTransportPrio[it->first] = (BYTE)(sectorAI->GetStationBuildSector(it->first).points / (MINBASEPOINTS * 0.05));
				else
					m_mTransportPrio[it->first] = (BYTE)(sectorAI->GetStationBuildSector(it->first).points / (MINBASEPOINTS * 0.15));

				if ((m_mTransportPrio[it->first]) < sectorAI->GetNumberOfTransportShips(it->first) * 4)
					m_mTransportPrio[it->first] = 0;

				// bei einer niedrigen Kolonieschiffpriorit�t (m�glich wenn keine gut zu terraformenden Systeme vorhanden
				// sind) wird die Transportschiffpriorit�t verdoppelt. Bei einer hohen Kolonieschiffpriorit�t wird
				// die Truppentransporterbaupriorit�t verringert
				if (m_mColoShipPrio[it->first] == NULL && sectorAI->GetNumberOfTransportShips(it->first) == NULL)
					m_mTransportPrio[it->first] *= 7;
				else if (m_mColoShipPrio[it->first] < 2)
					m_mTransportPrio[it->first] *= 2;
				else if (m_mColoShipPrio[it->first] > 5)
					m_mTransportPrio[it->first] /= 3;
			}
		}
	}

	// Priorit�ten f�r Kriegsschiffe ermitteln
	map<CString, int> shipPower;
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		shipPower[it->first] += sectorAI->GetCompleteDanger(it->first);
		if (max < shipPower[it->first])
			max = shipPower[it->first];
	}


	MYTRACE("ai")(MT::LEVEL_DEBUG, "CAIPrios::CalcShipPrios(): max Combatship Priority is: %d\n",max);
	// Maximum der Schiffsst�rken ermitteln
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		MYTRACE("ai")(MT::LEVEL_DEBUG, "Calc Shippowers: Race: %s has a complete shippower of %d - all shippower is %d\n",it->first, shipPower[it->first], sectorAI->GetCompleteDanger(it->first));
		if (shipPower[it->first] > 0)
			m_mCombatShipPrio[it->first] = (BYTE)(ceil)((float)max / (float)shipPower[it->first] * 1.5);
		else
			m_mCombatShipPrio[it->first] = MAXBYTE;
		MYTRACE("ai")(MT::LEVEL_DEBUG, "Calc Priorities: Race: %s - CombatShipPrio: %d - ColoshipPrio: %d - TransportPrio: %d\n",it->first, m_mCombatShipPrio[it->first], m_mColoShipPrio[it->first], m_mTransportPrio[it->first]);
		MYTRACE("ai")(MT::LEVEL_DEBUG, "Number of shiptypes: Race: %s - Colonyships: %d - Transporthips: %d\n",it->first, sectorAI->GetNumberOfColoships(it->first), sectorAI->GetNumberOfTransportShips(it->first));
	}
	MYTRACE("ai")(MT::LEVEL_DEBUG, "CAIPrios::CalcShipPrios() ... ready\n");
}

/// Funktion l�scht alle vorher berechneten Priorit�ten.
void CAIPrios::Clear(void)
{
	m_mColoShipPrio.clear();
	m_mTransportPrio.clear();
	m_mCombatShipPrio.clear();
	m_IntelAI.Reset();
}
