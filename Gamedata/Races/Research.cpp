// Research.cpp: Implementierung der Klasse CResearch.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Research.h"
#include "General/Loc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CResearch, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CResearch::CResearch()
{
	Reset();
}

CResearch::~CResearch()
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CResearch::CResearch(const CResearch & rhs)
{
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;
	m_iUniqueTech = rhs.m_iUniqueTech;

	m_iBioPercentage = rhs.m_iBioPercentage;
	m_iEnergyPercentage = rhs.m_iEnergyPercentage;
	m_iComputerPercentage = rhs.m_iComputerPercentage;
	m_iPropulsionPercentage = rhs.m_iPropulsionPercentage;
	m_iConstructionPercentage = rhs.m_iConstructionPercentage;
	m_iWeaponPercentage = rhs.m_iWeaponPercentage;
	m_iUniquePercentage = rhs.m_iUniquePercentage;

	m_lBioFP = rhs.m_lBioFP;
	m_lEnergyFP = rhs.m_lEnergyFP;
	m_lComputerFP = rhs.m_lComputerFP;
	m_lPropulsionFP = rhs.m_lPropulsionFP;
	m_lConstructionFP = rhs.m_lConstructionFP;
	m_lWeaponFP = rhs.m_lWeaponFP;
	m_lUniqueFP = rhs.m_lUniqueFP;

	m_bBioLocked = rhs.m_bBioLocked;
	m_bEnergyLocked = rhs.m_bEnergyLocked;
	m_bComputerLocked = rhs.m_bComputerLocked;
	m_bPropulsionLocked = rhs.m_bPropulsionLocked;
	m_bConstructionLocked = rhs.m_bConstructionLocked;
	m_bWeaponLocked = rhs.m_bWeaponLocked;
	m_bUniqueLocked = rhs.m_bUniqueLocked;

	m_iBioTechBoni = rhs.m_iBioTechBoni;
	m_iEnergyTechBoni = rhs.m_iEnergyTechBoni;
	m_iCompTechBoni = rhs.m_iCompTechBoni;
	m_iPropulsionTechBoni = rhs.m_iPropulsionTechBoni;
	m_iConstructionTechBoni = rhs.m_iConstructionTechBoni;
	m_iWeaponTechBoni = rhs.m_iWeaponTechBoni;

	m_bUniqueReady = rhs.m_bUniqueReady;
	m_iNumberOfUnique = rhs.m_iNumberOfUnique;

	for (int i = 0; i < 8; i++)
		m_strMessage[i] = rhs.m_strMessage[i];

	ResearchInfo = rhs.ResearchInfo;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CResearch & CResearch::operator=(const CResearch & rhs)
{
	if (this == &rhs)
		return *this;
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;
	m_iUniqueTech = rhs.m_iUniqueTech;

	m_iBioPercentage = rhs.m_iBioPercentage;
	m_iEnergyPercentage = rhs.m_iEnergyPercentage;
	m_iComputerPercentage = rhs.m_iComputerPercentage;
	m_iPropulsionPercentage = rhs.m_iPropulsionPercentage;
	m_iConstructionPercentage = rhs.m_iConstructionPercentage;
	m_iWeaponPercentage = rhs.m_iWeaponPercentage;
	m_iUniquePercentage = rhs.m_iUniquePercentage;

	m_lBioFP = rhs.m_lBioFP;
	m_lEnergyFP = rhs.m_lEnergyFP;
	m_lComputerFP = rhs.m_lComputerFP;
	m_lPropulsionFP = rhs.m_lPropulsionFP;
	m_lConstructionFP = rhs.m_lConstructionFP;
	m_lWeaponFP = rhs.m_lWeaponFP;
	m_lUniqueFP = rhs.m_lUniqueFP;

	m_bBioLocked = rhs.m_bBioLocked;
	m_bEnergyLocked = rhs.m_bEnergyLocked;
	m_bComputerLocked = rhs.m_bComputerLocked;
	m_bPropulsionLocked = rhs.m_bPropulsionLocked;
	m_bConstructionLocked = rhs.m_bConstructionLocked;
	m_bWeaponLocked = rhs.m_bWeaponLocked;
	m_bUniqueLocked = rhs.m_bUniqueLocked;

	m_iBioTechBoni = rhs.m_iBioTechBoni;
	m_iEnergyTechBoni = rhs.m_iEnergyTechBoni;
	m_iCompTechBoni = rhs.m_iCompTechBoni;
	m_iPropulsionTechBoni = rhs.m_iPropulsionTechBoni;
	m_iConstructionTechBoni = rhs.m_iConstructionTechBoni;
	m_iWeaponTechBoni = rhs.m_iWeaponTechBoni;

	m_bUniqueReady = rhs.m_bUniqueReady;
	m_iNumberOfUnique = rhs.m_iNumberOfUnique;

	for (int i = 0; i < 8; i++)
		m_strMessage[i] = rhs.m_strMessage[i];

	ResearchInfo = rhs.ResearchInfo;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CResearch::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	ResearchInfo.Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iBioTech;
		ar << m_iEnergyTech;
		ar << m_iCompTech;
		ar << m_iPropulsionTech;
		ar << m_iConstructionTech;
		ar << m_iWeaponTech;
		ar << m_iUniqueTech;
		MYTRACE("logsave")(MT::LEVEL_DEBUG, "Research.cpp: \t# LEVEL #: \tBio:%i, \tEn:%i, \tComp:%i, \tProp:%i, \tConstr:%i, \tWeap:%i, \tSpecial:%i\n", 
				m_iBioTech, m_iEnergyTech, m_iCompTech, m_iPropulsionTech, m_iConstructionTech, m_iWeaponTech, m_iUniqueTech);
		ar << m_iBioPercentage;
		ar << m_iEnergyPercentage;
		ar << m_iComputerPercentage;
		ar << m_iPropulsionPercentage;
		ar << m_iConstructionPercentage;
		ar << m_iWeaponPercentage;
		ar << m_iUniquePercentage;
		MYTRACE("logsave")(MT::LEVEL_DEBUG, "Research.cpp: \t# Percent #: \tBio:%i, \tEn:%i, \tComp:%i, \tProp:%i, \tConstr:%i, \tWeap:%i, \tSpecial:%i\n", 
					m_iBioPercentage, m_iEnergyPercentage, m_iComputerPercentage, m_iPropulsionPercentage, m_iConstructionPercentage, m_iWeaponPercentage, 
					m_iUniquePercentage);
		ar << m_lBioFP;
		ar << m_lEnergyFP;
		ar << m_lComputerFP;
		ar << m_lPropulsionFP;
		ar << m_lConstructionFP;
		ar << m_lWeaponFP;
		ar << m_lUniqueFP;
		MYTRACE("logsave")(MT::LEVEL_DEBUG, "Research.cpp: \t# FPoints #: \tBio:%g, \tEn:%g, \tComp:%g, \tProp:%g, \tConstr:%g, \tWeap:%g, \tSpecial:%g\n", 
				m_lBioFP, m_lEnergyFP, m_lComputerFP, m_lPropulsionFP, m_lConstructionFP, m_lWeaponFP, m_lUniqueFP);
		ar << m_bBioLocked;
		ar << m_bEnergyLocked;
		ar << m_bComputerLocked;
		ar << m_bPropulsionLocked;
		ar << m_bConstructionLocked;
		ar << m_bWeaponLocked;
		ar << m_bUniqueLocked;
		ar << m_iBioTechBoni;
		ar << m_iEnergyTechBoni;
		ar << m_iCompTechBoni;
		ar << m_iPropulsionTechBoni;
		ar << m_iConstructionTechBoni;
		ar << m_iWeaponTechBoni;
		MYTRACE("logsave")(MT::LEVEL_DEBUG, "Research.cpp: \t# Boni #: \tBio:%i, \tEn:%i, \tComp:%i, \tProp:%i, \tConstr:%i, \tWeap:%i\n", 
				m_iBioTechBoni, m_iEnergyTechBoni, m_iCompTechBoni, m_iPropulsionTechBoni, m_iConstructionTechBoni, m_iWeaponTechBoni);
		ar << m_bUniqueReady;
		ar << m_iNumberOfUnique;
		for (int i = 0; i < 8; i++)
		{
			if (!m_strMessage[i])
			MYTRACE("logsave")(MT::LEVEL_DEBUG, "Research.cpp: \t# m_iNumberOfUnique:%i, m_strMessage:%s\n", 
				i, m_strMessage[i]);
			ar << m_strMessage[i];
		}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iBioTech;
		ar >> m_iEnergyTech;
		ar >> m_iCompTech;
		ar >> m_iPropulsionTech;
		ar >> m_iConstructionTech;
		ar >> m_iWeaponTech;
		ar >> m_iUniqueTech;
		ar >> m_iBioPercentage;
		ar >> m_iEnergyPercentage;
		ar >> m_iComputerPercentage;
		ar >> m_iPropulsionPercentage;
		ar >> m_iConstructionPercentage;
		ar >> m_iWeaponPercentage;
		ar >> m_iUniquePercentage;
		ar >> m_lBioFP;
		ar >> m_lEnergyFP;
		ar >> m_lComputerFP;
		ar >> m_lPropulsionFP;
		ar >> m_lConstructionFP;
		ar >> m_lWeaponFP;
		ar >> m_lUniqueFP;
		ar >> m_bBioLocked;
		ar >> m_bEnergyLocked;
		ar >> m_bComputerLocked;
		ar >> m_bPropulsionLocked;
		ar >> m_bConstructionLocked;
		ar >> m_bWeaponLocked;
		ar >> m_bUniqueLocked;
		ar >> m_iBioTechBoni;
		ar >> m_iEnergyTechBoni;
		ar >> m_iCompTechBoni;
		ar >> m_iPropulsionTechBoni;
		ar >> m_iConstructionTechBoni;
		ar >> m_iWeaponTechBoni;
		ar >> m_bUniqueReady;
		ar >> m_iNumberOfUnique;
		for (int i = 0; i < 8; i++)
			ar >> m_strMessage[i];
		// Die Namen der Techs und deren Beschreibung neu einlesen
		ResearchInfo.SetTechInfos(0, m_iBioTech+1);
		ResearchInfo.SetTechInfos(1, m_iEnergyTech+1);
		ResearchInfo.SetTechInfos(2, m_iCompTech+1);
		ResearchInfo.SetTechInfos(3, m_iPropulsionTech+1);
		ResearchInfo.SetTechInfos(4, m_iConstructionTech+1);
		ResearchInfo.SetTechInfos(5, m_iWeaponTech+1);
	}
}

//////////////////////////////////////////////////////////////////////
// Memberzugriffsfunktionen
//////////////////////////////////////////////////////////////////////
// Mit dieser Funktion kann man den prozentualen Anteil �ndern, aber nat�rlich d�rfen nicht alle zusammen �ber 100% betragen
void CResearch::SetPercentage(BYTE tech, BYTE percentage)
{
	short diff = 0;	// Differenz des ge�nderten Anteils
	USHORT numberoflocks = 0;	// Anzahl der locked Gebiete
	// Schauen welche Gebiete alle Gelockt sind, diese Punkte von diff abziehen bzw. draufrechnen
	short difflock = 0;
	if (m_bBioLocked == TRUE)
	{
		difflock += m_iBioPercentage;
		numberoflocks++;
	}
	if (m_bEnergyLocked == TRUE)
	{
		difflock += m_iEnergyPercentage;
		numberoflocks++;
	}
	if (m_bComputerLocked == TRUE)
	{
		difflock += m_iComputerPercentage;
		numberoflocks++;
	}
	if (m_bConstructionLocked == TRUE)
	{
		difflock += m_iConstructionPercentage;
		numberoflocks++;
	}
	if (m_bPropulsionLocked == TRUE)
	{
		difflock += m_iPropulsionPercentage;
		numberoflocks++;
	}
	if (m_bWeaponLocked == TRUE)
	{
		difflock += m_iWeaponPercentage;
		numberoflocks++;
	}
	if (m_bUniqueLocked == TRUE)
	{
		difflock += m_iUniquePercentage;
		if (m_bUniqueReady == FALSE)
			numberoflocks++;
	}

	// Wenn wir die Uniqueforschung nicht zur Auswahl haben und aber 5 Gebiete gelocked sind,
	// k�nnen wir nichts ver�ndern, machen also nichts
	if (m_bUniqueReady == TRUE && numberoflocks == 5)
		tech = MAXBYTE;	// also nichts
	// Wenn wir die Uniqueforschung zur Auswahl haben und 6 Gebiete gelocked sind,
	// k�nnen wie auch nix machen
	if (m_bUniqueReady == FALSE && numberoflocks == 6)
		tech = MAXBYTE;	// also nichts

	// Setzen in der Switch-Anweisung den ge�nderten neuen Wert
	switch (tech)
	{
	case 0:	// Bio
		{
			if (m_bBioLocked == FALSE)
			{
				diff = m_iBioPercentage;
				diff = percentage-diff;
				m_iBioPercentage = percentage;
				if (m_iBioPercentage > (100-difflock))
					m_iBioPercentage = 100-difflock;

			}
			break;
		}
	case 1: // Energie
		{
			if (m_bEnergyLocked == FALSE)
			{
				diff = m_iEnergyPercentage;
				diff = percentage-diff;
				m_iEnergyPercentage = percentage;
				if (m_iEnergyPercentage > (100-difflock))
					m_iEnergyPercentage = 100-difflock;
			}
			break;
		}
	case 2: // Computer
		{
			if (m_bComputerLocked == FALSE)
			{
				diff = m_iComputerPercentage;
				diff = percentage-diff;
				m_iComputerPercentage = percentage;
				if (m_iComputerPercentage > (100-difflock))
					m_iComputerPercentage = 100-difflock;
			}
			break;
		}
	case 3: // Bautechnik
		{
			if (m_bConstructionLocked == FALSE)
			{
				diff = m_iConstructionPercentage;
				diff = percentage-diff;
				m_iConstructionPercentage = percentage;
				if (m_iConstructionPercentage > (100-difflock))
					m_iConstructionPercentage = 100-difflock;
			}
			break;
		}
	case 4: // Antriebstechnik
		{
			if (m_bPropulsionLocked == FALSE)
			{
				diff = m_iPropulsionPercentage;
				diff = percentage-diff;
				m_iPropulsionPercentage = percentage;
				if (m_iPropulsionPercentage > (100-difflock))
					m_iPropulsionPercentage = 100-difflock;
			}
			break;
		}
	case 5: // Waffentechnik
		{
			if (m_bWeaponLocked == FALSE)
			{
				diff = m_iWeaponPercentage;
				diff = percentage-diff;
				m_iWeaponPercentage = percentage;
				if (m_iWeaponPercentage > (100-difflock))
					m_iWeaponPercentage = 100-difflock;
			}
			break;
		}
	case 6: // Unique
		{
			if (m_bUniqueLocked == FALSE && m_bUniqueReady == FALSE)
			{
				diff = m_iUniquePercentage;
				diff = percentage-diff;
				m_iUniquePercentage = percentage;
				if (m_iUniquePercentage > (100-difflock))
					m_iUniquePercentage = 100-difflock;
			}
			break;
		}
	default:
		diff = 0;
	}
	// Wenn wir eine Forschung erh�ht haben
	// Jetzt die Punkte von den anderen wegnehmen
	while (diff > 0)
	{
		short changediff = diff;	// Hilfvariable
		if (tech != 0 && m_iBioPercentage > 0 && m_bBioLocked == FALSE) // nicht Bio erh�ht
		{
			m_iBioPercentage--;
			diff--;
			if (diff == 0)
				break;
		}
		if (tech != 1 && m_iEnergyPercentage > 0 && m_bEnergyLocked == FALSE) // nicht Energy erh�ht
		{
			m_iEnergyPercentage--;
			diff--;
			if (diff == 0)
				break;
		}
		if (tech != 2 && m_iComputerPercentage > 0 && m_bComputerLocked == FALSE) // nicht Computer erh�ht
		{
			m_iComputerPercentage--;
			diff--;
			if (diff == 0)
				break;
		}
		if (tech != 3 && m_iConstructionPercentage > 0 && m_bConstructionLocked == FALSE) // nicht Construction erh�ht
		{
			m_iConstructionPercentage--;
			diff--;
			if (diff == 0)
				break;
		}
		if (tech != 4 && m_iPropulsionPercentage > 0 && m_bPropulsionLocked == FALSE) // nicht Propulsion erh�ht
		{
			m_iPropulsionPercentage--;
			diff--;
			if (diff == 0)
				break;
		}
		if (tech != 5 && m_iWeaponPercentage > 0 && m_bWeaponLocked == FALSE) // nicht Weapon erh�ht
		{
			m_iWeaponPercentage--;
			diff--;
			if (diff == 0)
				break;
		}
		if (tech != 6 && m_iUniquePercentage > 0 && m_bUniqueReady == FALSE && m_bUniqueLocked == FALSE) // nicht Unique erh�ht
		{
			m_iUniquePercentage--;
			diff--;
			if (diff == 0)
				break;
		}
		// Wenn sich nichts ge�ndert hat, dann die Schleife verlassen, kann vorkommen, wenn wir locken
		if (changediff == diff)
			break;

	}
	// Wenn wir eine Forschung verringern
	// Jetzt Punkte auf die anderen Gebiete verteilen
	while (diff < 0 && diff <= difflock)
	{
		short changediff = diff;	// Hilfvariable
		if (tech != 0 && m_iBioPercentage < 100 && m_bBioLocked == FALSE) // nicht Bio erh�ht
		{
			m_iBioPercentage++;
			diff++;
			if (diff == 0)
				break;
		}
		if (tech != 1 && m_iEnergyPercentage < 100 && m_bEnergyLocked == FALSE) // nicht Energy erh�ht
		{
			m_iEnergyPercentage++;
			diff++;
			if (diff == 0)
				break;
		}
		if (tech != 2 && m_iComputerPercentage < 100 && m_bComputerLocked == FALSE) // nicht Computer erh�ht
		{
			m_iComputerPercentage++;
			diff++;
			if (diff == 0)
				break;
		}
		if (tech != 3 && m_iConstructionPercentage < 100 && m_bConstructionLocked == FALSE) // nicht Construction erh�ht
		{
			m_iConstructionPercentage++;
			diff++;
			if (diff == 0)
				break;
		}
		if (tech != 4 && m_iPropulsionPercentage < 100 && m_bPropulsionLocked == FALSE) // nicht Propulsion erh�ht
		{
			m_iPropulsionPercentage++;
			diff++;
			if (diff == 0)
				break;
		}
		if (tech != 5 && m_iWeaponPercentage < 100 && m_bWeaponLocked == FALSE) // nicht Weapon erh�ht
		{
			m_iWeaponPercentage++;
			diff++;
			if (diff == 0)
				break;
		}
		if (tech != 6 && m_iUniquePercentage < 100 && m_bUniqueReady == FALSE && m_bUniqueLocked == FALSE) // nicht Unique erh�ht
		{
			m_iUniquePercentage++;
			diff++;
			if (diff == 0)
				break;
		// Wenn sich nichts ge�ndert hat, dann die Schleife verlassen, kann vorkommen, wenn wir locken
		if (changediff == diff)
			break;
		}
	}
}

// Funktion locked ein Gebiet oder unlocked ein Gebiet
void CResearch::SetLock(BYTE tech, BOOLEAN locked)
{
	switch (tech)
	{
	case 0:
		{
			m_bBioLocked = locked;
			break;
		}
	case 1:
		{
			m_bEnergyLocked = locked;
			break;
		}
	case 2:
		{
			m_bComputerLocked = locked;
			break;
		}
	case 3:
		{
			m_bConstructionLocked = locked;
			break;
		}
	case 4:
		{
			m_bPropulsionLocked = locked;
			break;
		}
	case 5:
		{
			m_bWeaponLocked = locked;
			break;
		}
	case 6:
		{
			m_bUniqueLocked = locked;
			break;
		}
	}
}

// Funktion gibt den Lock-Status eines Gebiets zur�ck
BOOLEAN CResearch::GetLockStatus(BYTE tech) const
{
	switch (tech)
	{
	case 0:	{return m_bBioLocked;}
	case 1:	{return m_bEnergyLocked;}
	case 2:	{return m_bComputerLocked;}
	case 3:	{return m_bConstructionLocked;}
	case 4:	{return m_bPropulsionLocked;}
	case 5:	{return m_bWeaponLocked;}
	case 6:	{return m_bUniqueLocked;}
	}
	return 0;
}

/// Funktion setzt die einzelnen Forschungsboni.
void CResearch::SetResearchBoni(const short researchBoni[6])
{
	m_iBioTechBoni = researchBoni[0];
	m_iEnergyTechBoni = researchBoni[1];
	m_iCompTechBoni = researchBoni[2];
	m_iPropulsionTechBoni = researchBoni[3];
	m_iConstructionTechBoni = researchBoni[4];
	m_iWeaponTechBoni = researchBoni[5];
}

/// Funktion setzt die schon erforschten Forschungspunkte in einem Gebiet auf einen �bergebenen Wert.
void CResearch::SetFP(BYTE tech, UINT fp)
{
	switch (tech)
	{
	case 0:	{m_lBioFP = (float)fp;			break;}
	case 1:	{m_lEnergyFP = (float)fp;		break;}
	case 2:	{m_lComputerFP = (float)fp;		break;}
	case 3:	{m_lConstructionFP = (float)fp;	break;}
	case 4:	{m_lPropulsionFP = (float)fp;	break;}
	case 5:	{m_lWeaponFP = (float)fp;		break;}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion berechnet alles, was mit Forschung in einem Imperium zu tun hat. Diese Funktion wird in der
/// NextRound() Funktion aufgerufen. Als Parameter werden daf�r die aktuellen <code>FP</code> des Imperiums
/// �bergeben. Der R�ckgabewert dieser Funktion ist ein Zeiger auf das Attribut <code>m_strMessage</code> und
/// vom Typ CString*.
CString* CResearch::CalculateResearch(ULONG FP)
{
	// Die Boni auf die einzelnen Forschungsgebiete berechnen
	if (ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
	{
		m_iBioTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(1);
		m_iEnergyTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(1);
	}
	else if (ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
	{
		m_iCompTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(2);
		m_iPropulsionTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(2);
	}
	else if (ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
	{
		m_iConstructionTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(3);
		m_iWeaponTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(3);
	}

	for (int i = 0; i < 8; i++)
		m_strMessage[i] = "";
	m_lBioFP += (float)(FP*m_iBioPercentage)/100+(float)(FP*m_iBioPercentage)/100*m_iBioTechBoni/100;
	m_lEnergyFP += (float)(FP*m_iEnergyPercentage)/100+(float)(FP*m_iEnergyPercentage)/100*m_iEnergyTechBoni/100;
	m_lComputerFP += (float)(FP*m_iComputerPercentage)/100+(float)(FP*m_iComputerPercentage)/100*m_iCompTechBoni/100;
	m_lPropulsionFP += (float)(FP*m_iPropulsionPercentage)/100+(float)(FP*m_iPropulsionPercentage)/100*m_iPropulsionTechBoni/100;
	m_lConstructionFP += (float)(FP*m_iConstructionPercentage)/100+(float)(FP*m_iConstructionPercentage)/100*m_iConstructionTechBoni/100;
	m_lWeaponFP += (float)(FP*m_iWeaponPercentage)/100+(float)(FP*m_iWeaponPercentage)/100*m_iWeaponTechBoni/100;

	if (ResearchInfo.m_bChoiceTaken)
		m_lUniqueFP += (float)(FP*m_iUniquePercentage)/100;

	MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: ToAchieve: (each multiply per RESEARCHSPEED): Energy=125, Bio+Comp+Prop=150, Constr.+Weapon=175; if 150 and RESEARCHSPEED=1.25 than 187,50 FP needed\n");
	// Checken, ob wir eine neue Stufe erreicht haben
	CString s;
			s.Format("%lf", (float)(pow((m_lBioFP / ResearchInfo.GetBio(m_iBioTech)), 10) * 100));
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: m_lBioTechFP:%.2lf, \tm_iBioTech:%i, \tBiologyTech-DONE-Percentage: \t%i\n",
										m_lBioFP, ResearchInfo.GetBio(m_iBioTech), int(ResearchInfo.GetBio(m_iBioTech)/m_lBioFP));
			s= "";
	if (rand()%100+1 <= (float)(pow((m_lBioFP / ResearchInfo.GetBio(m_iBioTech)), 10) * 100))
//	if (m_lBioFP >= ResearchInfo.GetBio(m_iBioTech))
	{
		m_strMessage[0] = CLoc::GetString("BIO_FINISHED");
		m_iBioTech++;
		m_lBioFP = 0;
		ResearchInfo.SetTechInfos(0, m_iBioTech+1);
	}

			s.Format("%lf", (float)(pow((m_lEnergyFP / ResearchInfo.GetEnergy(m_iEnergyTech)), 10) * 100));
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: m_lEnergyFP:%.2lf, \tm_iEnergyTech:%i, \tEnergyTech-DONE-Percentage: \t%s\n", 
				m_lEnergyFP, ResearchInfo.GetEnergy(m_iEnergyTech), s);
			s= "";
	if (rand()%100+1 <= (float)(pow((m_lEnergyFP / ResearchInfo.GetEnergy(m_iEnergyTech)), 10) * 100))
//	if (m_lEnergyFP >= ResearchInfo.GetEnergy(m_iEnergyTech))
	{
		m_strMessage[1] = CLoc::GetString("ENERGY_FINISHED");
		m_iEnergyTech++;
		m_lEnergyFP = 0;
		ResearchInfo.SetTechInfos(1, m_iEnergyTech+1);
	}

			s.Format("%lf", (float)(pow((m_lComputerFP / ResearchInfo.GetComp(m_iCompTech)), 10) * 100));
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: m_lComputerFP:%.2lf, \tm_iCompTech:%i, \tComputerTech-DONE-Percentage: \t%s\n", 
				m_lComputerFP, ResearchInfo.GetComp(m_iCompTech), s);
			s= "";
	if (rand()%100+1 <= (float)(pow((m_lComputerFP / ResearchInfo.GetComp(m_iCompTech)), 10) * 100))
//	if (m_lComputerFP >= ResearchInfo.GetComp(m_iCompTech))
	{
		m_strMessage[2] = CLoc::GetString("COMPUTER_FINISHED");
		m_iCompTech++;
		m_lComputerFP = 0;
		ResearchInfo.SetTechInfos(2, m_iCompTech+1);
	}

			s.Format("%lf", (float)(pow((m_lPropulsionFP / ResearchInfo.GetPropulsion(m_iPropulsionTech)), 10) * 100));
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: m_lPropulsionFP:%.2lf, \tm_iPropulsionTech:%i,\tPropulsionTech-DONE-Percentage: \t%s\n",
				m_lPropulsionFP, ResearchInfo.GetPropulsion(m_iPropulsionTech) ,s);
			s= "";
	if (rand()%100+1 <= (float)(pow((m_lPropulsionFP / ResearchInfo.GetPropulsion(m_iPropulsionTech)), 10) * 100))
//	if (m_lPropulsionFP >= ResearchInfo.GetPropulsion(m_iPropulsionTech))
	{
		m_strMessage[3] = CLoc::GetString("PROPULSION_FINISHED");
		m_iPropulsionTech++;
		m_lPropulsionFP = 0;
		ResearchInfo.SetTechInfos(3, m_iPropulsionTech+1);
	}

			s.Format("%lf", (float)(pow((m_lConstructionFP / ResearchInfo.GetConstruction(m_iConstructionTech)), 10) * 100));
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: m_lConstrFP:%.2lf, \tm_iConstructionTech:%i, \tConstructTech-DONE-Percentage: \t%s\n",
				m_lConstructionFP, ResearchInfo.GetConstruction(m_iConstructionTech) ,s);
			s= "";
	if (rand()%100+1 <= (float)(pow((m_lConstructionFP / ResearchInfo.GetConstruction(m_iConstructionTech)), 10) * 100))
//	if (m_lConstructionFP >= ResearchInfo.GetConstruction(m_iConstructionTech))
	{
		m_strMessage[4] = CLoc::GetString("CONSTRUCTION_FINISHED");
		m_iConstructionTech++;
		m_lConstructionFP = 0;
		ResearchInfo.SetTechInfos(4, m_iConstructionTech+1);
	}

			s.Format("%lf", (float)(pow((m_lWeaponFP / ResearchInfo.GetWeapon(m_iWeaponTech)), 10) * 100));
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: m_lWeaponFP:%.2lf, \tm_iWeaponTech:%i, \tWeaponTech-DONE-Percentage: \t%s\n",
				m_lWeaponFP, ResearchInfo.GetWeapon(m_iWeaponTech) ,s);
			s= "";
	if (rand()%100+1 <= (float)(pow((m_lWeaponFP / ResearchInfo.GetWeapon(m_iWeaponTech)), 10) * 100))
//	if (m_lWeaponFP >= ResearchInfo.GetWeapon(m_iWeaponTech))
	{
		m_strMessage[5] = CLoc::GetString("WEAPON_FINISHED");
		m_iWeaponTech++;
		m_lWeaponFP = 0;
		ResearchInfo.SetTechInfos(5, m_iWeaponTech+1);
	}

		s.Format("%lf", (float)(pow((m_lUniqueFP /
		((ResearchInfo.GetBio(m_iNumberOfUnique) + ResearchInfo.GetEnergy(m_iNumberOfUnique)
		 + ResearchInfo.GetComp(m_iNumberOfUnique) + ResearchInfo.GetPropulsion(m_iNumberOfUnique)
		 + ResearchInfo.GetConstruction(m_iNumberOfUnique) + ResearchInfo.GetWeapon(m_iNumberOfUnique)) / SPECIAL_RESEARCH_DIV)), 10) * 100));
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: SpecialTech-DONE-Percentage: \t%s\n", s);
			s= "";
			MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: -----------------------------------\n");
	//	Wenn wir die Unique FP zusammenhaben, dann den Complex auf erforscht setzen und auch eine der 3 Wahlmgln.
	if (rand()%100+1 <= (float)(pow((m_lUniqueFP /
		((ResearchInfo.GetBio(m_iNumberOfUnique) + ResearchInfo.GetEnergy(m_iNumberOfUnique)
		 + ResearchInfo.GetComp(m_iNumberOfUnique) + ResearchInfo.GetPropulsion(m_iNumberOfUnique)
		 + ResearchInfo.GetConstruction(m_iNumberOfUnique) + ResearchInfo.GetWeapon(m_iNumberOfUnique)) / SPECIAL_RESEARCH_DIV)), 10) * 100))
//	if (m_lUniqueFP >= (ResearchInfo.GetBio(m_iNumberOfUnique)+ResearchInfo.GetEnergy(m_iNumberOfUnique)
//		+ResearchInfo.GetComp(m_iNumberOfUnique)+ResearchInfo.GetPropulsion(m_iNumberOfUnique)
//		+ResearchInfo.GetConstruction(m_iNumberOfUnique)+ResearchInfo.GetWeapon(m_iNumberOfUnique)))
	{
		m_lUniqueFP = 0;
		ResearchInfo.ChangeStatusOfComplex(RESEARCH_STATUS::RESEARCHED);
		// !!!! Hier die SetPercentage Fkt. aufrufen, um die Uniqueforschung Anteile wieder auf NULL zu setzen
		// k�nnten das sp�ter selbst nicht mehr machen, weil es sein kann, das wir auf die Uniqueforschung
		// nicht zugreifen k�nnen !!!!!!!!
		// Es d�rfen aber nicht alle normalen Gebiete gelocked sein, wenn ja, dann am besten alle automatisch unlocken
		if (m_bBioLocked == TRUE && m_bEnergyLocked == TRUE && m_bComputerLocked == TRUE && m_bConstructionLocked == TRUE &&
			m_bPropulsionLocked == TRUE && m_bWeaponLocked == TRUE)
		{
			m_bBioLocked = FALSE;
			m_bEnergyLocked = FALSE;
			m_bComputerLocked = FALSE;
			m_bConstructionLocked = FALSE;
			m_bPropulsionLocked = FALSE;
			m_bWeaponLocked = FALSE;
		}
		m_bUniqueLocked = FALSE;
		SetPercentage(6,0);
		m_iNumberOfUnique++;
		m_strMessage[6] = CLoc::GetString("SPECIAL_FINISHED");
		m_bUniqueReady = TRUE;

		// Wenn wir den Komplex "Forschung" erforscht haben, die Boni hier gleich berechnen, weil diese
		// sonst nicht in der MainView direkt nach Erforschen angezeigt werden
		if (ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
		{
			// Die Boni auf die einzelnen Forschungsgebiete berechnen
			if (ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
			{
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: BioBoni(old): \t%i \n", m_iBioTechBoni);
				m_iBioTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(1);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: BioBoni(new): \t%i \n", m_iBioTechBoni);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: EnergyBoni(old): \t%i \n", m_iEnergyTechBoni);
				m_iEnergyTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(1);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: EnergyBoni(new): \t%i \n", m_iEnergyTechBoni);
			}
			else if (ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
			{
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: CompBoni(old): \t%i \n", m_iCompTechBoni);
				m_iCompTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(2);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: CompBoni(new): \t%i \n", m_iCompTechBoni);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: PropBoni(old): \t%i \n", m_iPropulsionTechBoni);
				m_iPropulsionTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(2);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: PropBoni(new): \t%i \n", m_iPropulsionTechBoni);
			}
			else if (ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
			{
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: ConstrBoni(old): \t%i \n", m_iConstructionTechBoni);
				m_iConstructionTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(3);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: ConstrBoni(new): \t%i \n", m_iConstructionTechBoni);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: WeaponBoni(old): \t%i \n", m_iWeaponTechBoni);
				m_iWeaponTechBoni += ResearchInfo.GetResearchComplex(RESEARCH_COMPLEX::RESEARCH)->GetBonus(3);
				MYTRACE("logdata")(MT::LEVEL_DEBUG, "Research.cpp: WeaponBoni(new): \t%i \n", m_iWeaponTechBoni);
			}
		}
	}

	// Checken, ob wir alle Stufen auf das n�chste Level gebracht haben
	// wenn ja, dann eine neue UniqueForschung ausw�hlen
	if (m_bUniqueReady == TRUE && m_iUniqueTech < m_iBioTech && m_iUniqueTech < m_iEnergyTech && m_iUniqueTech < m_iCompTech &&
		m_iUniqueTech < m_iPropulsionTech && m_iUniqueTech < m_iConstructionTech && m_iUniqueTech < m_iWeaponTech)
	{
		if (m_iNumberOfUnique <= NoUC)
		{
			m_bUniqueReady = FALSE;
			m_iUniqueTech++;
			ResearchInfo.ChooseUniqueResearch();
			m_strMessage[7] = CLoc::GetString("SPECIAL_READY");
		}
	}
	return m_strMessage;
}

// setzt alle Werte wieder auf Standard
void CResearch::Reset()
{
	// aktuelle Forschungsstufen
	m_iBioTech = 0;				// aktuelle Biotechstufe des Imperiums
	m_iEnergyTech = 0;			// aktuelle Energietechstufe des Imperiums
	m_iCompTech = 0;			// aktuelle Computertechstufe des Imperiums
	m_iPropulsionTech = 0;		// aktuelle Antriebstechstufe des Imperiums
	m_iConstructionTech = 0;	// aktuelle Bautechstufe des Imperiums
	m_iWeaponTech = 0;			// aktuelle Waffentechstufe des Imperiums
	m_iUniqueTech = 0;			// aktuelle UniqueTechstufe des Imperiums
	// Informationen zu den Starttechnologien ermitteln
	for (int i = 0; i < 6; i++)
		this->ResearchInfo.SetTechInfos(i,0+1);
	// prozentuale Anteile auf die einzelnen Forschungen (alle Werte in Prozent)
	m_iBioPercentage = 16;
	m_iEnergyPercentage = 16;
	m_iComputerPercentage = 17;
	m_iPropulsionPercentage = 17;
	m_iConstructionPercentage = 17;
	m_iWeaponPercentage = 17;
	m_iUniquePercentage = 0;
	// aktuell erforschte FP�s in den einzelnen Forschungen
	m_lBioFP = 0;
	m_lEnergyFP = 0;
	m_lComputerFP = 0;
	m_lPropulsionFP = 0;
	m_lConstructionFP = 0;
	m_lWeaponFP = 0;
	m_lUniqueFP = 0;
	// ist die Forschung gelockt, d.h. wir k�nnen den prozentualen Anteil nicht �ndern
	m_bBioLocked = FALSE;
	m_bEnergyLocked = FALSE;
	m_bComputerLocked = FALSE;
	m_bPropulsionLocked = FALSE;
	m_bConstructionLocked = FALSE;
	m_bWeaponLocked = FALSE;
	m_bUniqueLocked = FALSE;
	// Boni auf die einzelnen Forschungsgebiete
	m_iBioTechBoni = 0;
	m_iEnergyTechBoni = 0;
	m_iCompTechBoni = 0;
	m_iPropulsionTechBoni = 0;
	m_iConstructionTechBoni = 0;
	m_iWeaponTechBoni = 0;

	m_bUniqueReady = TRUE;
	m_iNumberOfUnique = 1;

	for (int i = 0; i < 8; i++)
		m_strMessage[i] = "";

	for (int i = 0; i < NoUC; i++)
		ResearchInfo.m_ResearchComplex[i].Reset();
	ResearchInfo.m_bChoiceTaken = false;
	ResearchInfo.m_nCurrentComplex = RESEARCH_COMPLEX::NONE;
}
