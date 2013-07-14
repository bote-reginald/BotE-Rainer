#include "stdafx.h"
#include "ResearchAI.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"

CResearchAI::CResearchAI(void)
{
}

CResearchAI::~CResearchAI(void)
{
}

void CResearchAI::Calc(CBotEDoc* pDoc)
{
	ASSERT(pDoc);

	// Forschungsdurchschnitt aller Rassen berechnen
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	ASSERT(pmMajors);

	// Map mit zum Major zugeh�rigen Techlevel
	map<CMajor*, double> mTechLevels;
	list<double> lTechLevels;

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		// f�r menschliche Spieler wird die KI nichts tun
		if (!pMajor || pMajor->AHumanPlays())
			continue;

		double dTechLevel = pMajor->GetEmpire()->GetResearch()->GetBioTech() +
							pMajor->GetEmpire()->GetResearch()->GetEnergyTech() +
							pMajor->GetEmpire()->GetResearch()->GetCompTech() +
							pMajor->GetEmpire()->GetResearch()->GetPropulsionTech() +
							pMajor->GetEmpire()->GetResearch()->GetConstructionTech() +
							pMajor->GetEmpire()->GetResearch()->GetWeaponTech();
		dTechLevel /= 6.0;
		lTechLevels.push_back(dTechLevel);

		// Hat die Rasse �berhaupt eine Spezialforschung zur Auswahl? (auf FALSE pr�fen!)
		if (!pMajor->GetEmpire()->GetResearch()->GetUniqueReady())
			// Hat die Rasse noch keinen Bereich gew�hlt?
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == false)
				mTechLevels[pMajor] = dTechLevel;
	}

	// absteigend sortieren
	lTechLevels.sort();
	lTechLevels.reverse();
	// auf die besten zwei begrenzen
	lTechLevels.resize(2);

	// nur die Majors mit dem besten oder zweitbesten Techlevel versuchen eine Spezialforschung
	// zu beginnen. Dies machen sie aber nur, wenn sie mindestens eine Techstufe weiter sind, als
	// sie f�r die Spezialforschung ben�tigt haben
	for (list<double>::const_iterator it = lTechLevels.begin(); it != lTechLevels.end(); ++it)
	{
		// Major mit diesem Techlevel suchen
		for (map<CMajor*, double>::const_iterator it2 = mTechLevels.begin(); it2 != mTechLevels.end(); ++it2)
		{
			if (*it == it2->second)
			{
				CMajor* pMajor = it2->first;
				if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken())
					break;

				BYTE nUniqueTech = pMajor->GetEmpire()->GetResearch()->GetUniqueTech() + 1;
				// Pr�fen ob die Rasse schon ein Level weiter ist, als das was sie f�r die Spezialforschung braucht
				if (nUniqueTech < pMajor->GetEmpire()->GetResearch()->GetBioTech() &&
					nUniqueTech < pMajor->GetEmpire()->GetResearch()->GetEnergyTech() &&
					nUniqueTech < pMajor->GetEmpire()->GetResearch()->GetCompTech() &&
					nUniqueTech < pMajor->GetEmpire()->GetResearch()->GetConstructionTech() &&
					nUniqueTech < pMajor->GetEmpire()->GetResearch()->GetPropulsionTech() &&
					nUniqueTech < pMajor->GetEmpire()->GetResearch()->GetWeaponTech())
				{
					// zuf�llig einen Bereich w�hlen (1 bis 3)
					int nComplex = rand()%3 + 1;
					pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(nComplex);
					// 100% zuteilen
					pMajor->GetEmpire()->GetResearch()->SetPercentage(6, 100);
					CString sName = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
					MYTRACE("general")(MT::LEVEL_INFO, "CResearchAI::Calc(): %s choose in unique complex '%s' field %d and set level to 100%%\n", pMajor->GetRaceID(), sName, nComplex);
				}
			}
		}
	}
}
