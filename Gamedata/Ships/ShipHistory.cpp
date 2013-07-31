#include "stdafx.h"
#include "ShipHistory.h"

#include "Ships/Ships.h"

IMPLEMENT_SERIAL (CShipHistory, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShipHistory::CShipHistory(void)
{
}

CShipHistory::~CShipHistory(void)
{
	m_ShipHistory.RemoveAll();
}
/*
//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CShipHistory::CShipHistory(const CShipHistory &rhs)
{

}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CShipHistory & CShipHistory::operator=(const CShipHistory & rhs)
{
	if (this == &rhs)
		return *this;

	return *this;
}
*/

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CShipHistory::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_ShipHistory.GetSize();
		for (int i = 0; i < m_ShipHistory.GetSize(); i++)
		{
			ar << m_ShipHistory.GetAt(i).m_iBuildRound;
			ar << m_ShipHistory.GetAt(i).m_iDestroyRound;
			ar << m_ShipHistory.GetAt(i).m_iExperiance;
			ar << m_ShipHistory.GetAt(i).m_strCurrentSector;
			ar << m_ShipHistory.GetAt(i).m_strCurrentTask;
			ar << m_ShipHistory.GetAt(i).m_strTarget;
			ar << m_ShipHistory.GetAt(i).m_strKindOfDestroy;
			ar << m_ShipHistory.GetAt(i).m_strSectorName;
			ar << m_ShipHistory.GetAt(i).m_strShipClass;
			ar << m_ShipHistory.GetAt(i).m_strShipName;
			ar << m_ShipHistory.GetAt(i).m_strShipType;
		}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		ar >> number;
		m_ShipHistory.RemoveAll();
		CShipHistoryStruct shs;
		for (int i = 0; i < number; i++)
		{
			ar >> shs.m_iBuildRound;
			ar >> shs.m_iDestroyRound;
			ar >> shs.m_iExperiance;
			ar >> shs.m_strCurrentSector;
			ar >> shs.m_strCurrentTask;
			ar >> shs.m_strTarget;
			ar >> shs.m_strKindOfDestroy;
			ar >> shs.m_strSectorName;
			ar >> shs.m_strShipClass;
			ar >> shs.m_strShipName;
			ar >> shs.m_strShipType;
			m_ShipHistory.Add(shs);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion f�gt das Schiff, auf welches der �bergebene Zeiger zeigt dem Feld hinzu. Alle Angaben werden
/// dabei automatisch gemacht. Es wird ebenfalls �berpr�ft, dass dieses Schiff nicht schon hinzugef�gt wurde.
/// Zus�tzlich m�ssen als Parameter noch der Name des Systems �bergeben werden, in dem das Schiff gebaut wurde,
/// sowie die aktuelle Runde.
void CShipHistory::AddShip(const CShips* ship, const CString& buildsector, short round)
{
	// �berpr�fen, das dieses Schiff nicht schon in der Liste der Schiffe vorhanden ist
	for (int i = 0; i < m_ShipHistory.GetSize(); i++)
		if (ship->GetShipName() == m_ShipHistory.GetAt(i).m_strShipName)
		{
			//AfxMessageBox("BUG: Ship -" + ship->GetShipName() + "-  allready exists in shiphistory!\nPlease post a bugreport at www.birth-of-the-empires.de");
			MYTRACE("general")(MT::LEVEL_INFO, "Ship already exists in shiphistory: %d\n", ship->GetShipName());
			return;
		}
	CShipHistoryStruct temp;
	temp.m_strShipName = ship->GetShipName(); 
	temp.m_strShipType = ship->GetShipTypeAsString();
	temp.m_strShipClass = ship->GetShipClass();
	temp.m_strSectorName = buildsector;
	temp.m_strCurrentSector = buildsector;
	temp.m_strCurrentTask = ship->GetCurrentOrderAsString();
	temp.m_strTarget = ship->GetCurrentTargetAsString();
	temp.m_strKindOfDestroy = "";
	temp.m_iBuildRound = round;
	temp.m_iDestroyRound = 0;
	temp.m_iExperiance = ship->GetCrewExperience();
	MYTRACE("logships")(MT::LEVEL_INFO, "ShipHistory.cpp: added: %s (%s), build in round: %d, Experiance:%i, Target:%s, Order:%s\n", 
		ship->GetShipName(), ship->GetShipTypeAsString(), round, ship->GetCrewExperience(),ship->GetCurrentTargetAsString(),ship->GetCurrentOrderAsString());


	m_ShipHistory.Add(temp);
}

/// Funktion modifiziert den Eintrag in dem Feld <code>m_ShipHistory<code>. Dabei wird das Schiff �bergeben, dessen
/// Werte modifiziert werden sollen. Wenn ein Schiff aus irgendeinem Grund (Kampf, Kolonisierung usw.) zerst�rt
/// wurde, wird f�r den Parameter <code>destroyRound<code> die aktuelle Runde der Zerst�rung �bergeben und
/// f�r den Parameter <code>destroyType<code> die Art der Zerst�rung als CString �bergeben. Au�erdem wird der neue
/// Status des Schiffes im Parameter <code>status<code> �bergeben, z.B. zerst�rt, vermisst usw.
/// Konnte das Schiff modifiziert werden, so gibt die Funktion <code>true</code> zur�ck, sonst <code>false</code>
bool CShipHistory::ModifyShip(const CShips* ship, const CString& sector, short destroyRound, const CString& destroyType, const CString& status)
{
	for (int i = 0; i < m_ShipHistory.GetSize(); i++)
	{
		if (m_ShipHistory.GetAt(i).m_strShipName == ship->GetShipName())
		{
			m_ShipHistory.ElementAt(i).m_strCurrentSector = sector;
			m_ShipHistory.ElementAt(i).m_strCurrentTask = ship->GetCurrentOrderAsString();
			m_ShipHistory.ElementAt(i).m_iExperiance = ship->GetCrewExperience();
			m_ShipHistory.ElementAt(i).m_strTarget = ship->GetCurrentTargetAsString();

			if (destroyRound != 0)
			{
				m_ShipHistory.ElementAt(i).m_iDestroyRound = destroyRound;
				m_ShipHistory.ElementAt(i).m_strKindOfDestroy = destroyType;
				m_ShipHistory.ElementAt(i).m_strSectorName = sector;
				m_ShipHistory.ElementAt(i).m_strCurrentTask = status;
			}
			else
			{
				m_ShipHistory.ElementAt(i).m_iDestroyRound = 0;
				m_ShipHistory.ElementAt(i).m_strKindOfDestroy = "";
			}

			return true;
		}
	}
	return false;
}

/// Funktion entfernt ein bestimmtes Schiff aus der Schiffshistory.
void CShipHistory::RemoveShip(const CShips* ship)
{
	for (int i = 0; i < m_ShipHistory.GetSize(); i++)
		if (m_ShipHistory.GetAt(i).m_strShipName == ship->GetShipName())
		{
			m_ShipHistory.RemoveAt(i);
			return;
		}

	//AfxMessageBox("BUG: Ship -" + ship->GetShipName() + "- doesn't exist in shiphistory!\nPlease post a bugreport at www.birth-of-the-empires.de");
	MYTRACE("general")(MT::LEVEL_DEBUG, "Ship doesn't exist in shiphistory: %s\n", ship->GetShipName());
}

/// Funktion gibt die Anzahl der noch lebenden Schiffe zur�ck, wenn der Parameter <code>shipAlive</code> wahr ist.
/// Ansonsten gibt die Funktion die Anzahl der zerst�rten Schiffe zur�ck.
UINT CShipHistory::GetNumberOfShips(BOOLEAN shipAlive) const
{
	UINT number = 0;
	for (UINT i = 0; i < GetSizeOfShipHistory(); i++)
	{
		if (IsShipAlive(i) && shipAlive)
			number++;
		else if (!IsShipAlive(i) && !shipAlive)
			number++;
	}
	return number;
}

/// Resetfunktion f�r die CShipHistory Klasse
void CShipHistory::Reset(void)
{
	for (int i = 0; i < m_ShipHistory.GetSize(); )
		m_ShipHistory.RemoveAt(i);
	m_ShipHistory.RemoveAll();
}
