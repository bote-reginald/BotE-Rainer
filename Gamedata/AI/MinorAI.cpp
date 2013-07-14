#include "stdafx.h"
#include "MinorAI.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "Races\GenDiploMessage.h"
#include "General/Loc.h"
#include <algorithm>
#include <cassert>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMinorAI::CMinorAI(CRace* pRace) : CDiplomacyAI(pRace)
{
}

CMinorAI::~CMinorAI(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion berechnet, wie eine computergesteuerte Rasse, auf ein Angebot reagiert
/// Das ist die komplette KI der Reaktion auf irgendwelche Angebote.
/// @param info Information des diplomatischen Angebots
/// @return <code>ACCEPTED</code> f�r Annahme
///			<code>DECLINED</code> f�r Ablehnung
///			<code>NOT_REACTED</code> f�r keine Reaktion
ANSWER_STATUS::Typ CMinorAI::ReactOnOffer(const CDiplomacyInfo& info)
{
	if (m_pRace->GetRaceID() != info.m_sToRace)
	{
		MYTRACE("general")(MT::LEVEL_INFO, "CMinorAI::ReactOnOffer(): Race-ID %s difference from Info-ID %s", m_pRace->GetRaceID(), info.m_sToRace);
		return ANSWER_STATUS::DECLINED;
	}

	CRace* pFromRace = m_pDoc->GetRaceCtrl()->GetRace(info.m_sFromRace);
	if (!pFromRace)
		return ANSWER_STATUS::DECLINED;

	if (pFromRace->IsMajor())
	{
		CMinor* pMinor = dynamic_cast<CMinor*>(m_pRace);
		ASSERT(pMinor);

		// wurde die Rasse erobert, so kann die Minorrace nicht auf das Angebot reagieren
		if (pMinor->GetSubjugated())
		{
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: Minor is subjugated -> no answer\n");
			return ANSWER_STATUS::DECLINED;
		}

		// bei bestimmten Sachen kann die KI nicht besonders reagieren und liefert ein festes Ergebnis
		// wurde Krieg erkl�rt
		if (info.m_nType == DIPLOMATIC_AGREEMENT::WAR)
			return ANSWER_STATUS::ACCEPTED;
		// soll der Vertrag aufgel�st werden
		else if (info.m_nType == DIPLOMATIC_AGREEMENT::NONE)
			return ANSWER_STATUS::ACCEPTED;
		// wurde ein Bestechungsversuch unternommen
		else if (info.m_nType == DIPLOMATIC_AGREEMENT::CORRUPTION)
		{
			if (TryCorruption(info))
				return ANSWER_STATUS::ACCEPTED;
			else
				return ANSWER_STATUS::DECLINED;
		}
		// wurde ein Geschenk �bergeben
		else if (info.m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
		{
			ReactOnDowry(info);
			return ANSWER_STATUS::ACCEPTED;
		}

		// Pr�fen ob der Vertrag aufgrund aktuell bestehender Vertr�ge �berhaupt angenommen werden darf
		if (!pMinor->CanAcceptOffer(m_pDoc, info.m_sFromRace, info.m_nType))
			return ANSWER_STATUS::DECLINED;

		// nun werden die aktuellen Beziehungen der Minorrace zu allen anderen Majorraces gespeichert. Wenn
		// beim Vertragsangebot eine Mitgift gemacht wurde und die Minor das Angebot n�mlich ablehnt, dann
		// werden die Beziehungen wieder zur�ckgesetzt. Wenn die Minor den Vertrag akzeptiert, dann
		// gelten die durch das Geschenk gemachten neuen Beziehungswerte.
		map<CString, BYTE> mOldRelations = pMinor->m_mRelations;
		// auf Geschenke reagieren
		ReactOnDowry(info);
		BYTE byOurRelationToThem = pMinor->GetRelation(info.m_sFromRace);

		// wurde in den letzten 2 Runden von der Minorrasse selbst ein Angebot an den Major gemacht, dann
		// wird das Angebot angenommen, sofern der Vertrag eine kleiner oder gleich gro�e Wertigkeit hat
		if (info.m_nType > DIPLOMATIC_AGREEMENT::NONE && info.m_nFlag == DIPLOMACY_OFFER)
		{
			const CDiplomacyInfo* pLastOffer = m_pRace->GetLastOffer(info.m_sFromRace);
			if (pLastOffer != NULL)
				if (info.m_nType <= pLastOffer->m_nType)
					return ANSWER_STATUS::ACCEPTED;
		}

		// Wenn wir das Angebot wahrmachen k�nnten, berechnen ob es klappt
		short nNeededRelation = 100;

		if (info.m_nType == DIPLOMATIC_AGREEMENT::TRADE)
		{
			nNeededRelation = 40;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::HOSTILE))
				nNeededRelation += 30;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SOLOING))
				nNeededRelation += 20;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SECRET))
				nNeededRelation += 10;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
				nNeededRelation -= 10;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: TRADE: nNeededRelation:%d (standard is 40)\n",nNeededRelation);
		}
		else if (info.m_nType == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
		{
			nNeededRelation = 50;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::HOSTILE))
				nNeededRelation += 30;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SOLOING))
				nNeededRelation += 20;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SECRET))
				nNeededRelation += 10;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::PACIFIST))
				nNeededRelation -= 10;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: FRIENDSHIP: nNeededRelation:%d (standard is 50)\n",nNeededRelation);
		}
		else if (info.m_nType == DIPLOMATIC_AGREEMENT::COOPERATION)
		{
			nNeededRelation = 65;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::HOSTILE))
				nNeededRelation += 30;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SOLOING))
				nNeededRelation += 20;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SECRET))
				nNeededRelation += 10;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
				nNeededRelation -= 5;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::PACIFIST))
				nNeededRelation -= 10;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: COOPERATION: nNeededRelation:%d (standard is 65)\n",nNeededRelation);
		}
		else if (info.m_nType == DIPLOMATIC_AGREEMENT::AFFILIATION)
		{
			nNeededRelation = 85;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::HOSTILE))
				nNeededRelation += 30;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SOLOING))
				nNeededRelation += 20;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SECRET))
				nNeededRelation += 10;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: AFFILIATION: nNeededRelation:%d (standard is 85)\n",nNeededRelation);
		}
		else if (info.m_nType == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
		{
			nNeededRelation = 100;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::HOSTILE))
				nNeededRelation += 30;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SOLOING))
				nNeededRelation += 20;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SECRET))
				nNeededRelation += 10;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
				nNeededRelation -= 5;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::PACIFIST))
				nNeededRelation -= 10;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: MEMBERSHIP: nNeededRelation:%d (standard is 85)\n",nNeededRelation);
		}

		float fMulti = byOurRelationToThem + 100;
		fMulti /= 100;
		float fValue = byOurRelationToThem * fMulti;
		short nRandom = (short)fValue;
		nRandom = max(1, nRandom);
		short nTemp = 0;
		for (int i = 0; i < 5; i++)
			nTemp += rand()%nRandom + 1;
		nRandom = (short)(nTemp / 5);
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: byOurRelationToThem:%d, NeededRelation: %d, fMulti: %lf, nRandom: %d",
														byOurRelationToThem, nNeededRelation, fMulti, nRandom);

		//CString s;
		//s.Format("Beziehung: %d\nNeededRelation: %d\nMulti: %lf\nforRandom: %d",relations[race-1],neededRelation,multi,forRandom);
		//AfxMessageBox(s);

		// Wenn wir den Status erfolgreich �ndern
		if (nRandom >= nNeededRelation)
		{
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: ACCEPTED, because nRandom:%d >= nNeededRelation:%d\n",
														nRandom, nNeededRelation);
			return ANSWER_STATUS::ACCEPTED;
		}

		// Wenn der Vertrag nicht angenommen wurde, dann bleiben alle Beziehungswerte vorerst auf ihrem alten Stand
		pMinor->m_mRelations = mOldRelations;
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: NOT ACCEPTED: mOldRelations:%d\n",
														pMinor->m_mRelations);

		// Wenn wir es nicht schaffen den Status zu �ndern, so k�nnte sich die Beziehung sogar verschlechtern
		short relationMali = 1 + info.m_nType - pMinor->GetCorruptibility();
		if (relationMali > 0)
			relationMali = rand()%relationMali;
		else
			return ANSWER_STATUS::DECLINED;

		pMinor->SetRelation(info.m_sFromRace, -relationMali);
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::ReactOnOffer: DECLINED+maybe relationMali=%d, 1 + info.m_nType:%d - Corruptibility=%d) \n",
														relationMali, 1 + info.m_nType, pMinor->GetCorruptibility());

		return ANSWER_STATUS::DECLINED;
	}
	else
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "CMinorAI::ReactOnOffer(): %s could not react on offers from non Majors.", m_pRace->GetRaceID());
		assert(false);
	}

	return ANSWER_STATUS::DECLINED;
}

/// Funktion zur Erstellung eines diplomatischen Angebots.
/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
bool CMinorAI::MakeOffer(CString& sRaceID, CDiplomacyInfo& info)
{
	// nur zu 50% wird �berhaupt etwas angeboten.
	// dadurch kommt nicht jede Runde solch ein Angebot rein
	if (rand()%2 == 0)
		return false;

	CRace* pRace = m_pDoc->GetRaceCtrl()->GetRace(sRaceID);
	if (!pRace)
		return false;

	if (pRace->IsMajor())
	{
		CMinor* pMinor = dynamic_cast<CMinor*>(m_pRace);
		if (!pMinor)
			return false;

		if (pMinor->GetSubjugated())
			return false;

		CMajor* pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sRaceID));
		if (!pMajor)
			return false;

		if (pMinor->IsRaceContacted(sRaceID))
		{
			BYTE byOurRelationToThem = pMinor->GetRelation(sRaceID);
			DIPLOMATIC_AGREEMENT::Typ nAgreement = pMinor->GetAgreement(sRaceID);

			// jetzt zum Algorithmus:
			//	Aufgrund der Beziehung zur Majorrace macht die Minorrace dieser vielleicht ein diplomatisches Angebot

			// Checken ob wir ein Angebot �berhaupt machen k�nnen, z.B. wenn eine andere Hauptrasse
			// z.B. Mitgliedschaft mit der Minorrace hat, dann k�nnen wir ihr kein Angebot machen
			DIPLOMATIC_AGREEMENT::Typ nOthersAgreement = DIPLOMATIC_AGREEMENT::NONE;
			map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			{
				DIPLOMATIC_AGREEMENT::Typ nTemp = pMinor->GetAgreement(it->first);
				if (nTemp > nOthersAgreement)
					nOthersAgreement = nTemp;
			}
			// wenn die Minor irgendeine Mitgliedschaft mit einer anderen Rasse hat, dann macht sie kein Angebot
			if (nOthersAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
				return false;

			float fMulti = byOurRelationToThem + 100;
			fMulti /= 100;
			float fValue = byOurRelationToThem * fMulti;
			short nRandom = (short)fValue;
			nRandom = max(1, nRandom);
			short nTemp = 0;
			for (int i = 0; i < 5; i++)
				nTemp += rand()%nRandom + 1;
			nRandom = (short)(nTemp / 5);

			// Jetzt haben wir nen Randomwert, wenn dieser einen zuf�lligen Wert �berschreitet, bietet die
			// Minorrace zu 50% etwas an
			DIPLOMATIC_AGREEMENT::Typ nOffer = DIPLOMATIC_AGREEMENT::NONE;
			if (nRandom > 120 && nAgreement < DIPLOMATIC_AGREEMENT::MEMBERSHIP && nOthersAgreement <= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				nOffer = DIPLOMATIC_AGREEMENT::MEMBERSHIP;
			else if (nRandom > 105 && nAgreement < DIPLOMATIC_AGREEMENT::AFFILIATION && nOthersAgreement <= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				nOffer = DIPLOMATIC_AGREEMENT::AFFILIATION;
			else if (nRandom > 90 && nAgreement < DIPLOMATIC_AGREEMENT::COOPERATION && nOthersAgreement <= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				nOffer = DIPLOMATIC_AGREEMENT::COOPERATION;
			else if (nRandom > 75 && nAgreement < DIPLOMATIC_AGREEMENT::FRIENDSHIP && nOthersAgreement <= DIPLOMATIC_AGREEMENT::COOPERATION)
				nOffer = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
			else if (nRandom > 55 && nAgreement < DIPLOMATIC_AGREEMENT::TRADE && nOthersAgreement <= DIPLOMATIC_AGREEMENT::AFFILIATION)
				nOffer = DIPLOMATIC_AGREEMENT::TRADE;
			// nochmalige Sicherheitsabfrage
			if (nAgreement >= nOffer)
				nOffer = DIPLOMATIC_AGREEMENT::NONE;

			// Das k�nnte man noch von der Art der Rasse abh�ngig machen, kriegerische Rassen erkl�ren eher Krieg,
			// pazifistische Rasse erkl�ren gar keinen Krieg oder so �hnlich -> hier gemacht
			short nMinRel = 15;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::HOSTILE))		// Wir lieben Krieg
				nMinRel += 25;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::WARLIKE))		// Wir lieben Krieg
				nMinRel += 15;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SNEAKY))		// Wir sind eh hinterh�ltig
				nMinRel += 5;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::FINANCIAL))	// Krieg ist nicht immer gut f�rs Gesch�ft
				nMinRel -= 5;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))	// Krieg ist nicht immer gut f�r Forschung
				nMinRel -= 10;
			if (pMinor->IsRaceProperty(RACE_PROPERTY::PACIFIST))	// Wir hassen Krieg
				nMinRel -= 15;

			if (nRandom < nMinRel && byOurRelationToThem < nMinRel && nAgreement != DIPLOMATIC_AGREEMENT::WAR)
				nOffer = DIPLOMATIC_AGREEMENT::WAR;

			if (nOffer != DIPLOMATIC_AGREEMENT::NONE)
			{
				// wenn nur Aliendiplomatie m�glich ist, dann darf nur Krieg oder Freundschaft angeboten werden
				if (pMinor->HasSpecialAbility(SPECIAL_ALIEN_DIPLOMACY) || pMajor->HasSpecialAbility(SPECIAL_ALIEN_DIPLOMACY))
				{
					// Wenn gr��er als Freundschaft, dann nur Freundschaft anbieten falls noch nicht vorhanden
					if (nOffer >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						if (nAgreement < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
							nOffer = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
					}

					// Wenn nicht Krieg und auch nicht Freundschaft angeboten wurde (z.B. Handelsvertrag),
					// dann kein Angebot machen
					if (nOffer != DIPLOMATIC_AGREEMENT::WAR && nOffer != DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						nOffer = DIPLOMATIC_AGREEMENT::NONE;
						return false;
					}
				}

				// wichte Parameter des Infoobjekts zuweisen
				info.m_nFlag = DIPLOMACY_OFFER;
				info.m_nType = nOffer;
				info.m_sFromRace = pMinor->GetRaceID();
				info.m_sToRace = sRaceID;
				info.m_nSendRound = m_pDoc->GetCurrentRound() - 1;
				CGenDiploMessage::GenerateMinorOffer(info);
				MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "Minor: %s makes offer %d to Major %s\n", info.m_sFromRace, info.m_nType, info.m_sToRace);
				return true;
			}
		}
		return false;
	}
	else
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "Minor: %s makes offer %d to Major %s\n", info.m_sFromRace, info.m_nType, info.m_sToRace);
		assert(false);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion berechnet die Beziehungsverbesserungen durch die �bergabe von Credits und Ressourcen.
/// Die Credits werden hier jedoch nicht gutgeschrieben, sondern nur die Beziehung zur Majorrace verbessert.
/// @param info Referenz auf eine Diplomatieinformation
void CMinorAI::ReactOnDowry(const CDiplomacyInfo& info)
{
	USHORT nCreditsFromRes = (USHORT)CalcResInCredits(info);
	//CString s;
	//s.Format("Ressourcenwert: %d", nCreditsFromRes);
	//AfxMessageBox(s);

	//Berechnen wieviel die Credits eigentlich wert sind, wenn wir die langj�hrigen Beziehungen zu anderen
	//Majorraces mit einberechnen. Das bedeutet, umso l�nger jemand anderes diese Rasse kennt, umso mehr
	//AcceptancePoints wird diese bei der Rasse haben und umso schwerer wird es f�r andere Rassen die Beziehung
	//zu dieser durch Geschenke zu ver�ndern. Wir suchen uns also die Rasse, welche die meistens Acceptance-
	//Points bei dieser hat (au�er die Geldgeberrasse) und subtrahieren von diesem Wert unsere Akzeptanzpunkte.
	// Den dadurch erhaltenen Wert (nur positiv) ziehen wir von den Credits ab.
	int nCredits = info.m_nCredits + nCreditsFromRes;
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: nCredits %d (info.m_nCredits:%d + nCreditsFromRes:%d)\n", nCredits, info.m_nCredits, nCreditsFromRes);
	if (nCredits <= 0)
		return;

	CMinor* pMinor = dynamic_cast<CMinor*>(m_pRace);
	if (!pMinor)
		return;

	int nAcceptancePoints = 0;
	// h�chsten Wert (au�er eigenen) suchen
	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (info.m_sFromRace != it->first)
			nAcceptancePoints = max(nAcceptancePoints, pMinor->GetAcceptancePoints(it->first));

	// eigene AcceptancePoints mit einberechnen. Weil wenn wir selber eine gute Beziehung zu der Rasse haben,
	// dann soll unser Geld auch was wert sein
	if (pMinor->GetAcceptancePoints(info.m_sFromRace) > nAcceptancePoints)
		nAcceptancePoints = 0;
	else
		nAcceptancePoints -= pMinor->GetAcceptancePoints(info.m_sFromRace);
	// Grenzen einhalten
	if (nAcceptancePoints >= nCredits)
		nCredits = 1;
	else
		nCredits -= nAcceptancePoints;
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: nCredits:%d, our nAcceptancePoints:%d, Minor AcceptancePoints:%d\n", nCredits, nAcceptancePoints, pMinor->GetAcceptancePoints(info.m_sFromRace));

	//CString s;
	//s.Format("Geld: %d\nRelationPoints: %d\nF�d-relationpoints: %d\nFer-relationpoints: %d\nKli-relationpoints: %d\nRom-relationpoints: %d\nCar-relationpoints: %d\nDom-relationpoints: %d\n",
	//	credits,relationPoints,m_iRelationPoints[0],m_iRelationPoints[1],m_iRelationPoints[2],m_iRelationPoints[3],m_iRelationPoints[4],m_iRelationPoints[5]);
	//AfxMessageBox(s);

	// Berechnen welche obere Grenze wir durch die Menge Credits bekommen
	// Der berechnete Wert ist zuf�llig, innerhalb einiger Parameter, die durch die Credits vorgegeben sind
	short nRandomCredits = 0;
	// Schleife von 3 Durchl�ufen -> Zufallsvariablendurchnschitt, damit wir bei 5000 Credits nur sehr unwahrscheinlich
	// mal nen Wert von unter 1000 oder so bekommen
	for (int i = 0; i < 3; i++)
		nRandomCredits += rand()%(nCredits * 2 + 1);
	nRandomCredits = (short)(nRandomCredits / 3);
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: nRandomCredits %d\n", nRandomCredits);

	// Wert, den wir mit nRandomCredits erreichen m�ssen, um die Beziehung zu verbessern
	short nNeededValue = DIPLOMACY_PRESENT_VALUE;	// 200 ist "normal"

	if (pMinor->IsRaceProperty(RACE_PROPERTY::SOLOING))
		nNeededValue += 150;	// bei einer zur�ckgezogenen Rasse ist der Wert wesentlich h�her
	if (pMinor->IsRaceProperty(RACE_PROPERTY::HOSTILE))
		nNeededValue += 100;	// bei einer extrem feindlichen Rasse ist der Wert wesentlich h�her
	if (pMinor->IsRaceProperty(RACE_PROPERTY::SECRET))
		nNeededValue += 75;		// bei einer Geheimdienstrasse ist der Wert h�her
	if (pMinor->IsRaceProperty(RACE_PROPERTY::WARLIKE))
		nNeededValue += 50;		// bei einer kriegerischen Rasse ist der Wert h�her
	if (pMinor->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
		nNeededValue -= 25;		// bei einer industriellen Rasse ist der Wert niedriger
	if (pMinor->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
		nNeededValue -= 50;		// bei einer landwirtschaftlichen Rasse ist der Wert niedriger
	if (pMinor->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
		nNeededValue -= 100;	// bei einer finanziellen Rasse ist der Wert niedriger
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: nNeededValue before relation %d (Basic is 200)\n", nNeededValue);

	//Jetzt neededValue noch modifizieren mit der aktuellen Beziehung der Geldgeberrasse.
	//Alles �ber 50% wird auf die neededValue draufgerechnet, alles unter 50% wird von
	//neededValue abgezogen. Au�erdem werden noch die diplomatischen F�higkeiten der
	//einzelnen Majorraces mit einbezogen.
	CMajor* pMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(info.m_sFromRace));
	if (!pMajor)
		return;

	short nOurRelationToThem = pMinor->GetRelation(info.m_sFromRace);
	nNeededValue += nOurRelationToThem - 50;
	nNeededValue = (short)(nNeededValue * (100 - pMajor->GetDiplomacyBonus()) / 100);
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: nNeededValue after relation %d, nOurRelationToThem %lf\n", nNeededValue, nOurRelationToThem);

	// Jetzt berechnen, um wieviel sich die Beziehung ver�ndern w�rde
	float fBonus = 0.0f;
	if (nRandomCredits >= nNeededValue)
		fBonus = nRandomCredits / nNeededValue;	// Wieviel Prozent bekommen wir als Bonus dazu
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: fBonus %d (nRandomCredits / nNeededValue), double: nOurRelationToThem %lf\n", fBonus, nOurRelationToThem);

	//Jetzt den Modifikator der Bestechlichkeit der kleinen Rasse mit einbeziehen. Das bedeutet,
	//ob der fBonus so bleibt oder nicht. Bei gro�er Resistenz wird fBonus nochmal verringert und
	//wenn die Rasse sich von Geld gut beeinflussen l��t, dann wird fBonus sogar erh�ht

	switch (pMinor->m_iCorruptibility)
	{
	// kaum
	case 0:	fBonus *= 0.5f;	break;
	// wenig
	case 1:	fBonus *= 0.75f;break;
	// bei 2 ver�ndert sich ja nichts
	// viel
	case 3:	fBonus *= 1.25f;break;
	// sehr viel
	case 4:	fBonus *= 1.5f; break;
	}
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: fBonus %d after m_iCorruptibility:%lg\n", fBonus, pMinor->m_iCorruptibility);

	// Nun den fBonus runden und wir haben den Prozentwert, um wieviel sich die Beziehung verbessert.
	nOurRelationToThem += (short)fBonus;
	if (nOurRelationToThem > 100)
	{
		fBonus -= nOurRelationToThem - 100;
		nOurRelationToThem = 100;
	}
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: fBonus %d after ?, new nOurRelationToThem:%lf (100, if ?)\n", fBonus, nOurRelationToThem);
	short nOldRelation = pMinor->GetRelation(info.m_sFromRace);
	if (nOurRelationToThem - nOldRelation != 0)
		pMinor->SetRelation(info.m_sFromRace, nOurRelationToThem - nOldRelation);


	if (fBonus >= 1.0)
	{
		// Jetzt die Funktion aufrufen, die die Beziehung zu den anderen Rassen um den
		// Prozentwert verringert, den die Geldgeberrasse bekommen hat.
		this->CalcOtherMajorsRelationChange(info, (short)fBonus);
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: fBonus > 1.0 !! Relation to other races effected\n");
	}
}

/// Funktion berechnet die diplomatischen Auswirkungen auf die anderen Rassen, wenn die Beziehung
/// durch eine Rasse z.B. durch ein Geschenk verbessert wurde.
/// Diese Funktion wird am Ende von <func>ReactOnDowry</func> aufgerufen und zieht die Prozentpunkte, die die
/// Geldgeberrasse dazugewonnen hat von den anderen Rassen ab.
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @param nRelationChange gewonnene Prozentpunkte der Geschenkgeberrasse
void CMinorAI::CalcOtherMajorsRelationChange(const CDiplomacyInfo& info, short nRelationChange)
{
	// Der Bonuswert wird von den Beziehungen prozentweise abgezogen.
	// Es wird immer bei der schlechtesten Beziehung zuerst abgezogen, dann
	// bei der n�chst schlechtesten usw.
	// Wenn keine Bonuswertpunkte mehr abgezogen werden k�nnen ist die Funktion beendet.

	// Struktur mit Major-ID und Beziehungswert (dazu ein paar Operatoren definieren)
	struct MAJORLIST {
		 CString sID;
		 BYTE	 byRelation;

		bool operator< (const MAJORLIST& elem2) const { return byRelation < elem2.byRelation;}
		bool operator> (const MAJORLIST& elem2) const { return byRelation > elem2.byRelation;}
		MAJORLIST() : sID(""), byRelation(0) {}
		MAJORLIST(const CString& _sID, BYTE _byRelation) : sID(_sID), byRelation(_byRelation) {}
	};
	vector<MAJORLIST> vKnownMajors;

	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (info.m_sFromRace != it->first)
			if (m_pRace->IsRaceContacted(it->first))
				vKnownMajors.push_back(MAJORLIST(it->first, m_pRace->GetRelation(it->first)));
	// gibt es keine andere bekannte Rasse, so kann die Funktion verlassen werden.
	if (vKnownMajors.empty())
		return;

	// nun die bekannten Majors nach deren Beziehung zur Minorrace aufsteigend sortieren
	std::sort(vKnownMajors.begin(), vKnownMajors.end());

	// Wenn die Minorrace nicht alle Majors kennt, dann den die Bonusprozente etwas verringern.
	// Weil wenn sie nur eine kennt, dann w�rde der alles abgezogen werden.
	// Differenz zwischen allen Rassen und bekannten Rassen ausrechnen
	int nTemp = pmMajors->size() - vKnownMajors.size();
	// Diesen Wert nochmal halbieren. Beispiel: 6 Majors im Spiel, 1 Major bekannt -> nTemp = 2;
	nTemp = (int)(nTemp / 1.5f);
	nTemp = max(1, nTemp);
	nRelationChange /= nTemp;

	// nun die Prozentpunkte beginnend bei der Majorrace mit der schlechtesten Beziehung in einer Schritten
	// abziehen. Dies passiert solange, bis alle Prozentpunkte verteilt wurden.
	int nRace = 0;
	while (nRelationChange > 0)
	{
		m_pRace->SetRelation(vKnownMajors[nRace].sID, -1);
		nRace++;
		nRelationChange--;

		if (nRace == static_cast<int>(vKnownMajors.size()))
			nRace = 0;
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI: nRace:%d, nRelationChange:%d\n", nRace, nRelationChange);
	}
}

/// Funktion berechnet den Bestechungsversuch bei der Minorrace
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @return <code>true</code> wenn Bestechung erfolgreich ist, sonst <code>false</code>
bool CMinorAI::TryCorruption(const CDiplomacyInfo& info)
{
	CMinor* pMinor = dynamic_cast<CMinor*>(m_pRace);
	if (!pMinor)
		return false;

	CMajor* pCorruptedMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(info.m_sCorruptedRace));
	if (!pCorruptedMajor)
		return false;

	CMajor* pFromMajor = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(info.m_sFromRace));
	if (!pFromMajor)
		return false;

	int nCredits = info.m_nCredits + CalcResInCredits(info);
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::TryCorruption: nCredits:%d, info.m_nCredits:%d, CalcResInCredits(info):%d\n", 
																	nCredits, info.m_nCredits, CalcResInCredits(info));

	// Bestechungsresitance durch z.B. Kommunikationsnetzwerke holen
	if (pMinor->GetRaceKO() == CPoint(-1,-1))
		return false;

	CSystem* pSystem = &(m_pDoc->GetSystem(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y));
	ASSERT(pSystem);
	short nResistance = pSystem->GetProduction()->GetResistance();
	// Bei einer Bestechnung mit z.B. 5000 Credit�bergabe ergibt dies einen Wert von 5000.
	// Der Wert des Kommunikationsnetzwerkes wird diesen Wert verringern.
	// Danach l�uft der Algorithmus erst los. Wenn man also ein Kommunikationsnetzwerk mit
	// einem Resistancewert von 3000 besitzt, dann wird der Creditwert um 3000 verringert
	// und ergibt am Ende nur noch 2000 (5000 - 3000 = 2000).
	// Dies w�re dann so, als h�tte die Rasse nur 2000 Credits bei der Bestechung mitgegeben.
	// Dies verringert die Akzeptanzpunkte viel weniger und auch die Erfolgwahrscheinlichkeit nimmt ab.
	nCredits -= nResistance;
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::TryCorruption: nCredits (before max):%d, nResistance:%d\n", nCredits, nResistance);
	nCredits = max(nCredits, 0);

	// Jetzt noch die angesammelten AcceptancePoints der corruptedMajor verringern.
	// Somit haben wir hier die M�glichkeit, diese Punkte durch Bestechung zu verringern.
	// Bis jetzt wird der Wert noch durch nichts modifiziert.
	// zum Algorithmus:
	// Ich berechne nCredits/15 und ziehe den erhaltenen Wert von den AcceptancePoints ab
	pMinor->SetAcceptancePoints(info.m_sCorruptedRace, -nCredits / 15);
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::TryCorruption: nCredits (before max):%d\n", nCredits / 15);

	// Der Betrag der Credit kanns zwischen 0 und ca. 13.000 liegen.
	// Dieser zus�tzliche Betrag kann die Bestechung erleichtern.
	// Bei erfolgreicher Bestechung wird der Vertrag einer Majorrace gecancelt
	// und ihre angesammelten AcceptancePoints verringern sich.
	// Bei nicht erfolgreicher Bestechung wird der Vertrag nicht gecancelt, aber
	// auch hier werden die AcceptancePoints verringert.
	//
	// zum Algorithmus:
	// Ich laufe nCredits/250 mal die Schleife mit folgender Rechnung durch:
	// Die Beziehung der Nestecherrasse wird ins Verh�ltnis zur Beziehung der corruptedMajor
	// gesetzt, d.h. Beziehung[info.m_sFromRace] - Beziehung[info.m_sCorruptedRace].
	// Der erhaltene Wert wird auf rand()%100 addiert. Au�erdem wird noch
	// nCredits/750 draufgerechnet. Wenn dieser Gesamtwert gr��er als 90 ist, dann war
	// die Bestechung erfolgreich. Aber nicht immer 90, sondern Wert ist noch abh�ngig von
	// der Bestechlichkeit der kleinen Rasse.

	USHORT nCorruptionValue = 90;
	switch (pMinor->GetCorruptibility())
	{
	case 0:	// kaum (bestechlich)
			nCorruptionValue = 100;	break;
	case 1:	// wenig
			nCorruptionValue = 95;	break;
	// Bei 2 ver�ndert sich ja nichts
	case 3:	// viel
			nCorruptionValue = 85;	break;
	case 4: // sehr viel (sehr bestechlich)
			nCorruptionValue = 80;	break;
	}
	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::TryCorruption: nCorruptionValue:%d (standard is 90, 100 is hard corruptional)\n", nCorruptionValue);


	short nValue = 0;

	short nRelationFromMajor		= pMinor->GetRelation(info.m_sFromRace);
	short nRelationCorruptedMajor	= pMinor->GetRelation(info.m_sCorruptedRace);

	for (int i = 0; i <= (USHORT)nCredits / 250; i++)
	{
		const short nRelationDiff = nRelationFromMajor - nRelationCorruptedMajor;
		nValue = rand()%100 + nRelationDiff + nCredits / 750;
		// war die Bestechung erfolgreich?
		if (nValue > nCorruptionValue)
			break;
	}

	if (nValue > nCorruptionValue)
	{
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CorruptionWasSuccessful, because nValue:%d > nCorruptionValue:%d\n", nValue, nCorruptionValue);
		// Die Bestechnung war erfolgreich -> der Vertrag mit der CorruptedMajor wird gek�ndigt
		DIPLOMATIC_AGREEMENT::Typ nAgreement = pMinor->GetAgreement(info.m_sCorruptedRace);
		CString sText;
		switch (nAgreement)
		{
		case DIPLOMATIC_AGREEMENT::TRADE:
		{
			sText = CLoc::GetString("CANCEL_TRADE_AGREEMENT", FALSE, pMinor->m_sName);
			break;
		}
		case DIPLOMATIC_AGREEMENT::FRIENDSHIP:
		{
			sText = CLoc::GetString("CANCEL_FRIENDSHIP", FALSE, pMinor->m_sName);
			break;
		}
		case DIPLOMATIC_AGREEMENT::COOPERATION:
		{
			sText = CLoc::GetString("CANCEL_COOPERATION", FALSE, pMinor->m_sName);
			break;
		}
		case DIPLOMATIC_AGREEMENT::AFFILIATION:
		{
			sText = CLoc::GetString("CANCEL_AFFILIATION", FALSE, pMinor->m_sName);
			break;
		}
		case DIPLOMATIC_AGREEMENT::MEMBERSHIP:
		{
			sText = CLoc::GetString("CANCEL_MEMBERSHIP", FALSE, pMinor->m_sName);
			break;
		}
		}

		CEmpireNews message;
		if (!sText.IsEmpty())
		{
			pCorruptedMajor->SetAgreement(pMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
			pMinor->SetAgreement(info.m_sCorruptedRace, DIPLOMATIC_AGREEMENT::NONE);
			message.CreateNews(sText, EMPIRE_NEWS_TYPE::DIPLOMACY);
			pCorruptedMajor->GetEmpire()->AddMsg(message);
		}
		// Nachricht �ber Erfolg bei Bestecherrasse erzeugen
		sText = CLoc::GetString("CORRUPTION_SUCCESS", FALSE, pMinor->m_sName);
		message.CreateNews(sText, EMPIRE_NEWS_TYPE::DIPLOMACY);
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CorruptionWasSuccessful, because nValue:%d > nCorruptionValue:%d -> %s\n", nValue, nCorruptionValue, sText);
		pFromMajor->GetEmpire()->AddMsg(message);
		return true;
	}
	else
	{	// Die Bestechnung war nicht erfolgreich
		CString sText;
		CEmpireNews message;
		// nicht immer bekommt die corruptedRace die Nachricht, sondern nur wenn siehe "if"
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CorruptionWasSuccessful, because nValue:%d > nCorruptionValue:%d -> %s\n", nValue, nCorruptionValue, sText);
		if (nValue < nRelationCorruptedMajor - 10)
		{
			CString s = pFromMajor->GetEmpireNameWithArticle();
			// Satz mit gro�em Buchstaben beginnen
			CString sUpper = (CString)s.GetAt(0);
			s.SetAt(0, sUpper.MakeUpper().GetAt(0));

			sText = CLoc::GetString("TRYED_CORRUPTION", FALSE, s, pMinor->m_sName);
			message.CreateNews(sText, EMPIRE_NEWS_TYPE::DIPLOMACY);
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::Corruption was NOT successful and nValue:%d < nRelationCorruptedMajor:%d -10: -> %s\n", nValue, nCorruptionValue, sText);
			pCorruptedMajor->GetEmpire()->AddMsg(message);
		}

		sText = CLoc::GetString("CORRUPTION_FAILED", FALSE, pMinor->m_sName);
		message.CreateNews(sText, EMPIRE_NEWS_TYPE::DIPLOMACY);
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::Corruption was NOT successful: %s\n", sText);
		pFromMajor->GetEmpire()->AddMsg(message);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen
//////////////////////////////////////////////////////////////////////

/// Funktion berechnet, wie viel �bergebene Ressourcen als Credits wert sind.
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @return Wert der �bergebenen Ressourcen in Credits
int CMinorAI::CalcResInCredits(const CDiplomacyInfo& info)
{
	CMinor* pMinor = dynamic_cast<CMinor*>(m_pRace);
	if (!pMinor)
		return 0;

	// An Alienrassen k�nnen keine Ressourcen �bergeben werden
	if (pMinor->IsAlienRace())
		return 0;

	// zuerst muss berechnet werden, wie viel die �bergebenen Ressourcen als Credits wert sind

	// Titan wird getauscht im Verh�ltnis 5:1
	// Deuterium wird getauscht im Verh�ltnis 4.5:1
	// Duranium wird getauscht im Verh�ltnis 4:1
	// Crystal wird getauscht im Verh�ltnis 3.25:1
	// Iridium wird getauscht im Verh�ltnis 2.5:1
	// Deritium wird getauscht im Verh�ltnis 1:50

	float fValue = 0.0f;
	float fDiv = 0.0f;
	CSystem* pSystem = &(m_pDoc->GetSystem(pMinor->m_ptKO.x, pMinor->m_ptKO.y));

	if (info.m_nResources[TITAN] != 0)			// Titan �bergeben?
	{
		fValue = info.m_nResources[TITAN] / 5.0f;
		fDiv = (float)(pSystem->GetResourceStore(TITAN)) / 2000;
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: Titan! info.m_nResources[TITAN] %g / 5, fDiv:%g (float res from system / 2000)\n", 
			fValue, fDiv);
	}
	else if (info.m_nResources[DEUTERIUM] != 0)	// Deuterium �bergeben?
	{
		fValue = info.m_nResources[DEUTERIUM] / 4.5f;
		fDiv = (float)(pSystem->GetResourceStore(DEUTERIUM)) / 2000;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: DEUTERIUM! info.m_nResources[DEUTERIUM] %g / 4.5, fDiv:%g (float res from system / 2000)\n", 
			fValue, fDiv);
	}
	else if (info.m_nResources[DURANIUM] != 0)	// Duranium �bergeben?
	{
		fValue = info.m_nResources[DURANIUM] / 4.0f;
		fDiv = (float)(pSystem->GetResourceStore(DURANIUM)) / 2000;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: DURANIUM! info.m_nResources[DURANIUM] %g / 4, fDiv:%g (float res from system / 2000)\n", 
			fValue, fDiv);
	}
	else if (info.m_nResources[CRYSTAL] != 0)	// Kristalle �bergeben?
	{
		fValue = info.m_nResources[CRYSTAL] / 3.25f;
		fDiv = (float)(pSystem->GetResourceStore(CRYSTAL)) / 2000;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: CRYSTAL! info.m_nResources[CRYSTAL] %g / 3.25, fDiv:%g (float res from system / 2000)\n", 
			fValue, fDiv);
	}
	else if (info.m_nResources[IRIDIUM] != 0)	// Iridium �bergeben?
	{
		fValue = info.m_nResources[IRIDIUM] / 2.5f;
		fDiv = (float)(pSystem->GetResourceStore(IRIDIUM)) / 2000;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: IRIDIUM! info.m_nResources[IRIDIUM] %g / 2.5, fDiv:%g (float res from system / 2000)\n", 
			fValue, fDiv);
	}
	else if (info.m_nResources[DERITIUM] != 0)	// Deritium �bergeben?
	{
		fValue = info.m_nResources[DERITIUM] * 50;
		fDiv = (float)(pSystem->GetResourceStore(DERITIUM)) / 10;
			MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: DERITIUM! info.m_nResources[DERITIUM] %g * 50, fDiv:%g (float res from system / 10)\n", 
			fValue, fDiv);
	}
	
	//So, nun gibts aber nicht immer diesen Betrag! Daf�r fragen wir die
	//kompletten Lager ab. Umso mehr wir haben, umso weniger ist der
	//�bergebene Rohstoff f�r uns wert.
	//Daf�r berechnen wir einen Teiler. Wir nehmen die aktuelle
	//Menge des jeweiligen Rohstoffes, die in allen Systemen gelagert sind.
	//Mit diesem Teiler wird der erhaltene Wert des Rohstoffes multipliziert.
	//Dieser nun erhaltene Wert wird noch rassenabh�ngig modifiziert.

	if (fValue != 0.0f)
	{
		fDiv = max(1.0f, fDiv);
		fValue /= fDiv;
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: Value vs own Storage before RaceProperty fDiv:%g, fValue:%g( /=fDiv)\n", 
			fValue, fDiv);

		// Wert nochmal modifizieren, aufgrund der Rassenart
		if (pMinor->IsRaceProperty(RACE_PROPERTY::PRODUCER))
			fValue *= 1.5f;
		if (pMinor->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
			fValue *= 1.35f;
		if (pMinor->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			fValue *= 1.2f;
		if (pMinor->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			fValue *= 1.1f;
		if (pMinor->IsRaceProperty(RACE_PROPERTY::SECRET))
			fValue *= 0.9f;
		if (pMinor->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
			fValue *= 0.8f;
		MYTRACE("diplomacydetails")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: Value vs own Storage after RaceProperty fValue:%g\n", 
			fValue);

		fValue = max(0.0f, fValue);

	}
	MYTRACE("diplomacydetails")(MT::LEVEL_DEBUG, "MinorAI::CalcResInCredits: Value vs own Storage final fValue:%g\n", 
					fValue);
	return (int)fValue;
}
