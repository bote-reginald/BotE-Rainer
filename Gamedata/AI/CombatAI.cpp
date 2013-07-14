#include "stdafx.h"
#include "CombatAI.h"
#include "BotEDoc.h"
#include "Races/RaceController.h"
#include "Ships/Combat.h"
#include "Ships/Ships.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CCombatAI::CCombatAI(void)
{
}

CCombatAI::~CCombatAI(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
bool CCombatAI::CalcCombatTactics(const CArray<CShips*>& vInvolvedShips, const map<CString, CRace*>* pmRaces, map<CString, COMBAT_ORDER::Typ>& mCombatOrders, const CAnomaly* pAnomaly)
{
	// allgemeinen Kampfbefehl f�r alle beteiligten KI Rassen einstellen
	ApplyCombatOrders(vInvolvedShips, pmRaces, mCombatOrders, pAnomaly);

	// Haben alle beteiligten Rassen Gru�frequenzen ge�ffnet, so findet kein Kampf statt
	bool bAllHailingFrequencies = true;
	for (map<CString, COMBAT_ORDER::Typ>::const_iterator it = mCombatOrders.begin(); it != mCombatOrders.end(); ++it)
		if (it->second != COMBAT_ORDER::HAILING)
		{
			bAllHailingFrequencies = false;
			break;
		}
	if (bAllHailingFrequencies)
		return false;



	// genaue Befehle an alle beteiligten Schiffe �bergeben
	ApplyShipTactics(vInvolvedShips, mCombatOrders);

	return true;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
void CCombatAI::ApplyCombatOrders(const CArray<CShips*>& vInvolvedShips, const map<CString, CRace*>* pmRaces, map<CString, COMBAT_ORDER::Typ>& mCombatOrders, const CAnomaly* pAnomaly)
{
	// beteiligte Rassen
	set<CString> sInvolvedRaces;
	for (int i = 0; i < vInvolvedShips.GetSize(); i++)
		sInvolvedRaces.insert(vInvolvedShips[i]->GetOwnerOfShip());

	// Kampfbefehl berechnen
	for (set<CString>::const_iterator it = sInvolvedRaces.begin(); it != sInvolvedRaces.end(); ++it)
	{
		// KI-Rassen und Minors m�ssen noch ihre Befehle einstellen
		// Alle Clients haben ihre Befehle schon eingestellt
		if (mCombatOrders.find(*it) != mCombatOrders.end())
			continue;

		// Beziehung zu anderen Rassen beachten, daf�r Rassen aus Map holen
		map<CString, CRace*>::const_iterator iter = pmRaces->find(*it);
		if (iter == pmRaces->end())
			continue;

		CRace* pRace1 = iter->second;
		int nMinRelation = 100;
		for (set<CString>::const_iterator it2 = sInvolvedRaces.begin(); it2 != sInvolvedRaces.end(); ++it2)
		{
			if (*it != *it2)
			{
				iter = pmRaces->find(*it2);
				if (iter == pmRaces->end())
					continue;

				CRace* pRace2 = iter->second;
				// Beziehung der anderes Rasse zu uns
				if (!pRace2->IsMajor() || (pRace2->IsMajor() && ((CMajor*)pRace2)->AHumanPlays() == false))
					nMinRelation = min(nMinRelation, pRace2->GetRelation(pRace1->GetRaceID()));
				else if (!pRace1->IsMajor() || (pRace1->IsMajor() && ((CMajor*)pRace1)->AHumanPlays() == false))
					nMinRelation = min(nMinRelation, pRace1->GetRelation(pRace2->GetRaceID()));
			}
		}

		int nRaceMod = 0;
		if (pRace1->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nRaceMod -= 50;
		if (pRace1->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			nRaceMod -= 25;
		if (pRace1->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nRaceMod -= 10;
		if (pRace1->IsRaceProperty(RACE_PROPERTY::SNEAKY))
			nRaceMod -= 10;
		if (pRace1->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nRaceMod += 10;
		if (pRace1->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nRaceMod += 10;
		if (pRace1->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
			nRaceMod += 25;
		if (pRace1->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nRaceMod += 50;

		// bei gute Beziehungen wird sehr wahrscheinlich die Gru�frequenzen ge�ffnet
		if (rand()%100 < nMinRelation + nRaceMod)
		{
			mCombatOrders[*it] = COMBAT_ORDER::HAILING;
			continue;
		}

		// Ist die Beziehung nicht ausreichend, dann die Schiffst�rken im Sektor beachten und Gewinnchance ermitteln
		set<const CRace*> sFriends;
		set<const CRace*> sEnemies;
		double dWinningChance = CCombat::GetWinningChance(pRace1, vInvolvedShips, pmRaces, sFriends, sEnemies, pAnomaly);

		if (dWinningChance > 0.75)
			mCombatOrders[*it] = COMBAT_ORDER::AUTOCOMBAT;
		else if (dWinningChance < 0.25)
			mCombatOrders[*it] = COMBAT_ORDER::RETREAT;
		else
		{
			if (rand()%100 < dWinningChance * 100 - nRaceMod / 2)
				mCombatOrders[*it] = COMBAT_ORDER::AUTOCOMBAT;
			else
				mCombatOrders[*it] = COMBAT_ORDER::RETREAT;
		}
	}

	// Logging
	for (set<CString>::const_iterator it = sInvolvedRaces.begin(); it != sInvolvedRaces.end(); ++it)
	{
		CString sTactic;
		switch (mCombatOrders[*it])
		{
		case COMBAT_ORDER::NONE:		sTactic = "- (error)"; break;
		case COMBAT_ORDER::USER:		sTactic = "User"; break;
		case COMBAT_ORDER::HAILING:		sTactic = "Hailing Frequencies"; break;
		case COMBAT_ORDER::RETREAT:		sTactic = "Retreat"; break;
		case COMBAT_ORDER::AUTOCOMBAT:	sTactic = "Auto"; break;
		}
		MYTRACE("logcombat")(MT::LEVEL_DEBUG, "Race %s is involved in combat. Tactic: %s\n", *it, sTactic);
	}
}

void CCombatAI::ApplyShipTactics(const CArray<CShips*>& vInvolvedShips, map<CString, COMBAT_ORDER::Typ>& mCombatOrders)
{
	// eingestellte Befehle an die Schiffe �bergeben
	for (int i = 0; i < vInvolvedShips.GetSize(); i++)
	{
		CShips* pShip = vInvolvedShips[i];
		CString sOwner = pShip->GetOwnerOfShip();
		if (mCombatOrders.find(sOwner) != mCombatOrders.end())
		{
			COMBAT_ORDER::Typ nOrder = mCombatOrders[sOwner];
			if (nOrder == COMBAT_ORDER::AUTOCOMBAT)
			{
				// Kampfschiffe bekommen den Angreifen-Befehl
				pShip->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
				// Non-Combats bekommen den Meiden-Befehl
				if (pShip->IsNonCombat())
				{
					// Hat das Schiff Offensivpower, so kann es doch angreifen
					if (pShip->GetCompleteOffensivePower() <= 1)
						pShip->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
				}
				// Bei einem Autokampf wird sich kein Schiff automatisch zur�ckziehen!
			}
			else if (nOrder == COMBAT_ORDER::HAILING)
			{
				// Hier gilt das gleiche wie beim Autokampf
				// Au�er wenn alle anderen Rassen Gr��en oder einen R�ckzug machen,
				// dann lassen wir sie entkommen und zerschie�en nicht ihre Schiffe.

				// Pr�fen ob alle anderen Kampfbeteiligten Gr��en
				bool bAvoid = true;
				for (map<CString, COMBAT_ORDER::Typ>::const_iterator it = mCombatOrders.begin(); it != mCombatOrders.end(); ++it)
					if (it->first != sOwner)
						if (it->second != COMBAT_ORDER::HAILING && it->second != COMBAT_ORDER::RETREAT)
						{
							bAvoid = false;
							break;
						}
				if (bAvoid)
				{
					pShip->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
				}
				else
				{
					// gleiche Zuweisung wie beim Autokampf
					// Kampfschiffe bekommen den Angreifen-Befehl
					pShip->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
					// Non-Combats bekommen den Meiden-Befehl
					if (pShip->IsNonCombat())
					{
						// Hat das Schiff Offensivpower, so kann es doch angreifen
						if (pShip->GetCompleteOffensivePower() <= 1)
							pShip->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
					}
				}
			}
			else if (nOrder == COMBAT_ORDER::RETREAT)
			{
				// alle Schiffe bekommen den R�ckzugsbefehl, au�er sie haben Man�vrierbarkeit 0
				if (pShip->GetManeuverability() == 0 && pShip->IsNonCombat())
					pShip->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
				else if (pShip->GetManeuverability() == 0)
					pShip->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
				else
					pShip->SetCombatTactic(COMBAT_TACTIC::CT_RETREAT);
			}
			else if (nOrder == COMBAT_ORDER::USER)
			{
				// nichts umstellen, der Spieler hat die Befehle manuell eingestellt
			}
			else
				ASSERT(FALSE);
		}
	}
}
