// Sector.cpp: Implementierung der Klasse CSector.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sector.h"
#include "IOData.h"
#include "GenSectorName.h"
#include "FontLoader.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "Anomaly.h"
#include "General/Loc.h"
#include "Ships/ships.h"

#include <cassert>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CSector, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSector::CSector(void) :
	m_Attributes(0),
	m_bySunColor(0),
	m_KO(-1, -1)
{
	m_pAnomaly = NULL;
}

CSector::CSector(const CSector& other) :
	m_KO(other.m_KO),
	m_strSectorName(other.m_strSectorName),
	m_Attributes(other.m_Attributes),
	m_sOwnerOfSector(other.m_sOwnerOfSector),
	m_sColonyOwner(other.m_sColonyOwner),
	m_Status(other.m_Status),
	m_bShipPort(other.m_bShipPort),
	m_Outpost(other.m_Outpost),
	m_Starbase(other.m_Starbase),
	m_bWhoIsOwnerOfShip(other.m_bWhoIsOwnerOfShip),
	m_mNumbersOfShips(other.m_mNumbersOfShips),
	m_bIsStationBuild(other.m_bIsStationBuild),
	m_iScanPower(other.m_iScanPower),
	m_iNeededScanPower(other.m_iNeededScanPower),
	m_iNeededStationPoints(other.m_iNeededStationPoints),
	m_iStartStationPoints(other.m_iStartStationPoints),
	m_bySunColor(other.m_bySunColor),
	m_byOwnerPoints(other.m_byOwnerPoints),
	m_Planets(other.m_Planets)
{
	m_pAnomaly=NULL;
};

CSector& CSector::operator=(const CSector& other){
	m_KO = other.m_KO;
	m_strSectorName = other.m_strSectorName;
	m_Attributes = other.m_Attributes;
	m_sOwnerOfSector = other.m_sOwnerOfSector;
	m_sColonyOwner = other.m_sColonyOwner;
	m_Status = other.m_Status;
	m_bShipPort = other.m_bShipPort;
	m_Outpost = other.m_Outpost;
	m_Starbase = other.m_Starbase;
	m_bWhoIsOwnerOfShip = other.m_bWhoIsOwnerOfShip;
	m_mNumbersOfShips = other.m_mNumbersOfShips;
	m_bIsStationBuild = other.m_bIsStationBuild;
	m_iScanPower = other.m_iScanPower;
	m_iNeededScanPower = other.m_iNeededScanPower;
	m_iNeededStationPoints = other.m_iNeededStationPoints;
	m_iStartStationPoints = other.m_iStartStationPoints;
	m_bySunColor = other.m_bySunColor;
	m_byOwnerPoints = other.m_byOwnerPoints;
	m_Planets = other.m_Planets;
	m_pAnomaly = other.m_pAnomaly;

	return *this;
};

CSector::~CSector(void)
{
	if (m_pAnomaly!=NULL)
	{
		delete m_pAnomaly;
		m_pAnomaly = NULL;
	}
}

///////////////////////////////////////////////////////////////////////
// Speicher/Laden - Funktion
///////////////////////////////////////////////////////////////////////
void CSector::Serialize(CArchive &ar)
{
	// Funktion der Basisklasse aufrufen
	CObject::Serialize(ar);

	// Wird geschrieben?
	if (ar.IsStoring())
	// Alle Variablen in der richtigen Reihenfolge schreiben
	{
		CString secondStringMytrace = "";
		// (doesn't work) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: ##### Begin of sector list #############################");
		//if m_Attributes != 0 {
		//MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: -------------------------\n");
		//MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_sOwnerOfSector: %s\n", m_sOwnerOfSector);
		MYTRACE("savedetails")(MT::LEVEL_DEBUG, "Sector.cpp:(%d,%d) %s,Anomaly:%s,Outpost:%s,Starbase:%s,Owner:%s,ColonyOwner:%s,OwnerPoints:%d,Attributes:%d \n", 
			m_KO.y, m_KO.x, m_strSectorName, m_pAnomaly ? "yes" : "no", m_Outpost, m_Starbase, m_sOwnerOfSector, m_sColonyOwner, m_byOwnerPoints.size(), m_Attributes);
		
		// MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: -------------------------\n");
		//};
		ar << m_Attributes;
		// (see below) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_Attributes: %d\n", m_Attributes);
		ar << m_KO; 		// see above
		
		// alle Maps speichern
		ar << m_Status.size();
		// (size) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_Status.size: %s\n", m_Status.size);
		for (map<CString, DISCOVER_STATUS>::const_iterator it = m_Status.begin(); it != m_Status.end(); ++it)
			ar << it->first << static_cast<BYTE>(it->second);
		ar << m_bShipPort.size();
		// (size) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_KO: %s\n", m_KO);
		for (set<CString>::const_iterator it = m_bShipPort.begin(); it != m_bShipPort.end(); ++it)
			ar << *it;
		ar << m_Outpost; 		// (see above) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_Outpost: %s\n", m_Outpost);
		ar << m_Starbase; 		// (see above) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_Starbase: %s\n", m_Starbase);
		ar << m_bIsStationBuild.size();
		// (size) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_KO: %s\n", m_KO);
		for (set<CString>::const_iterator it = m_bIsStationBuild.begin(); it != m_bIsStationBuild.end(); ++it)
			ar << *it;
		ar << m_bWhoIsOwnerOfShip.size();
		// (size) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_KO: %s\n", m_KO);
		for (set<CString>::const_iterator it = m_bWhoIsOwnerOfShip.begin(); it != m_bWhoIsOwnerOfShip.end(); ++it)
					{
			//secondStringMytrace += "Sector.cpp: OwnerOfShip: it:%i, it-second:%i\n", *it;
			ar << *it;
		}
		
		ar << m_mNumbersOfShips.size();
		// (size) 
		// (unneccassary) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: NumbersOfShips: %i\n", m_mNumbersOfShips.size());
		for(std::map<CString, unsigned>::const_iterator it = m_mNumbersOfShips.begin(); it != m_mNumbersOfShips.end(); ++it)
		{
			if (it->second > 0)
			MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: NumbersOfShips:%i\n", it->second);
			ar << it->first << it->second;
		}
		ar << m_iNeededStationPoints.size();
		for (map<CString, short>::const_iterator it = m_iNeededStationPoints.begin(); it != m_iNeededStationPoints.end(); ++it)
			ar << it->first << it->second;
		if (m_iNeededStationPoints.size() > 0)
		MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: NeededStationPoints: %i\n", m_iNeededStationPoints.size());	

		ar << m_iStartStationPoints.size();
		for (map<CString, short>::const_iterator it = m_iStartStationPoints.begin(); it != m_iStartStationPoints.end(); ++it)
			ar << it->first << it->second;
		if (m_iStartStationPoints.size() > 0)
		MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_iStartStationPoints.size: %i\n", m_iStartStationPoints.size());
		
		ar << m_iScanPower.size();
		for (map<CString, short>::const_iterator it = m_iScanPower.begin(); it != m_iScanPower.end(); ++it)
		{
			// (unneccassary) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: 2m_iScanPower: it-first:%i, it-second:%i\n", it->second);
			ar << it->first << it->second;
		}
		if (m_iScanPower.size() > 0)
		//MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: 2m_iScanPower: %i\n", it->second);


		ar << m_iNeededScanPower.size();
		for (map<CString, short>::const_iterator it = m_iNeededScanPower.begin(); it != m_iNeededScanPower.end(); ++it)
			ar << it->first << it->second;

		ar << m_byOwnerPoints.size();
		
		//see below: MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_byOwnerPoints.size: %d\n", m_byOwnerPoints.size());
		for (map<CString, BYTE>::const_iterator it = m_byOwnerPoints.begin(); it != m_byOwnerPoints.end(); ++it)
			ar << it->first << it->second;

		ar << m_sColonyOwner; 		// (see below) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_sColonyOwner: %s\n", m_sColonyOwner);
		ar << m_sOwnerOfSector; 		// (see above) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_sOwnerOfSector: %s\n", m_sOwnerOfSector);

		// Nur wenn ein Sonnensystem in dem Sektor ist m�ssen die folgenden Variablen gespeichert werden
		if (GetSunSystem())
		{
			ar << m_strSectorName;
			//see above MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: SectorName: %s, ColonyOwner: %s, OwnerPoints:%d\n", m_strSectorName, m_sColonyOwner, m_byOwnerPoints.size());
			ar << m_bySunColor;
			ar << m_Planets.size();
			for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
				m_Planets.at(i).Serialize(ar);
			//MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: -------------------------\n");
		}

		ar << m_pAnomaly;
		// (see above) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: m_pAnomaly: %s\n", m_pAnomaly);
		// (doesn't work) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: ##### END of sector list #############################");
	}
	else
	// Alle Variablen in der richtigen Reihenfolge lesen
	{
		ar >> m_Attributes;
		ar >> m_KO;

		// Maps laden
		m_Status.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			BYTE value;
			ar >> key;
			ar >> value;
			m_Status[key] = static_cast<DISCOVER_STATUS>(value);
		}
		m_bShipPort.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString value;
			ar >> value;
			m_bShipPort.insert(value);
		}
		m_Outpost.Empty();
		ar >> m_Outpost;
		m_Starbase.Empty();
		ar >> m_Starbase;
		m_bIsStationBuild.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString value;
			ar >> value;
			m_bIsStationBuild.insert(value);
		}
		m_bWhoIsOwnerOfShip.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString value;
			ar >> value;
			m_bWhoIsOwnerOfShip.insert(value);
		}
		m_mNumbersOfShips.clear();
		mapSize = 0;
		ar >> mapSize;
		for(unsigned i = 0; i < mapSize; ++i)
		{
			CString key;
			ar >> key;
			unsigned value;
			ar >> value;
			m_mNumbersOfShips.insert(std::pair<CString, unsigned>(key, value));
		}
		m_iNeededStationPoints.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_iNeededStationPoints[key] = value;
		}
		m_iStartStationPoints.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_iStartStationPoints[key] = value;
		}
		m_iScanPower.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_iScanPower[key] = value;
		}
		m_iNeededScanPower.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_iNeededScanPower[key] = value;
		}
		m_byOwnerPoints.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			BYTE value;
			ar >> key;
			ar >> value;
			m_byOwnerPoints[key] = value;
		}

		ar >> m_sColonyOwner;
		ar >> m_sOwnerOfSector;
		// Nur wenn ein Sonnensystem in dem Sektor ist m�ssen die folgenden Variablen geladen werden
		if (GetSunSystem())
		{
			ar >> m_strSectorName;
			ar >> m_bySunColor;
			int number = 0;
			ar >> number;
			m_Planets.clear();
			m_Planets.resize(number);
			for (int i = 0; i < number; i++)
				m_Planets.at(i).Serialize(ar);
		}
		else
		{
			m_strSectorName = "";
			m_bySunColor = 0;
			m_Planets.clear();
		}

		if (VERSION >= 0.72)
		{
			if (m_pAnomaly)
			{
				delete m_pAnomaly;
				m_pAnomaly = NULL;
			}
			ar >> m_pAnomaly;
		}
	}
		// (doesn't work correct) MYTRACE("logsave")(MT::LEVEL_DEBUG, "Sector.cpp: ##### End of sector list #############################");
}

//////////////////////////////////////////////////////////////////
// Zugriffsfunktionen
//////////////////////////////////////////////////////////////////
/// Funktion gibt den Namen des Sektors zur�ck. Wenn in ihm kein Sonnensystem ist, dann wird "" zur�ckgegeben.
/// Wenn man aber den Parameter <code>longName<code> beim Aufruf der Funktion auf <code>TRUE<code> setzt, wird
/// versucht ein genauerer Sektorname zu generieren.
CString CSector::GetName(BOOLEAN longName) const
{
	if (!longName)
		{
			// following is for a big part ok, but error messages at start, ingame works -> Aufrufe mit longName checken
			//CString s;
			//s.Format("%s (%i/%i)", m_strSectorName, (m_KO.y), m_KO.x);
			//return s;
			return m_strSectorName;
		}
		
	else
	{
		if (GetSunSystem())
			return m_strSectorName;
		//no adding of sector_KO, because than Systems are not found anymore (e.g. Terra 13/14 <> Terra)
		else
		{
			CString s;
			s.Format("%s %i/%i", CLoc::GetString("SECTOR"), (m_KO.y), m_KO.x);
			return s;
		}
	}
}

/// Funktion gibt alle Einwohner aller Planeten in dem Sektor zur�ck.
float CSector::GetCurrentHabitants() const
{
	float currentHabitants = 0.0f;
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
		currentHabitants += m_Planets.at(i).GetCurrentHabitant();
	return currentHabitants;
}

/// Diese Funktion berechnet die vorhandenen Rohstoffe der Planeten im Sektor. �bergebn wird daf�r ein Feld f�r
/// die Ressourcen <code>res</code>.
void CSector::GetAvailableResources(BOOLEAN bResources[DERITIUM + 1], BOOLEAN bOnlyColonized/* = true */)
{
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
	{
		CPlanet* pPlanet = &m_Planets[i];
		if (!pPlanet->GetHabitable())
			continue;

		// wenn nur kolonisierte Planeten betrachtet werden sollen und der Planet nicht kolonisiert ist,
		// dann n�chsten Planeten betrachten
		if (bOnlyColonized && !pPlanet->GetColonized())
			continue;

		BOOLEAN bExists[DERITIUM + 1] = {FALSE};
		pPlanet->GetAvailableResources(bExists);
		for (int res = TITAN; res <= DERITIUM; res++)
			bResources[res] |= bExists[res];
	}
}

//////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////

/// Funktion generiert den Sektor. Dabei wird als Parameter die Wahrscheinlichkeit, ob in dem Sektor ein
/// Sonnensystem ist, im Paramter <code>sunProb</code> in Prozent �bergeben. Im Parameter <code>minorProb</code>
/// wird die Wahrscheinlichkeit in Prozent �bergeben, dass sich in dem Sektor eine Minorrace befindet.
void CSector::GenerateSector(int sunProb, int minorProb)
{
	if (GetSunSystem())
		return;

	// Zahl[0,99] generieren und vergleichen (Sonnensystem?)
	if (rand()%100 >= (100 - sunProb))
	{
		SetSunSystem(TRUE);
		// Zahl[0,99] generieren und vergleichen (Minorrace?)
		bool bMinor = rand()%100 >= (100 - minorProb);
		m_strSectorName = CGenSectorName::GetInstance()->GetNextRandomSectorName(m_KO, bMinor);
		// (see below) MYTRACE("init")(MT::LEVEL_DEBUG, "m_strSectorName: %s, m_KO:??\n", m_strSectorName);
		// bMinor wird in der Generierungsfunktion angepasst, falls es keine Minorracesystemnamen mehr gibt
		SetMinorRace(bMinor);

		// Es konnte ein Sektor f�r eine Minorrace generiert werden
		if (bMinor)
		{
			float currentHabitants = 0.0f;
			float maxHabitants = 0.0f;
			USHORT random = rand()%3+1;
			// Solange Planeten generieren, bis mind. eine zuf�llige Anzahl Bev�lkerung darauf leben
			do
			{
				this->CreatePlanets();
				currentHabitants = this->GetCurrentHabitants();
				//(funktioniert) MYTRACE("init")(MT::LEVEL_DEBUG, "Minor_currentHabitants: %g\n", currentHabitants);
				if (currentHabitants > 20.000f)
					break;
				// Wenn schon Bev�lkerung vorhanden ist und die maximale Bev�lkerungsanzahl hoch genug ist,
				// so wird abgebrochen
				if (currentHabitants > 0.0f)
				{
					for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
						maxHabitants += m_Planets.at(i).GetMaxHabitant();
					if (maxHabitants > (40.000f + random * 7))
						break;
				}
			} while (currentHabitants <= (15.000f / random));
			MYTRACE("init")(MT::LEVEL_DEBUG, "Sector.cpp: m_strSectorName: %s (%d/%d), currentHabitants: %g\n", m_strSectorName, m_KO.y, m_KO.x, currentHabitants);
		}
		else
		{
			// Wenn keine Minorrace im Sektor lebt
			this->CreatePlanets();
		}
	}
}

/// Diese Funktion generiert die Planeten in dem Sektor.
void CSector::CreatePlanets(const CString& sMajorID)
{
	m_Planets.clear();

	if (GetSunSystem())
	{
		// Es gibt 7 verschiedene Sonnenfarben
		m_bySunColor = rand()%7;

		if (sMajorID != "")
		{
			CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorPlanets.data";
			CStdioFile file;
			// Datei wird ge�ffnet
			if (file.Open(fileName, CFile::modeRead | CFile::typeText))
			{
				CString csInput;
				CString data[18];
				file.ReadString(csInput);
				bool endoffile=false;
				while(csInput!=sMajorID&&!endoffile) endoffile=!file.ReadString(csInput);//Zur Daten der gesuchten Majorrace springen
				if(!endoffile)//Wenn Major daten vorhanden
				{
					m_Planets.clear();
					file.ReadString(csInput);//Sonnenfarbe
					m_bySunColor = atoi(csInput);
					file.ReadString(csInput);//Planetenanzahl
					int planeten=atoi(csInput);
					CPlanet planet;
					for(int i=0;i<planeten;i++)
					{
						for(int j=0;j<18;j++)
						{
							file.ReadString(csInput);
							data[j]=csInput;
						}
						planet.SetName(data[0]);//Planeten Name
						planet.SetType(atoi(data[1]));  //Planetentyp
						planet.SetColonisized(atoi(data[2])); //Ist der Planet kolonisiert
						float maxHabitant=atoi(data[3])/1000;//f�r Einwohnerzahlen mit Nachkommastellen
						planet.SetMaxHabitant(maxHabitant); //Maximale Einwohner
						float curHabitant=atoi(data[4])/1000;
						planet.SetCurrentHabitant(curHabitant); //aktuelle Einwohner
						planet.SetHabitable(atoi(data[5]));//bewohnbar
						planet.SetTerraformed(atoi(data[6]));//terraformed
						planet.SetSize((PLANT_SIZE::Typ)atoi(data[7]));//Gr��e
						planet.SetClass(data[8][0]);//Planetenklasse Char
						planet.SetPlanetGrowth();
						planet.SetGraphicType(rand()%GRAPHICNUMBER);
						planet.SetBoni(atoi(data[9]),atoi(data[10]),atoi(data[11]),atoi(data[12]),atoi(data[13]),atoi(data[14]),atoi(data[15]),atoi(data[16]));//Boni 8 Zeilen
						planet.SetStartTerraformPoints(atoi(data[17]));//Terraformpoints
						planet.SetHasIndividualGraphic(true);
						m_Planets.push_back(planet);
					}
				}
				else
				{
					while (true)//Falls Major in der Datei nicht definiert ist wird der bisherige Generierungsalgorithmus benutzt
					{
						m_Planets.clear();

						short number = (rand()%8+1 + rand()%8+1 + rand()%8+1 + 1) / 3;
						PLANET_ZONE::Typ zone = PLANET_ZONE::HOT;
						// mit welcher Plantenzone soll zuf�llig begonnen werden
						int nRandom = rand()%10;
						if (nRandom == 0)
							zone = PLANET_ZONE::COOL;
						else if (nRandom < 3)
							zone = PLANET_ZONE::TEMPERATE;

						for (int i = 0; i < number; i++)
						{
							CPlanet planet;
							zone = planet.Create(m_strSectorName, zone, i, true);
							//MYTRACE("init")(MT::LEVEL_DEBUG, "planet.Create-MajorOld - zone= %s\n", zone);
							m_Planets.push_back(planet);
						}

						// aktuelle Bev�lkerung pr�fen
						float fCurrentHabitants = this->GetCurrentHabitants();
						if (fCurrentHabitants > 25.000f || fCurrentHabitants < 10.000f)
							continue;

						//MYTRACE("init")(MT::LEVEL_DEBUG, "planet.Create-MajorOld - fCurrentHabitants - Single Planet= %f\n", fCurrentHabitants);

						// maximale Bev�lkerung pr�fen
						float fMaxHabitants = 0.0f;
						for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
							fMaxHabitants += m_Planets.at(i).GetMaxHabitant();
						if (fMaxHabitants > 65.000f || fMaxHabitants < 45.000f)
							continue;

						// MYTRACE("init")(MT::LEVEL_DEBUG, "planet.Create-MajorOld - fMaxHabitants - Whole System = %f\n", fMaxHabitants);

						// pr�fen ob alle Rohstoffe vorhanden sind
						bool bResOkay = true;
						BOOLEAN bRes[DERITIUM + 1] = {FALSE};
						this->GetAvailableResources(bRes, true);
						// gibt es kein Deritium
						for (int i = TITAN; i <= IRIDIUM; i++)
						{
							if (!bRes[i])
							{
								bResOkay = false;
								break;
							}
						}
						if (!bResOkay)
							continue;

						// Deritium �berpr�fen
						if (!bRes[DERITIUM])
						{
							for (int p = 0; p < static_cast<int>(this->GetPlanets().size()); p++)
								if (this->GetPlanet(p)->GetCurrentHabitant() > 0 && this->GetPlanet(p)->GetColonized())
								{
									this->GetPlanet(p)->SetBoni(DERITIUM, TRUE);
									break;
								}
						}

						// Sektor f�r Majorrace konnte erstellt werden
						break;
					}

				}
			}
			else
			{
				AfxMessageBox("ERROR! Could not open file \"MajorPlanets.data\"...");
				exit(1);
			}
			file.Close();
		}
		else
		{
			int nSizeCounter = 0;
			// dreimal die Zufallsfunktion aufgerufen, damit die mittlere Planetenanzahl h�ufiger als ganz wenig oder
			// sehr viele Planeten vorkommt.
			short number = (rand()%8+1 + rand()%8+1 + rand()%8+1 + 1) / 3;
			PLANET_ZONE::Typ zone = PLANET_ZONE::HOT;
			// mit welcher Plantenzone soll zuf�llig begonnen werden
			int nRandom = rand()%10;
			if (nRandom == 0)
				zone = PLANET_ZONE::COOL;
			else if (nRandom < 3)
				zone = PLANET_ZONE::TEMPERATE;

			//MYTRACE("init")(MT::LEVEL_DEBUG, "planet.Create - zone= %s\n", zone);

			for (int i = 0; i < number; i++)
			{
				CPlanet planet;
				zone = planet.Create(m_strSectorName, zone, i, GetMinorRace());
				//MYTRACE("init")(MT::LEVEL_DEBUG, "planet.Create - zone2= %s\n", zone);
				m_Planets.push_back(planet);

				// nicht zu viele gro�e Planeten generieren, da diese dann nicht mehr
				// in die View passen

				// kleine und mittlere Planeten z�hlen die Systemgr��te um 1 hoch
				if (planet.GetSize() <= 1)
					nSizeCounter += 1;
				// alle anderen um 2
				else
					nSizeCounter += 2;

				if (nSizeCounter > 10)
					break;
				// (funktioniert) MYTRACE("init")(MT::LEVEL_DEBUG, "nSizeCounter(Generating Systems) = %d\n", nSizeCounter);
			}
		}
	}
}

/// Funktion erzeugt eine zuf�llige Anomalie im Sektor.
void CSector::CreateAnomaly(void)
{
	if (m_pAnomaly)
	{
		delete m_pAnomaly;
		m_pAnomaly = NULL;
	}

	m_pAnomaly = new CAnomaly();
	// (funktioniert nicht) MYTRACE("init")(MT::LEVEL_DEBUG, "m_pAnomaly(CreateAnomaly) = %f\n", m_pAnomaly);
}

/// Diese Funktion f�hrt das Planetenwachstum f�r diesen Sektor durch.
void CSector::LetPlanetsGrowth()
{
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
		m_Planets.at(i).PlanetGrowth();
}

/// Diese Funktion l�sst die Bev�lkerung auf allen Planeten zusammen um den �bergebenen Wert <code>Value</code>
/// schrumpfen.
void CSector::LetPlanetsShrink(float Value)
{
	// aktuelle Einwohner auf den einzelnen Planeten
	float* Habitants = new float[m_Planets.size()];

	// alle Einwohner im Sector
	float allHabitants = 0.0f;
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
	{
		allHabitants += m_Planets[i].GetCurrentHabitant();
		Habitants[i] = m_Planets[i].GetCurrentHabitant();
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "SECTOR.CPP: allHabitants = %g\n", allHabitants);
	}
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
		if (Habitants[i] > 0)
		{
			float prozAnteil = Habitants[i] / allHabitants;
			Habitants[i] += prozAnteil * Value;
			m_Planets[i].SetCurrentHabitant(Habitants[i]);
			if (m_Planets[i].GetCurrentHabitant() <= 0)
			{
				m_Planets[i].SetCurrentHabitant(0);
				m_Planets[i].SetColonisized(FALSE);
			}
		}
	delete[] Habitants;
}

/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
/// machen, werden diese Variablen sp�ter ja wieder korrekt gesetzt. Au�erdem werden auch die Besitzerpunkte wieder
/// gel�scht.
void CSector::ClearAllPoints()
{
	// Funktion bei jeder neuen Runde anfangs aufrufen!!! Wenn n�mlich in diesem Sektor gerade keine Station einer
	// Rasse gebaut wird, dann setzen wir auch die noch gebrauchten Punkte und die anf�nglich gebrauchten Punkte
	// wieder auf NULL

	// Falls der Planet gerade geterraformt wird, wird er hier erstmal wieder auf FALSE gesetzt.
	for (int i = 0; i < static_cast<int>(m_Planets.size()); i++)
		m_Planets[i].SetIsTerraforming(FALSE);

	m_byOwnerPoints.clear();

	// nun k�nnen alle StartStationPoint die auf 0 stehen in der Map gel�scht werden
	for (map<CString, short>::iterator it = m_iStartStationPoints.begin(); it != m_iStartStationPoints.end(); )
	{
		if (m_bIsStationBuild.find(it->first) == m_bIsStationBuild.end())
			it->second = 0;

		if (it->second == 0)
			it = m_iStartStationPoints.erase(it++);
		else
			++it;
	}
	m_bIsStationBuild.clear();

	m_bWhoIsOwnerOfShip.clear();
	m_mNumbersOfShips.clear();
	// Die ben�tigte Scanpower um Schiffe sehen zu k�nnen wieder auf NULL setzen
	m_iNeededScanPower.clear();
	m_iScanPower.clear();
	// Sagen das erstmal kein Au�enposten und keine Sternbasis in dem Sektor steht
	m_Outpost.Empty();
	m_Starbase.Empty();
	m_bShipPort.clear();
}

#pragma warning (push)
#pragma warning (disable : 4702)
/// Diese Funktion berechnet anhand der Besitzerpunkte und anderen Enfl�ssen, wem dieser Sektor schlussendlich
/// geh�rt.
void CSector::CalculateOwner(const CString& sSystemOwner)
{
	// Wenn in diesem Sektor das System jemanden geh�rt, oder hier eine Schiffswerft durch Au�enposten oder Sternbasis
	// steht, dann �ndert sich nichts am Besitzer
	if (sSystemOwner != "")
	{
		SetOwned(TRUE);
		m_sOwnerOfSector = sSystemOwner;
		// (unneccessary) MYTRACE("logdata")(MT::LEVEL_DEBUG, "SECTOR.CPP: m_sOwnerOfSector(not empty) = %s\n", m_sOwnerOfSector);
		return;
	}
	// Sektor geh�rt einer Minorrace
	else if (m_sOwnerOfSector != "" && sSystemOwner == "" && this->GetMinorRace() == TRUE)
		return;

	if(!m_Outpost.IsEmpty())
	{
		SetOwned(TRUE);
		m_sOwnerOfSector = m_Outpost;
		// (unneccessary) MYTRACE("logdata")(MT::LEVEL_DEBUG, "SECTOR.CPP: m_sOwnerOfSector = m_Outpost = %s\n", m_sOwnerOfSector);
		return;
	}
	if(!m_Starbase.IsEmpty())
	{
		SetOwned(TRUE);
		m_sOwnerOfSector = m_Starbase;
		// (unneccessary) MYTRACE("logdata")(MT::LEVEL_DEBUG, "SECTOR.CPP: m_sOwnerOfSector = m_Starbase = %s\n", m_sOwnerOfSector);
		return;
	}

	// Ist obiges nicht eingetreten, so geh�rt demjenigen der Sektor, wer die meisten Besitzerpunkte hat. Ist hier
	// Gleichstand haben wir neutrales Gebiet. Es werden mindst. 2 Punkte ben�tigt, um als neuer Besitzer des Sektors
	// zu gelten.
	BYTE mostPoints = 1;
	CString newOwner = "";
	for (map<CString, BYTE>::const_iterator it = m_byOwnerPoints.begin(); it != m_byOwnerPoints.end(); ++it)
	{
		if (it->second > mostPoints)
		{
			mostPoints = it->second;
			newOwner = it->first;
		}
		// bei Gleichstand wird der Besitzer wieder gel�scht
		else if (it->second == mostPoints)
			newOwner = "";
	}
	if (newOwner != "")
	{
		SetOwned(TRUE);
		SetScanned(newOwner);
		// (unneccessary) MYTRACE("logdata")(MT::LEVEL_DEBUG, "SECTOR.CPP: newOwner = %s\n", newOwner);
	}
	else
		SetOwned(FALSE);
	m_sOwnerOfSector = newOwner;
	// (unneccessary) MYTRACE("logdata")(MT::LEVEL_DEBUG, "SECTOR.CPP: m_sOwnerOfSector = newOwner = %s\n", m_sOwnerOfSector);
}
#pragma warning (pop)

/// Resetfunktion f�r die Klasse CSector
void CSector::Reset()
{
	m_Attributes = 0;

	// Maps l�schen
	m_Status.clear();
	m_iScanPower.clear();
	m_iNeededScanPower.clear();
	m_bShipPort.clear();
	m_bWhoIsOwnerOfShip.clear();
	m_mNumbersOfShips.clear();
	m_Outpost.Empty();
	m_Starbase.Empty();
	m_bIsStationBuild.clear();
	m_iStartStationPoints.clear();
	m_iNeededStationPoints.clear();
	m_byOwnerPoints.clear();

	m_sOwnerOfSector = "";
	m_sColonyOwner = "";
	m_strSectorName = "";
	m_Planets.clear();

	if (m_pAnomaly)
	{
		delete m_pAnomaly;
		m_pAnomaly = NULL;
	}
}

////////////////////////////////////////////////////////////////
// Zeichenfunktionen
////////////////////////////////////////////////////////////////
/// Diese Funktion zeichnet den Namen des Sektors.
void CSector::DrawSectorsName(CDC *pDC, CBotEDoc* pDoc, CMajor* pPlayer)
{
	// befindet sich kein Sonnensystem oder keine Anomalie im Sektor
	if (!GetSunSystem() && !GetAnomaly())
		return;

	ASSERT(pDC);
	ASSERT(pDoc);
	ASSERT(pPlayer);

	// Ist Sektor bekannt dann zeichne den Systemnamen
	if (this->GetKnown(pPlayer->GetRaceID()) == TRUE)
	{
		COLORREF clrTextColor = CFontLoader::GetFontColor(pPlayer, 0);
		pDC->SetTextColor(clrTextColor);
		CMajor* pOwner = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pDoc->GetSystem(m_KO.x, m_KO.y).GetOwnerOfSystem()));
		if (pOwner)
		{
			if (pPlayer->IsRaceContacted(pOwner->GetRaceID()) == true || pPlayer->GetRaceID() == pOwner->GetRaceID())
				pDC->SetTextColor(pOwner->GetDesign()->m_clrGalaxySectorText);
		}
		// Systemnamen zeichnen
		if (m_pAnomaly == NULL)
			pDC->DrawText(m_strSectorName, CRect(m_KO.x*STARMAP_SECTOR_WIDTH, m_KO.y*STARMAP_SECTOR_HEIGHT, m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT), DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		else
		{
			//CRect(m_KO.x*STARMAP_SECTOR_WIDTH-50,m_KO.y*STARMAP_SECTOR_HEIGHT,m_KO.x*STARMAP_SECTOR_WIDTH+90,m_KO.y*STARMAP_SECTOR_HEIGHT+40)
			pDC->DrawText(m_pAnomaly->GetMapName(m_KO), CRect(m_KO.x*STARMAP_SECTOR_WIDTH, m_KO.y*STARMAP_SECTOR_HEIGHT,m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT), DT_CENTER | DT_BOTTOM | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		}
	}
}

bool CSector::ShouldDrawShip(const CMajor& our_race, const CString& their_race_id) const {
	if(!GetOwnerOfShip(their_race_id))
		return false;
	const CString& our_id = our_race.GetRaceID();
	if(our_id == their_race_id)
		return true;
	if(GetNeededScanPower(their_race_id) < GetScanPower(our_id))
		return true;
	return our_race.GetAgreement(their_race_id) >= DIPLOMATIC_AGREEMENT::AFFILIATION;
}

bool CSector::ShouldDrawOutpost(const CMajor& our_race, const CString& their_race_id) const {
	if(!GetIsStationBuilding(their_race_id)
		&& !GetOutpost(their_race_id)
		&& !GetStarbase(their_race_id))
		return false;
	const CString& our_id = our_race.GetRaceID();
	if(our_id == their_race_id)
		return true;
	if(GetScanPower(our_id) > 0)
		return true;
	return our_race.GetAgreement(their_race_id) >= DIPLOMATIC_AGREEMENT::AFFILIATION;
}

/// Diese Funktion zeichnet die entsprechenden Schiffssymbole in den Sektor
void CSector::DrawShipSymbolInSector(Graphics *g, CBotEDoc* pDoc, CMajor* pPlayer)
{
	ASSERT(g);
	ASSERT(pDoc);
	ASSERT(pPlayer);

	// alle Rassen holen
	map<CString, CRace*>* pmRaces = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(pmRaces);

	CString sFilePath;
	short nCount = 0;

	CPoint pt;
	pt.x = m_KO.x * STARMAP_SECTOR_WIDTH;
	pt.y = m_KO.y * STARMAP_SECTOR_HEIGHT;

	// durch alle Rassen iterieren und Schiffsymbole zeichnen
	CString sAppPath = CIOData::GetInstance()->GetAppPath();
	for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
	{
		if (ShouldDrawShip(*pPlayer, it->first))
		{
			// handelt es sich um ein Alienschiff?
			if (pPlayer != it->second && it->second->IsAlienRace())
				sFilePath = sAppPath + "Graphics\\Symbols\\Entity.bop";
			// ist der Besitzer des Schiffes unbekannt, dann Fragezeichen zeichnen
			else if (pPlayer != it->second && pPlayer->IsRaceContacted(it->first) == false)
				sFilePath = sAppPath + "Graphics\\Symbols\\Unknown.bop";
			// sonst das Rassensymbol zeichnen
			else
				sFilePath = sAppPath + "Graphics\\Symbols\\" + it->first + ".bop";

			Bitmap* ship = Bitmap::FromFile(CComBSTR(sFilePath));
			// konnte die Grafik nicht geladen werden, dann wird ein Standardsymbol geladen
			if (!ship || ship->GetLastStatus() != Ok)
			{
				sFilePath = sAppPath + "Graphics\\Symbols\\Default.bop";
				ship = Bitmap::FromFile(CComBSTR(sFilePath));
			}
			g->DrawImage(ship, pt.x + 45 - nCount * 12, pt.y, 35, 35);
			delete ship;
			nCount++;
		}

		// Jetzt werden die Stationen wenn m�glich gezeichnet
		if (ShouldDrawOutpost(*pPlayer, it->first))
		{
			// kann keine Diplomatie aufgenommen werden, dann das Alien Symbol zeichnen
			if (pPlayer != it->second && it->second->IsAlienRace())
				sFilePath = sAppPath + "Graphics\\Symbols\\Entity.bop";
			// ist der besitzer des Schiffes unbekannt, dann Fragezeichen zeichnen
			else if (pPlayer != it->second && pPlayer->IsRaceContacted(it->first) == false)
				sFilePath = sAppPath + "Graphics\\Symbols\\Unknown.bop";
			// sonst das Rassensymbol zeichnen
			else
				sFilePath = sAppPath + "Graphics\\Symbols\\" + it->first + ".bop";

			Bitmap* ship = Bitmap::FromFile(CComBSTR(sFilePath));
			// konnte die Grafik nicht geladen werden, dann wird ein Standardsymbol geladen
			if (!ship || ship->GetLastStatus() != Ok)
			{
				sFilePath = sAppPath + "Graphics\\Symbols\\Default.bop";
				ship = Bitmap::FromFile(CComBSTR(sFilePath));
			}
			g->DrawImage(ship, pt.x, pt.y + 45, 35, 35);
			delete ship;
		}
	}
}

void CSector::IncrementNumberOfShips(const CString& race) {
	const std::map<CString, unsigned>::iterator found = m_mNumbersOfShips.find(race);
	if(found == m_mNumbersOfShips.end()) {
		m_mNumbersOfShips.insert(std::pair<CString, unsigned>(race, 1));
		return;
	}
	++(found->second);
}

/// Diese Funktion gibt die Scanpower zur�ck, die die Majorrace <code>Race</code> in diesem Sektor hat.
short CSector::GetScanPower(const CString& sRace, bool bWith_ships) const
{
	const CCommandLineParameters* const clp = resources::pClp;
	if(clp->SeeAllOfMap())
		return 200;

	unsigned scan_power_due_to_ship_number = 0;
	if(bWith_ships) {
		const CBotEDoc* pDoc = resources::pDoc;
		const CRaceController* pCtrl = pDoc->GetRaceCtrl();
		const CRace* pRace = pCtrl->GetRace(sRace);
		for(std::map<CString, unsigned>::const_iterator it = m_mNumbersOfShips.begin(); it != m_mNumbersOfShips.end(); ++ it) {
			if(pRace->GetRaceID() == it->first || pRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
				scan_power_due_to_ship_number += it->second;
		}
	}
	map<CString, short>::const_iterator it = m_iScanPower.find(sRace);
	if (it != m_iScanPower.end())
		return it->second + scan_power_due_to_ship_number;
	return scan_power_due_to_ship_number;
}

/// Funktion legt die Scanpower <code>scanpower</code>, welche die Majorrace <code>Race</code>
/// in diesem Sektor hat, fest.
void CSector::SetScanPower(short scanpower, const CString& Race)
{
	if (scanpower)
		m_iScanPower[Race] = scanpower;
	else
		m_iScanPower.erase(Race);
}

void CSector::PutScannedSquare(unsigned range, const int power,
		const CRace& race, bool bBetterScanner, bool patrolship, bool anomaly) {
	const CString& race_id = race.GetRaceID();
	float boni = 1.0f;
	// Wenn das Schiff die Patrouillieneigenschaft besitzt und sich in einem eigenen Sektor befindet,
	// dann wird die Scanleistung um 20% erh�ht.
	if(patrolship) {
		if(race_id == m_sOwnerOfSector || race.GetAgreement(m_sOwnerOfSector) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
			boni = 1.2f;
	}
	if(bBetterScanner) {
		range *= 1.5;
		boni += 0.5;
	}
	const int intrange = static_cast<int>(range);
	for (int i = -intrange; i <= intrange; ++i) {
		const int x = m_KO.x + i;
		if(0 <= x && x < STARMAP_SECTORS_HCOUNT) {
			for (int j = -intrange; j <= intrange; ++j) {
				const int y = m_KO.y + j;
				if(0 <= y && y < STARMAP_SECTORS_VCOUNT) {
					CBotEDoc* pDoc = resources::pDoc;
					CSector& scanned_sector = pDoc->GetSector(x, y);
					// Teiler f�r die Scanst�rke berechnen
					int div = max(abs(i), abs(j));
					if(anomaly)
						div *= 2;
					div = max(div, 1);
					const int old_scan_power = scanned_sector.GetScanPower(race_id, false);
					int new_scan_power = 0;
					if(anomaly) {
						new_scan_power = old_scan_power + power / div;
					} else {
						new_scan_power = (power * boni) / div;
						new_scan_power = max(old_scan_power, new_scan_power);
						if(race.IsMajor())
							scanned_sector.SetScanned(race_id);
					}
					//MYTRACE("logdata")(MT::LEVEL_INFO, "%d/%d: old_scan_power:%d, new_scan_power:%d, power:%d, boni:%d, race_id:%s\n", m_KO.y, m_KO.x, old_scan_power, new_scan_power, power, boni, race_id);
					scanned_sector.SetScanPower(new_scan_power, race_id);
				}//if(0 <= y && y < STARMAP_SECTORS_VCOUNT)
			}//for (int j = -range; j <= range; ++j)
		}//if(0 <= x && x < STARMAP_SECTORS_HCOUNT)
	}//for (int i = -range; i <= range; ++i)
}

static bool StationBuildContinuable(const CString& race, const CSector& sector) {
	const CString& owner = sector.GetOwnerOfSector();
	return owner.IsEmpty() || owner == race || sector.GetIsStationBuilding(race);
}

bool CSector::IsStationBuildable(SHIP_TYPE::Typ station, const CString& race) const {
	if(HasStarbase())
		return false;
	if(station == SHIP_TYPE::OUTPOST) {
		if(HasOutpost())
			return false;
		return StationBuildContinuable(race, *this);
	}
	assert(station == SHIP_TYPE::STARBASE);
	if(!GetOutpost(race))
		return false;
	return StationBuildContinuable(race, *this);
}

void CSector::RecalcPlanetsTerraformingStatus() {
	const CShipMap& sm = resources::pDoc->m_ShipMap;
	std::set<unsigned> terraformable;
	//unset terraforming status for all planets
	for(std::vector<CPlanet>::iterator i = m_Planets.begin(); i != m_Planets.end(); ++i) {
		i->SetIsTerraforming(FALSE);
		if(i->GetHabitable() && !i->GetTerraformed())
			terraformable.insert(i - m_Planets.begin());
	}
	//reset it for those which are actually terraformed at present
	for(CShipMap::const_iterator i = sm.begin(); i != sm.end(); ++i) {
		if(terraformable.empty())
			break;
		if(i->second->GetKO() != m_KO || i->second->GetCurrentOrder() != SHIP_ORDER::TERRAFORM)
			continue;
		const unsigned planet = i->second->GetTerraform();
		CPlanet& p = m_Planets.at(planet);
		assert(p.GetHabitable());
		//It is allowed to terraform the same planet with 2+ independent ships
		if(p.GetIsTerraforming() || p.GetTerraformed())
			continue;
		p.SetIsTerraforming(TRUE);
#pragma warning(push)
#pragma warning(disable: 4189)
		unsigned erased = terraformable.erase(planet);
		assert(erased == 1);
	}
}
#pragma warning(pop)
