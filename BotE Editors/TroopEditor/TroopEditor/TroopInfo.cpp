#include "stdafx.h"
#include "TroopInfo.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTroopInfo::CTroopInfo(void)
{
	m_strName = "";
	m_strDescription = "";
	m_strGraphicfile="ImageMissing.bop";
	m_byMaintenanceCosts = 0;
	memset(m_byNeededTechs, 0, sizeof(m_byNeededTechs));
	memset(m_iNeededResources, 0, sizeof(m_iNeededResources));
	m_iNeededIndustry = 0;
	m_byID = 0;
	m_sOwner = "";
	m_byOffense = 0;
	m_byDefense = 0;
	m_iExperiance = 0;
	m_iSize = 0;
	m_byMoralValue = 0;
}

/// Konstruktor mit kompletter Parameter�bergabe
CTroopInfo::CTroopInfo(const CString& name, const CString& desc,const CString& file, BYTE offense,BYTE defense, BYTE costs, BYTE techs[6], USHORT res[5],
					   USHORT ip, BYTE ID, const CString& sOwner, USHORT size, BYTE moralValue)
{
	m_strName = name;
	m_strDescription = desc;
	m_strGraphicfile=file;
	m_byMaintenanceCosts = costs;
	for (int i = 0; i < 6; i++)
		m_byNeededTechs[i] = techs[i];
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_iNeededResources[i] = res[i];
	m_iNeededIndustry = ip;
	m_byID = ID;
	m_sOwner = sOwner;
	m_byOffense = offense;
	m_byDefense = defense;
	m_iExperiance = 0;
	m_iSize = size;
	m_byMoralValue = moralValue;
}

CTroopInfo::~CTroopInfo(void)
{
}

// Serialisierungsfunktion

void CTroopInfo::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_strName;
		ar << m_strDescription;
		ar << m_strGraphicfile;
		ar << m_byMaintenanceCosts;
		for (int i = 0; i < 6; i++)
		ar << m_byNeededTechs[i];
		for (int i = TITAN; i <= IRIDIUM; i++)
		ar << m_iNeededResources[i];
		ar << m_iNeededIndustry;
		ar << m_byID;
		ar << m_sOwner;
		ar << m_byOffense;
		ar << m_byDefense;
		ar << m_iExperiance;
		ar << m_iSize;
		ar << m_byMoralValue;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_strName;
		ar >> m_strDescription;
		ar >> m_strGraphicfile;
		ar >> m_byMaintenanceCosts;
		for (int i = 0; i < 6; i++)
			ar >> m_byNeededTechs[i];
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar >> m_iNeededResources[i];
		ar >> m_iNeededIndustry;
		ar >> m_byID;
		ar >> m_sOwner;
		ar >> m_byOffense;
		ar >> m_byDefense;
		ar >> m_iExperiance;
		ar >> m_iSize;
		ar >> m_byMoralValue;
	}
}




/// Funktion gibt zur�ck, ob die Truppe mit der aktuellen Forschung einer Rasse baubar ist.
	/// @param researchLevels Forschungsstufen der Rasse
	/// @return Wahrheitswert
bool CTroopInfo::IsThisTroopBuildableNow(const BYTE reserachLevels[6]) const
{
	// zuerstmal die Forschungsstufen checken
	for (int i = 0; i < 6; i++)
		if (reserachLevels[i] < this->m_byNeededTechs[i])
		return false;
	return true;
}

