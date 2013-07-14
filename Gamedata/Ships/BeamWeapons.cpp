// BeamWeapons.cpp: Implementierung der Klasse CBeamWeapons.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "BeamWeapons.h"
#include "Constants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CBeamWeapons, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CBeamWeapons::CBeamWeapons() :
	//TODO diese Initialisierungen zu -1 sind vermutlich nicht sinnvoll
	m_iBeamPower(-1),
	m_iBeamType(-1),
	m_iBeamNumber(-1),
	m_byShootNumber(-1),
	m_byBonus(-1),
	m_byBeamLenght(-1),
	m_byRechargeTime(-1),
	m_bPiercing(FALSE),
	m_bModulating(FALSE)
{

}

CBeamWeapons::~CBeamWeapons()
{

}
//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CBeamWeapons::CBeamWeapons(const CBeamWeapons & rhs) :
	m_Firearc(rhs.m_Firearc),
	m_strBeamName(rhs.m_strBeamName)
{
	m_iBeamPower	= rhs.m_iBeamPower;
	m_iBeamType		= rhs.m_iBeamType;
	m_iBeamNumber	= rhs.m_iBeamNumber;
	m_byShootNumber = rhs.m_byShootNumber;
	m_byBonus		= rhs.m_byBonus;
	m_byBeamLenght	= rhs.m_byBeamLenght;
	m_byRechargeTime= rhs.m_byRechargeTime;
	m_bPiercing	= rhs.m_bPiercing;
	m_bModulating = rhs.m_bModulating;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CBeamWeapons & CBeamWeapons::operator=(const CBeamWeapons & rhs)
{
	if (this == &rhs)
		return *this;
	m_strBeamName	= rhs.m_strBeamName;
	m_iBeamPower	= rhs.m_iBeamPower;
	m_iBeamType		= rhs.m_iBeamType;
	m_iBeamNumber	= rhs.m_iBeamNumber;
	m_byShootNumber = rhs.m_byShootNumber;
	m_byBonus		= rhs.m_byBonus;
	m_byBeamLenght	= rhs.m_byBeamLenght;
	m_byRechargeTime= rhs.m_byRechargeTime;
	m_bPiercing	= rhs.m_bPiercing;
	m_bModulating = rhs.m_bModulating;
	m_Firearc		= rhs.m_Firearc;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CBeamWeapons::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	m_Firearc.Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_strBeamName;
		MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "BEAMWEAPONS.CPP: %s: Damage:%d, Type:%d, NrAtShip:%d, byShoot:%d, Bonus:%d, BeamTime:%d, RechargeTime:%d, Piercing:%s, Modulation:%s\n", 
			m_strBeamName, m_iBeamPower, m_iBeamType, m_iBeamNumber, m_byShootNumber, m_byBonus, m_byBeamLenght, m_byRechargeTime, 
			m_bPiercing ? "yes" : "no",
			m_bModulating ? "yes" : "no");
		//BEAMWEAPONS.CPP: Laser Array: Power:7, Type:8, Nr:3, byShoot:1, Bonus:30, Lenght:8, RechargeTime:12
		ar << m_iBeamPower;
		ar << m_iBeamType;
		ar << m_iBeamNumber;
		ar << m_byShootNumber;
		ar << m_byBonus;
		ar << m_byBeamLenght;
		ar << m_byRechargeTime;
		ar << m_bPiercing;
		ar << m_bModulating;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_strBeamName;
		ar >> m_iBeamPower;
		ar >> m_iBeamType;
		ar >> m_iBeamNumber;
		ar >> m_byShootNumber;
		ar >> m_byBonus;
		ar >> m_byBeamLenght;
		ar >> m_byRechargeTime;
		ar >> m_bPiercing;
		ar >> m_bModulating;
	}
}

// Funktion modifiziert diese Beamwaffe hier
void CBeamWeapons::ModifyBeamWeapon(BYTE BeamType, USHORT BeamPower, BYTE BeamNumber, const CString& BeamName, BOOLEAN Modulating,
									BOOLEAN Piercing, BYTE Bonus, BYTE BeamLenght, BYTE RechargeTime, BYTE ShootNumber)
{
	m_strBeamName	= BeamName;
	m_iBeamPower	= BeamPower;
	m_iBeamType		= BeamType;
	m_iBeamNumber	= BeamNumber;
	m_byShootNumber = ShootNumber;
	m_byBonus		= Bonus;
	m_byBeamLenght  = BeamLenght;
	m_byRechargeTime= RechargeTime;
	m_bPiercing	= Piercing;
	m_bModulating = Modulating;

}
