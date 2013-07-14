#include "stdafx.h"
#include "IntelAssignment.h"
#include "Races\RaceController.h"

IMPLEMENT_SERIAL (CIntelAssignment, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelAssignment::CIntelAssignment(void)
{
	Reset();
}

CIntelAssignment::~CIntelAssignment(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CIntelAssignment::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		for (int i = 0; i < 2; i++)
		{
			ar << m_byPercentage[i].size();
			for (map<CString, BYTE>::const_iterator it = m_byPercentage[i].begin(); it != m_byPercentage[i].end(); ++it)
				ar << it->first << it->second;
		}
		for (int i = 0; i < 4; i++)
		{
			ar << m_bySpyPercentage[i].size();
			for (map<CString, BYTE>::const_iterator it = m_bySpyPercentage[i].begin(); it != m_bySpyPercentage[i].end(); ++it)
				ar << it->first << it->second;

			ar << m_bySabPercentage[i].size();
			for (map<CString, BYTE>::const_iterator it = m_bySabPercentage[i].begin(); it != m_bySabPercentage[i].end(); ++it)
				ar << it->first << it->second;
		}
	}
	else if (ar.IsLoading())
	{
		size_t mapSize = 0;
		CString key = "";
		BYTE value = 0;

		for (int i = 0; i < 2; i++)
		{
			m_byPercentage[i].clear();
			ar >> mapSize;
			for (size_t t = 0; t < mapSize; t++)
			{
				ar >> key;
				ar >> value;
				m_byPercentage[i][key] = value;
			}
		}
		for (int i = 0; i < 4; i++)
		{
			m_bySpyPercentage[i].clear();
			ar >> mapSize;
			for (size_t t = 0; t < mapSize; t++)
			{
				ar >> key;
				ar >> value;
				m_bySpyPercentage[i][key] = value;
			}

			m_bySabPercentage[i].clear();
			ar >> mapSize;
			for (size_t t = 0; t < mapSize; t++)
			{
				ar >> key;
				ar >> value;
				m_bySabPercentage[i][key] = value;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion gibt die globale prozentuale Zuteilung auf die innere Sicherheit zur�ck.
BYTE CIntelAssignment::GetInnerSecurityPercentage() const
{
	int perc = 100;
	for (map<CString, BYTE>::const_iterator it = m_byPercentage[0].begin(); it != m_byPercentage[0].end(); ++it)
		perc -= it->second;
	for (map<CString, BYTE>::const_iterator it = m_byPercentage[1].begin(); it != m_byPercentage[1].end(); ++it)
		perc -= it->second;
	return perc;
}

/// Funktion gibt die einzelnen Spionagezuteilungen f�r eine bestimmte Rasse zur�ck.
BYTE CIntelAssignment::GetSpyPercentages(const CString& sRace, BYTE type)
{
	if (type != 4)
	{
		map<CString, BYTE>::const_iterator it = m_bySpyPercentage[type].find(sRace);
		if (it != m_bySpyPercentage[type].end())
			return it->second;
		else
			return 0;
	}
	else
	{
		BYTE byInnerSec = 0;
		for (int i = 0; i < 4; i++)
		{
			map<CString, BYTE>::const_iterator it = m_bySpyPercentage[i].find(sRace);
			if (it != m_bySpyPercentage[i].end())
				byInnerSec += it->second;
		}
		return 100 - byInnerSec;
	}
}

/// Funktion gibt die einzelnen Sabotagezuteilungen f�r eine bestimmte Rasse zur�ck.
BYTE CIntelAssignment::GetSabotagePercentages(const CString& sRace, BYTE type)
{
	if (type != 4)
	{
		map<CString, BYTE>::const_iterator it = m_bySabPercentage[type].find(sRace);
		if (it != m_bySabPercentage[type].end())
			return it->second;
		else
			return 0;
	}
	else
	{
		BYTE byInnerSec = 0;
		for (int i = 0; i < 4; i++)
		{
			map<CString, BYTE>::const_iterator it = m_bySabPercentage[i].find(sRace);
			if (it != m_bySabPercentage[i].end())
				byInnerSec += it->second;
		}
		return 100 - byInnerSec;
	}
}

/// Funktion �ndert die globale prozentuale Zuteilung der einzelnen Geheimdienstressorts. Dabei wird wenn
/// n�tig die Zuteilung der anderen Ressorts ver�ndert.
void CIntelAssignment::SetGlobalPercentage(BYTE type, BYTE perc, CMajor* pMajor, const CString& sRace, const map<CString, CMajor*>* pmMajors)
{
	USHORT known = 0;
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (it->first != pMajor->GetRaceID() && pMajor->IsRaceContacted(it->first))
			known++;
		// kennen wir die Rasse nicht, haben aber irgendwelche Prozentpunkte in diesem Bereich verteilt, so m�ssen
		// diese zuvor entfernt werden. Dies kann vorkommen, wenn wir die Rasse vergeheimdiensteln, sie danach
		// aber komplett aus dem Spiel ausscheidet
		else
		{
			if (m_byPercentage[0][it->first] > 0)
				m_byPercentage[0][it->first] = 0;
			if (m_byPercentage[1][it->first] > 0)
				m_byPercentage[1][it->first] = 0;
		}
	}

	// wenn die innere Sicherheitszuweisung ver�ndert wird
	if (type == 2 && known > 0)
	{
		map<CString, int> percentages[2];
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			percentages[0][it->first] = m_byPercentage[0][it->first];
			percentages[1][it->first] = m_byPercentage[1][it->first];
		}

		// solange durchgehen, bis der richtige Prozentwert erreicht wurde
		do
		{
			int diff = GetInnerSecurityPercentage() - perc;
			int t = diff / (known * 2);
			int rest = diff - t * (known * 2);
			// gleichen Anteil auf alle Gebiete von bekannten Rassen verteilen
			CArray<CString> vRandoms;
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
				if (it->first != pMajor->GetRaceID() && pMajor->IsRaceContacted(it->first))
				{
					vRandoms.Add(it->first);
					for (int j = 0; j < 2; j++)
						percentages[j][it->first] += t;
				}
			// bleibt noch ein Rest �brig (z.B. durch ungerade Anzahl), diesen zuf�llig auf ein Gebiet verteilen
			if (rest != 0 && vRandoms.GetSize())
			{
				CString sRandomID = vRandoms[rand()%vRandoms.GetSize()];
				percentages[rand()%2][sRandomID] += rest;
			}

			// �berpr�fen das nirgends mehr als 100% oder weniger als 0% vorhanden sind
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
				for (int j = 0; j < 2; j++)
				{
					if (percentages[j][it->first] < 0)
						percentages[j][it->first] = 0;
					else if (percentages[j][it->first] > 100)
						percentages[j][it->first] = 100;
				}

			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			{
				m_byPercentage[0][it->first] = percentages[0][it->first];
				m_byPercentage[1][it->first] = percentages[1][it->first];
			}
		} while (GetInnerSecurityPercentage() != perc);
	}

	// wenn irgendein Spionagebalken ver�ndert wird
	else if (type != 2 && pMajor->IsRaceContacted(sRace) && sRace != pMajor->GetRaceID())
	{
		BYTE innerSecurity = GetInnerSecurityPercentage();
		if (perc - m_byPercentage[type][sRace] > innerSecurity)
			perc = m_byPercentage[type][sRace] + innerSecurity;
		m_byPercentage[type][sRace] = perc;
	}
}

/// Funktion �ndert die genaueen Zuteilungen bei der Spionage. Also wieviel Prozent der Spionageabteilung
/// gehen z.B. in die Wirtschaftsspionage. Nicht vergebene Prozentpunkte befinden sich automatisch im
/// Spionagedepot f�r die jeweilige Rasse.
void CIntelAssignment::SetSpyPercentage(BYTE type, BYTE perc, const CString& sRace)
{
	if (type == 4)
	{
		int percentages[4];
		for (int i = 0; i < 4; i++)
			percentages[i] = m_bySpyPercentage[i][sRace];

		// solange durchgehen, bis der richtige Prozentwert erreicht wurde
		do
		{
			int diff = this->GetSpyPercentages(sRace, 4);
			diff -= perc;
			int t = diff / 4;
			int rest = diff - t * 4;
			// gleichen Anteil auf alle Gebiete von bekannten Rassen verteilen
			for (int i = 0; i < 4; i++)
				percentages[i] += t;
			// bleibt noch ein Rest �brig (z.B. durch ungerade Anzahl), diesen zuf�llig auf ein Gebiet verteilen
			if (rest != 0)
			{
				int random = rand()%4;
				percentages[random] += rest;
			}

			// �berpr�fen das nirgends mehr als 100% oder weniger als 0% vorhanden sind
			for (int i = 0; i < 4; i++)
			{
				if (percentages[i] < 0)
					percentages[i] = 0;
				else if (percentages[i] > 100)
					percentages[i] = 100;
			}
			for (int i = 0; i < 4; i++)
				m_bySpyPercentage[i][sRace] = percentages[i];

		} while (this->GetSpyPercentages(sRace, 4) != perc);
	}

	// wenn irgendein spezieller Spionagebalken (au�er Spionagedepot) ver�ndert wird
	else if (type != 4)
	{
		BYTE spyDepot = this->GetSpyPercentages(sRace, 4);
		if (perc - m_bySpyPercentage[type][sRace] > spyDepot)
			perc = m_bySpyPercentage[type][sRace] + spyDepot;
		m_bySpyPercentage[type][sRace] = perc;
	}
}

/// Funktion �ndert die genauen Zuteilungen bei der Sabotage. Also wieviel Prozent der Sabotageabteilung
/// gehen z.B. in die Wirtschaftssabotage. Nicht vergebene Prozentpunkte befinden sich automatisch im
/// Sabotagedepot f�r die jeweilige Rasse.
void CIntelAssignment::SetSabotagePercentage(BYTE type, BYTE perc, const CString& sRace)
{
	if (type == 4)
	{
		int percentages[4];
		for (int i = 0; i < 4; i++)
			percentages[i] = m_bySabPercentage[i][sRace];

		// solange durchgehen, bis der richtige Prozentwert erreicht wurde
		do
		{
			int diff = this->GetSabotagePercentages(sRace, 4);
			diff -= perc;
			int t = diff / 4;
			int rest = diff - t * 4;
			// gleichen Anteil auf alle Gebiete von bekannten Rassen verteilen
			for (int i = 0; i < 4; i++)
				percentages[i] += t;
			// bleibt noch ein Rest �brig (z.B. durch ungerade Anzahl), diesen zuf�llig auf ein Gebiet verteilen
			if (rest != 0)
			{
				int random = rand()%4;
				percentages[random] += rest;
			}

			// �berpr�fen das nirgends mehr als 100% oder weniger als 0% vorhanden sind
			for (int i = 0; i < 4; i++)
			{
				if (percentages[i] < 0)
					percentages[i] = 0;
				else if (percentages[i] > 100)
					percentages[i] = 100;
			}
			for (int i = 0; i < 4; i++)
				m_bySabPercentage[i][sRace] = percentages[i];

		} while (this->GetSabotagePercentages(sRace,4) != perc);
	}

	// wenn irgendein spezieller Spionagebalken (au�er Spionagedepot) ver�ndert wird
	else if (type != 4)
	{
		BYTE sabDepot = this->GetSabotagePercentages(sRace,4);
		if (perc - m_bySabPercentage[type][sRace] > sabDepot)
			perc = m_bySabPercentage[type][sRace] + sabDepot;
		m_bySabPercentage[type][sRace] = perc;
	}
}

/// Funktion entfernt eine ausgeschiedene Rasse aus allen Geheimdienstzuweisungen.
/// @param sRace Rasse, welche entfernt werden soll
void CIntelAssignment::RemoveRaceFromAssignments(const CString& sRace)
{
	m_byPercentage[0].erase(sRace);
	m_byPercentage[1].erase(sRace);

	for (int i = 0; i < 4; i++)
	{
		m_bySpyPercentage[i].erase(sRace);
		m_bySabPercentage[i].erase(sRace);
	}
}


/// Resetfunktion f�r das CIntelligence-Objekt.
void CIntelAssignment::Reset()
{
}
