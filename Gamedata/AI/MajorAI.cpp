#include "stdafx.h"
#include "MajorAI.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "Races\GenDiploMessage.h"

#include <algorithm>
#include <cassert>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMajorAI::CMajorAI(CRace* pRace) : CDiplomacyAI(pRace)
{
}

CMajorAI::~CMajorAI(void)
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
ANSWER_STATUS::Typ CMajorAI::ReactOnOffer(const CDiplomacyInfo& info)
{
	if (m_pRace->GetRaceID() != info.m_sToRace)
	{
		MYTRACE("general")(MT::LEVEL_WARNING, "CMajorAI::ReactOnOffer(): Warning: Race-ID %s difference from Info-ID %s", m_pRace->GetRaceID(), info.m_sToRace);
		return ANSWER_STATUS::NOT_REACTED;
	}

	CRace* pFromRace = m_pDoc->GetRaceCtrl()->GetRace(info.m_sFromRace);
	if (!pFromRace)
		return ANSWER_STATUS::NOT_REACTED;

	if (pFromRace->IsMajor())
	{
		return ReactOnMajorOffer(info);
	}
	else if (pFromRace->IsMinor())
	{
		return ReactOnMinorOffer(info);
	}
	else
		assert(false);

	return ANSWER_STATUS::NOT_REACTED;
}

/// Funktion zur Erstellung eines diplomatischen Angebots.
/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
bool CMajorAI::MakeOffer(CString& sRaceID, CDiplomacyInfo& info)
{
	CRace* pRace = m_pDoc->GetRaceCtrl()->GetRace(sRaceID);

	if (!pRace)
		return false;

	if (pRace->IsMajor())
	{
		return MakeMajorOffer(sRaceID, info);
	}
	else if (pRace->IsMinor())
	{
		return MakeMinorOffer(sRaceID, info);
	}
	else
		assert(false);

	return false;
}

/// Rasse bestimmt einen Vektor mit den Lieblingsrassen der Majorrace. Auf diese wird sich
/// beim Geschenkegeben und bei Mitgiften konzentriert.
void CMajorAI::CalcFavoriteMinors(void)
{
	m_sFavoriteMinor = "";

	CMajor* pOurRace = dynamic_cast<CMajor*>(m_pRace);
	if (!pOurRace)
		return;

	// Struktur mit Minor-ID und Beziehungswert (dazu ein paar Operatoren definieren)
	struct MINORLIST {
		 CString sID;
		 BYTE	 byRelation;

		bool operator< (const MINORLIST& elem2) const { return byRelation < elem2.byRelation;}
		bool operator> (const MINORLIST& elem2) const { return byRelation > elem2.byRelation;}
		MINORLIST() : sID(""), byRelation(0) {}
		MINORLIST(const CString& _sID, BYTE _byRelation) : sID(_sID), byRelation(_byRelation) {}
	};

	vector<MINORLIST> vFavoriteMinors;

	// Wir suchen hier nach der Minorrace, der ich ein Geschenk bzw. ein Angebot inkl. Mitgift machen w�rde
	// ich suche mir die Rassen raus, zu denen ich die beste Beziehung habe.
	// Au�erdem gebe ich auch keiner Rasse Geld bzw. Mitgifte, wenn ich schon eine Mitgliedschaft mit dieser habe,
	// deren Akzeptanz bei einer anderen Rasse zu hoch ist oder diese unterworfen wurde.
	map<CString, CMinor*>* pmMinors = m_pDoc->GetRaceCtrl()->GetMinors();
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
	{
		CMinor* pMinor = dynamic_cast<CMinor*>(it->second);
		// kennen wir die Minorrace und wurde diese nicht unterworfen
		if (pOurRace->IsRaceContacted(pMinor->GetRaceID()) && !pMinor->GetSubjugated())
			// ist die Beziehung noch kleiner 95 und wir haben noch keine Mitgliedschaft
			if (pMinor->GetRelation(pOurRace->GetRaceID()) < 95 && pMinor->GetAgreement(pOurRace->GetRaceID()) < DIPLOMATIC_AGREEMENT::MEMBERSHIP)
			{
				// ist die Akzeptanz bei einer anderen Major noch nicht zu hoch?
				bool bHighAcceptance = false;
				map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
				for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
					if (itt->first != pOurRace->GetRaceID())
						if ((pMinor->IsMemberTo(itt->first) && pMinor->GetAcceptancePoints(itt->first) > 3000)
							|| pMinor->GetAcceptancePoints(itt->first) > 4000)
						{
							bHighAcceptance = true;
							break;
						}
				// hat die Minor enen zu hohen Akzeptanzwert, so wird diese nicht zu den Lieblingsminors gez�hlt
				if (bHighAcceptance)
					continue;
				// ansonsten kann die Minor weiter betrachtet werden
				vFavoriteMinors.push_back(MINORLIST(pMinor->GetRaceID(), pMinor->GetRelation(pOurRace->GetRaceID())));
			}
	}

	// nun den Vektor mit Lieblingsminors sortieren
	std::sort(vFavoriteMinors.begin(), vFavoriteMinors.end());
	// absteigend sortieren
	std::reverse(vFavoriteMinors.begin(), vFavoriteMinors.end());

	// zu 10% wird die Minor an dritter Stelle genommen
	if (vFavoriteMinors.size() >= 3 && rand()%10 == 0)
		m_sFavoriteMinor = vFavoriteMinors[2].sID;
	// zu 33% wird die Minor an zweiter Stelle genommen
	else if (vFavoriteMinors.size() >= 2 && rand()%3 == 0)
		m_sFavoriteMinor = vFavoriteMinors[1].sID;
	else if (!vFavoriteMinors.empty())
		m_sFavoriteMinor = vFavoriteMinors[0].sID;

	MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "Favorite Minorrace of Major: %s is %s\n", m_pRace->GetRaceID(), m_sFavoriteMinor);
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion berechnet, wie eine computergesteuerte Rasse, auf ein Angebot eines Minors reagiert.
/// @param info Information des diplomatischen Angebots
/// @return <code>ACCEPTED</code> f�r Annahme
///			<code>DECLINED</code> f�r Ablehnung
///			<code>NOT_REACTED</code> f�r keine Reaktion
ANSWER_STATUS::Typ CMajorAI::ReactOnMinorOffer(const CDiplomacyInfo& info)
{
	if (info.m_nType == DIPLOMATIC_AGREEMENT::WAR)
		return ANSWER_STATUS::ACCEPTED;

	// Angenommen wird einfach zu 89%, zu 9% wird nicht reagiert und zu 2% abgelehnt
	int nResult = rand()%100;
	if (nResult > 10)
		return ANSWER_STATUS::ACCEPTED;
	if (nResult > 1)
		return ANSWER_STATUS::NOT_REACTED;

	return ANSWER_STATUS::DECLINED;
}

/// Diese Funktion berechnet, wie eine computergesteuerte Rasse, auf ein Angebot eines Majors reagiert.
/// @param info Information des diplomatischen Angebots
/// @return <code>ACCEPTED</code> f�r Annahme
///			<code>DECLINED</code> f�r Ablehnung
///			<code>NOT_REACTED</code> f�r keine Reaktion
ANSWER_STATUS::Typ CMajorAI::ReactOnMajorOffer(const CDiplomacyInfo& info)
{
	CRace* pRace = m_pRace;
	if (!pRace)
		return ANSWER_STATUS::NOT_REACTED;

	// Hier werden auch die Verbesserungen der Beziehung durch Geschenke bzw. deren Verschlechterung
	// wird hier auch behandelt
	if (info.m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
	{
		ReactOnDowry(info);
		return ANSWER_STATUS::ACCEPTED;
	}

	// berechnen ob es klappt
   	short nNeededRelation = 100;
	// Als erstes wird der Wert festgelegt, den wir sp�ter im Algorithmus zuf�llig erreichen m�ssen. Umso h�her dieser
	// Wert ist, umso unwahrscheinlicher wird es, das wir das Angebot annehmen. Dieser zu erreichende Wert ist abh�ngig
	// vom Angebot an uns und von unserer Rasseneigenschaft. Z.B. sind wir die F�deration, somit sind wir Pazifisten,
	// und nehmen Angebote gegen den Krieg eher an als andere Rassen. Kriegsangebote nehmen wir so gut wie gar nicht an.
	if (info.m_nType == DIPLOMATIC_AGREEMENT::NAP)
	{
		nNeededRelation = 35;
		if (pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 30;
		if (pRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			nNeededRelation += 20;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation -= 15;
		if (pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 25;
	}
	else if (info.m_nType == DIPLOMATIC_AGREEMENT::TRADE)
	{
		nNeededRelation = 55;
		if (pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 30;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 20;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 5;
		if (pRace->IsRaceProperty(RACE_PROPERTY::PRODUCER))
			nNeededRelation -= 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 25;
	}
	else if (info.m_nType == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
	{
		nNeededRelation = 70;
		if (pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 20;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 15;
		if (pRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			nNeededRelation += 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 5;
		if (pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 20;
		if (pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 25;
	}
	else if (info.m_nType == DIPLOMATIC_AGREEMENT::DEFENCEPACT)
	{
		nNeededRelation = 70;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 5;
		if (pRace->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
			nNeededRelation -= 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 15;
	}
	else if (info.m_nType == DIPLOMATIC_AGREEMENT::COOPERATION)
	{
		nNeededRelation = 80;
		if (pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 20;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 15;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
			nNeededRelation -= 5;
		if (pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 15;
		if (pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 20;
	}
	else if (info.m_nType == DIPLOMATIC_AGREEMENT::AFFILIATION)
	{
		nNeededRelation = 90;
		if (pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 15;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 5;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 5;
		if (pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 15;
	}
	else if (info.m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
	{
		int nRelationValue = pRace->GetRelation(info.m_sWarpactEnemy);

		// bei einer hinterh�ltigen Rasse gilt die Beziehung nur zu einem Viertel
		if (pRace->IsRaceProperty(RACE_PROPERTY::SNEAKY) && nRelationValue > 0)
			nRelationValue /= 4;

		nNeededRelation = 40 + nRelationValue;

		if (pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation += 40;
		if (pRace->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
			nNeededRelation += 20;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 15;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation += 10;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 5;
		if (pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 5;
		if (pRace->IsRaceProperty(RACE_PROPERTY::SNEAKY))
			nNeededRelation -= 15;
		if (pRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			nNeededRelation -= 25;
		if (pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation -= 40;
	}
	else if (info.m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
		return this->CalcDiplomacyRequest(info);

	// Beziehungsverbesserung durch Mitgifte betachten
	BYTE byOldRelation = pRace->GetRelation(info.m_sFromRace);
	ReactOnDowry(info);

	// Wurde in den letzten 2 Runden von der Majorrasse selbst ein Angebot an einen anderen Major gemacht, dann
	// wird das Angebot angenommen, sofern der Vertrag eine kleiner oder gleich gro�e Wertigkeit hat. Dies betrifft
	// nur "normale" Vertragsangebote
	if (info.m_nType > DIPLOMATIC_AGREEMENT::NONE && info.m_nFlag == DIPLOMACY_OFFER)
	{
		const CDiplomacyInfo* pLastOffer = m_pRace->GetLastOffer(info.m_sFromRace);
		if (pLastOffer != NULL)
			if (info.m_nType <= pLastOffer->m_nType)
				return ANSWER_STATUS::ACCEPTED;
	}

	// Hier werden zus�tzlich noch die Schiffsst�rken der anderen Rassen mit der unseren verglichen. Ist die andere Rasse
	// st�rker, so nehmen wir eher an.
	UINT nOurShipPower		= m_pDoc->GetStatistics()->GetShipPower(info.m_sToRace);
	UINT nTheirShipPower	= m_pDoc->GetStatistics()->GetShipPower(info.m_sFromRace);

	if (nOurShipPower > 0)
	{
		float compare = (float)(nTheirShipPower) / (float)(nOurShipPower);
		if (compare > 1.0f)
		{
			// der Wert, der von nNeededRelation abgezogen wird ist der Vergleichswert * [5,10];
			short nMinus = (short)(compare * (rand()%6 + 5));
			nNeededRelation -= nMinus;
		}
	}

	BYTE byRelation = pRace->GetRelation(info.m_sFromRace);
	double dMulti = (byRelation + 125.0) / 100.0;
	double dValue = byRelation * dMulti;
	short nRandom = (short)dValue;
	nRandom = max(nRandom, 1);
	short nTemp = 0;
	for (int i = 0; i < 5; i++)
		nTemp += rand()%nRandom + 1;
	nRandom = (short)(nTemp / 5);

	/*
	CString s;
	s.Format("Beziehung: %d\nNeededRelation: %d\nMulti: %lf\nforRandom: %d",m_iRelationshipOtherMajor[offerFrom],neededRelation,multi,forRandom);
	AfxMessageBox(s);
	*/

	// Wenn ein Angebot von einer schwachen Rasse kommt und unsere St�rke auch ziemlich schwach ist,
	// so nehmen wir das Angebot mit ziemlicher Wahrscheinlichkeit an. Dies wird gemacht, damit sich schw�chere
	// Rassen nicht untereinander noch mehr schw�chen, sondern sich zusammentun und gemeinsam gegen eine st�rkere Macht
	// k�mpfen.
	if (rand()%2 == NULL)
	{
		// maximale Schiffsst�rke berechnen
		UINT nMaxShipPower = 0;
		map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			UINT nShipPower = m_pDoc->GetStatistics()->GetShipPower(it->first);
			nMaxShipPower = max(nMaxShipPower, nShipPower);
		}

		// Wenn unsere Schiffst�rke und die der Rasse, welche das Angebot gemacht hat kleiner der H�lfte der
		// maximalen Schiffst�rke ist, dann wird das Angebot angenommen
		if (nTheirShipPower < nMaxShipPower * 0.5f)
			if (nOurShipPower < nMaxShipPower * 0.5f)
				nNeededRelation = 0;
	}

	// Wenn der Randomwert h�her gr��er gleich dem Wert ist, den wir erreichen mu�ten, dann wird angenommen
	if (nRandom >= nNeededRelation)
		return ANSWER_STATUS::ACCEPTED;

	// alte Beziehung wiederherstellen (konnte durch Mitgifte zuvor erh�ht werden)
	BYTE byCurrentRelation = pRace->GetRelation(info.m_sFromRace);
	if (byCurrentRelation > byOldRelation)
		pRace->SetRelation(info.m_sFromRace, (short)byOldRelation - (short)byCurrentRelation);

	// sonst wird zu 9/10 abgelehnt
	if (rand()%10 > 0)
		return ANSWER_STATUS::DECLINED;
	// oder dann "nicht reagiert" zur�ckgegeben
	else
		return ANSWER_STATUS::NOT_REACTED;
}

/// Diese Funktion berechnet wie eine Majorrace auf eine Forderung reagiert.
/// @param info Information des diplomatischen Angebots
/// @return <code>ACCEPTED</code> f�r Annahme oder <code>DECLINED</code> bei Ablehnung.
ANSWER_STATUS::Typ CMajorAI::CalcDiplomacyRequest(const CDiplomacyInfo& info)
{
	//Als erstes wird �berpr�ft wie unsere Beziehung zu der Rasse ist und welchen aktuellen Vertrag wir mit ihr
	//aufrechterhalten. Umso besser die Beziehung und umso h�herwertiger der Vertrag ist, desto eher nehmen wir
	//den Vertrag auch an. Wann nehme ich den Vertrag an? -> wenn wir einen bestimmten Prozentsatz mehr als der
	//globale Durchschnitt von der Forderung besitzen. Dieser Prozentsatz ist wie oben beschrieben abh�ngig von
	//der Beziehung und der Wertigkeit des Vertrages. Wenn ich diesen Vertrag nun annehmen w�rde, wird �berpr�ft,
	//auf welchem Planeten ich das meiste der geforderten Ressource habe. Wenn ich die Forderung damit erf�llen
	//k�nnte, wird angenommen. Bei gefordertem Credits brauche ich nicht die Planeten durchgehen.

	// zu 10% reagieren wir gar nicht auf die Forderung
	if (rand()%10 == 0)
		return ANSWER_STATUS::NOT_REACTED;

	CMajor* pRace = dynamic_cast<CMajor*>(m_pRace);
	if (!pRace)
		return ANSWER_STATUS::NOT_REACTED;

	BYTE byRelation = pRace->GetRelation(info.m_sFromRace);
	short nAgreement = pRace->GetAgreement(info.m_sFromRace);

	ANSWER_STATUS::Typ nAnswer = ANSWER_STATUS::ACCEPTED;
	// (14-(unsere Beziehung / 10 + Vertrag)) / 10 , 14 ist das Maximum, was wir erreichen k�nnten
	double dValue = (double)(14 - ((double)byRelation / 10 + nAgreement)) / 10;

	// Hier werden zus�tzlich noch die Schiffsst�rken der anderen Rassen mit der unseren verglichen. Ist die andere Rasse
	// st�rker, so nehmen wir eher an.
	UINT nOurShipPower		= m_pDoc->GetStatistics()->GetShipPower(info.m_sToRace);
	UINT nTheirShipPower	= m_pDoc->GetStatistics()->GetShipPower(info.m_sFromRace);
	if (nOurShipPower > 0)
	{
		float fCompare = (float)(nTheirShipPower) / (float)(nOurShipPower);
		if (fCompare > 1.0f)
			dValue -= fCompare;
		else
			dValue += fCompare;
	}

	dValue = max(dValue, 0);
	// umso niedriger Value ist, desto eher wird angenommen
	dValue += 1;

	// Jetzt haben wir den prozentulalen Wert (dValue) den wir brauchen, wenn Credits (mit)gefordert werden
	if (info.m_nCredits > 0)
	{
		// Durchschnittscredits berechnen
		long lCredits = 0;
		short nRaces = 0;

		map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			CMajor* pMajor = it->second;
			lCredits += pMajor->GetEmpire()->GetCredits();
			nRaces++;
		}

		if (nRaces)
			lCredits /= nRaces;

		// schauen ob wir den prozentualen Mehrwert der Durchschnittscredits besitzen
		if (pRace->GetEmpire()->GetCredits() > (lCredits * dValue))
			nAnswer = ANSWER_STATUS::ACCEPTED;
		else
			nAnswer = ANSWER_STATUS::DECLINED;
	}

	// Wenn Ressourcen (mit)gefordert werden
	if (nAnswer == ANSWER_STATUS::ACCEPTED)
	{
		short nRes = -1;
		if (info.m_nResources[TITAN])
			nRes = TITAN;
		else if (info.m_nResources[DEUTERIUM])
			nRes = DEUTERIUM;
		else if (info.m_nResources[DURANIUM])
			nRes = DURANIUM;
		else if (info.m_nResources[CRYSTAL])
			nRes = CRYSTAL;
		else if (info.m_nResources[IRIDIUM])
			nRes = IRIDIUM;
		else if (info.m_nResources[DERITIUM])
			nRes = DERITIUM;

		if (nRes != -1)
		{
			if (pRace->GetEmpire()->GetStorage()[nRes] > (m_pDoc->GetStatistics()->GetAverageResourceStorages()[nRes] * dValue))
			{
				// Wenn jetzt noch "answer" auf TRUE steht, dann w�rden wir die Forderung annehmen. Jetzt mu� ich noch
				// die Systeme durchgehen und schauen auf welchem ich das meiste dieser Ressource gelagert habe.
				// Dort ziehe ich es dann wenn m�glich ab. Falls es nicht geht, dann stelle ich answer auf FALSE.
				UINT nMaxRes = 0;
				CPoint ptSystem(-1,-1);
				for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
					for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
						if (m_pDoc->GetSystem(x,y).GetOwnerOfSystem() == info.m_sToRace)
						{
							nMaxRes = max(nMaxRes, m_pDoc->GetSystem(x,y).GetResourceStore(nRes));
							ptSystem.x = x;
							ptSystem.y = y;
						}

				// Ressource wenn m�glich abziehen
				if (ptSystem != CPoint(-1,-1) && nMaxRes >= info.m_nResources[nRes])
				{
					//CString s;
					//s.Format("Request from: %d an %d\nunsere Beziehung zu denen: %d\nunser Status: %d\nvalue: %lf\ngeforderte Res: %d Menge: %d\naus System: %d/%d\ndavon vorhanden: %d",requestFrom,m_iRaceNumber,m_iRelationshipOtherMajor[requestFrom],m_iDiplomacyStatus[requestFrom],value,res,requestedRessource[res],system.x,system.y,systems.at(system.x+(system.y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(res));
					//AfxMessageBox(s);
					m_pDoc->GetSystem(ptSystem.x, ptSystem.y).SetResourceStore(nRes, -info.m_nResources[nRes]);
				}
				else nAnswer = ANSWER_STATUS::DECLINED;
			}
			else
				nAnswer = ANSWER_STATUS::DECLINED;
		}
	}
	// Wenn ich, falls gefordert, die Ressourcen dazugebe und auch Credits gefordert sind und dieses auch geben
	// k�nnte, dann mu� ich dieses noch von meinem Credits abziehen
	if (nAnswer == ANSWER_STATUS::ACCEPTED && info.m_nCredits > 0)
	{
		//CString s;
		//s.Format("Request from: %d an %d\nunsere Beziehung zu denen: %d\nunser Status: %d\nvalue: %lf\ngefordertes Credits: %d\ndavon bei uns vorhanden: %d\nForderer besitzt: %d",requestFrom,m_iRaceNumber,m_iRelationshipOtherMajor[requestFrom],m_iDiplomacyStatus[requestFrom],value,requestedCredits,empire[m_iRaceNumber].GetCredits(),empire[requestFrom].GetCredits());
		//AfxMessageBox(s);
		pRace->GetEmpire()->SetCredits(-info.m_nCredits);
	}

	return nAnswer;
}

/// Funktion zur Erstellung eines diplomatischen Angebots an einen Minor.
/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
bool CMajorAI::MakeMinorOffer(const CString& sRaceID, CDiplomacyInfo& info)
{
	CMajor* pOurRace = dynamic_cast<CMajor*>(m_pRace);
	if (!pOurRace)
		return false;

	CMinor* pMinor = dynamic_cast<CMinor*>(m_pDoc->GetRaceCtrl()->GetRace(sRaceID));
	if (!pMinor)
		return false;

	DIPLOMATIC_AGREEMENT::Typ nOffer = DIPLOMATIC_AGREEMENT::NONE;

	if (pOurRace->IsRaceContacted(sRaceID) && !pMinor->GetSubjugated())
	{
		BYTE byTheirRelationToUs = pMinor->GetRelation(pOurRace->GetRaceID());
		DIPLOMATIC_AGREEMENT::Typ nAgreement = pOurRace->GetAgreement(sRaceID);

		// Wenn unsere Beziehung hoch genug ist und wir den jeweiligen Vertrag noch nicht haben, so machen wir mit einer
		// bestimmten Wahrscheinlichkeit ein Angebot. Wenn wir ein Angebot machen k�nnten, aber dies nicht aufgrund von
		// Vertr�gen mit anderen Majorraces geht, dann versuchen wir die Minorrace zu bestechen, wenn es unsere Favorit
		// Minorrace ist und die Beziehung zu ihr auch hoch genug ist
		DIPLOMATIC_AGREEMENT::Typ nOthersAgreement = DIPLOMATIC_AGREEMENT::NONE;
		CString sCorruptedMajor = "";

		// wenn nur Aliendiplomatie m�glich ist, dann darf nur Krieg oder Freundschaft angeboten werden
		bool bIsAlienDiplomacy = false;
		if (pOurRace->HasSpecialAbility(SPECIAL_ALIEN_DIPLOMACY) || pMinor->HasSpecialAbility(SPECIAL_ALIEN_DIPLOMACY))
			bIsAlienDiplomacy = true;

		map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			DIPLOMATIC_AGREEMENT::Typ nTemp = pMinor->GetAgreement(it->first);
			if (pOurRace->GetRaceID() != it->first && nTemp > nOthersAgreement)
			{
				nOthersAgreement	= nTemp;
				sCorruptedMajor		= it->first;	// Bestochen werden sollte, dann is dies hier auch gleich die Majorrace
			}
		}

		if (byTheirRelationToUs > 90 && nAgreement < DIPLOMATIC_AGREEMENT::MEMBERSHIP && nOthersAgreement < DIPLOMATIC_AGREEMENT::COOPERATION)
			nOffer = DIPLOMATIC_AGREEMENT::MEMBERSHIP;
		else if (byTheirRelationToUs > 85 && nAgreement < DIPLOMATIC_AGREEMENT::AFFILIATION && nOthersAgreement < DIPLOMATIC_AGREEMENT::COOPERATION)
			nOffer = DIPLOMATIC_AGREEMENT::AFFILIATION;
		else if (byTheirRelationToUs > 80 && nAgreement < DIPLOMATIC_AGREEMENT::COOPERATION && nOthersAgreement < DIPLOMATIC_AGREEMENT::COOPERATION)
			nOffer = DIPLOMATIC_AGREEMENT::COOPERATION;
		else if (byTheirRelationToUs > 70 && nAgreement < DIPLOMATIC_AGREEMENT::FRIENDSHIP && nOthersAgreement < DIPLOMATIC_AGREEMENT::AFFILIATION)
			nOffer = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
		else if (byTheirRelationToUs > 55 && nAgreement < DIPLOMATIC_AGREEMENT::TRADE && nOthersAgreement < DIPLOMATIC_AGREEMENT::MEMBERSHIP)
			nOffer = DIPLOMATIC_AGREEMENT::TRADE;

		// Das k�nnte man noch von der Art der Rasse abh�ngig machen, kriegerische Rassen erkl�ren eher Krieg,
		// pazifistische Rasse erkl�ren gar keinen Krieg oder so �hnlich -> hier gemacht
		// Auch noch �berpr�fen ob die Minorrace ein B�ndnis hat, weil wenn wir zu der verb�ndeten Majorrace
		// eine relativ gute Beziehung haben, dann erkl�ren wir keinen Krieg.
		if (nOthersAgreement < DIPLOMATIC_AGREEMENT::MEMBERSHIP && nOffer == DIPLOMATIC_AGREEMENT::NONE)
		{
			short nMinRel = 10;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))		// Wir lieben Krieg
				nMinRel += 25;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))		// Wir lieben Krieg
				nMinRel += 15;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::SNEAKY))		// Wir sind eh hinterh�ltig
				nMinRel += 5;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))		// Krieg ist nicht immer gut f�rs Gesch�ft
				nMinRel -= 5;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))	// Krieg ist nicht immer gut f�r Forschung
				nMinRel -= 5;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))		// Wir hassen Krieg
				nMinRel -= 10;

			if (byTheirRelationToUs < nMinRel && nAgreement != DIPLOMATIC_AGREEMENT::WAR)
				nOffer = DIPLOMATIC_AGREEMENT::WAR;

			// Nun checken das ich durch irgendeinen Pakt nicht nem Freund den Krieg miterkl�ren w�rde
			if (nOffer == DIPLOMATIC_AGREEMENT::WAR)
			{
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
					// w�rden der anderen Rasse sonst auch den Krieg erkl�ren
					if (it->first != pOurRace->GetRaceID() && pMinor->GetAgreement(it->first) == DIPLOMATIC_AGREEMENT::AFFILIATION)
						// Wenn wir die andere Majorrasse relativ gut leiden k�nnen, dann wird kein Krieg erkl�rt
						if (pOurRace->GetRelation(it->first) > rand()%21 + 40)
						{
							nOffer = DIPLOMATIC_AGREEMENT::NONE;
							break;
						}
			}
		}

		// wichtige Parameter des Infoobjektes zuweisen
		info.m_nFlag = DIPLOMACY_OFFER;
		info.m_sFromRace = pOurRace->GetRaceID();
		info.m_sToRace = sRaceID;
		info.m_nSendRound = m_pDoc->GetCurrentRound() - 1;

		if (!bIsAlienDiplomacy)
		{
			// Nun haben wir den Vertrag, den wir anbieten m�chten.
			// Jetzt muss bestimmt werden, ob die Rasse zu den Lieblingsrassen unsererseits z�hlt.
			// Dadurch soll sich beim Mitgift- und Geschenkegeben auf wenige Rassen konzentriert werden.
			bool bIsFavorite = (pMinor->GetRaceID() == m_sFavoriteMinor) ? true : false;

			// Wenn es sich bei der Minorrace um unsere FavoriteRace handelt, dann versuchen wir ihr
			// auch Mitgifte zum Vertrag zu geben.

			// sollen zum Vertragangebot noch Mitgifte gegeben werden?
			if (!bIsAlienDiplomacy && bIsFavorite && nOffer > DIPLOMATIC_AGREEMENT::NONE)
				this->GiveDowry(info);

			// Wenn wir kein Angebot an unsere Favoritrace gemacht haben bzw. wir konnten keins machen, dann geben wir
			// m�glicherweise ein Geschenk oder wir Bestechen diese Rasse.
			if (!bIsAlienDiplomacy && bIsFavorite && nOffer == DIPLOMATIC_AGREEMENT::NONE && byTheirRelationToUs <= 91)
				if (this->GiveDowry(info))
					nOffer = DIPLOMATIC_AGREEMENT::PRESENT;

			// Wenn wir der Favoritminor kein Geschenk gemacht haben, dann Bestechen wir diese wom�glich
			if (bIsFavorite && nOffer == DIPLOMATIC_AGREEMENT::NONE)
			{
				// Wir k�nnen nur bestechen, wenn wir mindst. Freundschaft mit der Minorrace haben oder diese Minorrace
				// hat mit irgendeiner anderen Rasse mindst. ein B�ndnis
				if (nOthersAgreement >= DIPLOMATIC_AGREEMENT::AFFILIATION)
				{
					// Nun schauen ob wir die Bestechung durchf�hren kann
					if (this->GiveDowry(info))
					{
						nOffer = DIPLOMATIC_AGREEMENT::CORRUPTION;
						info.m_sCorruptedRace = sCorruptedMajor;
					}
				}
				else if (pMinor->GetAgreement(pOurRace->GetRaceID()) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP
					&& pMinor->GetAgreement(pOurRace->GetRaceID()) != DIPLOMATIC_AGREEMENT::MEMBERSHIP
					&& nOthersAgreement > DIPLOMATIC_AGREEMENT::NONE)
				{
					// hier m�ssen wir noch den Gegner der Bestechung w�hlen
					// dieser wird zuf�llig unter den vorhandenen Majors ausgew�hlt
					vector<CString> vCorrupted;
					for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
						// wir nicht selbst
						if (it->first != pOurRace->GetRaceID() && pMinor->GetAgreement(it->first) > DIPLOMATIC_AGREEMENT::NONE)
							vCorrupted.push_back(it->first);
					// zuf�llige Auswahl
					if (!vCorrupted.empty())
					{
						sCorruptedMajor = vCorrupted[rand()%vCorrupted.size()];
						// Nun schauen ob wir die Bestechung durchf�hren kann
						if (this->GiveDowry(info))
						{
							nOffer = DIPLOMATIC_AGREEMENT::CORRUPTION;
							info.m_sCorruptedRace = sCorruptedMajor;
						}
					}
				}
			}
		}
		else
		{
			// wenn nur Aliendiplomatie m�glich ist, dann darf nur Krieg oder Freundschaft angeboten werden
			if (nOffer >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
			{
				if (nAgreement < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					nOffer = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
				else
					nOffer = DIPLOMATIC_AGREEMENT::NONE;
			}

			// Wenn nicht Krieg und auch nicht Freundschaft angeboten wurde (z.B. Handelsvertrag),
			// dann kein Angebot machen
			if (nOffer != DIPLOMATIC_AGREEMENT::WAR && nOffer != DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				nOffer = DIPLOMATIC_AGREEMENT::NONE;
		}
	}

	if (nOffer != DIPLOMATIC_AGREEMENT::NONE)
	{
		info.m_nType = nOffer;
		CGenDiploMessage::GenerateMajorOffer(info);
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "Major: %s makes offer %d to Minor %s\n", info.m_sFromRace, info.m_nType, info.m_sToRace);
		return true;
	}
	else
		return false;
}

/// Funktion zur Erstellung eines diplomatischen Angebots an einen Major.
/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
bool CMajorAI::MakeMajorOffer(CString& sRaceID, CDiplomacyInfo& info)
{
	// K�nnen ja nur ein Angebot machen, wenn wir die Rasse kennen
	// Wir machen auch nur ein Angebot, wenn wir die Rasse gut leiden k�nnen und die Rasse, an die das Angebot geht
	// auch uns gut leiden kann. Die Differenz der beiden Werte darf nicht zu gro� sein.
	CMajor* pOurRace = dynamic_cast<CMajor*>(m_pRace);
	if (!pOurRace)
		return false;

	CMajor* pTheirRace = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sRaceID));
	if (!pTheirRace)
		return false;

	if (pOurRace->IsRaceContacted(sRaceID))
	{
		BYTE byOurRelationToThem = pOurRace->GetRelation(sRaceID);
		BYTE byTheirRelationToUs = pTheirRace->GetRelation(pOurRace->GetRaceID());
		DIPLOMATIC_AGREEMENT::Typ nAgreement = pOurRace->GetAgreement(sRaceID);

		// jetzt zum Algorithmus:
		// Aufgrund der Beziehung zur Majorrace machen wir ihr vielleicht ein diplomatisches Angebot
		double dMulti = (byOurRelationToThem + 100) / 100;
		double dValue = byOurRelationToThem * dMulti;
		int nRandom = (int)dValue;
		nRandom = max(nRandom, 1);
		int nTemp = 0;
		for (int i = 0; i < 3; i++)
			nTemp += rand()%nRandom + 1;
		nRandom = (int)(nTemp / 3);

		// Hier werden zus�tzlich noch die Schiffsst�rken der anderen Rassen mit der unseren verglichen.
		// Ist die andere Rasse st�rker, so nehmen wir eher an.
		UINT nOurShipPower		= m_pDoc->GetStatistics()->GetShipPower(pOurRace->GetRaceID());
		UINT nTheirShipPower	= m_pDoc->GetStatistics()->GetShipPower(sRaceID);

		float fCompare = 0.0f;
		if (nOurShipPower)
		{
			fCompare = (float)nTheirShipPower / (float)nOurShipPower;
			if (fCompare > 1.0f)
				nRandom += (int)(fCompare * 5);
			else if ((nRandom - (int)fCompare * 5) > 0)
				nRandom -= (int)fCompare * 5;
			else
				nRandom = 0;
		}

		// wenn nur Aliendiplomatie m�glich ist, dann darf nur Krieg, Freundschaft oder NAP angeboten werden
		bool bIsAlienDiplomacy = false;
		if (pOurRace->HasSpecialAbility(SPECIAL_ALIEN_DIPLOMACY) || pTheirRace->HasSpecialAbility(SPECIAL_ALIEN_DIPLOMACY))
			bIsAlienDiplomacy = true;

		// Jetzt haben wir nen Randomwert, wenn dieser einen zuf�lligen Wert �berschreitet, bietet die
		// Majorrace zu 33% etwas an, dieser zuf�llige Wert ist wiederrum von unserer Rasseneigenschaft abh�ngig
		DIPLOMATIC_AGREEMENT::Typ nOffer = DIPLOMATIC_AGREEMENT::NONE;
		// nur zu 33% wird irgendein Vertrag auch angeboten.
		// Es soll nicht jede Runde ein Vertragsangebot gemacht werden.
		if (rand()%3 == 0)
		{
			if (nRandom > this->GetMinOfferValue(DIPLOMATIC_AGREEMENT::AFFILIATION) && nAgreement < DIPLOMATIC_AGREEMENT::AFFILIATION)
				nOffer = DIPLOMATIC_AGREEMENT::AFFILIATION;
			else if (nRandom > this->GetMinOfferValue(DIPLOMATIC_AGREEMENT::COOPERATION) && nAgreement < DIPLOMATIC_AGREEMENT::COOPERATION)
				nOffer = DIPLOMATIC_AGREEMENT::COOPERATION;
			else if (nRandom > this->GetMinOfferValue(DIPLOMATIC_AGREEMENT::DEFENCEPACT) && pOurRace->GetDefencePact(sRaceID) == false
				&& nAgreement != DIPLOMATIC_AGREEMENT::WAR && nAgreement != DIPLOMATIC_AGREEMENT::AFFILIATION)
				nOffer = DIPLOMATIC_AGREEMENT::DEFENCEPACT;
			else if (nRandom > this->GetMinOfferValue(DIPLOMATIC_AGREEMENT::FRIENDSHIP) && nAgreement < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				nOffer = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
			else if (nRandom > this->GetMinOfferValue(DIPLOMATIC_AGREEMENT::TRADE) && nAgreement < DIPLOMATIC_AGREEMENT::TRADE)
				nOffer = DIPLOMATIC_AGREEMENT::TRADE;
			else if (nRandom > this->GetMinOfferValue(DIPLOMATIC_AGREEMENT::NAP) && nAgreement < DIPLOMATIC_AGREEMENT::NAP)
				nOffer = DIPLOMATIC_AGREEMENT::NAP;
			// Sicherheitsabfrage
			if (nAgreement >= nOffer && nOffer != DIPLOMATIC_AGREEMENT::DEFENCEPACT) // weil DEFENCE_PACT negativ ist
				nOffer = DIPLOMATIC_AGREEMENT::NONE;

			//MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "Major: nOffer: %s\n", nOffer);

			// Hier jetzt schauen, dass die Beziehungen nicht so weit auseinanderliegen. Ein menschlicher Spieler w�rde
			// einem anderen ja auch nix anbieten, wenn er diesen leiden kann aber der andere ihn nicht leiden kann und
			// daher wahrscheinlich auch ablehnen wird.
			// Umso h�her meine Beziehung ist, umso mehr darf seine abweichen. Umso h�herwertig der Vertrag ist, umso weniger
			// darf sie abweichen
			// (double)(byOurRelationToThem) / 100				// Indikator f�r bessere Beziehung
			// abs(byTheirRelationToUs - byOurRelationToThem)	// Abweichung
			// 40 + nMod - abs(nOffer * 5) * Indikator			// soweit darf sie abweichen, 40 ist ein Erfahrungswert ;-)
			// nMod												// wenn wir milit�risch schw�cher sind kann die Beziehung weiter abweichen
			int nMod = 0;
			if (fCompare > 1.0f)
				nMod = (int)(fCompare * 5);
			if (abs(byTheirRelationToUs - byOurRelationToThem) > (double)(40 + nMod - abs(nOffer * 5)) * (double)(byOurRelationToThem) / 100)
			{
				//MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "byTheirRelationToUs: %s, byOurRelationToThem: %s\n", byTheirRelationToUs, byOurRelationToThem);
				//if (nOffer != 0)
				//{
				//	double temp = (double)(40 - abs(nOffer*5)) * (double)(byOurRelationToThem)/100;
				//	CString h;
				//	h.Format("Abweichung: %d\nVergleichswert: %lf\nWir %d - an %d",abs(relation-m_iRelationshipOtherMajor[race]),
				//		temp,m_iRaceNumber,race);
				//	AfxMessageBox(h);
				//}
				nOffer = DIPLOMATIC_AGREEMENT::NONE;
			}

			// Das k�nnte man noch von der Art der Rasse abh�ngig machen, kriegerische Rassen erkl�ren eher Krieg,
			// pazifistische Rasse erkl�ren gar keinen Krieg oder so �hnlich -> hier gemacht
			short nMinRel = 15;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))		// Wir lieben Krieg
				nMinRel += 25;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))		// Wir lieben Krieg
				nMinRel += 15;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::SNEAKY))		// Wir sind eh hinterh�ltig
				nMinRel += 5;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))		// Krieg ist nicht immer gut f�rs Gesch�ft
				nMinRel -= 5;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))	// Krieg ist nicht immer gut f�r Forschung
				nMinRel -= 10;
			if (pOurRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))		// Wir hassen Krieg
				nMinRel -= 15;

			float fModi = 0.0f;
			if (nTheirShipPower)
				fModi = (float)nOurShipPower / (float)nTheirShipPower;
			if (fModi > 1.0)
				fModi *= 3.0f;

			// wir sind st�rker als der Gegner
			if (fCompare < 1.0f)
				nMinRel += (short)fModi;	// somit wird auch Krieg erkl�rt, wenn die Beziehung eigentlich noch besser ist.
			if (fModi > 1.0 && nRandom + abs(nAgreement * 2) < nMinRel && byOurRelationToThem + abs(nAgreement * 2) < nMinRel && nAgreement != DIPLOMATIC_AGREEMENT::WAR)
				nOffer = DIPLOMATIC_AGREEMENT::WAR;

			// Hier m�glicherweise Angebot eines Kriegspaktes
			// das geht nat�rlich nur, wenn wir noch keinen Krieg mit der Rasse haben. Es ist aber m�glich, wenn wir
			// eine Runde zuvor einen Kriegspakt angeboten haben, der sp�ter angenommen wird und wir auch so der Rasse
			// den Krieg erkl�ren, das wir ihr eigentlich 2x den Krieg erkl�ren. Ist nicht so wichtig, sieht nur in der
			// Nachrichten�bersicht nicht so sch�n aus
			// wenn ich der Rasse den Krieg erkl�ren w�rde, dann suche ich mir m�glicherweise auch noch einen Kriegspaktpartner
			if (!bIsAlienDiplomacy && nOffer == DIPLOMATIC_AGREEMENT::WAR && rand()%4 > 0)	// zu 75% wird versucht ein Kriegspakt anzubieten
			{
				CString sLikeBest	= "";		// beste Beziehung von uns zu
				CString sLikeSecond = "";		// zweitbeste Beziehung von uns zu
				BYTE byTemp = 0;
				map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
					// nicht wir selbst und nicht die Rasse an die Krieg erkl�rt werden soll
					if (it->first != pOurRace->GetRaceID() && it->first != sRaceID)
						// Kriegspaktpartner muss uns und dem Kriegsgegner bekannt sein
						if (it->second->IsRaceContacted(pOurRace->GetRaceID()) && it->second->IsRaceContacted(sRaceID))
						{
							BYTE byRel = pOurRace->GetRelation(it->first);
							if (byRel > byTemp && byRel > rand()%50)
							{
								// Nur wenn wir noch keine Angebote an diese Rasse gemacht haben
								bool bOffer = false;
								for (UINT i = 0; i < m_pRace->GetOutgoingDiplomacyNews()->size(); i++)
									if (m_pRace->GetOutgoingDiplomacyNews()->at(i).m_nFlag == DIPLOMACY_OFFER && m_pRace->GetOutgoingDiplomacyNews()->at(i).m_sToRace == it->first && m_pRace->GetOutgoingDiplomacyNews()->at(i).m_sFromRace == m_pRace->GetRaceID())
									{
										bOffer = true;
										break;
									}
								if (bOffer)
									continue;

								byTemp = byRel;
								if (sLikeBest != "")
									sLikeSecond = sLikeBest;
								sLikeBest = it->first;
							}
						}

				CString sWarpactPartner = "";
				// zwischen besten und zweitbesten entscheiden
				if (sLikeBest != "" && sLikeSecond != "")
					sWarpactPartner = (rand()%3 > 0) ? sLikeBest : sLikeSecond;
				else if (sLikeBest != "")
					sWarpactPartner = sLikeBest;

				if (sWarpactPartner != "")
				{
					// Sicherheitsabfrage:
					// exisitiert der Kriegspaktgegner noch?
					CMajor* pPartner = dynamic_cast<CMajor*>(m_pDoc->GetRaceCtrl()->GetRace(sWarpactPartner));
					if (pPartner && pPartner->GetEmpire()->CountSystems() > 0)
					{
						info.m_sWarpactEnemy = sRaceID;
						// Nun �ndert sich die Rassen-ID, an die das Angebot geht. Es geht keine direkte
						// Kriegserkl�rung an die Rasse raus, sondern ein Kriegspaktvorschlag an den Partner
						sRaceID = sWarpactPartner;
						nTheirShipPower	= m_pDoc->GetStatistics()->GetShipPower(sRaceID);
						nAgreement = pOurRace->GetAgreement(sRaceID);
						nOffer = DIPLOMATIC_AGREEMENT::WARPACT;
					}
				}
			}
		}

		// Wenn bis jetzt noch kein Angebot gemacht wurde, dann wird �berpr�ft, ob es Sinn macht, sich gegen ein st�rkeres
		// fremdes Imperium mit einem anderen Partner zu verb�nden bzw. dem anderen Partner eine Freundschaft oder
		// Kooperation anzubieten. Dies hat den Vorteil, dass sich relativ schwache Imperien nicht auch noch gegenseitig
		// schw�chen, sondern sich eher verb�nden. In einem Lategame k�nnte z.B. eine Rasse so stark wie alle anderen Rassen
		// zusammen sein, dann sollten sich die anderen Rassen verb�nden und nicht selbst noch schw�chen.
		if (!bIsAlienDiplomacy && nOffer == DIPLOMATIC_AGREEMENT::NONE && nAgreement < DIPLOMATIC_AGREEMENT::FRIENDSHIP && rand()%5 == 0)
		{
			// maximale Schiffsst�rke berechnen
			UINT nMaxShipPower = 0;
			map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			{
				UINT nShipPower = m_pDoc->GetStatistics()->GetShipPower(it->first);
				nMaxShipPower = max(nMaxShipPower, nShipPower);
			}

			// Wenn unsere Schiffst�rke kleiner einem Drittel der maximalen Schiffst�rke ist und die
			// maximale Schiffsst�rke nicht zu der Rasse geh�rt, welcher wir hier ein Angebot machen wollen
			if (nMaxShipPower != nTheirShipPower)
				if (nOurShipPower < nMaxShipPower * 0.33f)
				{
					// Freundschaft oder Kooperation anbieten
					if (rand()%2 == NULL)
						nOffer = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
					else
						nOffer = DIPLOMATIC_AGREEMENT::COOPERATION;
				}
		}

		// wichte Parameter des Infoobjekts zuweisen
		info.m_nFlag = DIPLOMACY_OFFER;
		info.m_sFromRace = pOurRace->GetRaceID();
		info.m_sToRace = sRaceID;
		info.m_nSendRound = m_pDoc->GetCurrentRound() - 1;

		if (!bIsAlienDiplomacy)
		{
			// Ab hier kommt noch was gro�es dazu, n�mlich ob und wann eine computergesteuerte Rasse auch Mitgifte macht.
			// Credits kann sie ja immer dazugeben, aber Ressourcen geht nur, wenn sie mindst. einen Handelsvertrag hat.
			// Au�erdem k�nnte die Rasse ja auch ein Geschenk geben oder eine Forderung stellen.
			if (nOffer != DIPLOMATIC_AGREEMENT::NONE)
			{
				//	CString s;
				//	s.Format("Angebot: %d\nvon: %s\nan: %s",offer,CMajorRace::GetRaceName(m_iRaceNumber),CMajorRace::GetRaceName(race));
				//	AfxMessageBox(s);

				// Jetzt muss die Dauer des Vertrages festgelegt werden. Umso "h�herwertiger" ein Vertrag ist, desto
				// wahrscheinlicher sollte eine l�ngere Vertragsdauer auch sein. Das hei�t, wir holen uns eine Zufallszahl
				// von 0 bis 10 und addieren darauf den Betrag des Wertes eines Vertrages. Alles was gr��er als 10 ist wird
				// auf 0 gesetzt. 0 bedeutet unbegrenzt.
				if (nOffer != DIPLOMATIC_AGREEMENT::WAR && nOffer != DIPLOMATIC_AGREEMENT::WARPACT)
				{
					short nDuration = rand()%(11 + abs(nOffer));
					if (nDuration > 10)
						nDuration = 0;
					// hier Dauer des Vertrages festlegen
					info.m_nDuration = nDuration * 10;
				}

				info.m_nType = nOffer;
				if (nOffer != DIPLOMATIC_AGREEMENT::WAR)
					this->GiveDowry(info);
			}
			// wenn kein Angebot gemacht werden w�rde, so gibt die KI mit sehr geringer Wahrscheinlichkeit
			// auch Geschenke. Diese werden zur Zeit nicht durch einen bestimmten Grund gegeben,
			// sondern nur sehr unwahrscheinlich.
			// Zu 3.33% wird wenn nix angeboten m�glicherweise ein Geschenk gemacht.
			else if (rand()%30 == 0 && nAgreement != DIPLOMATIC_AGREEMENT::WAR)
			{
				// Geschenk machen
				if (GiveDowry(info))
					info.m_nType = DIPLOMATIC_AGREEMENT::PRESENT;
			}
			// wenn kein Angebot und kein Geschenk gemacht wird, dann wird vielleicht eine Forderung gestellt.
			else if (nAgreement != DIPLOMATIC_AGREEMENT::WAR)
			{
				// Der Modifikator ausgehend von den Schiffsst�rken ver�ndert die Wahrscheinlichkeit f�r eine Forderung.
				// Umso st�rker unsere Schiffe sind, desto eher wird auch eine Forderung gestellt. Hier werden zwei Zufalls-
				// variablen bestimmt, weil mit einer das Risiko trotz eines hohen modi-Werts eine Forderung zu stellen zu
				// gro� ist.
				float fModi = 0.0f;
				if (nTheirShipPower)
					fModi = (float)nOurShipPower / (float)nTheirShipPower;
				// old style
				/*fModi *= 2.0f;
				fModi = max(fModi, 1.0f);

				int temp = (int)(100 / fModi);
				temp = max(temp, 1);

				int a = rand()%temp;
				int b = rand()%temp;
				if (a < 2 && b < 7)
				{
				*/

				// new
				fModi *= 4.0f;
				int a = rand()%100;
				int b = rand()%100;
				if (a < 2 + fModi && b < 7 + fModi)
				{
					MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "REQUEST Calculation: rand: %d (a=(random)%d), (b=(random)%d), fModi=%lf ((we=%s: ShipPower (%d) / Enemy=%s: ShipPower(%d)) *4)\n",((int)(100 / fModi)), a, b, fModi, pOurRace->GetRaceID(), nOurShipPower, sRaceID, nTheirShipPower);
					MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "REQUEST Calculation-Formula: if a < (2+fModi) AND b < (7+fModi) then REQUEST");
					// Forderung stellen
					if (ClaimRequest(info))
						info.m_nType = DIPLOMATIC_AGREEMENT::REQUEST;
				}
			}
		}
		else if (nOffer != DIPLOMATIC_AGREEMENT::NONE)
		{
			// wenn nur Aliendiplomatie m�glich ist, dann darf nur Krieg, Freundschaft oder NAP angeboten werden
			if (nOffer >= DIPLOMATIC_AGREEMENT::FRIENDSHIP && nAgreement < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				nOffer = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
			else if (nOffer == DIPLOMATIC_AGREEMENT::NAP && nAgreement < DIPLOMATIC_AGREEMENT::NAP)
				nOffer = DIPLOMATIC_AGREEMENT::NAP;
			else if (nOffer == DIPLOMATIC_AGREEMENT::DEFENCEPACT)
				nOffer = DIPLOMATIC_AGREEMENT::NONE;
			else if (nOffer != DIPLOMATIC_AGREEMENT::WAR)
				nOffer = DIPLOMATIC_AGREEMENT::NONE;

			info.m_nType = nOffer;
		}
	}

	if (info.m_nType != DIPLOMATIC_AGREEMENT::NONE)
	{
		CGenDiploMessage::GenerateMajorOffer(info);
		MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "Major: %s makes offer %d to Major %s\n", info.m_sFromRace, info.m_nType, info.m_sToRace);
		return true;
	}
	else
		return false;
}

/// Funktion gibt den ben�tigten Wert zur�ck, der erreicht werden muss, damit diese Rasse
/// das diplomatische Angebot abgibt. Der Wert ist abh�ngig von den Rasseneigenschaften.
/// @param nOfferType Typ des Angebots
/// @return ben�tigter Wert
int CMajorAI::GetMinOfferValue(DIPLOMATIC_AGREEMENT::Typ nOfferType) const
{
	int nNeededRelation = INT_MAX;

	switch (nOfferType)
	{
	// Nichtangriffspakt
	case DIPLOMATIC_AGREEMENT::NAP:
		nNeededRelation = 50;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 50;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			nNeededRelation += 20;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation -= 15;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 25;
		break;
	// Handelsvertrag
	case DIPLOMATIC_AGREEMENT::TRADE:
		nNeededRelation = 70;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 40;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 15;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::PRODUCER))
			nNeededRelation -= 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 20;
		break;
	// Freundschaftsvertrag
	case DIPLOMATIC_AGREEMENT::FRIENDSHIP:
		nNeededRelation = 80;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 30;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 15;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			nNeededRelation += 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 20;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 30;
		break;
	// Verteidigungspakt
	case DIPLOMATIC_AGREEMENT::DEFENCEPACT:
		nNeededRelation = 100;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 20;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
			nNeededRelation -= 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 15;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 20;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 25;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation -= 30;
		break;
	// Kooperation
	case DIPLOMATIC_AGREEMENT::COOPERATION:
		nNeededRelation = 115;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 30;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 5;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 20;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 25;
		break;
	// B�ndnis
	case DIPLOMATIC_AGREEMENT::AFFILIATION:
		nNeededRelation = 145;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
			nNeededRelation += 15;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
			nNeededRelation += 10;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			nNeededRelation += 5;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
			nNeededRelation -= 5;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
			nNeededRelation -= 15;
		if (m_pRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			nNeededRelation -= 25;
		break;
	}

	return nNeededRelation;
}

/// Funktion berechnet wieviel und welche Mitgifte die KI bei einem Angebot mitgibt.
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @return <code>true</code> wenn eine Mitgift gegeben wurde, ansonsten <code>false</code>
bool CMajorAI::GiveDowry(CDiplomacyInfo& info)
{
	bool bGiveDowry = false;	// wurde ein Geschenk gemacht

	CMajor* pOurRace = dynamic_cast<CMajor*>(m_pRace);
	if (!pOurRace)
		return false;

	CRace* pToRace = m_pDoc->GetRaceCtrl()->GetRace(info.m_sToRace);
	if (!pToRace)
		return false;


	// Als erstes gibt die KI-gesteuerte Rasse nur Geld, wenn sie mindst. 10% mehr als von dem globalen
	// Durchschnittswert hat.
	// Dies k�nnte man sp�ter vielleicht nochmal �ndern, aber erstmal f�llt mir da kein besserer Algo ein.
	// Hier schauen ob ich auch noch Credits mit dazugebe.
	// Schauen ob ich 20% mehr als den Durchschnittswert aller erreiche bzw. wenn ich 25000 Credits habe,
	// dann gebe ich zu 50% auch Credits dazu.
	long nCredits = 0;
	USHORT counter = 0;

	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		if (pMajor && pMajor->GetEmpire()->CountSystems() > 0)
		{
			nCredits += pMajor->GetEmpire()->GetCredits();
			counter++;
		}
	}

	if (counter > 0)
		nCredits /= counter;

			// geht es an eine Minorrace, dann braucht es nicht so viel Geld �brig zu haben
	if ((pToRace->IsMinor() && pOurRace->GetEmpire()->GetCredits() > 3000 && rand()%3 == 0)
		||	// oder an eine Majorrace, da braucht es viel mehr Geld
		((pOurRace->GetEmpire()->GetCredits() >= (long)(nCredits * 1.2f) && pOurRace->GetEmpire()->GetCredits() > 1000)
		||	// bzw. wir haben sehr viel Geld �brig
		(pOurRace->GetEmpire()->GetCredits() > 25000 && rand()%2 > 0)))
	{
		nCredits = rand()%(pOurRace->GetEmpire()->GetCredits() + 1);
		// auf 250er Betr�ge runden
		nCredits = (long)(nCredits / 250) * 250;
		if (nCredits > 5000)
			nCredits = 5000;
		if (nCredits > 0)
		{
			// Jetzt das Credits in das Angebot schreiben
			info.m_nCredits = (USHORT)nCredits;
			// die Credits abziehen
			pOurRace->GetEmpire()->SetCredits(-nCredits);
			bGiveDowry |= true;
		}
	}

	// Ressourcengeschenk ist nur m�glich, wenn ein Handelsvertrag mit der anderen Rasse besteht
	if (pOurRace->GetAgreement(info.m_sToRace) >= DIPLOMATIC_AGREEMENT::TRADE)
	{
		// Ressourcen gibt die Rasse nur, wenn sie 10% mehr als den globalen Durschschnitt dieser Ressource hat.

		// zuerst random-wert ermitteln, welche Ressource ich dazugeben w�rde
		// daf�r benutze ich den durchschnittlichen Techlevel im Universum. Am Anfang wird deswegen nur Titan angeboten
		// sp�ter kommen dann auch die weiteren Ressourcen hinzu, wenn der Wert zu gro� ist machen wir das solange,
		// bis der Wert max. Deritium erh�lt
		USHORT whichRes = TITAN;
		do
		{
			whichRes = rand()%(m_pDoc->GetStatistics()->GetAverageTechLevel() + 1);
		}
		while (whichRes > DERITIUM);

		// Schauen ob wir mehr als 20% von der Ressource haben als der Durschnitt ist
		// nicht gleich in der do_while-Schleife, da ich nicht so oft dies als Mitgift geben m�chte
		bool bCanGive = false;

		if (pOurRace->GetEmpire()->GetStorage()[whichRes] >= (ULONG)(m_pDoc->GetStatistics()->GetAverageResourceStorages()[whichRes] * 1.2))
			bCanGive = true;

		// wenn wir das meiste davon haben, dann suchen wir unser System, wo wir am meisten von der Ressource gelagert haben
		if (bCanGive)
		{
			UINT nMostRes = 0;
			CPoint ptSystem(-1,-1);
			for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
				for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					if (m_pDoc->GetSystem(x,y).GetOwnerOfSystem() == pOurRace->GetRaceID())
					{
						nMostRes = max(m_pDoc->GetSystem(x,y).GetResourceStore(whichRes), nMostRes);
						ptSystem.x = x;
						ptSystem.y = y;
					}

			//CString d;
			//d.Format("Meiste Ressource (%d) liegt in System: %d:%d\nBesitzer %s\nMenge: %d\nglobaler Durchschnitt: %d",whichRes,system.x,system.y,this->GetRaceName(m_iRaceNumber),mostRes,averageRessourceStorages[whichRes]);
			//AfxMessageBox(d);

			// Jetzt geben wir eine Menge zwischen 0 und 20000 in 1000er Schritten als Mitgift (max. 20000)
			// Wert zwischen 0 und dem Lagerinhalt generieren
			UINT nGiveRes = rand()%(nMostRes + 1);
			// bei Deritium wird viel weniger �bergeben, bei den restlichen auf 1000er gerundet
			if (whichRes != DERITIUM)
				nGiveRes = (UINT)(nGiveRes / 1000) * 1000;

			if (nGiveRes > 20000)
				nGiveRes = 20000;
			if (nGiveRes)
			{
				// Jetzt in die Ressource f�r das Angebot schreiben
				info.m_nResources[whichRes] = (USHORT)nGiveRes;
				info.m_ptKO = ptSystem;
				// Die Ressource aus dem Lager des Systems abziehen
				m_pDoc->GetSystem(ptSystem.x, ptSystem.y).SetResourceStore(whichRes, (int)nGiveRes * (-1));
				bGiveDowry |= true;
			}
		}
	}

	return bGiveDowry;
}

/// Funktion berechnet ob und was f�r eine Forderung gestellt wird.
/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
/// @return <code>true</code> wenn eine Forderung gestellt wurde, ansonsten <code>false</code>
bool CMajorAI::ClaimRequest(CDiplomacyInfo& info)
{
	bool bClaimRequest = false;	// wurde eine Forderung gestellt

	CMajor* pOurRace = dynamic_cast<CMajor*>(m_pRace);
	if (!pOurRace)
		return false;

	CRace* pToRace = m_pDoc->GetRaceCtrl()->GetRace(info.m_sToRace);
	if (!pToRace)
		return false;

	// Wir stellen nur eine Forderung, wenn wir zuwenig von einer Ressource oder von Credits haben.
	// Zuwenig m�ssen wir anhand des globalen Durchschnitts berechnen.
	// Erstmal schauen ob ich eine Forderung mit Credits stelle, daf�r globalen Durchschnitt der Credits berechnen
	long nCredits = 0;
	USHORT counter = 0;

	map<CString, CMajor*>* pmMajors = m_pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CMajor* pMajor = it->second;
		if (pMajor && pMajor->GetEmpire()->CountSystems() > 0)
		{
			nCredits += pMajor->GetEmpire()->GetCredits();
			counter++;
		}
	}
	if (counter > 0)
		nCredits /= counter;

	// Hier werden zus�tzlich noch die Schiffsst�rken der anderen Rassen mit der unseren verglichen.
	bool bStronger = false;
	UINT nOurShipPower		= m_pDoc->GetStatistics()->GetShipPower(pOurRace->GetRaceID());
	UINT nTheirShipPower	= m_pDoc->GetStatistics()->GetShipPower(pToRace->GetRaceID());

	if (nOurShipPower > 0)
	{
		// wenn unsere Beziehung hoch genug ist, oder wir einen Freundschaftsvertrag haben, so werden die Schiffsst�rken
		// nicht mit beachtet
		if (pOurRace->GetRelation(info.m_sToRace) < 75 || pOurRace->GetAgreement(info.m_sToRace) < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
		{
			float fCompare = (float)nTheirShipPower / (float)nOurShipPower;
			if (fCompare < 0.6f && rand()%2 == 0)
				bStronger = true;
		}
	}

	// Jetzt haben wir den Durchschnittswert der Credits, wenn wir nur rand()%30Prozent von diesem Wert besitzen, dann
	// stellen wir eine Forderung an die andere Rasse
	if (bStronger || pOurRace->GetEmpire()->GetCredits() <= (long)(nCredits * (rand()%30 + 1) / 100))
	{
		// Dann wollen wir Credits sehen, Menge ist abh�ngig vom Techlevel, umso h�her, desto wahrscheinlicher ist
		// auch eine h�here Forderung
		nCredits = rand()%(500 + m_pDoc->GetStatistics()->GetAverageTechLevel() * 500);
		nCredits = (long)(nCredits / 250) * 250;
		if (nCredits > 5000)
			nCredits = 5000;
		if (nCredits != 0)
		{
			// Jetzt Credits in das Angebot schreiben
			info.m_nCredits = (USHORT)nCredits;
			bClaimRequest |= true;
		}
	}

	// Nun k�nnen wir auch noch Ressourcen fordern, auch zus�tzlich zu den Credit und wir ben�tigen daf�r auch keinen
	// aktuellen Handelsvertrag mit der Rasse ;-)
	// zuerst random-wert ermitteln, welche Ressource ich fordern w�rde
	// daf�r benutze ich den durchschnittlichen Techlevel im Universum. Am Anfang wird deswegen nur Titan angeboten
	// sp�ter kommen dann auch die weiteren Ressourcen hinzu, wenn der Wert zu gro� ist machen wir das solange,
	// bis der Wert max. Deritium erh�lt
	bool bCanGive = false;
	USHORT whichRes = TITAN;
	int count = 0;

	do {
		whichRes = rand()%(m_pDoc->GetStatistics()->GetAverageTechLevel() / 2 + 1);
		if (whichRes > DERITIUM)
			whichRes = DERITIUM;

		// Schauen ob wir weniger als rand()%50Prozent von der Ressource haben als der Durchschnitt
		if (bStronger || (pOurRace->GetEmpire()->GetStorage()[whichRes] <= (ULONG)(m_pDoc->GetStatistics()->GetAverageResourceStorages()[whichRes] * (rand()%50+1) / 100)
			&& pOurRace->GetEmpire()->GetStorage()[whichRes] != 0))
			bCanGive = true;
		count++;
	} while (count < 20 && bCanGive == false);

	// wenn wir zuwenig davon haben, dann stellen wir die Forderung
	if (bCanGive)
	{
		// Jetzt m�ssen wir eine Menge zwischen 0 und 20000 in 1000er Schritten ermitteln (max. 20000)
		// bei Deritium wird viel weniger �bergeben, bei den restlichen auf 1000er gerundet
		UINT nClaimRes = 0;
		if (whichRes != DERITIUM)
		{
			nClaimRes = rand()%(1000 + m_pDoc->GetStatistics()->GetAverageTechLevel() * 1250);
			nClaimRes = (UINT)(nClaimRes / 1000) * 1000;
		}
		else
			nClaimRes = rand()%(10 + m_pDoc->GetStatistics()->GetAverageTechLevel() * 5);

		if (nClaimRes > 20000)
			nClaimRes = 20000;

		if (nClaimRes)
		{
			// Jetzt in die Ressource f�r das Angebot schreiben
			info.m_nResources[whichRes] = (USHORT)nClaimRes;
			bClaimRequest |= true;
		}
	}

	return bClaimRequest;
}

/// Funktion berechnet die Beziehungsverbesserungen durch die �bergabe von Credits und Ressourcen.
/// Die Credits werden hier jedoch nicht gutgeschrieben, sondern nur die Beziehung zur Majorrace verbessert.
/// @param info Referenz auf eine Diplomatieinformation
void CMajorAI::ReactOnDowry(const CDiplomacyInfo& info)
{
	CMajor* pOurRace = dynamic_cast<CMajor*>(m_pRace);
	if (!pOurRace)
		return;

	// zuerst muss berechnet werden, wie viel die �bergebenen Ressourcen als Credits wert sind

	// Titan wird getauscht im Verh�ltnis 2.5:1
	// Deuterium wird getauscht im Verh�ltnis 2.75:1
	// Duranium wird getauscht im Verh�ltnis 2:1
	// Crystal wird getauscht im Verh�ltnis 1.625:1
	// Iridium wird getauscht im Verh�ltnis 1.25:1
	// Deritium wird getauscht im Verh�ltnis 1:100

	float fValue = 0.0f;
	float fDiv = 0.0f;

	if (info.m_nResources[TITAN] != 0)			// Titan �bergeben?
	{
		fValue = info.m_nResources[TITAN] / 2.5;
		fDiv = (float)info.m_nResources[TITAN] / (float)(pOurRace->GetEmpire()->GetStorage()[TITAN] + .00001);
	}
	else if (info.m_nResources[DEUTERIUM] != 0)	// Deuterium �bergeben?
	{
		fValue = info.m_nResources[DEUTERIUM] / 2.75;
		fDiv = (float)info.m_nResources[DEUTERIUM] / (float)(pOurRace->GetEmpire()->GetStorage()[DEUTERIUM] + .00001);
	}
	else if (info.m_nResources[DURANIUM] != 0)	// Duranium �bergeben?
	{
		fValue = info.m_nResources[DURANIUM] / 2;
		fDiv = (float)info.m_nResources[DURANIUM] / (float)(pOurRace->GetEmpire()->GetStorage()[DURANIUM] + .00001);
	}
	else if (info.m_nResources[CRYSTAL] != 0)	// Kristalle �bergeben?
	{
		fValue = info.m_nResources[CRYSTAL] / 1.625;
		fDiv = (float)info.m_nResources[CRYSTAL] / (float)(pOurRace->GetEmpire()->GetStorage()[CRYSTAL] + .00001);
	}
	else if (info.m_nResources[IRIDIUM] != 0)	// Iridium �bergeben?
	{
		fValue = info.m_nResources[IRIDIUM] / 1.25;
		fDiv = (float)info.m_nResources[IRIDIUM] / (float)(pOurRace->GetEmpire()->GetStorage()[IRIDIUM] + .00001);
	}
	else if (info.m_nResources[DERITIUM] != 0)	// Deritium �bergeben?
	{
		fValue = info.m_nResources[DERITIUM] * 100;
		fDiv = (float)info.m_nResources[DERITIUM] / (float)(pOurRace->GetEmpire()->GetStorage()[DERITIUM] + .00001);
	}

	//So, nun gibts aber nicht immer diesen Betrag! Daf�r fragen wir die
	//kompletten Lager ab. Umso mehr wir haben, umso weniger ist der
	//�bergebene Rohstoff f�r uns wert.

	//Daf�r berechnen wir einen Teiler. Wir nehmen die aktuelle
	//Menge des jeweiligen Rohstoffes, die in allen Systemen gelagert sind.
	//Mit diesem Teiler wird der erhaltene Wert des Rohstoffes multipliziert.
	//Dieser nun erhaltene Wert wird noch rassenabh�ngig modifiziert.

	if (fDiv > 1.0f)
		fDiv = 1.0f;
	else if (fDiv < 0.0f)
		fDiv = 0.0f;

	fValue *= fDiv;

	// Wert nochmal modifizieren, aufgrund der Rassenart
	if (fValue != 0.0f)
	{
		if (pOurRace->IsRaceProperty(RACE_PROPERTY::PRODUCER))
			fValue *= 1.5f;
		if (pOurRace->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
			fValue *= 1.35f;
		if (pOurRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
			fValue *= 1.2f;
		if (pOurRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
			fValue *= 1.1f;
		if (pOurRace->IsRaceProperty(RACE_PROPERTY::SECRET))
			fValue *= 0.9f;
		if (pOurRace->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
			fValue *= 0.8f;
	}

	fValue = max(0.0f, fValue);
	USHORT nCreditsFromRes = (USHORT)fValue;
	//CString s;
	//s.Format("Ressourcenwert: %d", nCreditsFromRes);
	//AfxMessageBox(s);

	USHORT nCredits = info.m_nCredits + nCreditsFromRes;
	// wenn keine Credits �bergeben werden, dann muss der Rest der Funktion nicht beachtet werden.
	if (nCredits == 0)
		return;

	// Das ist hier alles wesentlich k�rzer und nicht so komplex wie bei der Diplomatie mit
	// den Minorraces. Hier kommt es wesentlich mehr drauf an, wieviel Geld wir im Lager haben.
	// Nur an einer Stelle wird ein kleiner Unterschied bei der Art der Rasse gemacht.
	// Es soll relativ einfach sein, damit wir das Spiel nicht in eine Richtung lenken, jedes mal
	// kann und soll es ein bisschen anders sein.

	// Berechnen welche obere Grenze durch die Menge Credits gegeben ist.
	// Der berechnete Wert ist zuf�llig, innerhalb einiger Parameter, die durch die �bergebenen Credits vorgegeben sind
	USHORT nRandomCredits = 0;
	// Schleife von 3 Durchl�ufen -> Zufallsvariablendurchnschitt, damit wir bei 5000 Credits nur sehr unwahrscheinlich
	// mal einen Wert von unter 1000 oder so bekommen
	for (int i = 0; i < 3; i++)
		nRandomCredits += (rand()%(nCredits * 2) + 1);
	nRandomCredits = nRandomCredits / 3;

	// Wert, den wir mit nRandomCredits erreichen m�ssen, um die Beziehung zu verbessern
	short nNeededValue = DIPLOMACY_PRESENT_VALUE;	// 200 ist "normal"

	if (pOurRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
		nNeededValue += 150;	// bei einer zur�ckgezogenen Rasse ist der Wert wesentlich h�her
	if (pOurRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
		nNeededValue += 100;	// bei einer extrem feindlichen Rasse ist der Wert wesentlich h�her
	if (pOurRace->IsRaceProperty(RACE_PROPERTY::SECRET))
		nNeededValue += 75;		// bei einer Geheimdienstrasse ist der Wert h�her
	if (pOurRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
		nNeededValue += 50;		// bei einer kriegerischen Rasse ist der Wert h�her
	if (pOurRace->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
		nNeededValue -= 25;		// bei einer industriellen Rasse ist der Wert niedriger
	if (pOurRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
		nNeededValue -= 50;		// bei einer finanziellen Rasse ist der Wert niedriger

	// Jetzt berechnen, um wieviel sich die Beziehung ver�ndern w�rde
	float fBonus = 0.0f;
	if (nRandomCredits >= nNeededValue)
		// soviel Prozent kommt als Bonus hinzu
		fBonus = nRandomCredits / nNeededValue;

	// Jetzt die Abh�ngigkeit von den schon vorhandenen Credits im Lager beachten.
	// Hier wird ein Wert berechnet, n�mlich die gegebenen Credits/Creditlager von uns.
	// Der erhaltene Wert wird mit dem fBonus multipliziert und ergibt die Beziehungsverbesserung.
	// Der erhaltene Wert darf aber nicht gr��er als 1 sein!
	fValue = ((float)info.m_nCredits / (float)(pOurRace->GetEmpire()->GetCredits() + .001));
	if (fValue > 1.0f)
		fValue = 1.0f;
	else if (fValue < 0.0f)
		fValue = 0.0;

	fBonus *= fValue;
	// CString s;
	// s.Format("Credits: %d\nWert: %lf\nBeziehungsverbesserung: %lf\nCreditslager: %i",credits,value,bonus,empires[m_iRaceNumber].GetCredits());
	// AfxMessageBox(s);*/

	pOurRace->SetRelation(info.m_sFromRace, (short)fBonus);
}
