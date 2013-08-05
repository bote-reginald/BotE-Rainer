// Empire.cpp: Implementierung der Klasse CEmpire.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Empire.h"
#include "System\System.h"
#include "Galaxy\Sector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CEmpire, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CEmpire::CEmpire()
{
	Reset();
}
CEmpire::~CEmpire()
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEmpire::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	m_Research.Serialize(ar);
	m_Intelligence.Serialize(ar);
	m_GlobalStorage.Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		//MYTRACE("logdata")(MT::LEVEL_DEBUG, "--------------------------------------------");
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Empire: m_sEmpireID: = %s, m_nNumberOfSystems = %i, m_lFP = %i\n", m_sEmpireID, m_nNumberOfSystems, m_lFP);
		ar << m_nNumberOfSystems;
		//MYTRACE("logdata")(MT::LEVEL_DEBUG, "m_nNumberOfSystems (Empire) = %i\n", m_nNumberOfSystems);
		ar << m_iCredits;
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Empire: m_sEmpireID: = %s, m_iCredits = %i, Change = %i\n", m_sEmpireID, m_iCredits, m_iCreditsChange);
		ar << m_iCreditsChange;
		//MYTRACE("logdata")(MT::LEVEL_DEBUG, "m_iCreditsChange (Empire) = %i\n", m_iCreditsChange);
		ar << m_iShipCosts;
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Empire: m_sEmpireID: = %s, m_iShipCosts = %i, Support = %i\n", m_sEmpireID, m_iShipCosts, m_iPopSupportCosts);
		ar << m_iPopSupportCosts;
		//MYTRACE("logdata")(MT::LEVEL_DEBUG, "m_iPopSupportCosts (Empire) = %i\n", m_iPopSupportCosts);
		ar << m_lFP;
		//MYTRACE("logdata")(MT::LEVEL_DEBUG, "m_lFP (Empire) = %i\n", m_lFP);
		for (int i = 0; i <= DERITIUM; i++)
		{
			ar << m_lResourceStorages[i];
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Empire: m_sEmpireID: = %s, m_lResourceStorages[i] = %i\n", m_sEmpireID, m_lResourceStorages[i]);
		}
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Empire: --------------------------------------------");
		ar << m_sEmpireID;
		// see above MYTRACE("logdata")(MT::LEVEL_INFO, "m_sEmpireID (Empire): = %s\n", m_sEmpireID);
		ar << m_vMessages.GetSize();
		for (int i = 0; i < m_vMessages.GetSize(); i++)
		{
			m_vMessages.GetAt(i).Serialize(ar);
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "EmpireEvent:%s",m_vMessages);
		}
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Empire: --------------------------------------------");
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ClearMessagesAndEvents();
		int number = 0;
		ar >> m_nNumberOfSystems;
		ar >> m_iCredits;
		ar >> m_iCreditsChange;
		ar >> m_iShipCosts;
		ar >> m_iPopSupportCosts;
		ar >> m_lFP;
		for (int i = 0; i <= DERITIUM; i++)
			ar >> m_lResourceStorages[i];
		ar >> m_sEmpireID;
		ar >> number;
		m_vMessages.SetSize(number);
		for (int i = 0; i < number; i++)
			m_vMessages.GetAt(i).Serialize(ar);
	}

	m_Events.Serialize(ar);
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion generiert die Liste der Systeme f�r das Imperium anhand aller Systeme.
void CEmpire::GenerateSystemList(const std::vector<CSystem>& systems, const std::vector<CSector>& sectors)
{
	m_SystemList.RemoveAll();
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++) {
		const unsigned vertical_pos = y * STARMAP_SECTORS_HCOUNT;
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++) {
			const unsigned index = vertical_pos + x;
			if (systems.at(index).GetOwnerOfSystem() == m_sEmpireID)
			{
				m_SystemList.Add(EMPIRE_SYSTEMS(sectors.at(index).GetName(), CPoint(x,y)));
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Empire: System:%s (%d,%d)",EMPIRE_SYSTEMS(sectors.at(index).GetName(), CPoint(x,y)));
			}
		}
	}
}

/// Funktion addiert die �bergebene Anzahl an Forschungspunkten zu den Forschungspunkten des Imperiums.
void CEmpire::AddFP(int add)
{
	if (((long)m_lFP + add) < 0)
		m_lFP = 0;
	else
		m_lFP += add;
}

// Funktion setzt die Lager wieder auf NULL, damit wir die sp�ter wieder f�llen k�nnen
// die wirklichen Lager in den einzelnen Systemen werden dabei nicht anger�hrt
void CEmpire::ClearAllPoints(void)
{
	memset(m_lResourceStorages, 0, sizeof(m_lResourceStorages));
	m_lFP = 0;
	m_Intelligence.ClearSecurityPoints();
}

/// Funktion l�scht alle Nachrichten und Antworten an das Imperiums.
void CEmpire::ClearMessagesAndEvents(void)
{
	m_vMessages.RemoveAll();
	for (int i = 0; i < m_Events.GetSize(); i++)
		delete m_Events.GetAt(i);
	m_Events.RemoveAll();
}

void CEmpire::Reset(void)
{
	m_nNumberOfSystems = 1;
	m_sEmpireID = "";
	m_iCredits = 1000;
	m_iCreditsChange = 0;
	m_iShipCosts = 0;
	m_iPopSupportCosts = 0;

	m_Research.Reset();
	m_Intelligence.Reset();
	m_GlobalStorage.Reset();

	ClearAllPoints();
	ClearMessagesAndEvents();

	m_SystemList.RemoveAll();
}
