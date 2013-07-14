#include "stdafx.h"
#include "AssemblyList.h"
#include "System.h"
#include "Races\ResearchInfo.h"

IMPLEMENT_SERIAL (CAssemblyList, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CAssemblyList::CAssemblyList(void)
{
	Reset();
}

CAssemblyList::~CAssemblyList(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CAssemblyList::CAssemblyList(const CAssemblyList & rhs)
{
	m_iNeededIndustryForBuild = rhs.m_iNeededIndustryForBuild;
	m_iNeededTitanForBuild = rhs.m_iNeededTitanForBuild;
	m_iNeededDeuteriumForBuild = rhs.m_iNeededDeuteriumForBuild;
	m_iNeededDuraniumForBuild = rhs.m_iNeededDuraniumForBuild;
	m_iNeededCrystalForBuild = rhs.m_iNeededCrystalForBuild;
	m_iNeededIridiumForBuild = rhs.m_iNeededIridiumForBuild;
	m_iNeededDeritiumForBuild = rhs.m_iNeededDeritiumForBuild;
	m_bWasBuildingBought = rhs.m_bWasBuildingBought;
	m_iBuildCosts = rhs.m_iBuildCosts;
	for (int i = 0; i < ALE; i++)
	{
		m_iEntry[i] = rhs.m_iEntry[i];
		m_iNeededIndustryInAssemblyList[i] = rhs.m_iNeededIndustryInAssemblyList[i];
		m_iNeededTitanInAssemblyList[i] = rhs.m_iNeededTitanInAssemblyList[i];
		m_iNeededDeuteriumInAssemblyList[i] = rhs.m_iNeededDeuteriumInAssemblyList[i];
		m_iNeededDuraniumInAssemblyList[i] = rhs.m_iNeededDuraniumInAssemblyList[i];
		m_iNeededCrystalInAssemblyList[i] = rhs.m_iNeededCrystalInAssemblyList[i];
		m_iNeededIridiumInAssemblyList[i] = rhs.m_iNeededIridiumInAssemblyList[i];
		m_iNeededDeritiumInAssemblyList[i] = rhs.m_iNeededDeritiumInAssemblyList[i];
	}
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CAssemblyList & CAssemblyList::operator=(const CAssemblyList & rhs)
{
	if (this == &rhs)
		return *this;
	m_iNeededIndustryForBuild = rhs.m_iNeededIndustryForBuild;
	m_iNeededTitanForBuild = rhs.m_iNeededTitanForBuild;
	m_iNeededDeuteriumForBuild = rhs.m_iNeededDeuteriumForBuild;
	m_iNeededDuraniumForBuild = rhs.m_iNeededDuraniumForBuild;
	m_iNeededCrystalForBuild = rhs.m_iNeededCrystalForBuild;
	m_iNeededIridiumForBuild = rhs.m_iNeededIridiumForBuild;
	m_iNeededDeritiumForBuild = rhs.m_iNeededDeritiumForBuild;
	m_bWasBuildingBought = rhs.m_bWasBuildingBought;
	m_iBuildCosts = rhs.m_iBuildCosts;
	for (int i = 0; i < ALE; i++)
	{
		m_iEntry[i] = rhs.m_iEntry[i];
		m_iNeededIndustryInAssemblyList[i] = rhs.m_iNeededIndustryInAssemblyList[i];
		m_iNeededTitanInAssemblyList[i] = rhs.m_iNeededTitanInAssemblyList[i];
		m_iNeededDeuteriumInAssemblyList[i] = rhs.m_iNeededDeuteriumInAssemblyList[i];
		m_iNeededDuraniumInAssemblyList[i] = rhs.m_iNeededDuraniumInAssemblyList[i];
		m_iNeededCrystalInAssemblyList[i] = rhs.m_iNeededCrystalInAssemblyList[i];
		m_iNeededIridiumInAssemblyList[i] = rhs.m_iNeededIridiumInAssemblyList[i];
		m_iNeededDeritiumInAssemblyList[i] = rhs.m_iNeededDeritiumInAssemblyList[i];
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CAssemblyList::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		for (int i = 0; i < ALE; i++)
		{
			ar << m_iEntry[i];
			// Variablen geben die noch verbleibenden Kosten der Elemente in der Bauliste an
			ar << m_iNeededIndustryInAssemblyList[i];
			ar << m_iNeededTitanInAssemblyList[i];
			ar << m_iNeededDeuteriumInAssemblyList[i];
			ar << m_iNeededDuraniumInAssemblyList[i];
			ar << m_iNeededCrystalInAssemblyList[i];
			ar << m_iNeededIridiumInAssemblyList[i];
			ar << m_iNeededDeritiumInAssemblyList[i];
		}
		// Variablen, die Angeben, wieviel Industrie und Rohstoffe zum Bau ben�tigt werden
		ar << m_iNeededIndustryForBuild;
		ar << m_iNeededTitanForBuild;
		ar << m_iNeededDeuteriumForBuild;
		ar << m_iNeededDuraniumForBuild;
		ar << m_iNeededCrystalForBuild;
		ar << m_iNeededIridiumForBuild;
		ar << m_iNeededDeritiumForBuild;
		// Wurde das Geb�ude gekauft in dieser Runde gekauft
		ar << m_bWasBuildingBought;
		// Die Baukosten eines Auftrages
		ar << m_iBuildCosts;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		for (int i = 0; i < ALE; i++)
		{
			ar >> m_iEntry[i];
			// Variablen geben die noch verbleibenden Kosten der Elemente in der Bauliste an
			ar >> m_iNeededIndustryInAssemblyList[i];
			ar >> m_iNeededTitanInAssemblyList[i];
			ar >> m_iNeededDeuteriumInAssemblyList[i];
			ar >> m_iNeededDuraniumInAssemblyList[i];
			ar >> m_iNeededCrystalInAssemblyList[i];
			ar >> m_iNeededIridiumInAssemblyList[i];
			ar >> m_iNeededDeritiumInAssemblyList[i];
		}
		// Variablen, die Angeben, wieviel Industrie und Rohstoffe zum Bau ben�tigt werden
		ar >> m_iNeededIndustryForBuild;
		ar >> m_iNeededTitanForBuild;
		ar >> m_iNeededDeuteriumForBuild;
		ar >> m_iNeededDuraniumForBuild;
		ar >> m_iNeededCrystalForBuild;
		ar >> m_iNeededIridiumForBuild;
		ar >> m_iNeededDeritiumForBuild;
		// Wurde das Geb�ude gekauft in dieser Runde gekauft
		ar >> m_bWasBuildingBought;
		// Die Baukosten eines Auftrages
		ar >> m_iBuildCosts;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion gibt die jeweiligen verbleibenden Kosten f�r einen Bauauftrag zur�ck.
UINT CAssemblyList::GetNeededResourceInAssemblyList(USHORT entry, BYTE res) const
{
	switch (res)
	{
	case TITAN:			return this->GetNeededTitanInAssemblyList(entry);
	case DEUTERIUM:		return this->GetNeededDeuteriumInAssemblyList(entry);
	case DURANIUM:		return this->GetNeededDuraniumInAssemblyList(entry);
	case CRYSTAL:		return this->GetNeededCrystalInAssemblyList(entry);
	case IRIDIUM:		return this->GetNeededIridiumInAssemblyList(entry);
	case DERITIUM:		return this->GetNeededDeritiumInAssemblyList(entry);
	default:			return NULL;
	}
}

// Funktion gibt die jeweiligen Kosten f�r einen Bauauftrag zur�ck.
UINT CAssemblyList::GetNeededResourceForBuild(BYTE res) const
{
	switch (res)
	{
	case TITAN:			return this->GetNeededTitanForBuild();
	case DEUTERIUM:		return this->GetNeededDeuteriumForBuild();
	case DURANIUM:		return this->GetNeededDuraniumForBuild();
	case CRYSTAL:		return this->GetNeededCrystalForBuild();
	case IRIDIUM:		return this->GetNeededIridiumForBuild();
	case DERITIUM:		return this->GetNeededDeritiumForBuild();
	default:			return NULL;
	}
}

// Funktion berechnet die ben�tigten Rohstoffe. �bergeben wird die Information des gew�nschten Geb�udes bzw. des
// gew�nschten Schiffes oder der gew�nschten Truppe, der RunningNumber des Bauauftrages, einen Zeiger auf alle
// vorhandenen Geb�ude in dem System und ein Zeiger auf die Forschung
void CAssemblyList::CalculateNeededRessources(CBuildingInfo* buildingInfo, CShipInfo* shipInfo, CTroopInfo* troopInfo,
											  BuildingArray* m_Buildings, int RunningNumber, CResearchInfo* ResearchInfo, float modifier)
{
	// Wenn ein Update, dann mu� die RunningNumber des buildingInfo positiv sein und eins gr��er
	// -> dies beim Aufruf dieser Funktion beachten
	if (RunningNumber > 0 && RunningNumber < 10000)		// Also kein Update, sondern ein echtes Geb�ude
	{
		m_iNeededIndustryForBuild = buildingInfo->GetNeededIndustry();
		m_iNeededTitanForBuild = buildingInfo->GetNeededTitan();
		m_iNeededDeuteriumForBuild = buildingInfo->GetNeededDeuterium();
		m_iNeededDuraniumForBuild = buildingInfo->GetNeededDuranium();
		m_iNeededCrystalForBuild = buildingInfo->GetNeededCrystal();
		m_iNeededIridiumForBuild = buildingInfo->GetNeededIridium();
		m_iNeededDeritiumForBuild = buildingInfo->GetNeededDeritium();
	}
	else if (RunningNumber < 0)	// Wollen also ein Update machen
	{
		// Berechnen, wieviel Rohstoffe und Industry wir ben�tigen
		int NumberOfBuildings = m_Buildings->GetSize();
		RunningNumber *= (-1);
		m_iNeededIndustryForBuild = 0;
		m_iNeededTitanForBuild = 0;
		m_iNeededDeuteriumForBuild = 0;
		m_iNeededDuraniumForBuild = 0;
		m_iNeededCrystalForBuild = 0;
		m_iNeededIridiumForBuild = 0;
		m_iNeededDeritiumForBuild = 0;
		for (int i = 0; i < NumberOfBuildings; i++)
		{
			if (m_Buildings->GetAt(i).GetRunningNumber() == buildingInfo->GetPredecessorID())
			{
				// zur Zeit 80% der Rohstoffe aller Geb�ude des Typs aufbringen
				// und 60% der Industrie
				m_iNeededTitanForBuild += (int)(buildingInfo->GetNeededTitan()*0.8);
				m_iNeededDeuteriumForBuild += (int)(buildingInfo->GetNeededDeuterium()*0.8);
				m_iNeededDuraniumForBuild += (int)(buildingInfo->GetNeededDuranium()*0.8);
				m_iNeededCrystalForBuild += (int)(buildingInfo->GetNeededCrystal()*0.8);
				m_iNeededIridiumForBuild += (int)(buildingInfo->GetNeededIridium()*0.8);
				m_iNeededDeritiumForBuild += (int)(buildingInfo->GetNeededDeritium()*0.8);
				m_iNeededIndustryForBuild += (int)(buildingInfo->GetNeededIndustry()*0.6);
			}
		}
	}
	else if (RunningNumber >= 10000 && RunningNumber < 20000)	// Es handelt sich um ein Schiff
	{
		m_iNeededIndustryForBuild = shipInfo->GetNeededIndustry();
		m_iNeededTitanForBuild = shipInfo->GetNeededTitan();
		m_iNeededDeuteriumForBuild = shipInfo->GetNeededDeuterium();
		m_iNeededDuraniumForBuild = shipInfo->GetNeededDuranium();
		m_iNeededCrystalForBuild = shipInfo->GetNeededCrystal();
		m_iNeededIridiumForBuild = shipInfo->GetNeededIridium();
		m_iNeededDeritiumForBuild = shipInfo->GetNeededDeritium();
		// hier auch noch den eventuellen Bonus durch die Uniqueforschung "Allgemeine Schifftechnik"
		if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)	// 2 -> Allgemeine Schiffstechnik
		{
			m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededIndustryForBuild/100);
			m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededTitanForBuild/100);
			m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededDeuteriumForBuild/100);
			m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededDuraniumForBuild/100);
			m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededCrystalForBuild/100);
			m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededIridiumForBuild/100);
		}
		// hier auch noch den eventuellen Bonus durch die Uniqueforschung "friedliche Schifftechnik"
		// aber nur wenn es sich um ein Transportschiff oder Kolonieschiff handelt
		if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED && shipInfo->GetShipType() <= SHIP_TYPE::COLONYSHIP) 	// 3 -> Allgemeine Schiffstechnik
		{
			m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededIndustryForBuild/100);
			m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededTitanForBuild/100);
			m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededDeuteriumForBuild/100);
			m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededDuraniumForBuild/100);
			m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededCrystalForBuild/100);
			m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetBonus(3)*m_iNeededIridiumForBuild/100);
		}
	}
	else if (RunningNumber >= 20000)	// es handelt sich um Truppen
	{
		m_iNeededIndustryForBuild = troopInfo->GetNeededIndustry();
		m_iNeededTitanForBuild = troopInfo->GetNeededResources()[TITAN];
		m_iNeededDeuteriumForBuild = troopInfo->GetNeededResources()[DEUTERIUM];
		m_iNeededDuraniumForBuild = troopInfo->GetNeededResources()[DURANIUM];
		m_iNeededCrystalForBuild = troopInfo->GetNeededResources()[CRYSTAL];
		m_iNeededIridiumForBuild = troopInfo->GetNeededResources()[IRIDIUM];
		m_iNeededDeritiumForBuild = NULL;
		// hier auch noch den eventuellen Bonus durch die Uniqueforschung "Truppen"
		if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)	// 4 -> Truppen
		{
			m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(3)*m_iNeededIndustryForBuild/100);
			m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(3)*m_iNeededTitanForBuild/100);
			m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(3)*m_iNeededDeuteriumForBuild/100);
			m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(3)*m_iNeededDuraniumForBuild/100);
			m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(3)*m_iNeededCrystalForBuild/100);
			m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(3)*m_iNeededIridiumForBuild/100);
		}
	}
	// hier noch den eventuellen Bonus durch die Uniqueforschung "Wirtschaft"
	if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)	// 5 -> Wirtschaft
	{
		m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededIndustryForBuild/100);
		m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededTitanForBuild/100);
		m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededDeuteriumForBuild/100);
		m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededDuraniumForBuild/100);
		m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededCrystalForBuild/100);
		m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededIridiumForBuild/100);
	}
	// Modifikator noch mit einrechnen
	m_iNeededIndustryForBuild	= (UINT)(m_iNeededIndustryForBuild * modifier);
	m_iNeededTitanForBuild		= (UINT)(m_iNeededTitanForBuild * modifier);
	m_iNeededDeuteriumForBuild	= (UINT)(m_iNeededDeuteriumForBuild * modifier);
	m_iNeededDuraniumForBuild	= (UINT)(m_iNeededDuraniumForBuild * modifier);
	m_iNeededCrystalForBuild	= (UINT)(m_iNeededCrystalForBuild * modifier);
	m_iNeededIridiumForBuild	= (UINT)(m_iNeededIridiumForBuild * modifier);
	m_iNeededDeritiumForBuild	= (UINT)(m_iNeededDeritiumForBuild * modifier);
}

// Funktion berechnet die Kosten f�r jedes Update in der Bauliste. Wird in der NextRound() Funktion aufgerufen.
// Weil in der Zwischenzeit h�tte der Spieler ein Geb�ude bauen k�nnen, das die Kosten mancher Updates ver�ndern
// k�nnte. �bergeben wird ein Zeiger auf das Feld aller Geb�udeinformationen, ein Zeiger auf alle stehenden
// Geb�ude im System und ein Zeiger auf die Forschungsinformation.
void CAssemblyList::CalculateNeededRessourcesForUpdate(const BuildingInfoArray* follower, BuildingArray* m_Buildings, CResearchInfo* ResearchInfo)
{
	int NumberOfBuildings = m_Buildings->GetSize();
	for (int j = 0; j < ALE; j++)
		if (m_iEntry[j] < 0)
		{
			// Runningnumber des Geb�udes, welches hier geupdated werden soll
			// dies w�re der Vorg�nger der RunningNumber die in der Bauliste steht
			USHORT RunningNumber = follower->GetAt(follower->GetAt(m_iEntry[j]*(-1)-1).GetPredecessorID()-1).GetRunningNumber();
			m_iNeededIndustryForBuild	= 0;
			m_iNeededTitanForBuild		= 0;
			m_iNeededDeuteriumForBuild	= 0;
			m_iNeededDuraniumForBuild	= 0;
			m_iNeededCrystalForBuild	= 0;
			m_iNeededIridiumForBuild	= 0;
			m_iNeededDeritiumForBuild	= 0;
			for (int i = 0; i < NumberOfBuildings; i++)
				if (m_Buildings->GetAt(i).GetRunningNumber() == RunningNumber)
				{
					// zur Zeit 80% der Rohstoffe aller Geb�ude des Typs aufbringen und 60% der Industrie
					// Kosten sind die prozentualen des Nachfolgers
					m_iNeededTitanForBuild		+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededTitan()*0.8);
					m_iNeededDeuteriumForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededDeuterium()*0.8);
					m_iNeededDuraniumForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededDuranium()*0.8);
					m_iNeededCrystalForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededCrystal()*0.8);
					m_iNeededIridiumForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededIridium()*0.8);
					m_iNeededIndustryForBuild	+= (int)(follower->GetAt(m_iEntry[j]*(-1)-1).GetNeededIndustry()*0.6);
					// Total Cost == (Cost of Level X+1 Building) - ((Cost of Level X Building) * Recycle factor)

				}
			// hier noch den eventuellen Bonus durch die Uniqueforschung "Wirtschaft"
			if (ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)	// 5 -> Wirtschaft
			{
				m_iNeededIndustryForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededIndustryForBuild/100);
				m_iNeededTitanForBuild		-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededTitanForBuild/100);
				m_iNeededDeuteriumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededDeuteriumForBuild/100);
				m_iNeededDuraniumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededDuraniumForBuild/100);
				m_iNeededCrystalForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededCrystalForBuild/100);
				m_iNeededIridiumForBuild	-= (int)(ResearchInfo->GetResearchComplex(RESEARCH_COMPLEX::ECONOMY)->GetBonus(2)*m_iNeededIridiumForBuild/100);
			}
			// Wenn an der ersten Stelle der Auftragsliste ein Upgrade steht, kann man die Berechnung nicht so einfach
			// durchf�hren, als h�tte man es erst reingestellt. Denn dann w�rde der Bau nicht voranschreiten.
			// Man muss beachten, dass schon an diesem Auftrag gebaut wurde. Zuviel aufgebrachte Ressourcen bekommt man
			// aber nicht zur�ck.
			if (j == 0)
			{
				if ((ULONG)m_iNeededIndustryInAssemblyList[j] > m_iNeededIndustryForBuild)
					m_iNeededIndustryInAssemblyList[j]	= m_iNeededIndustryForBuild;
				if (m_iNeededTitanInAssemblyList[j]		> m_iNeededTitanForBuild)
					m_iNeededTitanInAssemblyList[j]		= m_iNeededTitanForBuild;
				if (m_iNeededDeuteriumInAssemblyList[j] > m_iNeededDeuteriumForBuild)
					m_iNeededDeuteriumInAssemblyList[j] = m_iNeededDeuteriumForBuild;
				if (m_iNeededDuraniumInAssemblyList[j]	> m_iNeededDuraniumForBuild)
					m_iNeededDuraniumInAssemblyList[j]	= m_iNeededDuraniumForBuild;
				if (m_iNeededCrystalInAssemblyList[j]	> m_iNeededCrystalForBuild)
					m_iNeededCrystalInAssemblyList[j]	= m_iNeededCrystalForBuild;
				if (m_iNeededIridiumInAssemblyList[j]	> m_iNeededIridiumForBuild)
					m_iNeededIridiumInAssemblyList[j]	= m_iNeededIridiumForBuild;
			}
			// Wenn der Auftrag nicht an erster Stelle in der Auftragsliste steht, dann kann man jedes Mal die
			// Ressourcen wieder von vorn berechnne lassen. Also so als h�tten wir dieses Upgrade gerade reingestellt.
			else
			{
				m_iNeededIndustryInAssemblyList[j]	= m_iNeededIndustryForBuild;
				m_iNeededTitanInAssemblyList[j]		= m_iNeededTitanForBuild;
				m_iNeededDeuteriumInAssemblyList[j] = m_iNeededDeuteriumForBuild;
				m_iNeededDuraniumInAssemblyList[j]	= m_iNeededDuraniumForBuild;
				m_iNeededCrystalInAssemblyList[j]	= m_iNeededCrystalForBuild;
				m_iNeededIridiumInAssemblyList[j]	= m_iNeededIridiumForBuild;
			}
		}
}

// Diese Funktion entfernt die ben�tigten Ressourcen aus dem lokalen Lager des Systems und falls Ressourcenrouten
// bestehen auch die Ressourcen in den Startsystemen der Route. Aber nur falls dies auch notwendig sein sollte.
void CAssemblyList::RemoveResourceFromStorage(BYTE res, const CPoint &ko, std::vector<CSystem>& systems, CArray<CPoint>* routesFrom)
{
	if (ko == CPoint(-1,-1))
		return;

	CSystem *system = &systems.at(ko.x+(ko.y)*STARMAP_SECTORS_HCOUNT);

	// f�r Deritium gibt es keine Ressourcenroute
	if (res != DERITIUM)
	{
		// zuerst wird immer versucht, die Ressourcen aus dem lokalen Lager zu nehmen
		long remainingRes = GetNeededResourceInAssemblyList(0, res) - system->GetResourceStore(res);
		// werden zus�tzliche Ressourcen aus anderen Lagern ben�tigt, so kann das lokale Lager
		// auf NULL gesetzt werden
		if (remainingRes > 0)
		{
			*system->GetResourceStorages(res) = NULL;
			// zus�tzliche Ressourcen m�ssen aus den Lagern der Systeme mit den Ressourcenrouten
			// bezogen werden. Daf�r ein Feld anlegen, indem alle Startsysteme mit der zur Ressouce passenden
			// Ressourcenroute beinhaltet sind.
			struct ROUTELIST {
				CResourceRoute *route;
				CPoint fromSystem;

				ROUTELIST() : route(0), fromSystem(0) {}
				ROUTELIST(CResourceRoute *_route, CPoint _fromSystem) : route(_route), fromSystem(_fromSystem) {}
			};
			CArray<ROUTELIST> routes;
			for (int j = 0; j < routesFrom->GetSize(); j++)
			{
				CPoint p = routesFrom->GetAt(j);
				for (int k = 0; k < systems.at(p.x+(p.y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetSize(); k++)
				{
					// Stimmt die Ressource �berein=
					if (systems.at(p.x+(p.y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetAt(k).GetResource() == res)
					{
						// Stimmt das Zielsystem mit unserem �berein?
						if (systems.at(p.x+(p.y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetAt(k).GetKO() == ko)
						{
							// pr�fen das die Route nicht schon verwendet wird
							bool bUsed = false;
							for (int l = 0; l < routes.GetSize(); l++)
								if (routes.GetAt(l).fromSystem == p)
								{
									bUsed = true;
									break;
								}
							if (!bUsed)
								routes.Add(ROUTELIST(&systems.at(p.x+(p.y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetAt(k), p));
						}
					}
				}
			}
			// in routes sind nun die Zeiger auf die richtigen Ressourcenrouten, also die Routen, welche auch den
			// passenden Rohstoff liefern k�nnten.
			while (routes.GetSize())
			{
				// zuf�llig eine Route aus den m�glichen ausw�hlen, damit nicht immer das gleiche System zuerst
				// die Rohstoffe liefern muss, falls mehrere Routen der selben Art ins System eingehen.
				int random = rand()%routes.GetSize();
				int percent = 0;
				CPoint start = routes.GetAt(random).fromSystem;
				// sind im jeweiligen Lager des Startsystem gen�gend Rohstoffe vorhanden
				if (systems.at(start.x+(start.y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(res) >= (ULONG)remainingRes)
				{
					*systems.at(start.x+(start.y)*STARMAP_SECTORS_HCOUNT).GetResourceStorages(res) -= remainingRes;
					if (GetNeededResourceInAssemblyList(0, res) > NULL)
						percent = 100 * remainingRes / GetNeededResourceInAssemblyList(0, res);
					CResourceRoute* pResRoute = routes.GetAt(random).route;
					pResRoute->SetPercent((BYTE)percent);
					remainingRes = 0;
				}
				else
				{
					remainingRes -= systems.at(start.x+(start.y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(res);
					if (GetNeededResourceInAssemblyList(0, res) > NULL)
						percent = 100 * systems.at(start.x+(start.y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(res) / GetNeededResourceInAssemblyList(0, res);
					CResourceRoute* pResRoute = routes.GetAt(random).route;
					pResRoute->SetPercent((BYTE)percent);
					*systems.at(start.x+(start.y)*STARMAP_SECTORS_HCOUNT).GetResourceStorages(res) = NULL;
				}
				// ROUTELIST Eintrag entfernen, wenn dieser abgearbeitet wurde
				routes.RemoveAt(random);

				// werden keine Ressourcen mehr ben�tigt, so kann abgebrochen werden
				if (remainingRes == 0)
				{
					routes.RemoveAll();
					break;
				}
			}
			ASSERT(remainingRes == 0);
		}
		// anderenfalls werden nur die ben�tigten Ressourcen aus dem lokalen Lager abgezogen
		else
			*system->GetResourceStorages(res) -= GetNeededResourceInAssemblyList(0, res);
	}
	else
		*system->GetResourceStorages(res) -= m_iNeededDeritiumInAssemblyList[0];
}

BOOLEAN CAssemblyList::MakeEntry(int runningNumber, const CPoint &ko, std::vector<CSystem>& systems, bool bOnlyTest)
{
	// Die Assemblylist durchgehen, ob wir einen Eintrag finden, der noch 0 ist
	// dort k�nnen wir den n�chsten speichern, gibt es keinen, dann ist die
	// Bauliste voll
	int entry = -1;
	if (!bOnlyTest)
	{
		for (int i = 0; i < ALE; i++)
		{
			if (m_iEntry[i] == 0)
			{
				entry = i;
				break;
			}
		}

		// pr�fen ob Bauliste schon voll!
		if (entry == -1)
			return FALSE;
	}

	CSystem* system = &systems.at(ko.x+(ko.y)*STARMAP_SECTORS_HCOUNT);
	// Ressourcenrouten durchgehen und wom�glich die m�glichen max. zus�tzlichen Ressourcen erfragen
	CArray<CPoint> routesFrom;
	ULONG resourcesFromRoutes[DERITIUM + 1];
	ULONG nResInDistSys[DERITIUM + 1];
	CPoint ptResourceDistributorKOs[DERITIUM + 1];

	for (int i = 0; i <= DERITIUM; i++)
	{
		resourcesFromRoutes[i] = 0;
		nResInDistSys[i] = 0;
		ptResourceDistributorKOs[i] = CPoint(-1,-1);
	}

	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetOwnerOfSystem() == system->GetOwnerOfSystem() && CPoint(x,y) != ko)
			{
				for (int i = 0; i < systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetSize(); i++)
				{
					if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetAt(i).GetKO() == ko)
					{
						if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetBlockade() == NULL && systems.at(ko.x+(ko.y)*STARMAP_SECTORS_HCOUNT).GetBlockade() == NULL)
						{
							routesFrom.Add(CPoint(x,y));
							BYTE res = systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetAt(i).GetResource();
							resourcesFromRoutes[res] += systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(res);
						}
					}
				}
				// gilt nicht bei Blockaden
				if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetBlockade() == NULL && systems.at(ko.x+(ko.y)*STARMAP_SECTORS_HCOUNT).GetBlockade() == NULL)
				{
					for (int res = TITAN; res <= DERITIUM; res++)
					{
						if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetProduction()->GetResourceDistributor(res))
						{
							ptResourceDistributorKOs[res] = CPoint(x,y);
							nResInDistSys[res] = systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(res);
						}
					}
				}
			}
		}
	}
	// �berpr�fen, ob wir gen�gend Rohstoffe in dem Lager haben
	for (int res = TITAN; res <= DERITIUM; res++)
	{
		UINT nNeededRes = this->GetNeededResourceForBuild(res);
		if (*system->GetResourceStorages(res) + resourcesFromRoutes[res] < nNeededRes && nResInDistSys[res] < nNeededRes)
			return FALSE;
	}
	if (bOnlyTest)
		return TRUE;

	// Ansonsten gibt es gen�gend Rohstoffe
	m_iEntry[entry] = runningNumber;
	// Was wir f�r das notwendige Projekt alles so brauchen speichern
	m_iNeededIndustryInAssemblyList[entry] = m_iNeededIndustryForBuild;
	m_iNeededTitanInAssemblyList[entry]	   = m_iNeededTitanForBuild;
	m_iNeededDeuteriumInAssemblyList[entry]= m_iNeededDeuteriumForBuild;
	m_iNeededDuraniumInAssemblyList[entry] = m_iNeededDuraniumForBuild;
	m_iNeededCrystalInAssemblyList[entry]  = m_iNeededCrystalForBuild;
	m_iNeededIridiumInAssemblyList[entry]  = m_iNeededIridiumForBuild;
	m_iNeededDeritiumInAssemblyList[entry]= m_iNeededDeritiumForBuild;
	// Nur wenn es der erste Eintrag im Baumen� ist wird alles abgezogen
	// ansonsten erst, nachdem das Projekt im ersten Eintrag fertig ist
	if (entry == 0)
	{
		for (int res = TITAN; res <= DERITIUM; res++)
		{
			UINT nNeededRes = this->GetNeededResourceForBuild(res);
			if (nNeededRes > 0)
			{
				// Ressource wird aus eigenem System bzw. �ber Ressourcenroute geholt
				if (*system->GetResourceStorages(res) + resourcesFromRoutes[res] >= nNeededRes)
					RemoveResourceFromStorage(res, ko, systems, &routesFrom);
				// reicht das nicht, so wird Ressource aus dem Verteier geholt
				else
				{
					CArray<CPoint> vNullRoutes;
					RemoveResourceFromStorage(res, ptResourceDistributorKOs[res], systems, &vNullRoutes);
				}
			}
		}
	}

	// Eintrag konnte gesetzt werden
	return TRUE;
}

// Funktion berechnet die Kosten des Bauauftrags, wenn man dieses sofort kaufen will.
// Die Kosten des Sofortkaufes sind von den Marktpreisen abh�ngig.
void CAssemblyList::CalculateBuildCosts(USHORT resPrices[5])
{
	if (m_iNeededIndustryInAssemblyList[0] > 1)
	{
		// Baukosten sind nun von den Kursen am Markt abh�ngig. Da die Preise am Markt f�r
		// 1000 Einheiten gelten wird hier durch 1000 geteilt. Die Preise vom Markt sind die,
		// welche wir am Anfang der Runde an unserer B�rse hatten. Damit ist es nicht m�glich die
		// Preise in der selben Runde wo wir kaufen wollen noch zu dr�cken.

		//  EDIT:
		//		Ich teile mittlerweile die Ressourcenpreise durch 2000, da der Kauf immer etwas teuer war.
		//		Dies hat aber rein balancingtechnische Gr�nde.
		// /EDIT
		m_iBuildCosts =         m_iNeededIndustryInAssemblyList[0];
		m_iBuildCosts += ((int)(m_iNeededTitanInAssemblyList[0]*resPrices[TITAN]/2000));
		m_iBuildCosts += ((int)(m_iNeededDeuteriumInAssemblyList[0]*resPrices[DEUTERIUM]/2000));
		m_iBuildCosts += ((int)(m_iNeededDuraniumInAssemblyList[0]*resPrices[DURANIUM]/2000));
		m_iBuildCosts += ((int)(m_iNeededCrystalInAssemblyList[0]*resPrices[CRYSTAL]/2000));
		m_iBuildCosts += ((int)(m_iNeededIridiumInAssemblyList[0]*resPrices[IRIDIUM]/2000));
	}
}

// Funktion setzt die noch restlichen Baukosten auf 1 und sagt, dass wir jetzt was gekauft haben
// Wenn wir kaufen k�nnen bestimmt die Fkt "CalculateBuildCosts()". Diese Fkt. immer vorher aufrufen.
// Die Creditskosten werden zur�ckgegeben
int CAssemblyList::BuyBuilding(int EmpiresCredits)
{
	// EmpiresCredits ist das aktuelle Credits des Imperiums
	if (m_iNeededIndustryInAssemblyList[0] > 1)
		if (m_iBuildCosts <= EmpiresCredits)
		{
			m_iNeededIndustryInAssemblyList[0] = 1;
			m_bWasBuildingBought = TRUE;
			return m_iBuildCosts;	// Gibt die Kosten zur�ck, wenn diese kleiner/gleich dem Creditsbestand sind
		}
	return 0;	// ansonsten gibts ne NULL zur�ck
}

// Die Funktion berechnet was nach dem Tick noch an verbleibender Industrieleistung aufzubringen ist.
// Ist der Bauauftrag fertig gibt die Funktion ein TRUE zur�ck. Wenn wir ein TRUE zur�ckbekommen, m�ssen
// wir direkt danach die Funktion ClearAssemblyList() aufrufen!
BOOLEAN CAssemblyList::CalculateBuildInAssemblyList(USHORT m_iIndustryProd)
{
	// Nur normale Geb�ude und Updates
	m_iNeededIndustryInAssemblyList[0] -= m_iIndustryProd;
	// Die ben�tigten Rohstoffe werden intern pro Runde halbiert
	// -> wenn wir also einen Auftrag abrechen, bekommen wir nicht
	// immer den vollen Betrag zur�ck.
	// z.B. nach 1 Runde  nur noch 3/4      => 0,75%
	// z.B. nach 2 Runden nur noch 9/16     => 0,5625%
	// z.B. nach 5 Runden nur noch 243/1024 => 0,2373046875%
	m_iNeededTitanInAssemblyList[0]		= (int)(m_iNeededTitanInAssemblyList[0]*0.75);
	m_iNeededDeuteriumInAssemblyList[0] = (int)(m_iNeededDeuteriumInAssemblyList[0]*0.75);
	m_iNeededDuraniumInAssemblyList[0]  = (int)(m_iNeededDuraniumInAssemblyList[0]*0.75);
	m_iNeededCrystalInAssemblyList[0]   = (int)(m_iNeededCrystalInAssemblyList[0]*0.75);
	m_iNeededIridiumInAssemblyList[0]	= (int)(m_iNeededIridiumInAssemblyList[0]*0.75);
	m_iNeededDeritiumInAssemblyList[0] = (int)(m_iNeededDeritiumInAssemblyList[0]*0.75);
	if (m_iNeededIndustryInAssemblyList[0] <= 0)
	{
		m_iNeededIndustryInAssemblyList[0] = 0;
		m_iNeededTitanInAssemblyList[0]	   = 0;
		m_iNeededDeuteriumInAssemblyList[0]= 0;
		m_iNeededDuraniumInAssemblyList[0] = 0;
		m_iNeededCrystalInAssemblyList[0]  = 0;
		m_iNeededIridiumInAssemblyList[0]  = 0;
		m_iNeededDeritiumInAssemblyList[0]= 0;
		return TRUE;
		// Wenn wir TRUE zur�ckbekommen m�ssen wir direkt danach die
		// Funktion ClearAssemblyList() aufrufen!
	}
	return FALSE;
}

// Funktion l�scht einen Eintrag aus der Bauliste, wenn das Geb�ude fertig wurde oder wir den ersten
// Eintrag manuell l�schen m�chten. Nach Aufruf dieser Funktion mu� unbedingt die Funktion
// CalculateVariables() aufgerufen werden.
void CAssemblyList::ClearAssemblyList(const CPoint &ko, std::vector<CSystem>& systems)
{
	// Alle prozentualen Anteile eines wom�glich fr�heren Bauauftrages aus den Ressourcenrouten l�schen
	CSystem* system = &systems.at(ko.x+(ko.y)*STARMAP_SECTORS_HCOUNT);

	CArray<CPoint> routesFrom;
	ULONG resourcesFromRoutes[DERITIUM + 1];
	ULONG nResInDistSys[DERITIUM + 1];
	CPoint ptResourceDistributorKOs[DERITIUM + 1];

	for (int i = 0; i <= DERITIUM; i++)
	{
		resourcesFromRoutes[i] = 0;
		nResInDistSys[i] = 0;
		ptResourceDistributorKOs[i] = CPoint(-1,-1);
	}

	// Ressourcenrouten durchgehen und wom�glich die m�glichen max. zus�tzlichen Ressourcen erfragen
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetOwnerOfSystem() == system->GetOwnerOfSystem() && CPoint(x,y) != ko)
			{
				for (int i = 0; i < systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetSize(); i++)
				{
					if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetAt(i).GetKO() == ko)
					{
						// prozentualen Anteil vom alten Auftrag zur�cksetzen
						systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->ElementAt(i).SetPercent(0);
						// Ressourcen �ber Route holen
						if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetBlockade() == NULL && systems.at(ko.x+(ko.y)*STARMAP_SECTORS_HCOUNT).GetBlockade() == NULL)
						{
							routesFrom.Add(CPoint(x,y));
							BYTE res = systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceRoutes()->GetAt(i).GetResource();
							resourcesFromRoutes[res] += systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(res);
						}
					}
				}
				// gilt nicht bei Blockaden
				if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetBlockade() == NULL && systems.at(ko.x+(ko.y)*STARMAP_SECTORS_HCOUNT).GetBlockade() == NULL)
				{
					for (int res = TITAN; res <= DERITIUM; res++)
					{
						if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetProduction()->GetResourceDistributor(res))
						{
							ptResourceDistributorKOs[res] = CPoint(x,y);
							nResInDistSys[res] = systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(res);
						}
					}
				}
			}
		}
	}

	// AssemblyList Eintrag des gebauten Geb�udes/Updates/Schiffes l�schen, wenn wir noch den
	// Eintrag an der n�chsten Stelle haben (sprich AssemblyList[1] != 0), dann alle
	// anderen Eintr�ge um eins nach vorn verschieben -> letzter wird frei
	m_iEntry[0] = 0;
	m_iNeededIndustryInAssemblyList[0] = 0;
	m_iNeededTitanInAssemblyList[0]    = 0;
	m_iNeededDeuteriumInAssemblyList[0]= 0;
	m_iNeededDuraniumInAssemblyList[0] = 0;
	m_iNeededCrystalInAssemblyList[0]  = 0;
	m_iNeededIridiumInAssemblyList[0]  = 0;
	m_iNeededDeritiumInAssemblyList[0]= 0;

	for (int i = 0; i < ALE - 1; i++)
	{
		// wenn der n�chste Eintrag ungleich 0 ist
		if (m_iEntry[i + 1] != 0)
		{
			m_iEntry[i] = m_iEntry[i + 1];
			m_iNeededIndustryInAssemblyList[i] = m_iNeededIndustryInAssemblyList[i + 1];
			m_iNeededTitanInAssemblyList[i]	   = m_iNeededTitanInAssemblyList[i + 1];
			m_iNeededDeuteriumInAssemblyList[i]= m_iNeededDeuteriumInAssemblyList[i + 1];
			m_iNeededDuraniumInAssemblyList[i] = m_iNeededDuraniumInAssemblyList[i + 1];
			m_iNeededCrystalInAssemblyList[i]  = m_iNeededCrystalInAssemblyList[i + 1];
			m_iNeededIridiumInAssemblyList[i]  = m_iNeededIridiumInAssemblyList[i + 1];
			m_iNeededDeritiumInAssemblyList[i]= m_iNeededDeritiumInAssemblyList[i + 1];

			// den Nachfolger �berall auf NULL setzen
			m_iEntry[i + 1] = 0;
			m_iNeededIndustryInAssemblyList[i + 1] = 0;
			m_iNeededTitanInAssemblyList[i + 1]    = 0;
			m_iNeededDeuteriumInAssemblyList[i + 1]= 0;
			m_iNeededDuraniumInAssemblyList[i + 1] = 0;
			m_iNeededCrystalInAssemblyList[i + 1]  = 0;
			m_iNeededIridiumInAssemblyList[i + 1]  = 0;
		}
		else
			break;
	}

	// Checken, ob der n�chste Eintrag auch baubar ist -> gen�gend RES im Lager
	// normalerweise kann man in der Bauliste ja nur Eintr�ge vornehmen, wenn
	// man gen�gend RES hat. Also sollte er auch baubar sein, wenn die IP
	// erbracht wurden. Aber durch Zufallsereignisse oder B�rsenverk�ufe von RES
	// kann man sp�ter ja zu wening davon haben. Und weil die RES erst abgezogen
	// wird, wenn das Geb�ude an erster Stelle in der Bauliste r�ckt, m�ssen
	// wird das �berpr�fen. Haben wir nicht genug RES, wird der Bauauftrag
	// gecancelt

	// �berpr�fen, ob wir gen�gend Rohstoffe in dem Lager haben
	for (int res = TITAN; res <= DERITIUM; res++)
	{
		UINT nNeededRes = this->GetNeededResourceInAssemblyList(0, res);
		if (*system->GetResourceStorages(res) + resourcesFromRoutes[res] < nNeededRes && nResInDistSys[res] < nNeededRes)
		{
			// Wenn nicht -> dann Eintrag wieder entfernen
			ClearAssemblyList(ko, systems);
			return;
		}
	}

	// Wenn er baubar ist, dann die Ressourcen entfernen
	for (int res = TITAN; res <= DERITIUM; res++)
	{
		UINT nNeededRes = this->GetNeededResourceInAssemblyList(0, res);
		if (nNeededRes > 0)
		{
			// Ressource wird aus eigenem System bzw. �ber Ressourcenroute geholt
			if (*system->GetResourceStorages(res) + resourcesFromRoutes[res] >= nNeededRes)
				RemoveResourceFromStorage(res, ko, systems, &routesFrom);
			// reicht das nicht, so wird Ressource aus dem Verteiler geholt
			else
			{
				CArray<CPoint> vNullRoutes;
				RemoveResourceFromStorage(res, ptResourceDistributorKOs[res], systems, &vNullRoutes);
			}
		}
	}
}

// Ordnet die Bauliste so, dass keine leeren Eintr�ge in der Mitte vorkommen k�nnen.
// Wird z.B. aufgerufen, nachdem wir einen Auftrag aus der Bauliste entfernt haben.
// Darf aber nicht aufgerufen werden, wenn wir den 0. Eintrag entfernen, dann m�ssen
// wir ClearAssemblyList() aufrufen, weil diese Funktion die n�tigen Rohstoffe gleich mit abzieht.
void CAssemblyList::AdjustAssemblyList(short entry)
{
	m_iEntry[entry] = 0;
	for (int i = entry; i < ALE-1; i++)	// Bauliste durchgehen
		if (m_iEntry[i] == 0)			// wenn der Eintrag 0 ist -> also nix
			if (i+1 != 0)				// und der n�chste Eintrag nicht 0 ist
			{							// n�chsten Eintrag um eins nach vorn
				m_iEntry[i] = m_iEntry[i+1];
				m_iNeededIndustryInAssemblyList[i]  = m_iNeededIndustryInAssemblyList[i+1];
				m_iNeededTitanInAssemblyList[i]		= m_iNeededTitanInAssemblyList[i+1];
				m_iNeededDeuteriumInAssemblyList[i] = m_iNeededDeuteriumInAssemblyList[i+1];
				m_iNeededDuraniumInAssemblyList[i]  = m_iNeededDuraniumInAssemblyList[i+1];
				m_iNeededCrystalInAssemblyList[i]	= m_iNeededCrystalInAssemblyList[i+1];
				m_iNeededIridiumInAssemblyList[i]	= m_iNeededIridiumInAssemblyList[i+1];
				m_iNeededDeritiumInAssemblyList[i] = m_iNeededDeritiumInAssemblyList[i+1];
				m_iEntry[i+1] = 0;
			}
}

// Resetfunktion
void CAssemblyList::Reset()
{
	m_iNeededIndustryForBuild = 0;	// ben�tigte IP zum Bau
	m_iNeededTitanForBuild = 0;		// ben�tigtes Titan zum Bau
	m_iNeededDeuteriumForBuild = 0;	// ben�tigtes Deuterium zum Bau
	m_iNeededDuraniumForBuild = 0;	// ben�tigtes Duranium zum Bau
	m_iNeededCrystalForBuild = 0;	// ben�tigtes Crystal zum Bau
	m_iNeededIridiumForBuild = 0;	// ben�tigtes Iridium zum Bau
	m_iNeededDeritiumForBuild = 0;	// ben�tigtes Deritium zum Bau
	m_bWasBuildingBought = 0;		// Wurde ein Geb�ude gekauft
	m_iBuildCosts = 0;				// Die Baukosten eines Bauauftrags
	for (int i = 0; i < ALE; i++)
	{
		m_iEntry[i] = 0;						// Bauauftrag
		m_iNeededIndustryInAssemblyList[i] = 0; // noch ben�tigte IP zum fertigstellen des Projektes
		m_iNeededTitanInAssemblyList[i] = 0;	// noch ben�tigtes Titan zum fertigstellen des Projektes
		m_iNeededDeuteriumInAssemblyList[i] = 0;// noch ben�tigtes Deuterium zum fertigstellen des Projektes
		m_iNeededDuraniumInAssemblyList[i] = 0;	// noch ben�tigtes Duranium zum fertigstellen des Projektes
		m_iNeededCrystalInAssemblyList[i] = 0;	// noch ben�tigtes Crystal zum fertigstellen des Projektes
		m_iNeededIridiumInAssemblyList[i] = 0;	// noch ben�tigtes Iridium zum fertigstellen des Projektes
		m_iNeededDeritiumInAssemblyList[i] = 0;// noch ben�tigtes Deritium zum fertigstellen des Projektes
	}
}
