// Building.cpp: Implementierung der Klasse CBuilding.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Building.h"

IMPLEMENT_SERIAL (CBuilding, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CBuilding::CBuilding() :
	m_iRunningNumber(0),
	m_bOnline(FALSE)
{
}

CBuilding::CBuilding(USHORT id) :
	m_iRunningNumber(id),
	m_bOnline(FALSE)
{
}

CBuilding::~CBuilding()
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CBuilding::CBuilding(const CBuilding & rhs)
{
	m_iRunningNumber = rhs.m_iRunningNumber;
	m_bOnline = rhs.m_bOnline;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CBuilding & CBuilding::operator=(const CBuilding & rhs)
{
	if (this == &rhs)
		return *this;

	m_iRunningNumber = rhs.m_iRunningNumber;
	m_bOnline = rhs.m_bOnline;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CBuilding::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iRunningNumber;
		// Ist das Geb�ude online?
		ar << m_bOnline;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iRunningNumber;
		// Ist das Geb�ude online?
		ar >> m_bOnline;
	}
}
