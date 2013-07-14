// ResearchInfo.cpp: Implementierung der Klasse CResearchInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResearchInfo.h"
#include "IOData.h"
#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL (CResearchInfo, CObject, 1)

// statische Variablen initialisieren
double CResearchInfo::m_dResearchSpeedFactor = 1.0;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CResearchInfo::CResearchInfo()
{
	m_nCurrentComplex = RESEARCH_COMPLEX::NONE;
	m_bChoiceTaken = false;
	for (int i = 0; i < NoUC; i++)
		m_ResearchComplex[i].Reset();
	for (int i = 0; i < 6; i++)
	{
		m_strTechName[i] = "";
		m_strTechDescription[i] = "";
	}
}

CResearchInfo::~CResearchInfo()
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CResearchInfo::CResearchInfo(const CResearchInfo & rhs)
{
	m_bChoiceTaken = rhs.m_bChoiceTaken;
	m_nCurrentComplex = rhs.m_nCurrentComplex;
	for (int i = 0; i < NoUC; i++)
		m_ResearchComplex[i] = rhs.m_ResearchComplex[i];
	for (int i = 0; i < 6; i++)
	{
		m_strTechName[i] = rhs.m_strTechName[i];
		m_strTechDescription[i] = rhs.m_strTechDescription[i];
	}
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CResearchInfo & CResearchInfo::operator=(const CResearchInfo & rhs)
{
	if (this == &rhs)
		return *this;
	m_bChoiceTaken = rhs.m_bChoiceTaken;
	m_nCurrentComplex = rhs.m_nCurrentComplex;
	for (int i = 0; i < NoUC; i++)
		m_ResearchComplex[i] = rhs.m_ResearchComplex[i];
	for (int i = 0; i < 6; i++)
	{
		m_strTechName[i] = rhs.m_strTechName[i];
		m_strTechDescription[i] = rhs.m_strTechDescription[i];
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CResearchInfo::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// Namen und Techbeschreibungen werden beim Laden neu eingelesen. Dies wird in der Research-Klasse gemacht.
	for (int i = 0; i < NoUC; i++)
		m_ResearchComplex[i].Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_nCurrentComplex;
		ar << m_bChoiceTaken;
		//MYTRACE("logsave")(MT::LEVEL_DEBUG, "ResearchINFO.CPP: m_nCurrentComplex: %s, m_bChoiceTaken: %b\n", m_nCurrentComplex, m_bChoiceTaken);
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int nComplex;
		ar >> nComplex;
		m_nCurrentComplex = (RESEARCH_COMPLEX::Typ)nComplex;
		ar >> m_bChoiceTaken;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion w�hlt zuf�llig ein Unique-Themengebiet aus den noch nicht erforschten Komplexen aus.
/// Vor Aufruf der Funktion sollte �berpr�ft werden, dass nicht schon alle Komplexe erforscht wurden, da
/// es sonst zum Absturz des Programms kommen k�nnte.
void CResearchInfo::ChooseUniqueResearch(void)
{
	// Vektor mit noch erforschbaren Spezialforschunen erstellen
	std::vector<int> vResearchableComplexes;
	for (int i = 0; i < NoUC; i++) {
		if (m_ResearchComplex[i].m_nComplexStatus == RESEARCH_STATUS::NOTRESEARCHED)
			vResearchableComplexes.push_back(i);
	}

	if (vResearchableComplexes.empty())
		return;

	// zuf�llig eine Spezialforschung aus den noch zur Verf�gung stehenden ausw�hlen
	unsigned choose_this = rand()%vResearchableComplexes.size();
	RESEARCH_COMPLEX::Typ nComplex = static_cast<RESEARCH_COMPLEX::Typ>(vResearchableComplexes.at(choose_this));

	// Status �ndern
	m_ResearchComplex[nComplex].m_nComplexStatus = RESEARCH_STATUS::RESEARCHING;
	m_nCurrentComplex = nComplex;
	m_bChoiceTaken = false;

	// Wenn wir einen Komplex ausgew�hlt haben, diesen erst generieren
	m_ResearchComplex[nComplex].GenerateComplex(nComplex);
}

/// Diese Funktion �ndert den Status des aktuellen Komplexes. Dabei �ndert sie gleichzeitig auch den Status
/// der zuvor gew�hlten Wahlm�glichkeit. Als Parameter wird dabei ein neuer Status <code>nNewStatus</code>
/// �bergeben.
void CResearchInfo::ChangeStatusOfComplex(RESEARCH_STATUS::Typ nNewStatus)
{
	if (m_nCurrentComplex == RESEARCH_COMPLEX::NONE)
	{
		ASSERT(m_nCurrentComplex != RESEARCH_COMPLEX::NONE);
		return;
	}

	m_ResearchComplex[m_nCurrentComplex].m_nComplexStatus = nNewStatus;
	// Wenn der neue Status RESEARCHED ist
	if (nNewStatus == RESEARCH_STATUS::RESEARCHED)
	{
		// Alle drei m�glichen Gebiete durchgehen
		for (int i = 0; i < 3; i++)
		{
			// Wenn gerade das Gebiet i erforscht wird
			if (m_ResearchComplex[m_nCurrentComplex].m_nFieldStatus[i] == RESEARCH_STATUS::RESEARCHING)
			{
				// alle anderen Gebiete auf "nicht erforscht" setzen
				memset(m_ResearchComplex[m_nCurrentComplex].m_nFieldStatus, RESEARCH_STATUS::NOTRESEARCHED, sizeof(m_ResearchComplex[m_nCurrentComplex].m_nFieldStatus));
				// gerade erforschtes Gebiet auf "erforscht" setzen
				m_ResearchComplex[m_nCurrentComplex].m_nFieldStatus[i] = RESEARCH_STATUS::RESEARCHED;
				break;
			}
		}

		// kein Komplex mehr ausgew�hlt
		m_nCurrentComplex = RESEARCH_COMPLEX::NONE;
	}
}

/// Diese Funktion w�hlt eine der drei M�glichkeiten der Uniqueforschung aus. Daf�r muss man das Gebiet, welches
/// erforscht werden soll mit dem Parameter <code>possibility</code> �bergeben. Genaueres steht in der Definition
/// dieser Funktion.
void CResearchInfo::SetUniqueResearchChoosePossibility(BYTE possibility)
{
	/*
		f�r die 1. Wahlm�glichkeit	-> possibility == 1
		f�r die 2. Wahlm�glichkeit	-> possibility == 2
		f�r die 3. Wahlm�glichkeit	-> possibility == 3
	*/
	if (m_nCurrentComplex != RESEARCH_COMPLEX::NONE && !m_bChoiceTaken)
	{
		// alle Gebiete erstmal auf nicht erforscht setzen
		memset(m_ResearchComplex[m_nCurrentComplex].m_nFieldStatus, RESEARCH_STATUS::NOTRESEARCHED, sizeof(m_ResearchComplex[m_nCurrentComplex].m_nFieldStatus));
		// gew�hltes Gebiet auf "wird erforscht" setzen
		m_ResearchComplex[m_nCurrentComplex].m_nFieldStatus[possibility-1] = RESEARCH_STATUS::RESEARCHING;
		m_bChoiceTaken = true;
	}
}

/// Diese Funktion ermittelt den Namen und die Beschreibung einer bestimmten Technologie, an der gerade geforscht
/// wird. Dies wird in den Attributen <code>m_strTechName</code> und <code>m_strTechDescription</code> gespeichert.
/// Als Parameter m�ssen daf�r die jeweilige Technologie <code>tech</code> und die Stufe <code>level</code>, die
/// aktuell erforscht wird �bergeben werden.
void CResearchInfo::SetTechInfos(BYTE tech, BYTE level)
{
	m_strTechName[tech] = "Future Tech";
	m_strTechDescription[tech] = "-";

	CResearchInfo::GetTechInfos(tech, level, m_strTechName[tech], m_strTechDescription[tech]);
}

/// Diese Funktion ermittelt den Namen und die Beschreibung einer bestimmten Technologie
/// Dies wird in den Parametern <code>m_sTechName</code> und <code>m_sTechDesc</code> gespeichert.
/// Als Parameter m�ssen daf�r die jeweilige Technologie <code>tech</code> und die Stufe <code>level</code>
/// �bergeben werden.
void CResearchInfo::GetTechInfos(BYTE tech, BYTE level, CString& sTechName, CString& sTechDesc)
{
	CString csInput;											// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Names\\Techs.data";		// Name des zu �ffnenden Files
	CStdioFile file;											// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))	// Datei wird ge�ffnet
	{
		int i = 0;
		int j = level * 12 + tech * 2;
		while (file.ReadString(csInput))
		{
			if (i == j)
				sTechName = csInput;
			else if (i-1 == j)
			{
				sTechDesc = csInput;
				break;
			}
			i++;
		}
	}
	else
	{
		MYTRACE("general")(MT::LEVEL_ERROR, "Could not open file \"Techs.data\"...\n");
		AfxMessageBox("ERROR! Could not open file \"Techs.data\"...\n(Maybe check your installation directory...)");
	}
	// Datei wird geschlossen
	file.Close();
}

ULONG CResearchInfo::GetBio( USHORT nTechLevel ) const
{
	return (ULONG)(pow(2.25f, nTechLevel) * 150.0 * m_dResearchSpeedFactor);
}

ULONG CResearchInfo::GetEnergy( USHORT nTechLevel ) const
{
	return (ULONG)(pow(2.25f, nTechLevel) * 125.0 * m_dResearchSpeedFactor);
}

ULONG CResearchInfo::GetComp( USHORT nTechLevel ) const
{
	return (ULONG)(pow(2.25f, nTechLevel) * 150.0 * m_dResearchSpeedFactor);
}

ULONG CResearchInfo::GetPropulsion( USHORT nTechLevel ) const
{
	return (ULONG)(pow(2.25f, nTechLevel) * 150.0 * m_dResearchSpeedFactor);
}

ULONG CResearchInfo::GetConstruction( USHORT nTechLevel ) const
{
	return (ULONG)(pow(2.25f, nTechLevel) * 175.0 * m_dResearchSpeedFactor);
}

ULONG CResearchInfo::GetWeapon( USHORT nTechLevel ) const
{
	return (ULONG)(pow(2.25f, nTechLevel) * 175.0 * m_dResearchSpeedFactor);
}
