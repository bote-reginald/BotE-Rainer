#include "stdafx.h"
#include "RaceController.h"
#include "Major.h"
#include "Minor.h"
#include "AI\MinorAI.h"
#include "IOData.h"

IMPLEMENT_SERIAL (CRaceController, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CRaceController::CRaceController(void)
{
	this->Reset();
}

CRaceController::~CRaceController(void)
{
	this->Reset();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CRaceController::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		// Rassen speichern
		ar << m_mRaces.size();
		for (map<CString, CRace*>::const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
			ar << it->first << it->second;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		// alle Rassen und Zeiger l�schen
		Reset();

		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			CRace* value = NULL;
			ar >> key;
			ar >> value;
			ASSERT(value);
			m_mRaces[key] = value;
		}

		// Array aller Majors und Minors anlegen
		for (map<CString, CRace*>::const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		{
			if (it->second->IsMajor())
				m_mMajors[it->first] = dynamic_cast<CMajor*>(it->second);
			else if (it->second->IsMinor())
				m_mMinors[it->first] = dynamic_cast<CMinor*>(it->second);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum Einlesen und Initialisieren aller am Spiel beteiligten Rassen
/// @param nSource Datenquelle der Rasseninformationen
/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
bool CRaceController::Init(int nSource/* = RACESOURCE_DATAFILE*/)
{
	// alle alten Rassen l�schen
	Reset();

	if ((InitMajors(nSource) & InitMinors(nSource) & InitAlienEntities(nSource)) == false)
		return false;

	// Array aller Majors und Minors anlegen
	for (map<CString, CRace*>::const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
	{
		if (it->second->IsMajor())
			m_mMajors[it->first] = dynamic_cast<CMajor*>(it->second);
		else if (it->second->IsMinor())
			m_mMinors[it->first] = dynamic_cast<CMinor*>(it->second);
	}

	// Startbeziehungen initialisieren
	InitRelations();

	return true;
}

/// Funktion gibt eine bestimmte Rasse zur�ck.
/// @param sID ID der gew�nschten Rassen
/// @return Zeiger auf gew�nschte Rasse
CRace* CRaceController::GetRace(const CString& sID) const
{
	map<CString, CRace*>::const_iterator it = m_mRaces.find(sID);

	if (it != m_mRaces.end())
		return it->second;
	else
		return NULL;
}

/// Funktion gibt alle Rassen eines bestimmten Types zur�ck.
/// @param type Typ der Rasse (MAJOR, MINOR)
/// @return Map aller Rassen eines bestimmten Types
map<CString, CRace*> CRaceController::GetRaces(CRace::RACE_TYPE type) const
{
	map<CString, CRace*> mMap;
	for (map<CString, CRace*>::const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		if (it->second->GetType() == type)
			mMap[it->first] = it->second;
	return mMap;
}

/// Funktion liefert die Minorrace, welche in einem bestimmten Sektor beheimatet ist.
/// @param sMinorsHome Heimatsystem
/// @return Zeiger auf Minorrace (<code>NULL</code> wenn die Rasse nicht gefunden werden konnte)
CMinor* CRaceController::GetMinorRace(const CString& sMinorsHome) const
{
	for (map<CString, CRace*>::const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		if (it->second->IsMinor() && it->second->GetHomesystemName() == sMinorsHome)
			return dynamic_cast<CMinor*>(it->second);

	return NULL;
}

/// Funktion entfernt eine nicht mehr zu benutzende Rasse.
/// @param sRaceID Rassen-ID
void CRaceController::RemoveRace(const CString& sRaceID)
{
	for (map<CString, CRace*>::iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
	{
		if (it->first == sRaceID)
		{
			delete it->second;
			it->second = NULL;
			m_mRaces.erase(it);
			break;
		}
	}

	m_mMajors.clear();
	m_mMinors.clear();
	// Array aller Majors und Minors neu anlegen
	for (map<CString, CRace*>::const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
	{
		if (it->second->IsMajor())
			m_mMajors[it->first] = dynamic_cast<CMajor*>(it->second);
		else if (it->second->IsMinor())
			m_mMinors[it->first] = dynamic_cast<CMinor*>(it->second);
	}
}

/// Funktion gibt die zu einer Majorrace zugeh�rige Netzwerkclient-ID zur�ck.
/// Maximal k�nnen sechs Clients bestehen. Jede Majorrace ben�tigt eine eindeutige
/// Client-ID.
/// @param sRaceID Rassen-ID einer Majorrace
/// @return Netzwerk-Client-ID
/// ALPHA5 -> noch fest!
network::RACE CRaceController::GetMappedClientID(const CString& sRaceID) const
{
	if (sRaceID == "MAJOR1")
		return network::RACE_1;
	if (sRaceID == "MAJOR2")
		return network::RACE_2;
	if (sRaceID == "MAJOR3")
		return network::RACE_3;
	if (sRaceID == "MAJOR4")
		return network::RACE_4;
	if (sRaceID == "MAJOR5")
		return network::RACE_5;
	if (sRaceID == "MAJOR6")
		return network::RACE_6;

	return network::RACE_NONE;
}

/// Funktion gibt die zu einer Client-ID zugeh�rige Major-ID zur�ck.
/// Maximal k�nnen sechs Clients bestehen. Jede Majorrace ben�tigt eine eindeutige
/// Client-ID.
/// @param client-ID Client-ID eines Spielers
/// @return Rassen-ID
/// ALPHA5 -> noch fest!
CString CRaceController::GetMappedRaceID(network::RACE clientID) const
{
	if (clientID == network::RACE_1)
		return "MAJOR1";
	if (clientID == network::RACE_2)
		return "MAJOR2";
	if (clientID == network::RACE_3)
		return "MAJOR3";
	if (clientID == network::RACE_4)
		return "MAJOR4";
	if (clientID == network::RACE_5)
		return "MAJOR5";
	if (clientID == network::RACE_6)
		return "MAJOR6";

	return "";
}

/// Funktion zum zur�cksetzen aller Werte auf Ausgangswerte.
void CRaceController::Reset(void)
{
	// Objekte in der Map l�schen
	for (map<CString, CRace*>::iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	// Map l�schen
	m_mRaces.clear();

	m_mMajors.clear();
	m_mMinors.clear();
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion zum Einlesen und Initialisieren der beteiligten Majors
/// @param nSource Datenquelle der Rasseninformationen
/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
bool CRaceController::InitMajors(int nSource/* = RACESOURCE_DATAFILE*/)
{
	if (nSource == RACESOURCE_DATAFILE)
	{
		// zuerst die Majors anlegen
		int nInfoCount = 29;
		int nCurRow = 0;
		CString sVersion;

		// Name des zu �ffnenden Files
		CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorRaces.data";
		// Varibale vom Typ CStdioFile
		CStdioFile file;
		// Array mit allen Rasseninfos
		CStringArray saInfo;

		// Datei �ffnen
		if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))
		{
			// auf sInput wird die jeweilige Zeile gespeichert
			CString sInput;
			while (file.ReadString(sInput))
			{
				// in der ersten Zeile m�ssen Versioninfos stehen
				if (nCurRow == 0)
				{
					sVersion = sInput;
					nCurRow++;
				}
				else
				{
					saInfo.Add(sInput);
					nCurRow++;
				}
			}
		}
		else
		{
			AfxMessageBox("Error! Could not open \"MajorRaces.data\"...");
			return false;
		}
		// Datei wird geschlossen
		file.Close();

		// pr�fen ob vollst�ndige Rasseninformationen vorhanden sind
		if (nCurRow < nInfoCount || (saInfo.GetSize())%nInfoCount != 0)
		{
			AfxMessageBox("Error in file!\nMajorRaces.data is corrupted");
			return false;
		}
		// pr�fen ob Version ausreichend ist
		if (atof(sVersion) > VERSION)
		{
			AfxMessageBox("Version conflict in file MajorRaces.data");
			return false;
		}

		// Majorrace nun anlegen
		int nPos = 0;
		while (nPos < saInfo.GetSize())
		{
			CMajor* pNewMajor = new CMajor();
			pNewMajor->Create(saInfo, nPos);
			// �berpr�fen das auch soviele Informationen ins Objekt geschrieben wurden
			// wir auch Informationen in der Datei standen
			if (nPos%nInfoCount != 0)
			{
				AfxMessageBox("Error in file MajorRaces.data!\nCould not create correct CMajor object!");
				return false;
			}
			// Rasse der Map hinzuf�gen
			if (m_mRaces.find(pNewMajor->GetRaceID()) != m_mRaces.end())
			{
				CString s;
				s.Format("Error in file MajorRaces.data!\nThe race-ID %s exists more then one time!", pNewMajor->GetRaceID());
				AfxMessageBox(s);
			}
			else
				m_mRaces[pNewMajor->GetRaceID()] = pNewMajor;
		}
	}
	return true;
}

/// Funktion zum Einlesen und Initialisieren der beteiligten Minors
/// @param nSource Datenquelle der Rasseninformationen
/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
bool CRaceController::InitMinors(int nSource/* = RACESOURCE_DATAFILE*/)
{
	// Minorrace nun anlegen
	if (nSource == RACESOURCE_DATAFILE)
	{
		// danach die Minors einlesen
		int nInfoCount = 14;
		int nCurRow = 0;
		CString sVersion;

		// Name des zu �ffnenden Files
		CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MinorRaces.data";
		// Varibale vom Typ CStdioFile
		CStdioFile file;
		// Array mit allen Rasseninfos
		CStringArray saInfo;
		//saInfo.SetSize(nInfoCount);

		// Datei �ffnen
		if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))
		{
			// auf sInput wird die jeweilige Zeile gespeichert
			CString sInput;
			while (file.ReadString(sInput))
			{
				// in der ersten Zeile m�ssen Versioninfos stehen
				if (nCurRow == 0)
				{
					sVersion = sInput;
					nCurRow++;
				}
				else
				{
					saInfo.Add(sInput);
					nCurRow++;
				}
			}
		}
		else
		{
			AfxMessageBox("Error! Could not open \"MinorRaces.data\"...");
			return false;
		}
		// Datei wird geschlossen
		file.Close();

		// pr�fen ob vollst�ndige Rasseninformationen vorhanden sind
		if (nCurRow < nInfoCount || (saInfo.GetSize())%nInfoCount != 0)
		{
			AfxMessageBox("Error in file!\nMinorRaces.data is corrupted");
			return false;
		}
		// pr�fen ob Version ausreichend ist
		if (atof(sVersion) > VERSION)
		{
			AfxMessageBox("Version conflict in file MinorRaces.data");
			return false;
		}

		// Minorraces nun anlegen
		int nPos = 0;
		while (nPos < saInfo.GetSize())
		{
			CMinor* pNewMinor = new CMinor();
			pNewMinor->Create(saInfo, nPos);
			// �berpr�fen das auch soviele Informationen ins Objekt geschrieben wurden
			// wir auch Informationen in der Datei standen
			if (nPos%nInfoCount != 0)
			{
				AfxMessageBox("Error in file MinorRaces.data!\nCould not create correct CMinor object!");
				return false;
			}
			// Rasse der Map hinzuf�gen
			if (m_mRaces.find(pNewMinor->GetRaceID()) != m_mRaces.end())
			{
				CString s;
				s.Format("Error in file MinorRaces.data!\nThe race-ID %s exists more then one time!", pNewMinor->GetRaceID());
				AfxMessageBox(s);
			}
			else
				m_mRaces[pNewMinor->GetRaceID()] = pNewMinor;
		}
	}
	return true;
}

/// Funktion zum Einlesen und Initialisieren der Aliens (Weltraummonster)
/// @param nSource Datenquelle der Rasseninformationen
/// @return <code>true</code> wenn Initalisierung erfolgreich war, sonst <code>false</code>
bool CRaceController::InitAlienEntities(int nSource/* = RACESOURCE_DATAFILE*/)
{
	// Aliens nun anlegen
	if (nSource == RACESOURCE_DATAFILE)
	{
		// danach die Minors einlesen
		int nInfoCount = 9;
		int nCurRow = 0;
		CString sVersion;

		// Name des zu �ffnenden Files
		CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\AlienEntities.data";
		// Varibale vom Typ CStdioFile
		CStdioFile file;
		// Array mit allen Rasseninfos
		CStringArray saInfo;

		// Datei �ffnen
		if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))
		{
			// auf sInput wird die jeweilige Zeile gespeichert
			CString sInput;
			while (file.ReadString(sInput))
			{
				// in der ersten Zeile m�ssen Versioninfos stehen
				if (nCurRow == 0)
				{
					sVersion = sInput;
					nCurRow++;
				}
				else
				{
					saInfo.Add(sInput);
					nCurRow++;
				}
			}
		}
		else
		{
			AfxMessageBox("Error! Could not open \"AlienEntities.data\"...");
			return false;
		}
		// Datei wird geschlossen
		file.Close();

		// pr�fen ob vollst�ndige Rasseninformationen vorhanden sind
		if (nCurRow < nInfoCount || (saInfo.GetSize())%nInfoCount != 0)
		{
			AfxMessageBox("Error in file!\nAlienEntities.data is corrupted");
			return false;
		}
		// pr�fen ob Version ausreichend ist
		if (atof(sVersion) > VERSION)
		{
			AfxMessageBox("Version conflict in file AlienEntities.data");
			return false;
		}

		// Minorraces nun anlegen
		int nPos = 0;
		while (nPos < saInfo.GetSize())
		{
			CMinor* pAlien = new CMinor();
			pAlien->CreateAlienEntities(saInfo, nPos);
			// �berpr�fen das auch soviele Informationen ins Objekt geschrieben wurden
			// wir auch Informationen in der Datei standen
			if (nPos%nInfoCount != 0)
			{
				AfxMessageBox("Error in file AlienEntities.data!\nCould not create correct CMinor object!");
				return false;
			}
			// Rasse der Map hinzuf�gen
			if (m_mRaces.find(pAlien->GetRaceID()) != m_mRaces.end())
			{
				CString s;
				s.Format("Error in file AlienEntities.data!\nThe race-ID %s exists more then one time!", pAlien->GetRaceID());
				AfxMessageBox(s);
			}
			else
				m_mRaces[pAlien->GetRaceID()] = pAlien;
		}
	}

	return true;
}

/// Funktion erstellt die Beziehungen der Rassen untereinander. Dabei werden jedoch nur die
/// Majorbeziehungen in beide Richtungen vergeben. Bei den Minors wird immer nur die Beziehung
/// von Minor zu Majors erstellt.
void CRaceController::InitRelations(void)
{
	for (map<CString, CRace*>::const_iterator it = m_mRaces.begin(); it != m_mRaces.end(); ++it)
		for (map<CString, CRace*>::const_iterator jt = m_mRaces.begin(); jt != m_mRaces.end(); ++jt)
			// handelt es sich nicht um die Rasse selbst und nicht um die Bezeihung Major -> Minor
			if (it->first != jt->first && jt->second->IsMajor())
			{
				// Grundbeziehung wird mit 50 initialisiert
				int nBase = 50;
				// Beziehungsbonus wird durch die Rassenart der anderen Rasse bestimmt
				int nBoni = 0;

				// und dann mit der Rasseneigenschaft angepasst
				if (it->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
				{
					nBase += 10;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 40;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
				{
					nBase -= 20;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 40;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni -= 50;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
				{
					nBase += 10;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 30;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 40;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
				{
					nBase += 5;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::SECRET))
				{
					nBase -= 10;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni -= 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
				{
					nBase += 5;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 40;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
				{
					nBase += 0;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 35;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
				{
					nBase += 25;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 55;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 30;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 35;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni -= 35;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 25;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 100;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
				{
					nBase -= 20;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni += 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni += 40;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni -= 10;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 20;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
				{
					nBase -= 25;

					if (jt->second->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
						nBoni -= 20;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::WARLIKE))
						nBoni -= 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SECRET))
						nBoni += 15;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PRODUCER))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni += 5;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SNEAKY))
						nBoni += 0;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::SOLOING))
						nBoni += 40;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
						nBoni -= 40;
				}
				if (it->second->IsRaceProperty(RACE_PROPERTY::HOSTILE))
				{
					nBase -= 40;
					if (jt->second->IsRaceProperty(RACE_PROPERTY::PACIFIST))
						nBoni -= 10;
				}

				int nRel = nBase + nBoni;

				if (nRel > 101)
					nRel = 101;
				else if (nRel < 1)
					nRel = 1;

				// 7 mal durchlaufen und einen Mittelwert bilden
				int nValue = 0;
				for (int i = 0; i < 7; i++)
					nValue += (rand()%nRel) * 2;
				nValue /= 7;

				it->second->SetRelation(jt->first, nValue);
			}
}
