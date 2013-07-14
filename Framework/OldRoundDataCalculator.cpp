#include "stdafx.h"
#include "OldRoundDataCalculator.h"

#include "Races/RaceController.h"
#include "General/Loc.h"
#include <cassert>


COldRoundDataCalculator::COldRoundDataCalculator(void)
{
}

COldRoundDataCalculator::~COldRoundDataCalculator(void)
{
}

COldRoundDataCalculator::COldRoundDataCalculator(CBotEDoc* pDoc)
{
	m_pDoc = pDoc;
}

void COldRoundDataCalculator::CreditsDestructionMoral(CMajor* pMajor, CSystem& system,
		const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo, float fDifficultyLevel) {
	// spielt es der Computer, so bekommt er etwas mehr Credits
	CSystemProd* prod = system.GetProduction();
	CEmpire* pEmpire = pMajor->GetEmpire();
	if (!pMajor->AHumanPlays())
		pEmpire->SetCredits(static_cast<int>(prod->GetCreditsProd() / fDifficultyLevel));
	else
		pEmpire->SetCredits(prod->GetCreditsProd());

		CString s;
		s.Format("%s", pMajor->GetEmpire());
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "ProductionBeforeDifficulty: %i, Owner: %s, fDifficultyLevel:%.2lf, ProductionAfterDifficulty: %i,\n", 
			prod->GetCreditsProd(), system.GetOwnerOfSystem(), fDifficultyLevel, int(prod->GetCreditsProd() / fDifficultyLevel));

	// Hier die Geb�ude abrei�en, die angeklickt wurden
	if (system.DestroyBuildings())
		system.CalculateNumberOfWorkbuildings(&BuildingInfo);

	// Variablen berechnen lassen, bevor der Planet w�chst -> diese ins Lager
	// nur berechnen, wenn das System auch jemandem geh�rt, ansonsten w�rde auch die Mind.Prod. ins Lager kommen
	// In CalculateStorages wird auch die Systemmoral berechnet. Wenn wir einen Auftrag mit
	// NeverReady (z.B. Kriegsrecht) in der Bauliste haben, und dieser Moral produziert, dann diese
	// auf die Moral anrechnen. Das wird dann in CalculateStorages gemacht.
	int list = system.GetAssemblyList()->GetAssemblyListEntry(0);
	if (list > 0 && list < 10000 && BuildingInfo[list-1].GetNeverReady() && system.GetMoral() <= 85)
		prod->AddMoralProd(BuildingInfo[list-1].GetMoralProd());
}

int COldRoundDataCalculator::DeritiumForTheAI(bool human, const CSector& /*sector*/, const CSystem& system, float fDifficultyLevel) {
	// KI Anpassungen (KI bekommt zuf�lig etwas Deritium geschenkt)
	int diliAdd = 0;
	if (!human && system.GetProduction()->GetDeritiumProd() == 0)
	{
		// umso h�her der Schwierigkeitsgrad, desto h�her die Wahrscheinlichkeit, das die KI
		// Deritium auf ihrem Systemen geschenkt bekommt
		int temp = rand()%((int)(fDifficultyLevel * 7.5));
		//rainer 
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "KI: System: %s - DiliAddProb: %f (NULL for adding Dili) - Difficulty: %i\n", system.GetOwnerOfSystem(), temp, fDifficultyLevel);
		//TRACE("KI: System: %s - DiliAddProb: %d (NULL for adding Dili) - Difficulty: %.2lf\n",sector.GetName(), temp, fDifficultyLevel);
		if (temp == 0)
			diliAdd = 1;
	}
	return diliAdd;
}

void COldRoundDataCalculator::ExecuteRebellion(CSector& sector, CSystem& system, CMajor* pMajor) const
{
	CEmpire* pEmpire = pMajor->GetEmpire();
	const CPoint& co = sector.GetKO();
	const CString& sectorname = sector.GetName();
	const CRaceController* pRaceCtrl = m_pDoc->GetRaceCtrl();

	// Nachricht �ber Rebellion erstellen
	CString news = CLoc::GetString("REBELLION_IN_SYSTEM", FALSE, sectorname);
	CEmpireNews message;
	message.CreateNews(news, EMPIRE_NEWS_TYPE::SOMETHING, "", co);
	pEmpire->AddMsg(message);
	if (pMajor->IsHumanPlayer())
	{
		const network::RACE client = pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
		m_pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
	}

	// zus�tzliche Eventnachricht (Lose a System to Rebellion #18) wegen der Moral an das Imperium
	message.CreateNews(pMajor->GetMoralObserver()->AddEvent(18, pMajor->GetRaceMoralNumber(), sectorname), EMPIRE_NEWS_TYPE::SOMETHING, "", co);
	pEmpire->AddMsg(message);

	if (sector.GetMinorRace())
	{
		CMinor* pMinor = pRaceCtrl->GetMinorRace(sectorname);
		assert(pMinor);
		sector.SetOwnerOfSector(pMinor->GetRaceID());

		if (sector.GetTakenSector() == FALSE)
		{
			pMinor->SetAgreement(pMajor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);
			pMajor->SetAgreement(pMinor->GetRaceID(), DIPLOMATIC_AGREEMENT::NONE);

			pMinor->SetRelation(pMajor->GetRaceID(), (-(rand()%50+20)));
			news = CLoc::GetString("MINOR_CANCELS_MEMBERSHIP", FALSE, pMinor->GetRaceName());
			CEmpireNews message;
			message.CreateNews(news, EMPIRE_NEWS_TYPE::DIPLOMACY, "", co);
			pEmpire->AddMsg(message);
		}
	}
	else
	{
		sector.SetOwnerOfSector("");
	}

	// wichtige Variablen zur�cksetzen
	sector.SetOwned(FALSE);
	sector.SetTakenSector(FALSE);
	sector.SetShipPort(FALSE, pMajor->GetRaceID());
	system.SetOwnerOfSystem("");
}

void COldRoundDataCalculator::ExecuteFamine(CSector& sector, CSystem& system, CMajor* pMajor) const
{
	const CPoint& co = sector.GetKO();
	CEmpire* pEmpire = pMajor->GetEmpire();
	sector.LetPlanetsShrink((float)(system.GetFoodStore()) * 0.01f);
	// nur wenn die Moral �ber 50 ist sinkt die Moral durch Hungersn�te
	if (system.GetMoral() > 50)
	{
		system.SetMoral((short)(system.GetFoodStore() / (system.GetHabitants() + 1))); // +1, wegen Division durch NULL umgehen
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "OldRoundDataCalc.CPP: Famine: Morale goes down %d\n", (short)(system.GetFoodStore() / (system.GetHabitants() + 1)));
	}
	system.SetFoodStore(0);

	CString news = CLoc::GetString("FAMINE", FALSE, sector.GetName());
	CEmpireNews message;
	message.CreateNews(news, EMPIRE_NEWS_TYPE::SOMETHING, "", co, false, 1);
	pEmpire->AddMsg(message);
	if (pMajor->IsHumanPlayer())
	{
		const network::RACE client = m_pDoc->m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
		m_pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
	}
}

void COldRoundDataCalculator::SystemMessage(const CSector& sector, CMajor* pMajor, const CString& key,
		EMPIRE_NEWS_TYPE::Typ message_typ, BYTE byFlag) const {
	const CString& news = CLoc::GetString(key, FALSE, sector.GetName());
	CEmpireNews message;
	message.CreateNews(news, message_typ, "", sector.GetKO(), false, byFlag);
	pMajor->GetEmpire()->AddMsg(message);
	if (pMajor->IsHumanPlayer()) {
		const network::RACE client = m_pDoc->m_pRaceCtrl->GetMappedClientID(pMajor->GetRaceID());
		m_pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
	}
}

void COldRoundDataCalculator::HandlePopulationEffects(const CSector& sector, CSystem& system, CMajor* pMajor) const {
	const float fCurrentHabitants = sector.GetCurrentHabitants();
	CEmpire* pEmpire = pMajor->GetEmpire();
	pEmpire->AddPopSupportCosts((UINT)fCurrentHabitants * POPSUPPORT_MULTI);
	// Funktion gibt TRUE zur�ck, wenn wir durch die Bev�lkerung eine neue Handelsroute anlegen k�nnen
	if (system.SetHabitants(fCurrentHabitants))
	{
		// wenn die Spezialforschung "mindestens 1 Handelsroute erforscht wurde, dann die Meldung erst bei
		// der 2.ten Handelroute bringen
		// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
		bool bMinOneRoute = pEmpire->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED;
		if (bMinOneRoute == false || (bMinOneRoute == true && (int)(system.GetHabitants() / TRADEROUTEHAB) > 1))
			SystemMessage(sector, pMajor, "ENOUGH_HABITANTS_FOR_NEW_TRADEROUTE", EMPIRE_NEWS_TYPE::ECONOMY, 4);
	}
}

static void MilitaryMessage(const CSector& sector, CMajor* pMajor, const CString& key,
		const CString& object_name) {
	const CString& s = CLoc::GetString(key,FALSE,
		object_name,sector.GetName());
	CEmpireNews message;
	message.CreateNews(s,EMPIRE_NEWS_TYPE::MILITARY,sector.GetName(),sector.GetKO());
	pMajor->GetEmpire()->AddMsg(message);
}

void COldRoundDataCalculator::FinishBuild(const int to_build, const CSector& sector, CSystem& system,
		CMajor* pMajor, const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo) const {
	int list = to_build;
	const CPoint& co = sector.GetKO();
	// Ab jetzt die Abfrage ob Geb�ude oder ein Update fertig wurde
	if (list > 0 && list < 10000 && !BuildingInfo[list-1].GetNeverReady())	// Es wird ein Geb�ude gebaut
	{
		// Die Nachricht, dass neues Geb�ude fertig ist mit allen Daten generieren
		CEmpireNews message;
		message.CreateNews(BuildingInfo[list-1].GetBuildingName(), EMPIRE_NEWS_TYPE::ECONOMY, sector.GetName(), co);
		pMajor->GetEmpire()->AddMsg(message);
		// Geb�ude bauen
		m_pDoc->BuildBuilding(list, co);
		// und Geb�ude (welches letztes im Feld) ist auch gleich online setzen, wenn
		// gen�gend Arbeiter da sind
		unsigned short CheckValue = system.SetNewBuildingOnline(&BuildingInfo);
		// Nachricht generieren das das Geb�ude nicht online genommen werden konnte
		if (CheckValue == 1)
			SystemMessage(sector, pMajor, "NOT_ENOUGH_WORKER", EMPIRE_NEWS_TYPE::SOMETHING, 1);
		else if (CheckValue == 2)
			SystemMessage(sector, pMajor, "NOT_ENOUGH_ENERGY", EMPIRE_NEWS_TYPE::SOMETHING, 2);
	}
	else if (list < 0)	// Es wird ein Update gemacht
	{
		list *= (-1);
		// Die Nachricht, dass neues Geb�udeupdate fertig wurde, mit allen Daten generieren
		CEmpireNews message;
		message.CreateNews(BuildingInfo[list-1].GetBuildingName(),EMPIRE_NEWS_TYPE::ECONOMY,sector.GetName(),co,true);
		pMajor->GetEmpire()->AddMsg(message);
		// Vorg�nger von "list" holen
		// Geb�ude mit RunningNumbner == nPredecessorID werden durch UpdateBuilding() gel�scht und
		// deren Anzahl wird zur�ckgegeben.
		USHORT nPredecessorID = BuildingInfo[list-1].GetPredecessorID();
		ASSERT(nPredecessorID > 0);
		const CBuildingInfo* pPredecessorBuilding = &(BuildingInfo[nPredecessorID - 1]);
		ASSERT(pPredecessorBuilding->GetRunningNumber() == nPredecessorID);
		int nNumberOfNewBuildings = system.UpdateBuildings(nPredecessorID, pPredecessorBuilding->GetNeededEnergy());
		// So, nun bauen wir so viel mal das n�chste
		for (int z = 0; z < nNumberOfNewBuildings; z++)
		{
			m_pDoc->BuildBuilding(list,co);

			// falls das geupgradete Geb�ude Energie ben�tigt wird versucht es gleich online zu setzen
			if (m_pDoc->GetBuildingInfo(list).GetNeededEnergy() > 0 && system.SetNewBuildingOnline(&BuildingInfo) == 2)
				SystemMessage(sector, pMajor, "NOT_ENOUGH_ENERGY", EMPIRE_NEWS_TYPE::SOMETHING, 2);
		}
	}
	else if (list >= 10000 && list < 20000)	// Es wird ein Schiff gebaut
	{
		m_pDoc->BuildShip(list, co, pMajor->GetRaceID());
		MilitaryMessage(sector, pMajor, "SHIP_BUILDING_FINISHED", m_pDoc->m_ShipInfoArray[list-10000].GetShipTypeAsString());
	}
	else if (list >= 20000)					// Es wird eine Truppe gebaut
	{
		m_pDoc->BuildTroop(list-20000, co);
		MilitaryMessage(sector, pMajor, "TROOP_BUILDING_FINISHED", m_pDoc->m_TroopInfo[list-20000].GetName());
	}
}

static void CreditsIfBought(CAssemblyList* assembly_list, const int IPProd, CEmpire* pEmpire) {
	// Wenn Geb�ude gekauft wurde, dann die in der letzten Runde noch erbrachte IP-Leistung
	// den Credits des Imperiums gutschreiben, IP-Leistung darf aber nicht gr��er der Baukosten sein
	if (assembly_list->GetWasBuildingBought() == TRUE)
	{
		int goods = IPProd;
		if (goods > assembly_list->GetBuildCosts())
			goods = assembly_list->GetBuildCosts();
		pEmpire->SetCredits(goods);
		assembly_list->SetWasBuildingBought(FALSE);
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "OldRoundDataCalculator.cpp: wasBought=TRUE: IPProd was %d, BuildCost: %d\n", 
			IPProd, assembly_list->GetBuildCosts());
	}
}

void COldRoundDataCalculator::Build(const CSector& sector, CSystem& system, CMajor* pMajor,
		const CArray<CBuildingInfo,CBuildingInfo>& BuildingInfo) const {
	// Hier berechnen, wenn was in der Bauliste ist, und ob es fertig wird usw.
	CAssemblyList* assembly_list = system.GetAssemblyList();
	const int list = assembly_list->GetAssemblyListEntry(0);
	if (list != 0)	// wenn was drin ist
	{
		const int IPProd = system.CalcIPProd(BuildingInfo, list);
		// Ein Bauauftrag ist fertig gestellt worden
		if (assembly_list->CalculateBuildInAssemblyList(IPProd))
		{
			CreditsIfBought(assembly_list, IPProd, pMajor->GetEmpire());
			//MYTRACE("logdata")(MT::LEVEL_DEBUG, "assembly_list %s\n", assembly_list);
			FinishBuild(list, sector, system, pMajor, BuildingInfo);
			// Nach CalculateBuildInAssemblyList wird ClearAssemblyList() aufgerufen, wenn der Auftrag fertig wurde.
			// Normalerweise wird nach ClearAssemblyList() die Funktion CalculateVariables() aufgerufen, wegen Geld durch
			// Handelsg�ter wenn nix mehr drin steht. Hier mal testweise weggelassen, weil diese Funktion
			// sp�ter eh f�r das System aufgerufen wird und wir bis jetzt glaub ich keine Notwendigkeit
			// haben die Funktion CalculateVariables() aufzurufen.
			assembly_list->ClearAssemblyList(sector.GetKO(), m_pDoc->m_Systems);
			// Wenn die Bauliste nach dem letzten gebauten Geb�ude leer ist, eine Nachricht generieren
			if (assembly_list->GetAssemblyListEntry(0) == 0)
				SystemMessage(sector, pMajor, "EMPTY_BUILDLIST", EMPIRE_NEWS_TYPE::SOMETHING, 0);
		}
		assembly_list->CalculateNeededRessourcesForUpdate(&BuildingInfo, system.GetAllBuildings(), pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
	}
}
