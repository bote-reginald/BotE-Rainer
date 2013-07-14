#include "stdafx.h"
#include "AttackSystem.h"
#include "Ships/Ships.h"

#include "Ships/ShipMap.h"
#include "General/Loc.h"
#include "Galaxy/Sector.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CAttackSystem::CAttackSystem(void) :
	m_KO(-1, -1),
	m_pDefender(),
	m_pSystem(),
	m_pSector(),
	m_pBuildingInfos(),
	m_sMonopolOwner(),
	m_bTroopsInvolved(FALSE),
	m_bAssultShipInvolved(FALSE),
	m_fKilledPop(0.0f),
	m_iDestroyedBuildings(0)
{
}

CAttackSystem::~CAttackSystem(void)
{
	m_pShips.RemoveAll();
	m_pTroops.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion initiiert das CAttackSystem Objekt mit den entsprechenden Variablen. Dabei �bernimmt sie als
/// Parameter einen Zeiger auf die verteidigende Rasse <code>pDefender</code>, einen Zeiger auf das System <code>system</code>,
/// welches angegriffen wird, einen Zeiger auf das komplette Feld aller Schiffe <code>ships</code>, einen Zeiger auf den
/// zum System geh�renden Sektor <code>sector</code>, einen Zeiger auf die Geb�udeinformationen <code>buildingInfos</code>
/// und das Feld mit den Monopolbesitzern <code>monopolOwner</code>.
void CAttackSystem::Init(CRace* pDefender, CSystem* system, CShipMap* ships, CSector* sector, BuildingInfoArray* buildingInfos, const CString* monopolOwner)
{
	m_pDefender = pDefender;
	m_pSystem = system;
	m_pSector = sector;
//	MYTRACE("general")(MT::LEVEL_INFO, "System Attack: Defender %s, System %s, Sector %s\n", pDefender, system, sector);
	m_pBuildingInfos = buildingInfos;
	m_KO = sector->GetKO();;
	m_sMonopolOwner = monopolOwner;

	m_bTroopsInvolved = FALSE;
	m_bAssultShipInvolved = FALSE;
	m_fKilledPop = 0.0;
	m_iDestroyedBuildings = 0;

	m_pShips.RemoveAll();

	for (CShipMap::iterator i = ships->begin(); i != ships->end(); ++i)
		if (i->second->GetKO() == m_KO && i->second->GetCurrentOrder() == SHIP_ORDER::ATTACK_SYSTEM)
		{
			// Schiffe mit R�ckzugsbefehl nehmen nicht am Systemangriff teil
			if (i->second->GetCombatTactic() != COMBAT_TACTIC::CT_RETREAT)
				m_pShips.Add(i->second);

			// Wenn das Schiff eine Flotte besitzt, diese Schiffe auch dem Feld hinzuf�gen
			for (CShips::iterator j = i->second->begin(); j != i->second->end(); ++j)
				if (j->second->GetCombatTactic() != COMBAT_TACTIC::CT_RETREAT)
					m_pShips.Add(j->second);
		}
}

/// Diese Funktion f�hrt den Angriff durch. Au�erdem werden alle Berechnungen der Auswirkungen des Angriffs
/// durchgef�hrt. Der R�ckgabewert ist <code>TRUE</code>, wenn der Angriff erfolgreich war, bei Misserfolg
/// ist der R�ckgabewert <code>FALSE</code>.
BOOLEAN CAttackSystem::Calculate()
{
	m_fKilledPop = (float)m_pSector->GetCurrentHabitants();
	USHORT killedTroopsInSystem = m_pSystem->GetTroops()->GetSize();

	int shipDefence = 0;
	if (m_pSystem->GetOwnerOfSystem() != "" && m_pDefender != NULL && m_pDefender->IsMajor())
		shipDefence = m_pSystem->GetProduction()->GetShipDefend();
//MYTRACE("general")(MT::LEVEL_INFO, "m_sOwnerOfSystem %s\n", sRace);

	// Zuerst wird die Schiffsabwehr des Systems beachtet. Dadurch k�nnen schon einige Schiffe zerst�rt werden.
	this->CalculateShipDefence();
	// Danach wird das System bombardiert.
	this->CalculateBombAttack();
	// Jetzt kommt es zum Bodenangriff in dem System durch die Truppen.
	this->CalculateTroopAttack();

	// abschlie�ende Berechnungen f�r den Systemangriff
	// Get�tete Bev�lkerung berechnen
	m_fKilledPop -= (float)m_pSector->GetCurrentHabitants();
	VERIFY(m_fKilledPop >= 0);

	if (m_fKilledPop != 0.0f)
	{
		CString n;
		n.Format("%.3lf", m_fKilledPop);
		m_strNews.Add(CLoc::GetString("KILLED_POP_BY_SYSTEMATTACK",0,n,m_pSector->GetName()));
		MYTRACE("logsystemattack")(MT::LEVEL_INFO, "%f KILLED_POP_BY_SYSTEMATTACK\n", m_fKilledPop);
	}
	// verlorene Truppen berechnen
	USHORT killedTroopsInTransport = 0;
	killedTroopsInSystem -= m_pSystem->GetTroops()->GetSize();
	if (killedTroopsInSystem > 0)
	{
		CString n; n.Format("%d",killedTroopsInSystem);
		m_strNews.Add(CLoc::GetString("KILLED_TROOPS_IN_SYSTEM",0,n,m_pSector->GetName()));
		MYTRACE("logsystemattack")(MT::LEVEL_INFO, "%d KILLED_TROOPS_IN_SYSTEM\n", killedTroopsInSystem);
	}
	// Erfahrung der Schiffe berechnen
	// (Bev�lkerungsverlust in Mrd.) * 100 + aktive shipdefence EP.
	int XP = (int)(m_fKilledPop * 100 + shipDefence);
	MYTRACE("logsystemattack")(MT::LEVEL_INFO, "%i Experience complete gained\n", XP);
	// den XP-Wert gleichverteilt auf alle noch lebenden Schiffe anrechnen
	if (m_pShips.GetSize() > 0)
	{
		XP /= m_pShips.GetSize();
		for (int i = 0; i < m_pShips.GetSize(); i++)
			m_pShips.GetAt(i)->SetCrewExperiance(XP);
	}


	// Das System wurde erfolgreich erobert
	if (m_pTroops.GetSize() > 0 && m_pSystem->GetHabitants() > 0.0f)
	{
		// Das System wurde erobert. Truppen aus dem Transporter werden in dem System stationiert
		m_pSector->SetOwned(TRUE);
		m_pSystem->SetOwnerOfSystem(m_pTroops.GetAt(0)->GetOwner());
		// jetzt die Truppen dem System hinzuf�gen
		for (int i = 0; i < m_pTroops.GetSize(); i++)
			m_pSystem->AddTroop(m_pTroops.GetAt(i));

		// Alle Truppen von den Transportern l�schen, da diese jetzt im System sind
		for (int i = 0; i < m_pShips.GetSize(); i++)
		{
			//unset system attack command
			m_pShips.GetAt(i)->UnsetCurrentOrder();
			for (int j = 0; j < m_pShips.GetAt(i)->GetTransportedTroops()->GetSize();)
			{
				if (m_pShips.GetAt(i)->GetTransportedTroops()->GetAt(j).GetOffense() == NULL)
					killedTroopsInTransport++;
				m_pShips.GetAt(i)->GetTransportedTroops()->RemoveAt(j);
			}
		}
		if (killedTroopsInTransport > 0)
		{
			CString n; n.Format("%d",killedTroopsInTransport);
			m_strNews.Add(CLoc::GetString("KILLED_TROOPS_IN_TRANSPORTS",0,n,m_pSector->GetName()));
			MYTRACE("logsystemattack")(MT::LEVEL_INFO, "%s attacking troops killed\n", CLoc::GetString("KILLED_TROOPS_IN_TRANSPORTS",0,n,m_pSector->GetName()));
		}
		if (m_bTroopsInvolved)
			m_strNews.InsertAt(0,CLoc::GetString("INVASION_SUCCESSFUL",0,m_pSector->GetName()));
			MYTRACE("logsystemattack")(MT::LEVEL_INFO, "INVASION_SUCCESSFUL");
		return TRUE;
	}
	// das System wurde bombardiert bzw. die ganze Bev�lkerung vernichtet
	else
	{
		// Alle Truppen mit St�rke == NULL von den Transportern l�schen, da diese im Kampf gefallen sind und
		// nicht wieder mitgenommen werden k�nnen
		for (int i = 0; i < m_pShips.GetSize(); i++)
			for (int j = 0; j < m_pShips.GetAt(i)->GetTransportedTroops()->GetSize(); j++)
				if (m_pShips.GetAt(i)->GetTransportedTroops()->GetAt(j).GetOffense() == NULL)
				{
					m_pShips.GetAt(i)->GetTransportedTroops()->RemoveAt(j--);
					killedTroopsInTransport++;
				}
		if (killedTroopsInTransport > 0)
		{
			CString n; n.Format("%d",killedTroopsInTransport);
			m_strNews.Add(CLoc::GetString("KILLED_TROOPS_IN_TRANSPORTS",0,n,m_pSector->GetName()));
			MYTRACE("logsystemattack")(MT::LEVEL_INFO, "%d attacking troops killed\n", n);
		}
		if (m_bTroopsInvolved == TRUE && m_pSystem->GetHabitants() > 0.0f)
			{
			m_strNews.InsertAt(0,CLoc::GetString("INVASION_FAILED",0,m_pSector->GetName()));
			MYTRACE("logsystemattack")(MT::LEVEL_INFO, "INVASION_FAILED");
			}
		else if (m_bTroopsInvolved == FALSE && m_pSystem->GetHabitants() > 0.0f)
			{
			m_strNews.InsertAt(0,CLoc::GetString("SYSTEM_BOMBED",0,m_pSector->GetName()));
			MYTRACE("logsystemattack")(MT::LEVEL_INFO, "SYSTEM_BOMBED");
			}
		else
			{
			m_strNews.InsertAt(0,CLoc::GetString("ALL_LIFE_DIED",0,m_pSector->GetName()));
			MYTRACE("logsystemattack")(MT::LEVEL_INFO, "ALL_LIFE_DIED");
			}
		return FALSE;
	}
}

/// Diese Funktion gibt zur�ck, ob der Verteidiger ungleich dem/den Angreifer/n ist.
BOOLEAN CAttackSystem::IsDefenderNotAttacker(const CString& sDefender, const set<CString>* attacker) const
{
	if (attacker->find(sDefender) == attacker->end())
			return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Private Funktion, die allein die Schiffsabwehr berechnet
void CAttackSystem::CalculateShipDefence()
{
	MYTRACE("logsystemattack")(MT::LEVEL_INFO, "CAttackSystem::CalculateShipDefence() begin...\n");
	int defence = 0;
	USHORT killedShips = 0;
	if (m_pSystem->GetOwnerOfSystem() != "" && m_pDefender != NULL && m_pDefender->IsMajor())
		defence = m_pSystem->GetProduction()->GetShipDefend();
	MYTRACE("logsystemattack")(MT::LEVEL_INFO, "ShipDefence (total) %d\n", defence);
	// (unneccassary) MYTRACE("logsystemattack")(MT::LEVEL_INFO, "AttackSystem.cpp: Defender %s, System %s, ShipDefence %d\n", m_pDefender, m_pSystem, defence);
	// einfacher Algorithmus:
	//		Der Defencewert wird durch die Anzahl der angreifenden Schiffe geteilt. Dann wird der Anzahl der
	//		angreifenden Schiffe zuf�llig auf ein Schiff geschossen, welches jedesmal defence / Anzahl (+-20%)
	//		Schaden verursacht.
	for (int i = 0; i < m_pShips.GetSize(); i++)
	{
		if (m_pShips.GetAt(i)->GetTransportedTroops()->GetSize() > 0)
			m_bTroopsInvolved = TRUE;
		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "TransportedTroops: %d\n", m_pShips.GetAt(i)->GetTransportedTroops()->GetSize());
		USHORT hit = rand()%m_pShips.GetSize();
		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "hit: %d\n", hit);
		int dam = (defence * (rand()%41+80))/100 / m_pShips.GetSize();
		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "dam before Assault: %d\n", dam);
		// Wenn es sich um ein Assaultship handelt, so werden 20% des Schadens vermieden
		if (m_pShips[hit]->HasSpecial(SHIP_SPECIAL::ASSULTSHIP))
			dam = (int)(dam * 0.8f);
		m_pShips[hit]->GetHull()->SetCurrentHull(-dam);
		// (doesn't work) 
		if (dam > 0)
		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "System Attack: Ship %s hit:%d, dam:%d, Hull:%d\n",m_pShips[hit]->GetShipName(), hit, dam, m_pShips[hit]->GetHull()->GetCurrentHull());
		//MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "System Attack: Ship %s hit:%d, dam:%d, Hull:%d\n",m_pShips[hit]->GetShipName(), hit, dam, m_pShips[hit]->GetHull()->GetCurrentHull());
		
	}
	// Alle Schiffe, die mittlerweile keine H�lle mehr haben sind zerst�rt!
	for (int i = 0; i < m_pShips.GetSize(); i++)
	{
		if (m_pShips.GetAt(i)->GetHull()->GetCurrentHull() == NULL)
		{
			m_pShips.RemoveAt(i--);
MYTRACE("general")(MT::LEVEL_INFO, "System Attack: Ship destroyed s %s\n", m_pShips);
//MYTRACE("general")(MT::LEVEL_INFO, "System Attack: Ship destroyed i %i\n", m_pShips);
			MYTRACE("logsystemattack")(MT::LEVEL_INFO, "System Attack: Ship destroyed: %d\n", m_pShips);
			killedShips++;
		}
		else
		{
			// Wenn das	Schiff nicht zerst�rt wurde und Truppen an Bord hat, so wird das Truppenfeld gef�llt
			for (int j = 0; j < m_pShips.GetAt(i)->GetTransportedTroops()->GetSize(); j++)
			{
				m_pTroops.Add(&m_pShips.GetAt(i)->GetTransportedTroops()->GetAt(j));
				//m_pShips.GetAt(i)->GetTransportedTroops()->RemoveAt(j);
			}
		}
	}
	if (killedShips == 0)
		{
		m_strNews.Add(CLoc::GetString("NO_ATTACKING_SHIPS_KILLED",0,m_pSector->GetName()));
		MYTRACE("logsystemattack")(MT::LEVEL_INFO, "NO_ATTACKING_SHIPS_KILLED\n");
		}
	else
	{
		CString n; n.Format("%d",killedShips);
		m_strNews.Add(CLoc::GetString("ATTACKING_SHIPS_KILLED",0,n,m_pSector->GetName()));
		MYTRACE("logsystemattack")(MT::LEVEL_INFO, "%d ATTACKING_SHIPS_KILLED\n", killedShips);
	}
	MYTRACE("logsystemattack")(MT::LEVEL_INFO, "ATTACKING Troops: %d\n", m_pTroops.GetSize());
/*	CString s;
	s.Format("Anzahl der angreifenden Truppen: %d",m_pTroops.GetSize());
	AfxMessageBox(s);
*/
}

/// Private Funktion, die allein die Systembombardierung beachtet
void CAttackSystem::CalculateBombAttack()
{
	MYTRACE("logsystemattack")(MT::LEVEL_INFO, "CAttackSystem::CalculateBombAttack() begin...\n");
	// Bei der Bombardierung werden nur die Torpedos der Schiffe beachtet.
	// Bei der Bombardierung k�nnen Geb�ude, Truppen und Bev�lkerung zerst�rt werden bzw. sterben. Aktivierte Schilde
	// in dem System k�nnen dies reduzieren bzw. auch komplett verhindern.
	int shield = m_pSystem->GetProduction()->GetShieldPower();
	MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "shields: %i\n", shield);
	int torpedoDamage = 0;
	for (int i = 0; i < m_pShips.GetSize(); i++)
		for (int j = 0; j < m_pShips.GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
		{
			int dmg = m_pShips.GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTorpedoPower() *
				m_pShips.GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumber() *
				m_pShips.GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumberOfTupes();
			// Wenn es sich um ein Assultship handelt, so wird der Torpedoschaden um 20% erh�ht
			if (m_pShips.GetAt(i)->HasSpecial(SHIP_SPECIAL::ASSULTSHIP))
			{
				dmg = (int)(dmg * 1.2f);
				m_bAssultShipInvolved = TRUE;
			}
			torpedoDamage += dmg;
		}

		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp: TorpedoDamageBeforeShields = %i\n", torpedoDamage);
	// Die Schilde werden vom Torpedoschaden abgezogen
	if ((torpedoDamage - shield) < 0)
		torpedoDamage = 0;
	else
		torpedoDamage -= shield;

	MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp: TorpedoDamageAfterShields = %i\n", torpedoDamage);


	// Nachricht das Schilde den Schaden im System verringert haben
	if (shield > 0)
	{
		m_strNews.Add(CLoc::GetString("SHIELDS_SAVED_LIFE",0,m_pSector->GetName()));
MYTRACE("logsystemattack")(MT::LEVEL_INFO, "SHIELDS_SAVED_LIFE\n", torpedoDamage);
	}

/*	CString s;
	s.Format("Torpedoschaden im System: %i",torpedoDamage);
	AfxMessageBox(s);
*/
	m_iDestroyedBuildings = 0;
	// Durch den Torpedoschaden kann sich die Bev�lkerung in dem System verringern.
	if (torpedoDamage > 0)
	{
		float killedPop = (float)((rand()%torpedoDamage)*0.00075f);
		MYTRACE("logsystemattack")(MT::LEVEL_INFO, "killedPop = %g\n", killedPop);
		m_pSector->LetPlanetsShrink(-killedPop);
		m_pSystem->SetHabitants(m_pSector->GetCurrentHabitants());

		// Dann werden zuf�llig Geb�ude zerst�rt. Umso mehr Geb�ude vorhanden sind, desto wahrscheinlicher werden diese
		// zerst�rt. Auch stationierte Truppen k�nnen bei einem Angriff vernichtet werden.
		for (int i = 0; i < torpedoDamage; i++)
		{
			if (rand()%50 < m_pSystem->GetAllBuildings()->GetSize())
			{
				m_pSystem->GetAllBuildings()->RemoveAt(rand()%m_pSystem->GetAllBuildings()->GetSize());
				m_iDestroyedBuildings++;
			}
			if (rand()%25 < m_pSystem->GetTroops()->GetSize())
				m_pSystem->GetTroops()->RemoveAt(rand()%m_pSystem->GetTroops()->GetSize());
			torpedoDamage /= 4;
		}
		// Wenn der Torpedoangriff Geb�ude zerst�rt hat, dann dies bei dem System beachten. Deshalb werden die Werte des
		// Systems neu berechnet. Speziell muss man hier die Geb�ude beachten, die durch einen Energiemangel h�tten ausfallen
		// k�nnen. Diese auch offline schalten und dann die Sache nochmal berechnen.
		if (m_pSystem->GetOwnerOfSystem() != "" && m_pDefender != NULL && m_pDefender->IsMajor())
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pDefender);
			CResearchInfo* pInfo = pMajor->GetEmpire()->GetResearch()->GetResearchInfo();
			m_pSystem->CalculateVariables(m_pBuildingInfos, pInfo, m_pSector->GetPlanets(), pMajor, m_sMonopolOwner);
			m_pSystem->CheckEnergyBuildings(m_pBuildingInfos);
			m_pSystem->CalculateVariables(m_pBuildingInfos, pInfo, m_pSector->GetPlanets(), pMajor, m_sMonopolOwner);
		}

		if (m_iDestroyedBuildings != 0)
		{
			CString n;
			n.Format("%d", m_iDestroyedBuildings);
			m_strNews.Add(CLoc::GetString("DESTROYED_BUILDINGS_BY_SYSTEMATTACK",0,n,m_pSector->GetName()));
			MYTRACE("logsystemattack")(MT::LEVEL_INFO, "%d BUILDINGS destroyed by SYSTEMATTACK\n", m_iDestroyedBuildings);
		}
	}
}

/// Private Funktion, die allein den Angriff durch Truppen berechnet
void CAttackSystem::CalculateTroopAttack()
{
	MYTRACE("logsystemattack")(MT::LEVEL_INFO, "CAttackSystem::CalculateTroopAttack() begin...\n");
	// Wenn ein Assaultship am Angriff beteiligt ist, so bekommen die angreifenden Truppen einen 20% St�rkebonus
	BYTE offenceBoni = 0;
	if (m_bAssultShipInvolved)
		{
		offenceBoni = 20;
		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "AssaultShipInvolved, offenceBoni = %d\n", offenceBoni);
		}

	// Zuerst k�mpfen die angreifenden Truppen gegen
	// die Bodenabwehr in dem System. Danach werden die stationierten Truppen gegen die Angreifer k�mpfen. Wenn diese
	// vernichtet wurden, dann k�mpft noch der Rest der Bev�lkerung gegen die angreifenden Truppen. Wenn ein Gro�teil
	// dieser vernichtet wurde, dann gilt das System als erobert.
	int groundDefence = m_pSystem->GetProduction()->GetGroundDefend();	// <- beinhaltet schon GroundDefendBoni!
MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "groundDefence = %d\n", groundDefence);
	// Dieser Verteidigungswert addiert sich zum Verteidigungswert durch die Bev�lkerung. Aller 5Mrd. Bev�lkerung bekommt
	// man eine Verteidigungseinheit. Das w�rde bedeuten, dass man mit 50Mrd. Bev�lkerung 10 Einheiten mit der
	// Angriffsst�rke von 10 bekommt. Ein GroundDefence-Wert von z.B. 30 wird behandelt, als w�ren zus�tzliche
	// 30Mrd. Bev�lkerung in diesem System. Somit bekommt man zus�tzliche 6 Einheiten und jeder Angriff dieser Verteidigung
	// wird mit einer 10er St�rke durchgef�hrt. Also genauso stark wie die Bev�lkerung.
	for (int i = 0; i < groundDefence; i += 5)
	{
		if (m_pTroops.GetSize() == 0)
			break;
		CTroopInfo* ti = new CTroopInfo();
		ti->SetDefense(10);
		int number = rand()%m_pTroops.GetSize();
		BYTE result = m_pTroops.GetAt(number)->Attack((CTroop*)ti, offenceBoni, 0);
		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp: result: %d\n", result);
		// Die angreifende Einheit hat verloren
		if (result > 0)
		{
			// Alle Truppen mit St�rke NULL werden am Ende von den Schiffen gel�scht
			m_pTroops.GetAt(number)->SetOffense(0);
			m_pTroops.RemoveAt(number);
		}
		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem: ti2 = %d\n", ti);
		delete ti;
	}

	// Hier der Angriff gegen die stationierten Truppen
	while (m_pSystem->GetTroops()->GetSize() > 0)
	{
		if (m_pTroops.GetSize() == 0)
			break;
		int numberAtt = rand()%m_pTroops.GetSize();
		int numberDef = rand()%m_pSystem->GetTroops()->GetSize();
		BYTE result = m_pTroops.GetAt(numberAtt)->Attack(&m_pSystem->GetTroops()->GetAt(numberDef), offenceBoni,
			m_pSystem->GetProduction()->GetGroundDefendBoni());
		if (result == 0)
			m_pSystem->GetTroops()->RemoveAt(numberDef);
		else if (result == 1)
		{
			// Alle Truppen mit St�rke NULL werden am Ende von den Schiffen gel�scht
			m_pTroops.GetAt(numberAtt)->SetOffense(0);
			m_pTroops.RemoveAt(numberAtt);
		}
		else
		{
			m_pSystem->GetTroops()->RemoveAt(numberDef);
			// Alle Truppen mit St�rke NULL werden am Ende von den Schiffen gel�scht
			m_pTroops.GetAt(numberAtt)->SetOffense(0);
			m_pTroops.RemoveAt(numberAtt);
		}
	}

	BOOLEAN fighted = FALSE;
	USHORT maxFightsFromPop = (USHORT)ceil(m_pSystem->GetHabitants() / 5) * m_pSystem->GetMoral() / 100;
	MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp: maxFightsFromPop (Habitants/5)*morale: = %d \n", maxFightsFromPop);
	// zu allerletzt der Angriff gegen die verteidigende Bev�lkerung
	while (m_pTroops.GetSize() > 0 && maxFightsFromPop > 0)
	{
		CTroopInfo* ti = new CTroopInfo();
		// Hier wird die Rasseneigenschaft der verteidigenden Bev�lkerung beachtet.
		int nPower = 10;
		if (m_pDefender != NULL)
		{
			if (m_pDefender->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
				nPower -= 2;
			if (m_pDefender->IsRaceProperty(RACE_PROPERTY::WARLIKE))
				nPower += 10;
			if (m_pDefender->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
				nPower -= 3;
			if (m_pDefender->IsRaceProperty(RACE_PROPERTY::PACIFIST))
				nPower -= 5;
			if (m_pDefender->IsRaceProperty(RACE_PROPERTY::SNEAKY))
				nPower += 5;
			if (m_pDefender->IsRaceProperty(RACE_PROPERTY::SOLOING))
				nPower += 2;
			if (m_pDefender->IsRaceProperty(RACE_PROPERTY::HOSTILE))
				nPower += 7;

			MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp: DefencePower (nPower)3: %i\n", nPower);

			if (nPower > MAXBYTE)
				nPower = MAXBYTE;
			else if (nPower < 0)
				nPower = 0;

			MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp: DefencePower (nPower): %i\n", nPower);
		}

		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp: DefencePower (nPower): %i\n", nPower);


		ti->SetDefense((BYTE)nPower);

		// wie stark sich die Bev�lkerung verteidigt h�ngt vom Moralwert derer ab
		ti->SetDefense(ti->GetDefense() * m_pSystem->GetMoral() / 100);
		int number = rand()%m_pTroops.GetSize();
		BYTE result = m_pTroops.GetAt(number)->Attack((CTroop*)ti, offenceBoni, m_pSystem->GetProduction()->GetGroundDefendBoni());
		// Die angreifende Einheit hat verloren
		if (result != 0)
		{
			// Wenn die Angriffst�rke dieser Einheit NULL betr�gt, ist sie vernichtet. Am Ende des Kampfes k�nnen so die
			// einzelnen Truppen aus den Transportern gel�scht werden
			m_pTroops.GetAt(number)->SetOffense(0);
			m_pTroops.RemoveAt(number);
		}
		if (result != 1)
		{
			// Durch den Angriff der Truppen kann sich die Bev�lkerung in dem System verringern.
			m_pSector->LetPlanetsShrink(-(float)(rand()%5));
			fighted = TRUE;
			maxFightsFromPop--;
		}

		MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp:ti: %i\n", ti);
		delete ti;
	}
	if (fighted)
	{
		m_pSystem->SetHabitants(m_pSector->GetCurrentHabitants());
		if (m_pSystem->GetOwnerOfSystem() != "" && m_pDefender != NULL && m_pDefender->IsMajor())
		{
			CMajor* pMajor = dynamic_cast<CMajor*>(m_pDefender);
			CResearchInfo* pInfo = pMajor->GetEmpire()->GetResearch()->GetResearchInfo();
			m_pSystem->CalculateVariables(m_pBuildingInfos, pInfo, m_pSector->GetPlanets(), pMajor, m_sMonopolOwner);
			m_pSystem->CheckEnergyBuildings(m_pBuildingInfos);
			m_pSystem->CalculateVariables(m_pBuildingInfos, pInfo, m_pSector->GetPlanets(), pMajor, m_sMonopolOwner);
		}
	}

	// Wenn Truppen verschiedener Imperien angegriffen haben, so m�ssen diese noch gegeneinander antreten. Es kann immer
	// nur ein Imperium dieses System erobern.
	if (m_pTroops.GetSize() > 0)
		for (int i = 0; i < m_pTroops.GetSize(); i++)
			if (m_pTroops.GetSize() > (i+1))
				if (m_pTroops.GetAt(i)->GetOwner() != m_pTroops.GetAt(i+1)->GetOwner())
				{
					MYTRACE("logsystemattack")(MT::LEVEL_DEBUG, "ATTACKSystem.cpp: Troops from different agressors alive");
					BYTE result = m_pTroops.GetAt(i)->Attack(m_pTroops.GetAt(i+1), 0, 0);
					if (result == 0)
					{
						// Alle Truppen mit St�rke NULL werden am Ende von den Schiffen gel�scht
						m_pTroops.GetAt(i+1)->SetOffense(0);
						m_pTroops.RemoveAt(i+1);
					}
					else if (result == 1)
					{
						// Alle Truppen mit St�rke NULL werden am Ende von den Schiffen gel�scht
						m_pTroops.GetAt(i)->SetOffense(0);
						m_pTroops.RemoveAt(i);
					}
					else
					{
						m_pTroops.GetAt(i)->SetOffense(0);
						m_pTroops.RemoveAt(i);
						// hier gleich nochmal an der selben Stelle, da der Nachfolger ja um eins nach vorn gerutscht ist
						m_pTroops.GetAt(i)->SetOffense(0);
						m_pTroops.RemoveAt(i);
					}
					i = 0;
				}
}
