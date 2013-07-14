#include "stdafx.h"
#include "DiplomacyController.h"
#include "RaceController.h"
#include "GenDiploMessage.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Ships/Ships.h"
#include "General/Loc.h"

#include <cassert>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CDiplomacyController::CDiplomacyController(void)
{
}

CDiplomacyController::~CDiplomacyController(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum Versenden von diplomatischen Angeboten
void CDiplomacyController::Send(void)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	std::map<CString, CRace*>* races = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(races);

	// KI Angebote erstellen lassen
	for (map<CString, CRace*>::const_iterator it = races->begin(); it != races->end(); ++it)
	{
		CRace* pRace = it->second;
		ASSERT(pRace);

		pRace->MakeOffersAI();
	}

	// durch alle Rassen iterieren und Angebote versenden
	for (map<CString, CRace*>::const_iterator it = races->begin(); it != races->end(); ++it)
	{
		CRace* pRace = it->second;
		ASSERT(pRace);

		// alle eingegangenen Antworten aus der letzten Runde l�schen
		for (UINT i = 0; i < pRace->GetIncomingDiplomacyNews()->size(); i++)
		{
			CDiplomacyInfo* pInfo = &(pRace->GetIncomingDiplomacyNews()->at(i));
			if (pInfo->m_nFlag == DIPLOMACY_ANSWER)
				pRace->GetIncomingDiplomacyNews()->erase(pRace->GetIncomingDiplomacyNews()->begin() + i--);
		}

		// nun durch alle ausgehenden Nachrichten iterieren
		for (UINT i = 0; i < pRace->GetOutgoingDiplomacyNews()->size(); i++)
		{
			CDiplomacyInfo* pInfo = &(pRace->GetOutgoingDiplomacyNews()->at(i));
			// exisitiert die Zielrasse?
			if (races->find(pInfo->m_sToRace) != races->end())
			{
				CRace* pToRace = (*races)[pInfo->m_sToRace];
				// Angebote senden
				if (pToRace->IsMajor())
				{
					SendToMajor(pDoc, dynamic_cast<CMajor*>(pToRace), pInfo);
				}
				else if (pToRace->IsMinor())
				{
					SendToMinor(pDoc, dynamic_cast<CMinor*>(pToRace), pInfo);
				}
			}
		}

		// alle ausgehenden Nachrichten werden gel�scht
		pRace->GetOutgoingDiplomacyNews()->clear();
	}
}

/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots.
void CDiplomacyController::Receive(void)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	std::map<CString, CRace*>* races = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(races);

	// durch alle Rassen iterieren und Nachrichten empfangen sowie darauf reagieren
	for (map<CString, CRace*>::const_iterator it = races->begin(); it != races->end(); ++it)
	{
		CRace* pRace = it->second;
		ASSERT(pRace);

		// nun durch alle eingegangen Nachrichten iterieren und darauf reagieren
		for (UINT i = 0; i < pRace->GetIncomingDiplomacyNews()->size(); i++)
		{
			CDiplomacyInfo* pInfo = &(pRace->GetIncomingDiplomacyNews()->at(i));
			// exisitiert die Rasse welche die Nachricht gesendet hat?
			if (races->find(pInfo->m_sFromRace) != races->end())
			{
				CRace* pToRace = (*races)[pInfo->m_sToRace];
				if (pToRace->IsMajor())
				{
					ReceiveToMajor(pDoc, dynamic_cast<CMajor*>(pToRace), pInfo);
				}
				else if (pToRace->IsMinor())
				{
					ReceiveToMinor(pDoc, dynamic_cast<CMinor*>(pToRace), pInfo);
				}
			}
		}

		// alle eingegangenen Angebote aus der letzten Runde l�schen
		for (UINT i = 0; i < pRace->GetIncomingDiplomacyNews()->size(); i++)
		{
			CDiplomacyInfo* pInfo = &(pRace->GetIncomingDiplomacyNews()->at(i));
			if (pInfo->m_nSendRound < pDoc->GetCurrentRound() - 1)
				pRace->GetIncomingDiplomacyNews()->erase(pRace->GetIncomingDiplomacyNews()->begin() + i--);
		}
	}

	// die diplomatische Konsistenz �berpr�fen und Auswirkungen beachten
	CalcDiplomacyFallouts(pDoc);
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion �berpr�ft die diplomatische Konsistenz und berechnet die direkten diplomatischen Auswirkungen.
/// Sie sollte nach <func>Receive</func> aufgerufen werden.
/// Stellt die Funktion Probleme fest, so werden diese automatisch behoben.
/// @param pDoc Zeiger auf das Dokument
void CDiplomacyController::CalcDiplomacyFallouts(CBotEDoc* pDoc)
{
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	map<CString, CMinor*>* pmMinors = pDoc->GetRaceCtrl()->GetMinors();

	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
	{
		CMinor* pMinor = it->second;

		// Die diplomatische Konsistenz �berpr�fen! z.B kann niemand mehr eine Freundschaft haben, wenn jemand eine
		// Mitgliedschaft mit dieser Rasse hat. Vertr�ge werden gek�ndigt, wenn die Minorrace unterworfen wurde
		pMinor->CheckDiplomaticConsistence(pDoc);
		pMinor->PerhapsCancelAgreement(pDoc);
		if (pMinor->GetRaceKO() == CPoint(-1,-1))
			continue;

		for (map<CString, CMajor*>::const_iterator jt = pmMajors->begin(); jt != pmMajors->end(); ++jt)
		{
			CMajor* pMajor = jt->second;

			// Wenn wir mit der Minorrace mindst. einen Handelsvertrag abgeschlossen haben, dann wird deren Sector gescannt/gesehen
			if (pMinor->GetAgreement(pMajor->GetRaceID()) >= DIPLOMATIC_AGREEMENT::TRADE)
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetScanned(pMajor->GetRaceID());
			// Wenn wir mit der Minorrace mindst. einen Freundschaftsvertrag abgeschlossen haben, dann wird deren Sector bekannt
			if (pMinor->GetAgreement(pMajor->GetRaceID()) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetKnown(pMajor->GetRaceID());
			// Wenn wir eine Mitgliedschaft mit der kleinen Rasse haben und das System noch der kleinen Rasse geh�rt, dann bekommen wir das
			if (pMinor->IsMemberTo(pMajor->GetRaceID()) && pDoc->GetSystem(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).GetOwnerOfSystem() == "")
			{
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetFullKnown(pMajor->GetRaceID());
				pDoc->GetSystem(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetOwnerOfSystem(pMajor->GetRaceID());
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetOwnerOfSector(pMajor->GetRaceID());
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetOwned(TRUE);
				// Der Sector gilt jetzt als nicht eingenommen
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetTakenSector(FALSE);
				// Nun Geb�ude in neuen System bauen
				pDoc->GetSystem(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).BuildBuildingsForMinorRace(&pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y), &pDoc->BuildingInfo, pDoc->GetStatistics()->GetAverageTechLevel(), pMinor);
				// Geb�ude so weit wie m�glich mit Arbeitern besetzen
				pDoc->GetSystem(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetWorkersIntoBuildings();
				// alle Schiffe der Minor gehen nun an den Major
				for (CShipMap::iterator i = pDoc->m_ShipMap.begin(); i != pDoc->m_ShipMap.end(); ++i)
				{
					if (i->second->GetOwnerOfShip() == pMinor->GetRaceID())
					{
						// Der Ehlenen Besch�tzer geht niemals an den Major
						if (pMinor->GetRaceID() == "EHLEN" && i->second->GetShipType() == SHIP_TYPE::STARBASE)
							continue;

						i->second->SetOwnerOfShip(pMajor->GetRaceID());
						// Schiff in die Shiphistory stecken
						pMajor->GetShipHistory()->AddShip(i->second, pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).GetName(true), pDoc->GetCurrentRound());
					}
				}
			}
		}
	}

	// Hat die Rasse die Mitgliedschaft gek�ndigt, wurde Bestochen oder irgendein anderer Grund ist daf�r
	// verantwortlich, warum eine Major pl�tzlich nicht mehr Zugriff auf das System der Minor hat.
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
	{
		CMinor* pMinor = it->second;
		if (pMinor->GetRaceKO() == CPoint(-1,-1))
			continue;

		CString sOwner = pDoc->GetSystem(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).GetOwnerOfSystem();
		if (sOwner != "")
		{
			// Wenn wir eine Mitgliedschaft bei der kleinen Rasse hatten, sprich uns das System noch geh�rt, wir aber
			// der kleinen Rasse den Krieg erkl�ren bzw. den Vertrag aufheben (warum auch immer?!?) und das System nicht
			// gewalt�tig erobert wurde, dann geh�rt uns das System auch nicht mehr
			if (pMinor->GetAgreement(sOwner) != DIPLOMATIC_AGREEMENT::MEMBERSHIP && pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).GetMinorRace() == TRUE &&
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).GetTakenSector() == FALSE)
			{
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetOwned(FALSE);
				pDoc->GetSector(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetOwnerOfSector(pMinor->GetRaceID());
				pDoc->GetSystem(pMinor->GetRaceKO().x, pMinor->GetRaceKO().y).SetOwnerOfSystem("");
				CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sOwner));
				if (pMajor)
				{
					int nCount = pMajor->GetEmpire()->CountSystems();
					pMajor->GetEmpire()->SetNumberOfSystems(nCount - 1);
				}
			}
		}
	}
}

/// Funktion zum Versenden von diplomatischen Angeboten an eine Majorrace.
/// @param pDoc Zeiger auf das Dokument
/// @param pToMajor Zeiger auf die Empf�ngerrasse
/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
void CDiplomacyController::SendToMajor(CBotEDoc* pDoc, CMajor* pToMajor, CDiplomacyInfo* pInfo)
{
	// nur Reaktion beim diplomatischen Angebot
	if (pInfo->m_nFlag != DIPLOMACY_OFFER)
		return;

	// Die Rasse welche die Nachricht abgeschickt hat
	CRace* pFromRace = pDoc->GetRaceCtrl()->GetRace(pInfo->m_sFromRace);
	if (!pFromRace)
		return;

	////////////////////////////////////////////////////////////////////////////////////////
	// das Angebot geht an einen Major
	////////////////////////////////////////////////////////////////////////////////////////
	CMajor* pWarpactEnemy = NULL;
	if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
	{
		pWarpactEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pInfo->m_sWarpactEnemy));
		if (!pWarpactEnemy)
			return;
	}

	// Imperiumsnamen mit bestimmten Artikel holen
	CString sEmpireAssignedArticleName = pToMajor->GetEmpireNameWithAssignedArticle();
	CString sEmpireArticleName = "";
	if (pFromRace->IsMajor())
	{
		// Imperiumsnamen inkl. Artikel holen
		sEmpireArticleName = dynamic_cast<CMajor*>(pFromRace)->GetEmpireNameWithArticle();
		// gro� beginnen
		CString sUpper = (CString)sEmpireArticleName.GetAt(0);
		sEmpireArticleName.SetAt(0, sUpper.MakeUpper().GetAt(0));
	}

	CString sAgreement;
	switch (pInfo->m_nType)
	{
	case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CLoc::GetString("TRADE_AGREEMENT_WITH_ARTICLE");	break;}
	case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	{sAgreement = CLoc::GetString("FRIENDSHIP_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::COOPERATION:	{sAgreement = CLoc::GetString("COOPERATION_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::AFFILIATION:	{sAgreement = CLoc::GetString("AFFILIATION_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::MEMBERSHIP:	{sAgreement = CLoc::GetString("MEMBERSHIP_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::NAP:			{sAgreement = CLoc::GetString("NON_AGGRESSION_WITH_ARTICLE");	break;}
	case DIPLOMATIC_AGREEMENT::DEFENCEPACT:	{sAgreement = CLoc::GetString("DEFENCE_PACT_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::WARPACT:		{sAgreement = CLoc::GetString("WAR_PACT_WITH_ARTICLE", FALSE, pWarpactEnemy->GetRaceName()); break;}
	case DIPLOMATIC_AGREEMENT::REQUEST:		{sAgreement = CLoc::GetString("REQUEST_WITH_ARTICLE");			break;}
	}

	CEmpireNews message;

	// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir ein Geschenk gemacht haben
	if (pInfo->m_nSendRound == pDoc->GetCurrentRound() - 1)
	{
		if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
		{
			if (pFromRace->IsMajor())
			{
				CString s = CLoc::GetString("WE_GIVE_PRESENT", FALSE, sEmpireAssignedArticleName);
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
				dynamic_cast<CMajor*>(pFromRace)->GetEmpire()->AddMsg(message);

				s = CLoc::GetString("WE_GET_PRESENT", FALSE, sEmpireArticleName);
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
				pToMajor->GetEmpire()->AddMsg(message);
				// Die Credits des Geschenkes gutschreiben
				pToMajor->GetEmpire()->SetCredits(pInfo->m_nCredits);
				// Die Rohstoffe des Geschenkes gutschreiben
				for (int k = TITAN; k <= IRIDIUM; k++)
					if (pInfo->m_nResources[k] > 0)
						pToMajor->GetEmpire()->GetGlobalStorage()->AddRessource(pInfo->m_nResources[k], k, CPoint(-1,-1));
				// Deritium kommt nicht ins globale Lager sondern ins Heimatsystem
				if (pInfo->m_nResources[DERITIUM] > 0)
				{
					CPoint p = pDoc->GetRaceKO(pToMajor->GetRaceID());
					// geh�rt das System auch noch dem Major
					if (p != CPoint(-1,-1) && pDoc->GetSystem(p.x, p.y).GetOwnerOfSystem() == pToMajor->GetRaceID())
						pDoc->GetSystem(p.x, p.y).SetDeritiumStore(pInfo->m_nResources[DERITIUM]);
				}

				// Angebot in den Nachrichteneingang legen
				pToMajor->GetIncomingDiplomacyNews()->push_back(*pInfo);
			}
		}
		// handelt es sich um eine Forderung
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
		{
			if (pFromRace->IsMajor())
			{
				CString s = CLoc::GetString("WE_HAVE_REQUEST", FALSE, sEmpireAssignedArticleName, sAgreement);
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
				dynamic_cast<CMajor*>(pFromRace)->GetEmpire()->AddMsg(message);

				s = CLoc::GetString("WE_GET_REQUEST", FALSE, sEmpireArticleName, sAgreement);
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
				pToMajor->GetEmpire()->AddMsg(message);

				// Angebot in den Nachrichteneingang legen
				pToMajor->GetIncomingDiplomacyNews()->push_back(*pInfo);
			}
		}
		// handelt es sich um eine Kriegserkl�rung
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::WAR)
		{
			// Krieg erkl�ren
			DeclareWar(pFromRace, pToMajor, pInfo, true);

			// aufgrund diplomatischer Beziehungen k�nnte so weiter Krieg erkl�rt werden
			std::vector<CString> vEnemies;
			if (pFromRace->IsMajor())
				vEnemies = GetEnemiesFromContract(pDoc, dynamic_cast<CMajor*>(pFromRace), pToMajor);
			// allen weiteren Gegnern den Krieg erkl�ren
			for (UINT i = 0; i < vEnemies.size(); i++)
			{
				CRace* pEnemy = pDoc->GetRaceCtrl()->GetRace(vEnemies[i]);
				if (pEnemy)
				{
					CDiplomacyInfo war = *pInfo;
					// lediglich die Zielrasse und den Kriegsgrund anpassen
					war.m_sToRace		= pEnemy->GetRaceID();
					war.m_sWarPartner	= pToMajor->GetRaceID();
					DeclareWar(pFromRace, pEnemy, &war, false);
				}
			}
		}
		// sonstige Vertragangebote
		else
		{
			// das Angebot stammt von einem Major
			if (pFromRace->IsMajor())
			{
				CString s = CLoc::GetString("WE_MAKE_MAJ_OFFER", FALSE, sEmpireAssignedArticleName, sAgreement);
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
				dynamic_cast<CMajor*>(pFromRace)->GetEmpire()->AddMsg(message);

				s = CLoc::GetString("WE_GET_MAJ_OFFER", FALSE, sEmpireArticleName, sAgreement);
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
				pToMajor->GetEmpire()->AddMsg(message);
			}
			// das Angebot stammt von einem Minor
			else if (pFromRace->IsMinor())
			{
				CString s = "";
				switch (pInfo->m_nType)
				{
				case DIPLOMATIC_AGREEMENT::TRADE:		s = CLoc::GetString("MIN_OFFER_TRADE", FALSE, pFromRace->GetRaceName());	break;
				case DIPLOMATIC_AGREEMENT::FRIENDSHIP:s = CLoc::GetString("MIN_OFFER_FRIEND", FALSE, pFromRace->GetRaceName());	break;
				case DIPLOMATIC_AGREEMENT::COOPERATION:			s = CLoc::GetString("MIN_OFFER_COOP", FALSE, pFromRace->GetRaceName());		break;
				case DIPLOMATIC_AGREEMENT::AFFILIATION:			s = CLoc::GetString("MIN_OFFER_AFFI", FALSE, pFromRace->GetRaceName());		break;
				case DIPLOMATIC_AGREEMENT::MEMBERSHIP:			s = CLoc::GetString("MIN_OFFER_MEMBER", FALSE, pFromRace->GetRaceName());	break;
				}
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
				pToMajor->GetEmpire()->AddMsg(message);
			}
			// Angebot in den Nachrichteneingang legen
			pToMajor->GetIncomingDiplomacyNews()->push_back(*pInfo);
		}
	}
}

/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots f�r eine Majorrace.
/// @param pDoc Zeiger auf das Dokument
/// @param pToMajor Zeiger auf die Empf�ngerrasse
/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
void CDiplomacyController::ReceiveToMajor(CBotEDoc* pDoc, CMajor* pToMajor, CDiplomacyInfo* pInfo)
{
	// nur Reaktion beim diplomatischen Angebot
	if (pInfo->m_nFlag != DIPLOMACY_OFFER)
		return;

	// Die Rasse welche die Nachricht abgeschickt hat
	CRace* pFromRace = pDoc->GetRaceCtrl()->GetRace(pInfo->m_sFromRace);
	if (!pFromRace)
		return;

	////////////////////////////////////////////////////////////////////////////////////////
	// das Angebot geht an einen Major
	////////////////////////////////////////////////////////////////////////////////////////
	CMajor* pWarpactEnemy = NULL;
	if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
	{
		pWarpactEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pInfo->m_sWarpactEnemy));
		if (!pWarpactEnemy)
			return;
	}

	CString sAgreement;
	switch (pInfo->m_nType)
	{
	case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CLoc::GetString("TRADE_AGREEMENT_WITH_ARTICLE");	break;}
	case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	{sAgreement = CLoc::GetString("FRIENDSHIP_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::COOPERATION:	{sAgreement = CLoc::GetString("COOPERATION_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::AFFILIATION:	{sAgreement = CLoc::GetString("AFFILIATION_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::MEMBERSHIP:	{sAgreement = CLoc::GetString("MEMBERSHIP_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::NAP:			{sAgreement = CLoc::GetString("NON_AGGRESSION_WITH_ARTICLE");	break;}
	case DIPLOMATIC_AGREEMENT::DEFENCEPACT:	{sAgreement = CLoc::GetString("DEFENCE_PACT_WITH_ARTICLE");		break;}
	case DIPLOMATIC_AGREEMENT::WARPACT:		{sAgreement = CLoc::GetString("WAR_PACT_WITH_ARTICLE", FALSE, pWarpactEnemy->GetRaceName()); break;}
	case DIPLOMATIC_AGREEMENT::REQUEST:		{sAgreement = CLoc::GetString("REQUEST_WITH_ARTICLE");			break;}
	}

	CEmpireNews message;

	if (pInfo->m_nSendRound == pDoc->GetCurrentRound() - 2)
	{
		// Nur wenn der Computer die Rasse spielt
		if (!pToMajor->AHumanPlays())
			pToMajor->ReactOnOfferAI(pInfo);

		// Antwort der Majorrace erzeugen
		if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::PRESENT && pInfo->m_nType != DIPLOMATIC_AGREEMENT::WAR)
		{
			CDiplomacyInfo answer = *pInfo;
			answer.m_nSendRound = pDoc->GetCurrentRound();
			CGenDiploMessage::GenerateMajorAnswer(answer);

			// Die Antwort geht an einen Major
			if (pFromRace->IsMajor())
			{
				CMajor* pFromMajor = dynamic_cast<CMajor*>(pFromRace);
				assert(pFromMajor);
				pFromRace->GetIncomingDiplomacyNews()->push_back(answer);

				// Nachricht �ber Vertragsannahme oder Ablehnung
				if (answer.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && answer.m_nType != DIPLOMATIC_AGREEMENT::WARPACT)
				{
					CString sAgreement2;
					switch (answer.m_nType)
					{
						case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement2 = CLoc::GetString("TRADE_AGREEMENT");	break;}
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	{sAgreement2 = CLoc::GetString("FRIENDSHIP");		break;}
						case DIPLOMATIC_AGREEMENT::COOPERATION:	{sAgreement2 = CLoc::GetString("COOPERATION");		break;}
						case DIPLOMATIC_AGREEMENT::AFFILIATION:	{sAgreement2 = CLoc::GetString("AFFILIATION");		break;}
						case DIPLOMATIC_AGREEMENT::NAP:			{sAgreement2 = CLoc::GetString("NON_AGGRESSION");	break;}
						case DIPLOMATIC_AGREEMENT::DEFENCEPACT:	{sAgreement2 = CLoc::GetString("DEFENCE_PACT");		break;}
					}

					// Das Angebot wurde angenommen
					if (answer.m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
					{
						CString s  = CLoc::GetString("WE_ACCEPT_MAJ_OFFER", FALSE, sAgreement, pFromMajor->GetEmpireNameWithAssignedArticle());
						CString s2 = CLoc::GetString("MAJ_ACCEPT_OFFER", TRUE, pToMajor->GetEmpireNameWithArticle(), sAgreement2);

						// Diplomatischen Status festlegen
						pFromRace->SetAgreement(pToMajor->GetRaceID(), answer.m_nType);
						pToMajor->SetAgreement(pFromRace->GetRaceID(), answer.m_nType);
						if (answer.m_nType != DIPLOMATIC_AGREEMENT::DEFENCEPACT)
						{
							pFromMajor->SetAgreementDuration(pToMajor->GetRaceID(), answer.m_nDuration);
							pToMajor->SetAgreementDuration(pFromRace->GetRaceID(), answer.m_nDuration);
						}
						else
						{
							pFromMajor->SetDefencePactDuration(pToMajor->GetRaceID(), answer.m_nDuration);
							pToMajor->SetDefencePactDuration(pFromRace->GetRaceID(), answer.m_nDuration);
						}

						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
						pToMajor->GetEmpire()->AddMsg(message);
						message.CreateNews(s2, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
						pFromMajor->GetEmpire()->AddMsg(message);

						// zus�tzliche Eventnachricht wegen der Moral an die Imperien
						CString sEventText = "";
						CString sEventText2 = "";

						switch (answer.m_nType)
						{
						// Sign Trade Treaty #34
						case DIPLOMATIC_AGREEMENT::TRADE:
							{
								sEventText = pToMajor->GetMoralObserver()->AddEvent(34, pToMajor->GetRaceMoralNumber(), pFromMajor->GetEmpireNameWithAssignedArticle());
								sEventText2 = pFromMajor->GetMoralObserver()->AddEvent(34, pFromMajor->GetRaceMoralNumber(), pToMajor->GetEmpireNameWithAssignedArticle());
								break;
							}
						// Sign Friendship/Cooperation Treaty #35
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP:
							{
								sEventText = pToMajor->GetMoralObserver()->AddEvent(35, pToMajor->GetRaceMoralNumber(), pFromMajor->GetEmpireNameWithAssignedArticle());
								sEventText2 = pFromMajor->GetMoralObserver()->AddEvent(35, pFromMajor->GetRaceMoralNumber(), pToMajor->GetEmpireNameWithAssignedArticle());
								break;
							}
						// Sign Friendship/Cooperation Treaty #35
						case DIPLOMATIC_AGREEMENT::COOPERATION:
							{
								sEventText = pToMajor->GetMoralObserver()->AddEvent(35, pToMajor->GetRaceMoralNumber(), pFromMajor->GetEmpireNameWithAssignedArticle());
								sEventText2 = pFromMajor->GetMoralObserver()->AddEvent(35, pFromMajor->GetRaceMoralNumber(), pToMajor->GetEmpireNameWithAssignedArticle());
								break;
							}
						// Sign an Affiliation Treaty #36
						case DIPLOMATIC_AGREEMENT::AFFILIATION:
							{
								sEventText = pToMajor->GetMoralObserver()->AddEvent(36, pToMajor->GetRaceMoralNumber(), pFromMajor->GetEmpireNameWithAssignedArticle());
								sEventText2 = pFromMajor->GetMoralObserver()->AddEvent(36, pFromMajor->GetRaceMoralNumber(), pToMajor->GetEmpireNameWithAssignedArticle());
								break;
							}
						}
						if (!sEventText.IsEmpty())
						{
							message.CreateNews(sEventText, EMPIRE_NEWS_TYPE::SOMETHING);
							pToMajor->GetEmpire()->AddMsg(message);
						}
						if (!sEventText2.IsEmpty())
						{
							message.CreateNews(sEventText2, EMPIRE_NEWS_TYPE::SOMETHING);
							pFromMajor->GetEmpire()->AddMsg(message);
						}

						// Die m�glicherweise dazugegebenen Credits und die Ressourcen gutschreiben.
						pToMajor->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= IRIDIUM; res++)
							pToMajor->GetEmpire()->GetGlobalStorage()->AddRessource(answer.m_nResources[res], res, CPoint(-1,-1));
						// Deritium kommt nicht ins globale Lager sondern ins Heimatsystem
						if (pInfo->m_nResources[DERITIUM] > 0)
						{
							CPoint p = pDoc->GetRaceKO(pToMajor->GetRaceID());
							// geh�rt das System auch noch dem Major
							if (p != CPoint(-1,-1) && pDoc->GetSystem(p.x, p.y).GetOwnerOfSystem() == pToMajor->GetRaceID())
								pDoc->GetSystem(p.x, p.y).SetDeritiumStore(pInfo->m_nResources[DERITIUM]);
						}

						// Beziehungsverbesserung
						pToMajor->SetRelation(pFromRace->GetRaceID(), abs(answer.m_nType * 2));
						pFromRace->SetRelation(pToMajor->GetRaceID(), abs(answer.m_nType * 2));

						// Wenn die Beziehung zu schlecht ist, z.B. nachdem ein Krieg war und ein Nichtangriffspakt nachher
						// angenommen wurde, so die Beziehung auf ein Minimum setzen.
						if (pToMajor->GetRelation(pFromRace->GetRaceID()) < (answer.m_nType + 3) * 10)
						{
							int add = ((answer.m_nType + 3) * 10 - pToMajor->GetRelation(pFromRace->GetRaceID()));
							pToMajor->SetRelation(pFromRace->GetRaceID(), add);
						}
						// Wenn die Beziehung zu schlecht ist, z.B. nachdem ein Krieg war und ein Nichtangriffspakt nachher
						// angenommen wurde, so die Beziehung auf ein Minimum setzen.
						if (pFromRace->GetRelation(pToMajor->GetRaceID()) < (answer.m_nType + 3) * 10)
						{
							int add = ((answer.m_nType + 3) * 10 - pFromRace->GetRelation(pToMajor->GetRaceID()));
							pFromRace->SetRelation(pToMajor->GetRaceID(), add);
						}
					}
					// Wir haben das Angebot abgelehnt oder nicht darauf reagiert
					else
					{
						// Nachricht �ber Ablehnung
						if (answer.m_nAnswerStatus == ANSWER_STATUS::DECLINED)
						{
							CString s = CLoc::GetString("WE_DECLINE_MAJ_OFFER", FALSE, sAgreement, pFromRace->GetRaceName());
							message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
							pToMajor->GetEmpire()->AddMsg(message);

							CString s2 = CLoc::GetString("MAJ_DECLINE_OFFER", TRUE, pToMajor->GetEmpireNameWithArticle(), sAgreement2);
							message.CreateNews(s2, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
							pFromMajor->GetEmpire()->AddMsg(message);

							// Beziehungsverschlechterung, wenn unser Angebot abgelehnt wird, so geht die
							// Beziehung von uns zum Ablehner st�rker runter, als die vom Ablehner zu uns
							pToMajor->SetRelation(pFromRace->GetRaceID(), -(USHORT)(rand()%(abs(answer.m_nType))) / 2);
							pFromRace->SetRelation(pToMajor->GetRaceID(), -(USHORT)(rand()%(abs(answer.m_nType))));
						}
						// es wurde nicht auf das Angebot reagiert
						else
						{
							CString s = CLoc::GetString("NOT_REACTED", TRUE, pToMajor->GetEmpireNameWithArticle());
							message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
							pFromMajor->GetEmpire()->AddMsg(message);
						}

						// Wenn wir das Angebot abgelehnt haben, dann bekommt die Majorrace, die es mir gemacht hat
						// ihre Ressourcen und ihre Credits wieder zur�ck, sofern sie es mir als Anreiz mit zum Vertrags-
						// angebot gemacht haben
						pFromMajor->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= DERITIUM; res++)
						{
							CPoint pt = answer.m_ptKO;
							if (pt != CPoint(-1,-1))
								if (pDoc->GetSystem(pt.x, pt.y).GetOwnerOfSystem() == pFromRace->GetRaceID())
									pDoc->GetSystem(pt.x, pt.y).SetResourceStore(res, answer.m_nResources[res]);
						}
					}
				}
				// Forderungsannahme oder Ablehnung
				else if (answer.m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
				{
					CString s;
					// Wir haben die Forderung angenommen
					if (answer.m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
					{
						s = CLoc::GetString("WE_ACCEPT_REQUEST", TRUE, pFromMajor->GetEmpireNameWithAssignedArticle());
						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
						pToMajor->GetEmpire()->AddMsg(message);

						s = CLoc::GetString("OUR_REQUEST_ACCEPT", TRUE, pToMajor->GetEmpireNameWithArticle());
						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
						pFromMajor->GetEmpire()->AddMsg(message);

						// Die geforderten Credits und die Ressourcen gutschreiben.
						pFromMajor->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= IRIDIUM; res++)
							pFromMajor->GetEmpire()->GetGlobalStorage()->AddRessource(answer.m_nResources[res], res, CPoint(-1,-1));
						// Deritium kommt nicht ins globale Lager sondern ins Heimatsystem
						if (pInfo->m_nResources[DERITIUM] > 0)
						{
							CPoint p = pDoc->GetRaceKO(pFromRace->GetRaceID());
							// geh�rt das System auch noch dem Major
							if (p != CPoint(-1,-1) && pDoc->GetSystem(p.x, p.y).GetOwnerOfSystem() == pFromRace->GetRaceID())
								pDoc->GetSystem(p.x, p.y).SetDeritiumStore(pInfo->m_nResources[DERITIUM]);
						}

						// Beziehungsverbesserung bei Annahme auf der fordernden Seite (0 bis 10 Punkte)
						pFromRace->SetRelation(pToMajor->GetRaceID(), rand()%DIPLOMATIC_AGREEMENT::REQUEST);
					}
					// Wir haben die Forderung abgelehnt
					else if (answer.m_nAnswerStatus == ANSWER_STATUS::DECLINED)
					{
						s = CLoc::GetString("WE_DECLINE_REQUEST", TRUE, pFromMajor->GetEmpireNameWithAssignedArticle());
						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
						pToMajor->GetEmpire()->AddMsg(message);

						s = CLoc::GetString("OUR_REQUEST_DECLINE", TRUE, pToMajor->GetEmpireNameWithArticle());
						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
						pFromMajor->GetEmpire()->AddMsg(message);
						// Beziehungsverschlechterung bei Ablehnung, hier wird ein bissl vom Computer gecheated.
						// Wenn ein Computer von einem anderen Computer die Forderung ablehnt, dann wird die Beziehung
						// nicht ganz so stark verschlechtert als wenn ein Mensch die Forderung des Computers ablehnt
						if (pFromMajor->AHumanPlays() == false)
							pFromMajor->SetRelation(pToMajor->GetRaceID(), (short)((-rand()%DIPLOMATIC_AGREEMENT::REQUEST)/5));
						else
							pFromMajor->SetRelation(pToMajor->GetRaceID(), -rand()%DIPLOMATIC_AGREEMENT::REQUEST);
					}
					// Wir haben nicht auf die Forderung reagiert
					else
					{
						s = CLoc::GetString("NOT_REACTED_REQUEST", FALSE, pToMajor->GetEmpireNameWithArticle());
						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
						pFromMajor->GetEmpire()->AddMsg(message);

						// Beziehungsverschlechterung bei Ablehnung, hier wird ein bissl vom Computer gecheated.
						// Wenn ein Computer von einem anderen Computer die Forderung ablehnt, dann wird die Beziehung
						// nicht ganz so stark verschlechtert als wenn ein Mensch die Forderung des Computers ablehnt
						if (pFromMajor->AHumanPlays() == false)
							pFromMajor->SetRelation(pToMajor->GetRaceID(), (short)((-rand()%DIPLOMATIC_AGREEMENT::REQUEST)/10));
						else
							pFromMajor->SetRelation(pToMajor->GetRaceID(), (short)((-rand()%DIPLOMATIC_AGREEMENT::REQUEST)/2));
					}
				}
				// Kriegspaktannahme oder Ablehnung (das ist hier ein bissl kompliziert, weil wir alle mgl. B�ndnisse beachten m�ssen
				else if (answer.m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
				{
					CString s;

					// Wir haben den Kriegspakt angenommen
					if (answer.m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
					{
						MYTRACE("diplomacy")(MT::LEVEL_DEBUG, "Race: %s accepted WARPACT from %s versus %s", pToMajor->GetRaceID(), pFromRace->GetRaceID(), pWarpactEnemy->GetRaceID());

						s = CLoc::GetString("WE_ACCEPT_WARPACT", FALSE, pWarpactEnemy->GetRaceName(), pFromMajor->GetEmpireNameWithAssignedArticle());
						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
						pToMajor->GetEmpire()->AddMsg(message);
						s = CLoc::GetString("OUR_WARPACT_ACCEPT", TRUE, pToMajor->GetEmpireNameWithArticle(), pWarpactEnemy->GetRaceName());
						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
						pFromMajor->GetEmpire()->AddMsg(message);

						// Beziehungsverbesserung
						pToMajor->SetRelation(pFromRace->GetRaceID(), abs(answer.m_nType));
						pFromRace->SetRelation(pToMajor->GetRaceID(), abs(answer.m_nType));

						// Die m�glicherweise dazugegebenen Credits und die Ressourcen gutschreiben.
						pToMajor->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= IRIDIUM; res++)
							pToMajor->GetEmpire()->GetGlobalStorage()->AddRessource(answer.m_nResources[res], res, CPoint(-1,-1));
						// Deritium kommt nicht ins globale Lager sondern ins Heimatsystem
						if (pInfo->m_nResources[DERITIUM] > 0)
						{
							CPoint p = pDoc->GetRaceKO(pToMajor->GetRaceID());
							// geh�rt das System auch noch dem Major
							if (p != CPoint(-1,-1) && pDoc->GetSystem(p.x, p.y).GetOwnerOfSystem() == pToMajor->GetRaceID())
								pDoc->GetSystem(p.x, p.y).SetDeritiumStore(pInfo->m_nResources[DERITIUM]);
						}

						// Kriegserkl�rung erstellen (f�r den der das Angebot angenommen hat)
						// nun muss in der Antwort die ToRace mit dem Kriegspaktgegner ersetzt werden
						// zus�tzlich muss die Antwort sich um einen Krieg handeln, nicht um den Kriegspakt
						CDiplomacyInfo warOffer;
						warOffer.m_sToRace		= pWarpactEnemy->GetRaceID();
						warOffer.m_sFromRace	= answer.m_sFromRace;
						warOffer.m_nFlag		= DIPLOMACY_OFFER;
						warOffer.m_nSendRound	= answer.m_nSendRound;
						warOffer.m_nType		= DIPLOMATIC_AGREEMENT::WAR;
						CGenDiploMessage::GenerateMajorOffer(warOffer);

						DeclareWar(pToMajor, pWarpactEnemy, &warOffer, true);
						// aufgrund diplomatischer Beziehungen k�nnte so weiter Krieg erkl�rt werden
						std::vector<CString> vEnemies;
						vEnemies = GetEnemiesFromContract(pDoc, pToMajor, pWarpactEnemy);
						// allen weiteren Gegnern den Krieg erkl�ren
						for (UINT i = 0; i < vEnemies.size(); i++)
						{
							if (vEnemies[i] != pFromRace->GetRaceID())
							{
								CRace* pEnemy = pDoc->GetRaceCtrl()->GetRace(vEnemies[i]);
								if (pEnemy)
								{
									CDiplomacyInfo war = warOffer;
									// lediglich die Zielrasse und den Krieggrund anpassen
									war.m_sToRace		= pEnemy->GetRaceID();
									war.m_sWarPartner	= pWarpactEnemy->GetRaceID();
									DeclareWar(pToMajor, pEnemy, &war, false);
								}
							}
						}
						// Kriegserkl�rung erstellen (f�r den der das Angebot abgegeben hat)
						// dabei muss der, der das Angebot abgegeben hat umgesetzt werden
						warOffer.m_sFromRace = answer.m_sToRace;
						CGenDiploMessage::GenerateMajorOffer(warOffer);

						DeclareWar(pFromRace, pWarpactEnemy, &warOffer, true);
						// aufgrund diplomatischer Beziehungen k�nnte so weiter Krieg erkl�rt werden
						vEnemies.clear();
						vEnemies = GetEnemiesFromContract(pDoc, pFromMajor, pWarpactEnemy);
						// allen weiteren Gegnern den Krieg erkl�ren
						for (UINT i = 0; i < vEnemies.size(); i++)
						{
							if (vEnemies[i] != pToMajor->GetRaceID())
							{
								CRace* pEnemy = pDoc->GetRaceCtrl()->GetRace(vEnemies[i]);
								if (pEnemy)
								{
									CDiplomacyInfo war = warOffer;
									// lediglich die Zielrasse und den Krieggrund anpassen
									war.m_sToRace		= pEnemy->GetRaceID();
									war.m_sWarPartner	= pWarpactEnemy->GetRaceID();
									DeclareWar(pFromRace, pEnemy, &war, false);
								}
							}
						}
					}
					// das Angebot wurde abgelehnt bzw. ignoriert
					else
					{
						// Wir haben den Kriegspakt abgelehnt
						if (answer.m_nAnswerStatus == ANSWER_STATUS::DECLINED)
						{
							s = CLoc::GetString("WE_DECLINE_WARPACT", FALSE, pFromMajor->GetEmpireNameWithAssignedArticle(), pWarpactEnemy->GetRaceName());
							message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
							pToMajor->GetEmpire()->AddMsg(message);
							s = CLoc::GetString("OUR_WARPACT_DECLINE", TRUE, pToMajor->GetEmpireNameWithArticle(), pWarpactEnemy->GetRaceName());
							message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
							pFromMajor->GetEmpire()->AddMsg(message);

							// Beziehungsverschlechterung
							pToMajor->SetRelation(pFromRace->GetRaceID(), -(USHORT)(rand()%(abs(answer.m_nType)))/2);
							pFromRace->SetRelation(pToMajor->GetRaceID(), -(USHORT)(rand()%(abs(answer.m_nType))));
						}
						// auf unseren Kriegspakt wurde nicht reagiert
						else
						{
							s = CLoc::GetString("NOT_REACTED_WARPACT", TRUE, pToMajor->GetEmpireNameWithArticle(), pWarpactEnemy->GetRaceName());
							message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
							pFromMajor->GetEmpire()->AddMsg(message);
						}

						// Wenn wir das Angebot abgelehnt haben, dann bekommt die Majorrace, die es mir gemacht hat
						// ihre Ressourcen und ihre Credits wieder zur�ck, sofern sie es mir als Anreiz mit zum Vertrags-
						// angebot gemacht haben
						pFromMajor->GetEmpire()->SetCredits(answer.m_nCredits);
						for (int res = TITAN; res <= DERITIUM; res++)
						{
							CPoint pt = answer.m_ptKO;
							if (pt != CPoint(-1,-1))
								if (pDoc->GetSystem(pt.x, pt.y).GetOwnerOfSystem() == pFromRace->GetRaceID())
									pDoc->GetSystem(pt.x, pt.y).SetResourceStore(res, answer.m_nResources[res]);
						}
					}
				}
			}
			// die Antwort geht an einen Minor
			else if (pFromRace->IsMinor())
			{
				// wir haben das Angebot der Minor angenommen
				if (pInfo->m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
				{
					CString sEventText = "";
					if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
						sEventText = pToMajor->GetMoralObserver()->AddEvent(10, pToMajor->GetRaceMoralNumber() , pFromRace->GetRaceName());

					// wenn es keine Bestechung ist
					if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::CORRUPTION)
					{
						// Pr�fen ob der Vertrag aufgrund aktuell bestehender Vertr�ge �berhaupt angenommen werden darf
						// Wenn die Minorrace unser Angebot in der gleichen Runde angenommen hat, dann
						// wird der Vertrag hier nicht gesetzt
						CMinor* pMinor = dynamic_cast<CMinor*>(pFromRace);
						ASSERT(pMinor);
						if (!pMinor->CanAcceptOffer(pDoc, pToMajor->GetRaceID(), pInfo->m_nType))
							return;

						// nur Text bei Vertragsformen erstellen
						if (!sAgreement.IsEmpty())
						{
							CString	s = CLoc::GetString("FEMALE_ARTICLE", TRUE) + " " + pFromRace->GetRaceName() + " " + CLoc::GetString("MIN_ACCEPT_OFFER", FALSE, sAgreement, pToMajor->GetEmpireNameWithAssignedArticle());
							message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
							pToMajor->GetEmpire()->AddMsg(message);
						}

						// Moralevent einplanen
						CString sEventText = "";
						switch (pInfo->m_nType)
						{
						case DIPLOMATIC_AGREEMENT::TRADE: sEventText = pToMajor->GetMoralObserver()->AddEvent(34, pToMajor->GetRaceMoralNumber(), pFromRace->GetRaceName());	break;
						// Sign Friendship/Cooperation Treaty #35
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP: sEventText = pToMajor->GetMoralObserver()->AddEvent(35, pToMajor->GetRaceMoralNumber(), pFromRace->GetRaceName()); break;
						// Sign Friendship/Cooperation Treaty #35
						case DIPLOMATIC_AGREEMENT::COOPERATION: sEventText = pToMajor->GetMoralObserver()->AddEvent(35, pToMajor->GetRaceMoralNumber(), pFromRace->GetRaceName());	break;
						// Sign an Affiliation Treaty #36
						case DIPLOMATIC_AGREEMENT::AFFILIATION: sEventText = pToMajor->GetMoralObserver()->AddEvent(36, pToMajor->GetRaceMoralNumber(), pFromRace->GetRaceName());	break;
						// Sign a Membership #10
						case DIPLOMATIC_AGREEMENT::MEMBERSHIP: sEventText = pToMajor->GetMoralObserver()->AddEvent(10, pToMajor->GetRaceMoralNumber() , pFromRace->GetRaceName());	break;
						}

						if (!sEventText.IsEmpty())
						{
							CEmpireNews message;
							message.CreateNews(sEventText, EMPIRE_NEWS_TYPE::SOMETHING);
							pToMajor->GetEmpire()->AddMsg(message);
						}

						// Vertrag setzen
						pFromRace->SetAgreement(pToMajor->GetRaceID(), pInfo->m_nType);
						pToMajor->SetAgreement(pFromRace->GetRaceID(), pInfo->m_nType);
					}
				}
				// wir haben das Angebot der Minor abgelehnt
				else if (pInfo->m_nAnswerStatus == ANSWER_STATUS::DECLINED)
				{
					switch (pInfo->m_nType)
					{
						case DIPLOMATIC_AGREEMENT::TRADE:		sAgreement = CLoc::GetString("TRADE_AGREEMENT"); break;
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	sAgreement = CLoc::GetString("FRIENDSHIP"); break;
						case DIPLOMATIC_AGREEMENT::COOPERATION:	sAgreement = CLoc::GetString("COOPERATION"); break;
						case DIPLOMATIC_AGREEMENT::AFFILIATION:	sAgreement = CLoc::GetString("AFFILIATION"); break;
						case DIPLOMATIC_AGREEMENT::MEMBERSHIP:	sAgreement = CLoc::GetString("MEMBERSHIP"); break;
					}
					CString s = CLoc::GetString("WE_DECLINE_MIN_OFFER", FALSE, sAgreement, pFromRace->GetRaceName());
					message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
					pToMajor->GetEmpire()->AddMsg(message);
				}
			}
		}
	}
}

/// Funktion zum Versenden von diplomatischen Angeboten an eine Minorrace.
/// @param pDoc Zeiger auf das Dokument
/// @param pToMinor Zeiger auf die Empf�ngerrasse
/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
void CDiplomacyController::SendToMinor(CBotEDoc* pDoc, CMinor* pToMinor, CDiplomacyInfo* pInfo)
{
	// nur Reaktion beim diplomatischen Angebot
	if (pInfo->m_nFlag != DIPLOMACY_OFFER)
		return;

	// Nur wenn es ein Major abgeschickt hat macht dies auch Sinn hier weiter zu machen. Die
	// anderen Rassenarten bekommen keine Imperiumsnachrichten
	CMajor* pFromMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pInfo->m_sFromRace));
	if (!pFromMajor)
		return;

	// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen erzeugen
	if (pInfo->m_nSendRound == pDoc->GetCurrentRound() - 1)
	{
		// Angebot der Minor in den Nachrichteneingang legen
		if (pInfo->m_nType !=DIPLOMATIC_AGREEMENT:: WAR && pInfo->m_nType != DIPLOMATIC_AGREEMENT::NONE)
			pToMinor->GetIncomingDiplomacyNews()->push_back(*pInfo);

		////////////////////////////////////////////////////////////////////////////////////////
		// das Angebot geht an einen Minor
		////////////////////////////////////////////////////////////////////////////////////////
		// Imperiumsnamen inkl. Artikel holen
		CString sEmpireName = pFromMajor->GetEmpireNameWithArticle();
		// gro� beginnen
		CString sUpper = (CString)sEmpireName.GetAt(0);
		sEmpireName.SetAt(0, sUpper.MakeUpper().GetAt(0));

		CString s;
		CEmpireNews message;

		// Wenn eine Runde vergangen ist, dann Nachricht erstellen, dass ein Geschenk gemacht wurde.
		if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
		{
			// Credits geschenkt
			if (pInfo->m_nCredits > 0)
			{
				CString sCredits;
				sCredits.Format("%d", pInfo->m_nCredits);
				s = sEmpireName + " " + CLoc::GetString("CREDITS_PRESENT", FALSE, sCredits, pToMinor->GetRaceName());
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
				pFromMajor->GetEmpire()->AddMsg(message);
				s = "";
			}
			// Ressourcen geschenkt (allein oder zus�tzlich zu Credits)
			if (pInfo->m_nResources[TITAN] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[TITAN]);
				s = sEmpireName + " " + CLoc::GetString("TITAN_PRESENT", FALSE, number, pToMinor->GetRaceName());
			}
			else if (pInfo->m_nResources[DEUTERIUM] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[DEUTERIUM]);
				s = sEmpireName + " " + CLoc::GetString("DEUTERIUM_PRESENT", FALSE, number, pToMinor->GetRaceName());
			}
			else if (pInfo->m_nResources[DURANIUM] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[DURANIUM]);
				s = sEmpireName + " " + CLoc::GetString("DURANIUM_PRESENT", FALSE, number, pToMinor->GetRaceName());
			}
			else if (pInfo->m_nResources[CRYSTAL] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[CRYSTAL]);
				s = sEmpireName + " " + CLoc::GetString("CRYSTAL_PRESENT", FALSE, number, pToMinor->GetRaceName());
			}
			else if (pInfo->m_nResources[IRIDIUM] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[IRIDIUM]);
				s = sEmpireName + " " + CLoc::GetString("IRIDIUM_PRESENT", FALSE, number, pToMinor->GetRaceName());
			}
			else if (pInfo->m_nResources[DERITIUM] > 0)
			{
				CString number;
				number.Format("%d", pInfo->m_nResources[DERITIUM]);
				s = sEmpireName + " " + CLoc::GetString("DERITIUM_PRESENT", FALSE, number, pToMinor->GetRaceName());
			}
		}
		// Nachricht machen, das wir eine Bestechung versuchen
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::CORRUPTION)
		{
			s = sEmpireName + " " + CLoc::GetString("TRY_CORRUPTION", FALSE, pToMinor->GetRaceName());
		}
		// Nachricht machen, das wir Vertrag aufgehoben haben
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::NONE)
		{
			s = sEmpireName + " " + CLoc::GetString("CANCEL_CONTRACT", FALSE, pToMinor->GetRaceName());
			pFromMajor->SetAgreement(pToMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
			pToMinor->SetAgreement(pFromMajor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
		}
		// Nachricht erstellen, dass Krieg erkl�rt wurde
		else if (pInfo->m_nType == DIPLOMATIC_AGREEMENT::WAR)
		{
			// Krieg erkl�ren
			DeclareWar(pFromMajor, pToMinor, pInfo, true);

			// aufgrund diplomatischer Beziehungen k�nnte so weiter Krieg erkl�rt werden
			std::vector<CString> vEnemies;
			vEnemies = GetEnemiesFromContract(pDoc, pFromMajor, pToMinor);
			// allen weiteren Gegnern den Krieg erkl�ren
			for (UINT i = 0; i < vEnemies.size(); i++)
			{
				CRace* pEnemy = pDoc->GetRaceCtrl()->GetRace(vEnemies[i]);
				if (pEnemy)
				{
					CDiplomacyInfo war = *pInfo;
					// lediglich die Zielrasse und den Krieggrund anpassen
					war.m_sToRace		= pEnemy->GetRaceID();
					war.m_sWarPartner	= pToMinor->GetRaceID();
					DeclareWar(pFromMajor, pEnemy, &war, false);
				}
			}
		}
		// Nachricht machen, das wir einen Vertrag angeboten haben
		else
		{
			CString sAgreement;
			switch(pInfo->m_nType)
			{
			case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CLoc::GetString("TRADE_AGREEMENT_WITH_ARTICLE");	break;}
			case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	{sAgreement = CLoc::GetString("FRIENDSHIP_WITH_ARTICLE");		break;}
			case DIPLOMATIC_AGREEMENT::COOPERATION:	{sAgreement = CLoc::GetString("COOPERATION_WITH_ARTICLE");		break;}
			case DIPLOMATIC_AGREEMENT::AFFILIATION:	{sAgreement = CLoc::GetString("AFFILIATION_WITH_ARTICLE");		break;}
			case DIPLOMATIC_AGREEMENT::MEMBERSHIP:	{sAgreement = CLoc::GetString("MEMBERSHIP_WITH_ARTICLE");		break;}
			}
			s = sEmpireName + " " + CLoc::GetString("OUR_MIN_OFFER", FALSE, pToMinor->GetRaceName(), sAgreement);
		}

		if (!s.IsEmpty())
		{
			message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
			pFromMajor->GetEmpire()->AddMsg(message);
		}
	}
}

/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots f�r eine Minorrace.
/// @param pDoc Zeiger auf das Dokument
/// @param pToMajor Zeiger auf die Empf�ngerrasse
/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
void CDiplomacyController::ReceiveToMinor(CBotEDoc* pDoc, CMinor* pToMinor, CDiplomacyInfo* pInfo)
{
	// nur Reaktion beim diplomatischen Angebot
	if (pInfo->m_nFlag != DIPLOMACY_OFFER)
		return;

	// Die Rasse welche die Nachricht abgeschickt hat
	CMajor* pFromMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pInfo->m_sFromRace));
	if (!pFromMajor)
		return;

	////////////////////////////////////////////////////////////////////////////////////////
	// das Angebot geht an einen Minor
	////////////////////////////////////////////////////////////////////////////////////////
	if (pInfo->m_nSendRound == pDoc->GetCurrentRound() - 2)
	{
		pToMinor->ReactOnOfferAI(pInfo);

		if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::PRESENT && pInfo->m_nType != DIPLOMATIC_AGREEMENT::WAR)
		{
			if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::NONE)
			{
				CDiplomacyInfo answer = *pInfo;
				answer.m_nSendRound = pDoc->GetCurrentRound();
				CGenDiploMessage::GenerateMinorAnswer(answer);
				pFromMajor->GetIncomingDiplomacyNews()->push_back(answer);
			}

			// Minor hat angenommen
			if (pInfo->m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
			{
				CString sEventText = "";

				// wenn es keine Bestechung ist
				if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::CORRUPTION)
				{
					CString sAgreement;
					switch (pInfo->m_nType)
					{
					case DIPLOMATIC_AGREEMENT::TRADE:		{sAgreement = CLoc::GetString("TRADE_AGREEMENT_WITH_ARTICLE");	break;}
					case DIPLOMATIC_AGREEMENT::FRIENDSHIP:{sAgreement = CLoc::GetString("FRIENDSHIP_WITH_ARTICLE");		break;}
					case DIPLOMATIC_AGREEMENT::COOPERATION:			{sAgreement = CLoc::GetString("COOPERATION_WITH_ARTICLE");		break;}
					case DIPLOMATIC_AGREEMENT::AFFILIATION:			{sAgreement = CLoc::GetString("AFFILIATION_WITH_ARTICLE");		break;}
					case DIPLOMATIC_AGREEMENT::MEMBERSHIP:			{sAgreement = CLoc::GetString("MEMBERSHIP_WITH_ARTICLE");		break;}
					}
					// nur Text bei Vertragsformen erstellen
					if (!sAgreement.IsEmpty())
					{
						CString	s = CLoc::GetString("FEMALE_ARTICLE", TRUE) + " " + pToMinor->GetRaceName() + " " + CLoc::GetString("MIN_ACCEPT_OFFER", FALSE, sAgreement, pFromMajor->GetEmpireNameWithAssignedArticle());
						CEmpireNews message;
						message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
						pFromMajor->GetEmpire()->AddMsg(message);
					}
					pToMinor->SetAgreement(pFromMajor->GetRaceID(), pInfo->m_nType);
					pFromMajor->SetAgreement(pToMinor->GetRaceID(), pInfo->m_nType);

					// Moralevent einplanen
					switch (pInfo->m_nType)
					{
					case DIPLOMATIC_AGREEMENT::TRADE: sEventText = pFromMajor->GetMoralObserver()->AddEvent(34, pFromMajor->GetRaceMoralNumber(), pToMinor->GetRaceName());	break;
					// Sign Friendship/Cooperation Treaty #35
					case DIPLOMATIC_AGREEMENT::FRIENDSHIP: sEventText = pFromMajor->GetMoralObserver()->AddEvent(35, pFromMajor->GetRaceMoralNumber(), pToMinor->GetRaceName()); break;
					// Sign Friendship/Cooperation Treaty #35
					case DIPLOMATIC_AGREEMENT::COOPERATION: sEventText = pFromMajor->GetMoralObserver()->AddEvent(35, pFromMajor->GetRaceMoralNumber(), pToMinor->GetRaceName());	break;
					// Sign an Affiliation Treaty #36
					case DIPLOMATIC_AGREEMENT::AFFILIATION: sEventText = pFromMajor->GetMoralObserver()->AddEvent(36, pFromMajor->GetRaceMoralNumber(), pToMinor->GetRaceName());	break;
					// Sign a Membership #10
					case DIPLOMATIC_AGREEMENT::MEMBERSHIP: sEventText = pFromMajor->GetMoralObserver()->AddEvent(10, pFromMajor->GetRaceMoralNumber() , pToMinor->GetRaceName());	break;
					}

					if (!sEventText.IsEmpty())
					{
						CEmpireNews message;
						message.CreateNews(sEventText, EMPIRE_NEWS_TYPE::SOMETHING);
						pFromMajor->GetEmpire()->AddMsg(message);
					}
				}

				// �bergebene Ressourcen ins System der Minor einlagern
				CPoint pt = pToMinor->GetRaceKO();
				if (pt != CPoint(-1,-1))
				{
					for (int res = TITAN; res <= DERITIUM; res++)
						pDoc->GetSystem(pt.x, pt.y).SetResourceStore(res, pInfo->m_nResources[res]);
				}
			}
			else if (pInfo->m_nAnswerStatus == ANSWER_STATUS::DECLINED)
			{
				if (pInfo->m_nType != DIPLOMATIC_AGREEMENT::CORRUPTION)
				{
					CString s = "";
					CString sAgreement = "";
					switch (pInfo->m_nType)
					{
						case DIPLOMATIC_AGREEMENT::TRADE:		sAgreement = CLoc::GetString("TRADE_AGREEMENT"); break;
						case DIPLOMATIC_AGREEMENT::FRIENDSHIP:	sAgreement = CLoc::GetString("FRIENDSHIP"); break;
						case DIPLOMATIC_AGREEMENT::COOPERATION:	sAgreement = CLoc::GetString("COOPERATION"); break;
						case DIPLOMATIC_AGREEMENT::AFFILIATION:	sAgreement = CLoc::GetString("AFFILIATION"); break;
						case DIPLOMATIC_AGREEMENT::MEMBERSHIP:	sAgreement = CLoc::GetString("MEMBERSHIP"); break;
					}

					s = CLoc::GetString("MIN_DECLINE_OFFER", FALSE, pToMinor->GetRaceName(), sAgreement);
					CEmpireNews message;
					message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
					pFromMajor->GetEmpire()->AddMsg(message);

					// Wenn das Angebot abgelehnt wurde, dann bekommt die Majorrace, die es gemacht hat
					// ihre Ressourcen und ihre Credits wieder zur�ck, sofern sie es als Anreiz mit zum Vertrags-
					// angebot gemacht haben
					pFromMajor->GetEmpire()->SetCredits(pInfo->m_nCredits);
					for (int res = TITAN; res <= DERITIUM; res++)
					{
						CPoint pt = pInfo->m_ptKO;
						if (pt != CPoint(-1,-1))
							if (pDoc->GetSystem(pt.x, pt.y).GetOwnerOfSystem() == pFromMajor->GetRaceID())
								pDoc->GetSystem(pt.x, pt.y).SetResourceStore(res, pInfo->m_nResources[res]);
					}
				}
			}
		}
	}
}

/// Funktion zum Berechnen aller betroffenen Rassen, welchen zus�tzlich der Krieg erkl�rt wird.
/// Dies geschieht dadurch, wenn der Kriegsgegner B�ndnisse oder Verteidigungspakt mit anderen Rassen besitzt.
/// @param pDoc Zeiger auf das Dokument
/// @param pFromMajor Zeiger auf die kriegserkl�rende Majorrace
/// @param pToRace Zeiger auf die Rasse, welcher der Krieg erkl�rt wurde
/// @return Feld mit allen betroffenen Rassen, denen auch der Krieg erkl�rt werden muss
std::vector<CString> CDiplomacyController::GetEnemiesFromContract(CBotEDoc* pDoc, CMajor* pFromMajor, CRace* pToRace)
{
	std::vector<CString> vEnemies;
	// Wenn wir jemanden den Krieg erkl�ren und dieser Jemand ein B�ndnis oder einen
	// Verteidigungspakt mit einer anderen Majorrace hat, so erkl�ren wir der anderen Majorrace
	// auch den Krieg. Au�erdem erkl�ren wir auch jeder Minorrace den Krieg, mit der der Jemand
	// mindst. ein B�ndnis hatte.
	map<CString, CRace*>* mRaces = pDoc->GetRaceCtrl()->GetRaces();
	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); ++it)
	{
		// nicht wir selbst
		if (it->first != pToRace->GetRaceID())
		{
			// hat die Rasse mit der anderen Rasse ein B�ndnis oder einen Verteidigungspakt
			if (pToRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::AFFILIATION || (pToRace->IsMajor() && dynamic_cast<CMajor*>(pToRace)->GetDefencePact(it->first)))
			{
				// haben wir nicht schon Krieg mit dem anderen Major
				if (pFromMajor->GetAgreement(it->first) != DIPLOMATIC_AGREEMENT::WAR)
				{
					vEnemies.push_back(it->first);
					MYTRACE("general")(MT::LEVEL_INFO, "Race: %s declares Race: %s WAR because affiliation contract with Race %s", pFromMajor->GetRaceID(), it->first, pToRace->GetRaceID());
				}
			}
		}
	}

	return vEnemies;
}

/// Funktion erkl�rt den Krieg zwischen zwei Rassen und nimmt dabei alle notwendigen Arbeiten vor.
/// @param pFromRace Zeiger auf die kriegserkl�rende Rasse
/// @param pEnemy Zeiger auf die Rasse, welcher Krieg erkl�rt wird
/// @param pInfo Diplomatieobjekt
/// @param bWithMoralEvent <code>true</code> wenn Moralevent mit eingeplant werden soll
void CDiplomacyController::DeclareWar(CRace* pFromRace, CRace* pEnemy, CDiplomacyInfo* pInfo, bool bWithMoralEvent)
{
	// haben wir schon Krieg, so kann keiner nochmal erkl�rt werden
	if (pFromRace->GetAgreement(pEnemy->GetRaceID()) == DIPLOMATIC_AGREEMENT::WAR)
		return;

	CString s;
	CEmpireNews message;

	if (pFromRace->IsMajor())
	{
		CMajor* pFromMajor = dynamic_cast<CMajor*>(pFromRace);
		assert(pFromMajor);
		if (pEnemy->IsMajor())
			s = CLoc::GetString("WE_DECLARE_WAR", FALSE, dynamic_cast<CMajor*>(pEnemy)->GetEmpireNameWithAssignedArticle());
		else if (pEnemy->IsMinor())
			s = CLoc::GetString("WE_DECLARE_WAR_TO_MIN", FALSE, pEnemy->GetRaceName());

		message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
		pFromMajor->GetEmpire()->AddMsg(message);

		if (bWithMoralEvent)
		{
			// zus�tzliche Eventnachricht wegen der Moral an das Imperium
			CString sEventText = "";
			short nAgreement = pFromRace->GetAgreement(pEnemy->GetRaceID());
			CString sParam = CLoc::GetString("FEMALE_ARTICLE") + " " + pEnemy->GetRaceName();
			if (pEnemy->IsMajor())
				sParam = dynamic_cast<CMajor*>(pEnemy)->GetEmpireNameWithArticle();

			// Declare War on an Empire with Defense Pact #28 (nur, wenn wir einen Vertrag kleiner als den der
			// Kooperation haben und dazu auch noch einen Verteidigungspakt)
			if (pEnemy->IsMajor() && nAgreement < DIPLOMATIC_AGREEMENT::COOPERATION && pFromMajor->GetDefencePact(pEnemy->GetRaceID()) == true)
				sEventText = pFromMajor->GetMoralObserver()->AddEvent(28, pFromMajor->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire when Neutral #24
			else if (nAgreement == DIPLOMATIC_AGREEMENT::NONE)
				sEventText = pFromMajor->GetMoralObserver()->AddEvent(24, pFromMajor->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire when Non-Aggression #25
			else if (nAgreement == DIPLOMATIC_AGREEMENT::NAP)
				sEventText = pFromMajor->GetMoralObserver()->AddEvent(25, pFromMajor->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire with Trade Treaty #26
			else if (nAgreement == DIPLOMATIC_AGREEMENT::TRADE)
				sEventText = pFromMajor->GetMoralObserver()->AddEvent(26, pFromMajor->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire with Friendship Treaty #27
			else if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				sEventText = pFromMajor->GetMoralObserver()->AddEvent(27, pFromMajor->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire with CooperationTreaty #29
			else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
				sEventText = pFromMajor->GetMoralObserver()->AddEvent(29, pFromMajor->GetRaceMoralNumber(), sParam);
			// Declare War on an Empire with Affiliation #30
			else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
				sEventText = pFromMajor->GetMoralObserver()->AddEvent(30, pFromMajor->GetRaceMoralNumber(), sParam);

			if (!sEventText.IsEmpty())
			{
				message.CreateNews(sEventText, EMPIRE_NEWS_TYPE::SOMETHING);
				pFromMajor->GetEmpire()->AddMsg(message);
			}
		}

		if (pEnemy->IsMajor())
		{
			CMajor* pEnemyMajor = dynamic_cast<CMajor*>(pEnemy);
			// Antwort auf Kriegserkl�rung erstellen
			CDiplomacyInfo answer = *pInfo;
			CGenDiploMessage::GenerateMajorAnswer(answer);
			pFromMajor->GetIncomingDiplomacyNews()->push_back(answer);

			// Imperiumsnamen inkl. Artikel holen
			CString sEmpireArticleName = pFromMajor->GetEmpireNameWithArticle();
			// gro� beginnen
			CString sUpper = (CString)sEmpireArticleName.GetAt(0);
			sEmpireArticleName.SetAt(0, sUpper.MakeUpper().GetAt(0));

			// ging die Kriegserkl�rung direkt an uns (also nicht indirekt durch diplomatische Pakte)
			if (pInfo->m_sWarPartner.IsEmpty())
			{
				s = CLoc::GetString("WE_GET_WAR", FALSE, sEmpireArticleName);
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
				pEnemyMajor->GetEmpire()->AddMsg(message);
			}
			// wurde uns der Krieg aufgrund einer diplomatischen Beziehung indirekt erkl�rt
			else
			{
				// Nachricht und Kriegserkl�rung wegen unserem B�ndnispartner
				CBotEDoc* pDoc = resources::pDoc;
				ASSERT(pDoc);
				CRace* pPartner = pDoc->GetRaceCtrl()->GetRace(pInfo->m_sWarPartner);
				if (!pPartner)
					return;

				s = sEmpireArticleName + " " + CLoc::GetString("WAR_TO_PARTNER", FALSE, pPartner->GetRaceName());
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
				pEnemyMajor->GetEmpire()->AddMsg(message);

				s = sEmpireArticleName + " " + CLoc::GetString("WAR_TO_US_AS_PARTNER");
				message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
				pEnemyMajor->GetEmpire()->AddMsg(message);
			}
		}
		else if (pEnemy->IsMinor())
		{
			// Antwort auf Kriegserkl�rung erstellen
			CDiplomacyInfo answer = *pInfo;
			CGenDiploMessage::GenerateMinorAnswer(answer);
			pFromMajor->GetIncomingDiplomacyNews()->push_back(answer);
		}
	}
	else if (pFromRace->IsMinor())
	{
		s = CLoc::GetString("MIN_OFFER_WAR", FALSE, pFromRace->GetRaceName());
		message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY, 2);
		// Kriegserkl�rung von Minor kann nur an Majors gehen
		dynamic_cast<CMajor*>(pEnemy)->GetEmpire()->AddMsg(message);
	}

	// Moral f�r den, dem Krieg erkl�rt wurde (braucht nur bei Majors gemacht werden)
	if (pEnemy->IsMajor())
	{
		CMajor* pEnemyMajor = dynamic_cast<CMajor*>(pEnemy);
		assert(pEnemyMajor);
		// zus�tzliche Eventnachricht wegen der Moral an das Imperium
		CString sParam = CLoc::GetString("FEMALE_ARTICLE") + " " + pFromRace->GetRaceName();
		if (pFromRace->IsMajor())
			sParam = dynamic_cast<CMajor*>(pFromRace)->GetEmpireNameWithArticle();
		CString sEventText = "";
		short nAgreement = pFromRace->GetAgreement(pEnemy->GetRaceID());
		// Other Empire Declares War when Neutral #31
		if (nAgreement == DIPLOMATIC_AGREEMENT::NONE)
			sEventText = pEnemyMajor->GetMoralObserver()->AddEvent(31, pEnemyMajor->GetRaceMoralNumber(), sParam);
		// Other Empire Declares War with an Affiliation (or Membership) #33
		else if (nAgreement >= DIPLOMATIC_AGREEMENT::AFFILIATION)
			sEventText = pEnemyMajor->GetMoralObserver()->AddEvent(33, pEnemyMajor->GetRaceMoralNumber(), sParam);
		// Other Empire Declares War with Treaty #32
		else
			sEventText = pEnemyMajor->GetMoralObserver()->AddEvent(32, pEnemyMajor->GetRaceMoralNumber(), sParam);

		if (!sEventText.IsEmpty())
		{
			message.CreateNews(sEventText, EMPIRE_NEWS_TYPE::SOMETHING);
			pEnemyMajor->GetEmpire()->AddMsg(message);
		}

		// Angebot in den Nachrichteneingang legen
		pEnemyMajor->GetIncomingDiplomacyNews()->push_back(*pInfo);
	}

	// im Kriegsfall sofort Krieg erkl�ren und Beziehung verschlechtern
	// Beziehungen runtersetzen
	pFromRace->SetRelation(pEnemy->GetRaceID(), -100);
	pEnemy->SetRelation(pFromRace->GetRaceID(), -100);
	// Vertragsform auf Krieg setzen
	pFromRace->SetAgreement(pEnemy->GetRaceID(), DIPLOMATIC_AGREEMENT::WAR);
	pEnemy->SetAgreement(pFromRace->GetRaceID(), DIPLOMATIC_AGREEMENT::WAR);
}
