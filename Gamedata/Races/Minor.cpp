#include "stdafx.h"
#include "RaceController.h"
#include "Major.h"
#include "Minor.h"
#include "BotEDoc.h"
#include "AI\MinorAI.h"
#include "HTMLStringBuilder.h"
#include "Ships/Ships.h"
#include "General/Loc.h"

IMPLEMENT_SERIAL (CMinor, CRace, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMinor::CMinor(void)
{
	this->Reset();
}

CMinor::~CMinor(void)
{
	this->Reset();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMinor::Serialize(CArchive &ar)
{
	CRace::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_ptKO;						// Koordinaten der Rasse
		ar << m_iTechnologicalProgress;		// wie fortschrittlich ist die Minorrace?
		ar << m_iCorruptibility;			// wie stark �ndert sich die Beziehung beim Geschenke geben?
		ar << m_bSpaceflight;				// Spaceflightnation (hat Schiffe)

		ar << m_bSubjugated;				// wurde die Rasse unterworfen
		// Akzeptanzpunkte (Rassen-ID, Punkte)
		ar << m_mAcceptance.size();
		for (map<CString, short>::const_iterator it = m_mAcceptance.begin(); it != m_mAcceptance.end(); ++it)
			ar << it->first << it->second;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_ptKO;						// Koordinaten der Rasse
		ar >> m_iTechnologicalProgress;		// wie fortschrittlich ist die Minorrace?
		ar >> m_iCorruptibility;			// wie stark �ndert sich die Beziehung beim Geschenke geben?
		ar >> m_bSpaceflight;				// Spaceflightnation (hat Schiffe)

		ar >> m_bSubjugated;			// wurde die Rasse unterworfen
		// Akzeptanzpunkte (Rassen-ID, Punkte)
		m_mAcceptance.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_mAcceptance[key] = value;
		}

		// Diplomatieobjekt neu erstellen
		// Minorrace - KI anlegen
		if (m_pDiplomacyAI)
		{
			delete m_pDiplomacyAI;
			m_pDiplomacyAI = NULL;
		}
		m_pDiplomacyAI = new CMinorAI(this);
	}
}

//////////////////////////////////////////////////////////////////////
// Zugriffsfunktionen
//////////////////////////////////////////////////////////////////////

/// Funktion gibt die Akzeptanzpunkte zu einer gew�nschten Majorrace zur�ck.
/// @param sRaceID Rassen-ID der gew�nschten Majorrace
/// @return Akzeptanzpunkte
short CMinor::GetAcceptancePoints(const CString& sRaceID) const
{
	map<CString, short>::const_iterator it = m_mAcceptance.find(sRaceID);

	if (it != m_mAcceptance.end())
		return it->second;
	else
		return 0;
}

/// Funktion addiert Akzeptanzpunkte zu den Vorhandenen Punkten dazu.
/// @param sRaceID Rassen-ID der gew�nschten Majorrace
/// @param nAdd hinzuzuf�gende Akzeptanzpunkte
void CMinor::SetAcceptancePoints(const CString& sRaceID, short nAdd)
{
	if (nAdd == 0)
		return;

		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Minor.cpp: %s, m_mAcceptance[sRaceID]: %d, nAdd: %d\n", sRaceID, m_mAcceptance[sRaceID], nAdd);
	
	m_mAcceptance[sRaceID] += nAdd;
	

	map<CString, short>::iterator it = m_mAcceptance.find(sRaceID);

	// Konsistenzpr�fung durchf�hren
	// Obergrenze setzen, einfach mal auf 5000
	it->second = min(5000, it->second);
	// Untergrenze ist null
	it->second = max(0, it->second);

	if (it->second == 0)
		m_mAcceptance.erase(it);
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion l��t die Minorrace mit einer gewissen Wahrscheinlichkeit einen weiteren Planeten in ihrem System
/// kolonisieren.
/// @param pDoc Zeiger auf das Dokument
/// @return <code>true</code> wenn ein neuer Planet kolonisiert wurde, ansonsten <code>false</code>
bool CMinor::PerhapsExtend(CBotEDoc* pDoc)
{
	ASSERT(pDoc);

	// Wenn der dazugeh�rige Sektor nicht der kleinen Rasse geh�rt, also eine andere Rasse diese vereinnahmt hat,
	// dann w�chst das System auch nicht mehr automatisch
	if (pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetOwnerOfSector() != m_sID)
		return false;

	std::vector<CPlanet>& planets = pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetPlanets();
	bool bColonized = false;
	for (int i = 0; i < static_cast<int>(planets.size()); i++)
	{
		// ist der Planet noch nicht geterraformt
		if (planets.at(i).GetColonized() == FALSE && planets.at(i).GetHabitable() == TRUE && planets.at(i).GetTerraformed() == FALSE)
		{
			// mit einer gewissen Wahrscheinlichkeit wird der Planet terrageformt und kolonisiert
			// (ok) MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor hasn't terraformed all planets yet");
			if (rand()%200 >= (200 - (m_iTechnologicalProgress+1)))
			{
				bColonized = true;
				planets.at(i).SetNeededTerraformPoints(planets.at(i).GetNeededTerraformPoints());
				planets.at(i).SetTerraformed(TRUE);
				planets.at(i).SetColonisized(TRUE);
				planets.at(i).SetIsTerraforming(FALSE);
				if (planets.at(i).GetMaxHabitant() < 1.0f)
					planets.at(i).SetCurrentHabitant(planets.at(i).GetMaxHabitant());
				else
					planets.at(i).SetCurrentHabitant(1.0f);
				MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor.cpp: Minor has colonized another planet: Habitants: %g\n", planets.at(i).GetCurrentHabitant());
			}
		}
		// ist der Planet schon geterraformt
		else if (planets.at(i).GetColonized() == FALSE && planets.at(i).GetTerraformed() == TRUE)
		{
			// (ok) MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor hasn't colonized all planets yet");
			// dann wird mit einer gr��eren Wahrscheinlichkeit kolonisiert
			if (rand()%200 >= (200 - 3*(m_iTechnologicalProgress+1)))
			{
				bColonized = true;
				planets.at(i).SetColonisized(TRUE);
				if (planets.at(i).GetMaxHabitant() < 1.0f)
					planets.at(i).SetCurrentHabitant(planets.at(i).GetMaxHabitant());
				else
					planets.at(i).SetCurrentHabitant(1.0f);
				MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor.cpp: Minor has colonized another planet: Habitants: %g\n", planets.at(i).GetCurrentHabitant());
			}
		}
	}

	if (bColonized)
		// dann sind im System auch weitere Einwohner hinzugekommen
		pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetHabitants(pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetCurrentHabitants());

	return bColonized;
}

/// Diese Funktion l�sst die Minorrace vielleicht ein Schiff bauen
/// @param pDoc Zeiger auf das Dokument
void CMinor::PerhapsBuildShip(CBotEDoc* pDoc)
{
	ASSERT(pDoc);

	// wenn keine Spaceflightrasse, dann Abbruch
	if (!m_bSpaceflight)
		return;

	if (m_ptKO == CPoint(-1,-1))
		return;

	// Wenn der dazugeh�rige Sektor nicht der kleinen Rasse geh�rt, also eine andere Rasse diese vereinnahmt hat,
	// dann w�chst das System auch nicht mehr automatisch
	if (pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetOwnerOfSector() != m_sID)
		return;

	// Vielleicht baut die Minorrace ein Schiff in dieser Runde
	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
	{
		CShipInfo* pShipInfo = &(pDoc->m_ShipInfoArray[i]);
		if (pShipInfo->GetRace() == MINORNUMBER)
		{
			if (pShipInfo->GetOnlyInSystem() == this->GetHomesystemName())
			{
				// schauen ob es technologisch baubar ist
				BYTE byAvgTechLevel = pDoc->GetStatistics()->GetAverageTechLevel();
				BYTE byTechLevel = byAvgTechLevel + m_iTechnologicalProgress / 2;

				BYTE researchLevels[6] = {byTechLevel, byTechLevel, byTechLevel, byTechLevel, byTechLevel, byTechLevel};
				if (pShipInfo->IsThisShipBuildableNow(researchLevels))
				{
					// Wahrscheinlichkeit berechnen, ob das Schiff gebaut wird
					int nNumber = 1;
					for(CShipMap::const_iterator j = pDoc->m_ShipMap.begin(); j != pDoc->m_ShipMap.end(); ++j)
					{
						if (j->second->GetOwnerOfShip() == m_sID && j->second->GetShipClass() == pShipInfo->GetShipClass())
							nNumber++;
					}
					nNumber *= 5;
					if (rand()%nNumber == 0)
					{
						CShipMap::iterator ship = pDoc->BuildShip(pShipInfo->GetID(), GetRaceKO(), m_sID);
						// Befehl f�r nicht "b�se" Rassen auf Meiden stellen
						if (!IsRaceProperty(RACE_PROPERTY::HOSTILE)
							&& !IsRaceProperty(RACE_PROPERTY::WARLIKE)
							&& !IsRaceProperty(RACE_PROPERTY::SNEAKY)
							&& !IsRaceProperty(RACE_PROPERTY::SECRET))
						{
							ship->second->UnsetCurrentOrder();
							ship->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
						}
						return;
					}
				}
			}
		}
	}
}

/// Funktion berechnet wieviele Beziehungspunkte eine Majorrace f�r l�ngere Beziehungsdauer bekommt.
/// Umso mehr Punkte sie hat, umso schwerer wird es f�r andere Majorraces diese Rasse durch Geschenke
/// zu beeinflussen
/// @param pDoc Zeiger auf das Dokument
void CMinor::CalcAcceptancePoints(CBotEDoc* pDoc)
{
	ASSERT(pDoc);

	// alle Majors holen
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// und durchiterieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// nur wenn die Rasse bekannt ist weitermachen
		if (!IsRaceContacted(it->first))
			continue;

		short nAgreement = GetAgreement(it->first);
		MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor.cpp: CalcAcceptancePoints: Agreement:%d\n", nAgreement);
		short nAccPoints = 0;
		MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor.cpp: CalcAcceptancePoints: Agreement:%d, nAccPoints at start:%d\n", nAgreement, nAccPoints);
		// bei keinem aktuell laufendem Vertrag verringern sich die gesammelten Punkte langsam
		if (nAgreement == DIPLOMATIC_AGREEMENT::NONE)
			nAccPoints -= rand()%80 + 1;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
			nAccPoints += 10;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
			nAccPoints += 20;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
			nAccPoints += 30;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
		{
			nAccPoints += 40;
			// bei einer Mitgliedschaft erh�ht sich womoglich auch die Beziehung ein wenig
			SetRelation(it->first, rand()%2);
		}
		// bei Krieg werden alle Punkte gel�scht
		else if (nAgreement == DIPLOMATIC_AGREEMENT::WAR)
			nAccPoints -= GetAcceptancePoints(it->first);
		MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor.cpp: CalcAcceptancePoints: nAccPoints final:%d\n", nAccPoints);

		// Akzeptanzpunkte nun hinzuf�gen bzw. abziehen
		SetAcceptancePoints(it->first, nAccPoints);
	}
}

/// Funktion berechnet, wieviel Rohstoffe pro Runde (also bei jedem NextRound() aufrufen) von der
/// Minorrace verbraucht werden. Wenn die Minorrace die M�glichkeit hat, den jeweiligen Rohstoff
/// selbst abzubauen, damm ist der Verbrauch nat�rlich geringer.
/// @param pDoc Zeiger auf das Dokument
void CMinor::ConsumeResources(CBotEDoc* pDoc)
{
	ASSERT(pDoc);

	// bewohnbar sind:    C,F,G,H,K,L,M,N,O,P,Q,R
	BOOLEAN exist[DERITIUM + 1] = {0};
	pDoc->GetSector(m_ptKO.x, m_ptKO.y).GetAvailableResources(exist, true);

	short div;
	int value;

	// Titan
	exist[TITAN] == TRUE ? div = 1000 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(TITAN, -value);

	//CString s;
	//s.Format("Name der Rasse: %s\nTitanabbau: %d\nVerbrauch: %d\nLager: %d",m_strRaceName,exist[0],value,m_iRessourceStorage[0]);
	//AfxMessageBox(s);

	// Deuterium
	exist[DEUTERIUM] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(DEUTERIUM, -value);

	// Duranium
	exist[DURANIUM] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(DURANIUM, -value);

	// Kristalle
	exist[CRYSTAL] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(CRYSTAL, -value);

	// Iridium
	exist[IRIDIUM] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	value = min(3000, value);
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(IRIDIUM, -value);

	// Deritium
	value = rand()%2;
	pDoc->GetSystem(m_ptKO.x, m_ptKO.y).SetResourceStore(DERITIUM, -value);
}

/// Funktion �berpr�ft, ob die Minorrace das Angebot aufgrund anderer Vertr�ge �berhaupt annehmen kann.
/// @param pDoc Zeiger auf das Dokument
/// @param sMajorID MajorraceID, von welcher das Angebot stammt
/// @param nType angebotener Vertrag
/// @return <code>true</code> wenn das Angebot theoretisch angenommen werden k�nnte, ansonsten <code>false</code>
bool CMinor::CanAcceptOffer(CBotEDoc* pDoc, const CString& sMajorID, short nType) const
{
	ASSERT(pDoc);

	// Nur wenn der aktuelle Vertrag nicht h�herwertiger ist als der angebotene, dann wird er akzeptiert
	if (this->GetAgreement(sMajorID) >= nType)
		return false;

	// Checken ob wir ein Angebot �berhaupt annehmen k�nnen, wenn z.B. eine andere Hauptrasse
	// eine Mitgliedschaft mit einer Minorrace hat, dann k�nnen wir ihr kein Angebot machen, au�er
	// Krieg erkl�ren, Geschenke geben und Bestechen
	DIPLOMATIC_AGREEMENT::Typ nOthersAgreement = DIPLOMATIC_AGREEMENT::NONE;
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// nicht wir selbst
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (it->first != sMajorID)
		{
			DIPLOMATIC_AGREEMENT::Typ nTemp = this->GetAgreement(it->first);
			if (nTemp > nOthersAgreement)
				nOthersAgreement = nTemp;
		}
	}

	if ((nType == DIPLOMATIC_AGREEMENT::COOPERATION || nType == DIPLOMATIC_AGREEMENT::AFFILIATION || nType == DIPLOMATIC_AGREEMENT::MEMBERSHIP) && nOthersAgreement > DIPLOMATIC_AGREEMENT::FRIENDSHIP)
		return false;
	if (nType == DIPLOMATIC_AGREEMENT::TRADE && nOthersAgreement > DIPLOMATIC_AGREEMENT::AFFILIATION)
		return false;
	if (nType == DIPLOMATIC_AGREEMENT::FRIENDSHIP && nOthersAgreement > DIPLOMATIC_AGREEMENT::COOPERATION)
		return false;

	return true;
}

/// Funktion checkt die diplomatische Konsistenz und generiert bei K�ndigungen auch die entsprechende Nachricht
/// f�r das entsprechende Imperium der Majorrace.
/// @param pDoc Zeiger auf das Dokument
void CMinor::CheckDiplomaticConsistence(CBotEDoc* pDoc)
{
	ASSERT(pDoc);

	// alle Majors holen
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// und durchiterieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// nur wenn die Rasse bekannt ist weitermachen
		if (!IsRaceContacted(it->first))
			continue;

		// Wenn die Minorrace unterworfen wurde, so wird jeglicher Vertrag gek�ndigt aller bakannten Majors gek�ndigt
		if (GetSubjugated())
		{
			DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(it->first);
			CString s = "";
			switch (nAgreement)
			{
			case DIPLOMATIC_AGREEMENT::TRADE: {s = CLoc::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_sName);	break;}
			case DIPLOMATIC_AGREEMENT::FRIENDSHIP: {s = CLoc::GetString("CANCEL_FRIENDSHIP", FALSE, m_sName);	break;}
			case DIPLOMATIC_AGREEMENT::COOPERATION: {s = CLoc::GetString("CANCEL_COOPERATION", FALSE, m_sName);	break;}
			case DIPLOMATIC_AGREEMENT::AFFILIATION: {s = CLoc::GetString("CANCEL_AFFILIATION", FALSE, m_sName);	break;}
			case DIPLOMATIC_AGREEMENT::MEMBERSHIP: {s = CLoc::GetString("CANCEL_MEMBERSHIP", FALSE, m_sName);	break;}
			}
			// Krieg bleibt weiterhin bestehen
			if (nAgreement != DIPLOMATIC_AGREEMENT::WAR)
			{
				// Vertrag bei beiden Rassen aufl�sen
				SetAgreement(it->first, DIPLOMATIC_AGREEMENT::NONE);
				CMajor* pMajor = it->second;
				pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
				if (!s.IsEmpty())
				{
					CEmpireNews message;
					message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
					pMajor->GetEmpire()->AddMsg(message);
				}
			}
		}

		DIPLOMATIC_AGREEMENT::Typ nMajorsAgreement = GetAgreement(it->first);
		// hat die andere Rasse eine Mitgliedschaft, dann m�ssen Handelsvertrag und Freundschaft von allen anderen Rassen gek�ndigt werden
		if (nMajorsAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
		{
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (it->first == itt->first)
					continue;

				DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(itt->first);
				CString s = "";

				if (nAgreement == DIPLOMATIC_AGREEMENT::TRADE)
					s = CLoc::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					s = CLoc::GetString("CANCEL_FRIENDSHIP", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
					s = CLoc::GetString("CANCEL_COOPERATION", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
					s = CLoc::GetString("CANCEL_AFFILIATION", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
					s = CLoc::GetString("CANCEL_MEMBERSHIP", FALSE, m_sName);

				if (!s.IsEmpty())
				{
					// Vertrag bei beiden Rassen aufl�sen
					SetAgreement(itt->first, DIPLOMATIC_AGREEMENT::NONE);
					CMajor* pMajor = itt->second;
					pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
					CEmpireNews message;
					message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
					pMajor->GetEmpire()->AddMsg(message);
				}
			}
		}
		else if (nMajorsAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
		{
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (it->first == itt->first)
					continue;

				DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(itt->first);
				CString s = "";

				if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					s = CLoc::GetString("CANCEL_FRIENDSHIP", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
					s = CLoc::GetString("CANCEL_COOPERATION", FALSE, m_sName);
				else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
					s = CLoc::GetString("CANCEL_AFFILIATION", FALSE, m_sName);

				if (!s.IsEmpty())
				{
					// Vertrag bei beiden Rassen aufl�sen
					SetAgreement(itt->first, DIPLOMATIC_AGREEMENT::NONE);
					CMajor* pMajor = itt->second;
					pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
					CEmpireNews message;
					message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
					pMajor->GetEmpire()->AddMsg(message);
				}
			}
		}
		else if (nMajorsAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
		{
			for (map<CString, CMajor*>::const_iterator itt = pmMajors->begin(); itt != pmMajors->end(); ++itt)
			{
				if (it->first == itt->first)
					continue;

				DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(itt->first);
				CString s = "";

				if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
					s = CLoc::GetString("CANCEL_COOPERATION", FALSE, m_sName);

				if (!s.IsEmpty())
				{
					// Vertrag bei beiden Rassen aufl�sen
					SetAgreement(itt->first, DIPLOMATIC_AGREEMENT::NONE);
					CMajor* pMajor = itt->second;
					pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
					CEmpireNews message;
					message.CreateNews(s, EMPIRE_NEWS_TYPE::DIPLOMACY);
					pMajor->GetEmpire()->AddMsg(message);
				}
			}
		}
	}
}

/// Funktion �berpr�ft die Beziehungen zu den Hauptrassen. Wenn diese zuweit vom Vertrag abweicht k�nnte gek�ndigt werden.
/// @param pDoc Zeiger auf das Dokument
void CMinor::PerhapsCancelAgreement(CBotEDoc* pDoc)
{
	ASSERT(pDoc);

	// alle Majors holen
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// und durchiterieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// nur zu 25% wird in dieser Runde dann auch gek�ndigt
		if (rand()%3 != 0)
			continue;
		// nur wenn die Rasse bekannt ist weitermachen
		if (!IsRaceContacted(it->first))
			continue;

		short nRelation	= GetRelation(it->first);
		DIPLOMATIC_AGREEMENT::Typ nAgreement = GetAgreement(it->first);
		if (nRelation < nAgreement * 12 && nAgreement >= DIPLOMATIC_AGREEMENT::TRADE && nAgreement <= DIPLOMATIC_AGREEMENT::MEMBERSHIP)
		{
			// Jetzt wird gek�ndigt
			CString sText;
			switch (nAgreement)
			{
			case DIPLOMATIC_AGREEMENT::TRADE:
			{
				sText = CLoc::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_sName);
				break;
			}
			case DIPLOMATIC_AGREEMENT::FRIENDSHIP:
			{
				sText = CLoc::GetString("CANCEL_FRIENDSHIP", FALSE, m_sName);
				break;
			}
			case DIPLOMATIC_AGREEMENT::COOPERATION:
			{
				sText = CLoc::GetString("CANCEL_COOPERATION", FALSE, m_sName);
				break;
			}
			case DIPLOMATIC_AGREEMENT::AFFILIATION:
			{
				sText = CLoc::GetString("CANCEL_AFFILIATION", FALSE, m_sName);
				break;
			}
			case DIPLOMATIC_AGREEMENT::MEMBERSHIP:
			{
				sText = CLoc::GetString("CANCEL_MEMBERSHIP", FALSE, m_sName);
				break;
			}
			}

			if (!sText.IsEmpty())
			{
				CMajor* pMajor = it->second;
				pMajor->SetAgreement(m_sID, DIPLOMATIC_AGREEMENT::NONE);
				SetAgreement(it->first, DIPLOMATIC_AGREEMENT::NONE);
				CEmpireNews message;
				message.CreateNews(sText, EMPIRE_NEWS_TYPE::DIPLOMACY);
				pMajor->GetEmpire()->AddMsg(message);
			}
		}
	}
}

/// Funktion erstellt eine Tooltipinfo der Rasse.
/// @return	der erstellte Tooltip-Text
CString CMinor::GetTooltip(void) const
{
	CString sTip = __super::GetTooltip();
	sTip += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Technischen Fortschritt anzeigen
	CString sProgress(CLoc::GetString("TECHNICAL_PROGRESS"));
	sProgress = CHTMLStringBuilder::GetHTMLColor(sProgress, _T("silver"));
	sProgress = CHTMLStringBuilder::GetHTMLHeader(sProgress, _T("h4"));
	sProgress += CHTMLStringBuilder::GetHTMLStringNewLine();
	sProgress += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sProgress += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString s = "";
	switch (GetTechnologicalProgress())
	{
	case 0: s = CLoc::GetString("VERY_UNDERDEVELOPED");	break;
	case 1: s = CLoc::GetString("UNDERDEVELOPED");		break;
	case 2: s = CLoc::GetString("NORMAL_DEVELOPED");	break;
	case 3: s = CLoc::GetString("DEVELOPED");			break;
	case 4: s = CLoc::GetString("VERY_DEVELOPED");		break;
	}

	s = CHTMLStringBuilder::GetHTMLColor(s);
	s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h5"));
	sProgress += s;
	sProgress += CHTMLStringBuilder::GetHTMLStringNewLine();
	sProgress += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Bestechlichkeit anzeigen
	CString sCor(CLoc::GetString("CORRUPTIBILITY"));
	sCor = CHTMLStringBuilder::GetHTMLColor(sCor, _T("silver"));
	sCor = CHTMLStringBuilder::GetHTMLHeader(sCor, _T("h4"));
	sCor += ": ";
	//sCor += CHTMLStringBuilder::GetHTMLStringNewLine();
	//sCor += CHTMLStringBuilder::GetHTMLStringHorzLine();
	//sCor += CHTMLStringBuilder::GetHTMLStringNewLine();

	s = "";
	switch (GetCorruptibility())
	{
	case 0: s = CLoc::GetString("VERY_LOW_CORRUPTIBILITY");	break;
	case 1: s = CLoc::GetString("LOW_CORRUPTIBILITY");		break;
	case 2: s = CLoc::GetString("NORMAL_CORRUPTIBILITY");	break;
	case 3: s = CLoc::GetString("HIGH_CORRUPTIBILITY");		break;
	case 4: s = CLoc::GetString("VERY_HIGH_CORRUPTIBILITY");break;
	}
	sCor = s;
/*
	CBotEDoc* pDoc;
	ASSERT(pDoc);
	// alle Majors holen
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// und durchiterieren
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		// nur wenn die Rasse bekannt ist weitermachen
		if (!IsRaceContacted(it->first))
			continue;

		short nAgreement = GetAgreement(it->first);
		//MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor.cpp: CalcAcceptancePoints: Agreement:%d\n", nAgreement);
		short nAccPoints = 0;
		//MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor.cpp: CalcAcceptancePoints: Agreement:%d, nAccPoints at start:%d\n", nAgreement, nAccPoints);
		// bei keinem aktuell laufendem Vertrag verringern sich die gesammelten Punkte langsam
		/*if (nAgreement == DIPLOMATIC_AGREEMENT::NONE)
			nAccPoints -= rand()%80 + 1;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::FRIENDSHIP)
			nAccPoints += 10;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::COOPERATION)
			nAccPoints += 20;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::AFFILIATION)
			nAccPoints += 30;
		else if (nAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
		{
			nAccPoints += 40;
			// bei einer Mitgliedschaft erh�ht sich womoglich auch die Beziehung ein wenig
			SetRelation(it->first, rand()%2);
		}
		// bei Krieg werden alle Punkte gel�scht
		else if (nAgreement == DIPLOMATIC_AGREEMENT::WAR)
			nAccPoints -= GetAcceptancePoints(it->first);
		MYTRACE("logdetails")(MT::LEVEL_DEBUG, "Minor.cpp: CalcAcceptancePoints: nAccPoints final:%d\n", nAccPoints);

		// Akzeptanzpunkte nun hinzuf�gen bzw. abziehen
		SetAcceptancePoints(it->first, nAccPoints);
		/
	//}

	CString sRelation;
	sRelation = "";

sRelation.Format("xx",GetRelation(this));

	CString sAcceptance;
	sAcceptance = "";
sAcceptance.Format("xx",GetAcceptancePoints(it->first));
}
*/


CString sRelation;
	sRelation = "Relation not ready";

	CString sAcceptance;
	sAcceptance = "Acceptance not ready ";
//
	CString sDesc;
	sDesc = " ";
	sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc, _T("silver"));
	sDesc += CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h4"));
	//sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
	//sDesc += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
	sDesc += m_sDesc;
	sDesc += CHTMLStringBuilder::GetHTMLColor(sDesc, _T("silver"));

	//sDesc += m_sDesc;

	s = CHTMLStringBuilder::GetHTMLColor(sDesc);
	s = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));
	sCor += s;
	sCor += CHTMLStringBuilder::GetHTMLStringNewLine();
//*/

	return CHTMLStringBuilder::GetHTMLCenter(sTip + sProgress + sCor + sRelation + sAcceptance + sDesc);
}

/// Funktion zum erstellen einer Rasse.
/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
/// @param saInfo Referenz auf Rasseninformationen
/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
void CMinor::Create(const CStringArray& saInfo, int& nPos)
{
	ASSERT(nPos >= 0);

	Reset();

	// Minorrace nun anlegen
	m_sID				= saInfo[nPos++];				// Rassen-ID
	m_sID.Remove(':');
	m_sHomeSystem		= m_sID;						// Name des Heimatsystems
	m_sHomeSystem.MakeLower();
	CString sChar = (CString)m_sHomeSystem.GetAt(0);
	m_sHomeSystem.SetAt(0, sChar.MakeUpper().GetAt(0));

	m_sName				= saInfo[nPos++];				// Rassenname
	m_sDesc				= saInfo[nPos++];				// Rassenbeschreibung
	//(ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "MinorRace: m_sName: %s, m_sHomeSystem: %s\n", m_sName, m_sHomeSystem);

	// grafische Attribute
	m_sGraphicFile				= saInfo[nPos++];		// Name der zugeh�rigen Grafikdatei

	// Beziehungen
	// ALPHA5 muss nat�rlich noch auf das neue System umgestellt werden -> werden nicht mehr in der data Datei gespeichert
	//for (int i = 0; i < 6; i++)
	//{
	//	int ALPHA5 = atoi(saInfo[nPos++]);
	//}
	//TODO
	//relations from the minor data file aren't used
	nPos = nPos + 6;

	m_iTechnologicalProgress = atoi(saInfo[nPos++]);
	m_RaceType			= RACE_TYPE_MINOR;						// Rassentyp (Major, Medior, Minor)

	// mehrere Rasseneigenschaften sind durch Komma getrennt
	CString sRaceProperties = saInfo[nPos++];
	int nStart = 0;
	while (nStart < sRaceProperties.GetLength())
	{
		RACE_PROPERTY::Typ nProperty = (RACE_PROPERTY::Typ)atoi(sRaceProperties.Tokenize(",", nStart));
		ASSERT(nProperty >= RACE_PROPERTY::NOTHING_SPECIAL && nProperty <= RACE_PROPERTY::HOSTILE);
		SetRaceProperty(nProperty, true);				// Rasseneigenschaften
	}

	m_bSpaceflight		= atoi(saInfo[nPos++]) == 0 ? false : true;
	m_iCorruptibility	= atoi(saInfo[nPos++]);


	// Schiffsnummer vergeben
	m_byShipNumber = MINORNUMBER;
	
	// Minorrace - KI anlegen
	m_pDiplomacyAI = new CMinorAI(this);

//	MYTRACE("logdata")(MT::LEVEL_DEBUG, "MinorRace: m_byShipNumber %s, m_pDiplomacyAI %s\n", m_byShipNumber, m_pDiplomacyAI);
}

/// Funktion zum erstellen von Weltraummonstern
/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
/// @param saInfo Referenz auf Rasseninformationen
/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
void CMinor::CreateAlienEntities(const CStringArray& saInfo, int& nPos)
{
	ASSERT(nPos >= 0);

	Reset();

	// Minorrace nun anlegen
	m_sID				= saInfo[nPos++];				// Rassen-ID
	m_sID.Remove(':');
	m_sName				= saInfo[nPos++];				// Rassenname
	m_sDesc				= saInfo[nPos++];				// Rassenbeschreibung

	// grafische Attribute
	m_sGraphicFile		= saInfo[nPos++];				// Name der zugeh�rigen Grafikdatei
	m_iTechnologicalProgress = atoi(saInfo[nPos++]);
	m_RaceType			= RACE_TYPE_MINOR;				// Rassentyp (Major, Medior, Minor)

	//MYTRACE("logdata")(MT::LEVEL_DEBUG, "AlienRace: m_sName %s, m_RaceType %s\n", m_sName, m_RaceType);


	// mehrere Rasseneigenschaften sind durch Komma getrennt
	CString sRaceProperties = saInfo[nPos++];
//	MYTRACE("logdata")(MT::LEVEL_DEBUG, "AlienRace: sRaceProperties %s\n", sRaceProperties);
	int nStart = 0;
	while (nStart < sRaceProperties.GetLength())
	{
		RACE_PROPERTY::Typ nProperty = (RACE_PROPERTY::Typ)atoi(sRaceProperties.Tokenize(",", nStart));
		ASSERT(nProperty >= RACE_PROPERTY::NOTHING_SPECIAL && nProperty <= RACE_PROPERTY::HOSTILE);
		SetRaceProperty(nProperty, true);				// Rasseneigenschaften
	}
	m_nSpecialAbility	= atoi(saInfo[nPos++]);

	m_bSpaceflight		= atoi(saInfo[nPos++]) == 0 ? false : true;
	m_iCorruptibility	= atoi(saInfo[nPos++]);

	// Schiffsnummer vergeben
	m_byShipNumber = MINORNUMBER;

	// Minorrace - KI anlegen
	m_pDiplomacyAI = new CMinorAI(this);
//	MYTRACE("logdata")(MT::LEVEL_DEBUG, "AlienRace: m_byShipNumber %s, m_pDiplomacyAI %s\n", m_byShipNumber, m_pDiplomacyAI);
}

/// Funktion zum zur�cksetzen aller Werte auf Ausgangswerte.
void CMinor::Reset(void)
{
	CRace::Reset();

	m_ptKO = CPoint(-1,-1);					// Koordinaten der Rasse
	m_iTechnologicalProgress = 0;			// wie fortschrittlich ist die Minorrace?
	m_iCorruptibility = 0;					// wie stark �ndert sich die Beziehung beim Geschenke geben?
	m_bSpaceflight = false;					// Spaceflightnation (hat Schiffe)
	m_bSubjugated = false;					// wurde die Rasse unterworfen
	m_mAcceptance.clear();					// Punkte die eine MajorRace durch l�ngere Beziehung mit der Rasse ansammelt, wird schwerer diese Rasse wegzukaufen
}

void CMinor::Contact(const CRace& Race, const CPoint& p) {
	CRace::Contact(Race, p);
}

bool CMinor::IsMemberTo(const CString& MajorID) const {
	if(!MajorID.IsEmpty())
		return GetAgreement(MajorID) == DIPLOMATIC_AGREEMENT::MEMBERSHIP;

	for(std::map<CString, DIPLOMATIC_AGREEMENT::Typ>::const_iterator it = m_mAgreement.begin();
		it != m_mAgreement.end(); ++it) {
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "MinorRace: m_mAgreement %s\n", m_mAgreement);
		if(it->second == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
			return true;
	}
	return false;
}
