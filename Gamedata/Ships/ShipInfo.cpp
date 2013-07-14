// ShipInfo.cpp: Implementierung der Klasse CShipInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShipInfo.h"
#include "Ship.h"
#include "Races\Empire.h"
#include "General/Loc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CShipInfo, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShipInfo::CShipInfo() :
	m_iRace(0),
	m_iBioTech(0),
	m_iEnergyTech(0),
	m_iCompTech(0),
	m_iPropulsionTech(0),
	m_iConstructionTech(0),
	m_iWeaponTech(0),
	m_iNeededIndustry(0),
	m_iNeededTitan(0),
	m_iNeededDeuterium(0),
	m_iNeededDuranium(0),
	m_iNeededCrystal(0),
	m_iNeededIridium(0),
	m_iNeededDeritium(0),
	m_iBaseIndustry(0),
	m_iBaseTitan(0),
	m_iBaseDeuterium(0),
	m_iBaseDuranium(0),
	m_iBaseCrystal(0),
	m_iBaseIridium(0),
	m_iBaseDeritium(0)
{
}

CShipInfo::~CShipInfo()
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CShipInfo::CShipInfo(const CShipInfo & rhs) : CShip(rhs)
{
	m_iRace = rhs.m_iRace;
	// n�tige Forschung
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;
	// aktuell n�tige Rohstoffe zum Bau des Schiffes
	m_iNeededIndustry = rhs.m_iNeededIndustry;
	m_iNeededTitan = rhs.m_iNeededTitan;
	m_iNeededDeuterium = rhs.m_iNeededDeuterium;
	m_iNeededDuranium = rhs.m_iNeededDuranium;
	m_iNeededCrystal = rhs.m_iNeededCrystal;
	m_iNeededIridium = rhs.m_iNeededIridium;
	m_iNeededDeritium = rhs.m_iNeededDeritium;
	// n�tige Rohstoffe zum Bau, so wie die Grundkosten sind, denn durch CalculateFinalCosts werden die n�tigen Rohstoffe
	// und Industrieleistung erh�ht. Beim n�chsten Aufruf brauchen wir aber wieder die urspr�nglichen Kosten
	m_iBaseIndustry = rhs.m_iBaseIndustry;
	m_iBaseTitan = rhs.m_iBaseTitan;
	m_iBaseDeuterium = rhs.m_iBaseDeuterium;
	m_iBaseDuranium = rhs.m_iBaseDuranium;
	m_iBaseCrystal = rhs.m_iBaseCrystal;
	m_iBaseIridium = rhs.m_iBaseIridium;
	m_iBaseDeritium = rhs.m_iBaseDeritium;
	// n�tiger Systemname
	m_strOnlyInSystem = rhs.m_strOnlyInSystem;
	// Schiffsklasse, welche durch diese Schiffsklasse ersetzt wird
	m_strObsoletesClass = rhs.m_strObsoletesClass;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CShipInfo & CShipInfo::operator=(const CShipInfo & rhs)
{
	if (this == &rhs)
		return *this;

	// Basisklasse
	__super::operator=(rhs);

	m_iRace = rhs.m_iRace;
	// n�tige Forschung
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;
	// aktuell n�tige Rohstoffe zum Bau des Schiffes
	m_iNeededIndustry = rhs.m_iNeededIndustry;
	m_iNeededTitan = rhs.m_iNeededTitan;
	m_iNeededDeuterium = rhs.m_iNeededDeuterium;
	m_iNeededDuranium = rhs.m_iNeededDuranium;
	m_iNeededCrystal = rhs.m_iNeededCrystal;
	m_iNeededIridium = rhs.m_iNeededIridium;
	m_iNeededDeritium = rhs.m_iNeededDeritium;
	// n�tige Rohstoffe zum Bau, so wie die Grundkosten sind, denn durch CalculateFinalCosts werden die n�tigen Rohstoffe
	// und Industrieleistung erh�ht. Beim n�chsten Aufruf brauchen wir aber wieder die urspr�nglichen Kosten
	m_iBaseIndustry = rhs.m_iBaseIndustry;
	m_iBaseTitan = rhs.m_iBaseTitan;
	m_iBaseDeuterium = rhs.m_iBaseDeuterium;
	m_iBaseDuranium = rhs.m_iBaseDuranium;
	m_iBaseCrystal = rhs.m_iBaseCrystal;
	m_iBaseIridium = rhs.m_iBaseIridium;
	m_iBaseDeritium = rhs.m_iBaseDeritium;
	// n�tiger Systemname
	m_strOnlyInSystem = rhs.m_strOnlyInSystem;
	// Schiffsklasse, welche durch diese Schiffsklasse ersetzt wird
	m_strObsoletesClass = rhs.m_strObsoletesClass;

	return *this;
}


///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CShipInfo::Serialize(CArchive &ar)
{
	__super::Serialize(ar);
	
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		//MYTRACE("logsave")(MT::LEVEL_INFO, "SHIPINFO.CPP: -------------------------------------");
		ar << m_iRace;
		MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: m_iRace: %d, m_strOnlyInSystem: %s, m_strObsoletesClass: %s, Techs: B:%d, E:%d, Comp:%d, P:%d, C:%d, W:%d \n", m_iRace, m_strOnlyInSystem, m_strObsoletesClass, m_iBioTech, m_iEnergyTech, m_iCompTech, m_iPropulsionTech, m_iConstructionTech, m_iWeaponTech);
		ar << m_iBioTech;
		ar << m_iEnergyTech;
		ar << m_iCompTech;
		ar << m_iPropulsionTech;
		ar << m_iConstructionTech;
		ar << m_iWeaponTech;
		// (ok, see above) MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: Techs: B:%d, E:%d, Comp:%d, P:%d, C:%d, W:%d \n", m_iBioTech, m_iEnergyTech, m_iCompTech, m_iPropulsionTech, m_iConstructionTech, m_iWeaponTech);
		ar << m_iNeededIndustry;
		ar << m_iNeededTitan;
		ar << m_iNeededDeuterium;
		ar << m_iNeededDuranium;
		ar << m_iNeededCrystal;
		ar << m_iNeededIridium;
		ar << m_iNeededDeritium;
		MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: Needed: Ind:%d, T:%d, Deu:%d, Du:%d, C:%d, I:%d, Deritium:%d \n", m_iNeededIndustry, m_iNeededTitan, m_iNeededDeuterium, m_iNeededDuranium, m_iNeededCrystal, m_iNeededIridium, m_iNeededDeritium);
		ar << m_iBaseIndustry;
		ar << m_iBaseTitan;
		ar << m_iBaseDeuterium;
		ar << m_iBaseDuranium;
		ar << m_iBaseCrystal;
		ar << m_iBaseIridium;
		ar << m_iBaseDeritium;
		MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: Base: Ind:%d, T:%d, Deu:%d, Du:%d, C:%d, I:%d, Deritium:%d \n", m_iNeededIndustry, m_iNeededTitan, m_iNeededDeuterium, m_iNeededDuranium, m_iNeededCrystal, m_iNeededIridium, m_iNeededDeritium);
		ar << m_strOnlyInSystem;
		ar << m_strObsoletesClass;
		MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: -------------------------------------");
	}
	
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iRace;
		ar >> m_iBioTech;
		ar >> m_iEnergyTech;
		ar >> m_iCompTech;
		ar >> m_iPropulsionTech;
		ar >> m_iConstructionTech;
		ar >> m_iWeaponTech;
		ar >> m_iNeededIndustry;
		ar >> m_iNeededTitan;
		ar >> m_iNeededDeuterium;
		ar >> m_iNeededDuranium;
		ar >> m_iNeededCrystal;
		ar >> m_iNeededIridium;
		ar >> m_iNeededDeritium;
		ar >> m_iBaseIndustry;
		ar >> m_iBaseTitan;
		ar >> m_iBaseDeuterium;
		ar >> m_iBaseDuranium;
		ar >> m_iBaseCrystal;
		ar >> m_iBaseIridium;
		ar >> m_iBaseDeritium;
		ar >> m_strOnlyInSystem;
		ar >> m_strObsoletesClass;
	}
}

// Funktion l�scht die Bewaffnung
void CShipInfo::DeleteWeapons()
{
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); )
		m_TorpedoWeapons.RemoveAt(i);
	for (int i = 0; i < m_BeamWeapons.GetSize(); )
		m_BeamWeapons.RemoveAt(i);
	m_TorpedoWeapons.RemoveAll();
	m_BeamWeapons.RemoveAll();
}

// Funktion berechnet die finalen Kosten zum Bau des Schiffes. Diese mu� immer aufgerufen werden, wenn wir was an
// dem Schiffsobjekt ge�ndert haben.
void CShipInfo::CalculateFinalCosts()
{
	m_iNeededIndustry	= 0;
	m_iNeededTitan		= 0;
	m_iNeededDeuterium	= 0;
	m_iNeededDuranium	= 0;
	m_iNeededCrystal	= 0;
	m_iNeededIridium	= 0;
	m_iNeededDeritium	= 0;
	// Industrieleistung, die wir zus�tzlich durch bessere Schilde erbringen m�ssen
/*	m_iNeededIndustry += m_Shield.GetMaxShield() / 10 * m_Shield.GetShieldType();
	// Industrieleistung, die wir zus�tzlich durch bessere H�lle  erbringen m�ssen
	// besseres Material erh�ht die Industrieleistung nicht, sondern nur ob wir eine Doppelh�lle haben
	m_iNeededIndustry += (USHORT)(m_Hull.GetBaseHull() * (m_Hull.GetDoubleHull()));
	// Industrieleistung die wir zus�tzlich erbringen m�ssen, wenn wir besserer Beams dranbauen
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		m_iNeededIndustry += (USHORT)(10 * (m_BeamWeapons.GetAt(i).GetBeamPower() * m_BeamWeapons.GetAt(i).GetBeamType()));
	// Industrieleistung die wir zus�tzlich erbringen m�ssen, wenn wir besserer Torpedos dranbauen
//	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
//		m_iNeededIndustry += (USHORT)((m_TorpedoWeapons.GetAt(i).GetTorpedoPower() * m_TorpedoWeapons.GetAt(i).GetNumber())
//							 / m_TorpedoWeapons.GetAt(i).GetTupeFirerate() * m_TorpedoWeapons.GetAt(i).GetNumberOfTupes() * 50);
*/
	// Ab hier neue Kostenberechnung. Es geht mehr der Wert aller 100s ein. Also nicht mehr der absolute St�rkewert.
	// Es wird nur ein gewisser Anteil durch den Beamtype mit aufgerechnet, da dieser sonst keinen Einfluss auf den
	// Beamschaden hat, nur auf die Reichweite der Beams.
	USHORT beamTypeAdd = 0;
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		beamTypeAdd += (USHORT)(GetBeamWeapons()->GetAt(i).GetBeamPower()
								* GetBeamWeapons()->GetAt(i).GetBeamNumber()
								* m_BeamWeapons.GetAt(i).GetBeamType()
								* 3);
	if (m_BeamWeapons.GetSize() > 1)
		beamTypeAdd		/= m_BeamWeapons.GetSize();
	MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: m_iNeededIndustry=%i\n", m_iNeededIndustry);
	m_iNeededIndustry	+= beamTypeAdd;
	MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: m_iNeededIndustryAfterBeam=%i (beamTypeAdd:%i)\n", m_iNeededIndustry, beamTypeAdd);
	m_iNeededIndustry	+= this->GetCompleteOffensivePower();
	MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: m_iNeededIndustryWithGetCompleteOffensivePower=%i,(CompleteOffensivePower=%i)\n", m_iNeededIndustry, this->GetCompleteOffensivePower());
	// Eine Doppelh�lle verteuert die Industriekosten nochmals um 25%
	if (m_Hull.GetDoubleHull())
		m_iNeededIndustry	+= (UINT)(this->GetCompleteDefensivePower() / 1.5);
	else
		m_iNeededIndustry	+= this->GetCompleteDefensivePower() / 2;
	MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: m_iNeededIndustry+dblHull=%i,(CompleteDefensivePower=%i / 1,5, with NoDblHull /2)\n", m_iNeededIndustry, this->GetCompleteDefensivePower());

	m_iNeededIndustry	+= GetShield()->GetMaxShield() / 200 * ((USHORT)pow((float)GetShield()->GetShieldType(), 2.5f));
	MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: m_iNeededIndustry+Shields=%i,(MaxShields/200*SType, 2.5f), MaxShield=%i, pow=%i\n", m_iNeededIndustry, GetShield()->GetMaxShield(), (USHORT)pow((float)GetShield()->GetShieldType(), 2.5f));
	m_iNeededIndustry	/= 2;
	MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "##### RESULT #####: SHIPINFO.CPP: m_iNeededIndustry/2=%i #####################\n", m_iNeededIndustry);
	MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "SHIPINFO.CPP: -----------------------------\n");

	// Kosten erstmal wieder auf die Ausgangswerte setzen
	m_iNeededIndustry	+= m_iBaseIndustry;
	m_iNeededTitan		+= m_iBaseTitan;
	m_iNeededDeuterium	+= m_iBaseDeuterium;
	m_iNeededDuranium	+= m_iBaseDuranium;
	m_iNeededCrystal	+= m_iBaseCrystal;
	m_iNeededIridium	+= m_iBaseIridium;
	m_iNeededDeritium	+= m_iBaseDeritium;

	// zus�tzliche Rohstoffe die wir f�r bessere Anbauten/Umbauten ben�tigen
	// wir brauchen f�r bessere H�llen auch besseres Material
	switch (m_Hull.GetHullMaterial())
	{
	case TITAN: m_iNeededTitan += (USHORT)(m_Hull.GetBaseHull() * (m_Hull.GetDoubleHull()+1)); break;
	case DURANIUM: m_iNeededDuranium += (USHORT)(m_Hull.GetBaseHull() * (m_Hull.GetDoubleHull()+1)); break;
	case IRIDIUM: m_iNeededIridium += (USHORT)(m_Hull.GetBaseHull() * (m_Hull.GetDoubleHull()+1)); break;
	}
	// f�r bessere Torpedos (ab 500 Dmg und mehr) brauchen wir zus�tzliches Deuterium
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
	{
		if (m_TorpedoWeapons.GetAt(i).GetTorpedoPower() >= 500)
			m_iNeededDeuterium += m_TorpedoWeapons.GetAt(i).GetTorpedoPower() * m_TorpedoWeapons.GetAt(i).GetNumber()
			* m_TorpedoWeapons.GetAt(i).GetNumberOfTupes();
	}
}

// Funktion bestimmt die 1. Order des Schiffs nach dem Bau anhand dessen Typs
void CShipInfo::SetStartOrder()
{
	SetCurrentOrderAccordingToType();
}

void CShipInfo::SetStartTactic()
{
	SetCombatTacticAccordingToType();
}

// Funktion zeichnet wichtige Informationen zu dem Schiff
void CShipInfo::DrawShipInformation(Graphics* g, CRect rect, Gdiplus::Font* font, Gdiplus::Color clrNormal, Gdiplus::Color clrMark, CResearch* research)
{
	CRect r;
	CString s;
	r.SetRect(rect.left,rect.top+8,rect.right,rect.top+33);
	s.Format("%s",this->GetShipTypeAsString());

	SolidBrush fontBrush(clrMark);
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);

	// Gr��e, Unterhalt und Reichweite anzeigen
	r.SetRect(rect.left,rect.top+36,rect.right,rect.top+56);
	CString speed = CLoc::GetString("SPEED");
	CString range = CLoc::GetString("RANGE");
	SHIP_RANGE::Typ tmpRange = m_iRange;
	BYTE tmpSpeed = m_iSpeed;
	if (research->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
	{
		if (m_iRange == SHIP_RANGE::SHORT)
			tmpRange = (SHIP_RANGE::Typ)(research->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(1));
	}
	else if (research->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
	{
		if (m_iSpeed == 1)
			tmpSpeed = (BYTE)(research->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(2));
	}

	if (rect.Width() >= 501)
	{
		CString Size = CLoc::GetString("SHIPSIZE");
		CString sMaintenance = CLoc::GetString("SHIPCOSTS");
		switch (tmpRange)
		{
		case SHIP_RANGE::SHORT: s.Format("%s: %d  -  %s: %d  -  %s: %d  -  %s: %s",Size, m_nShipSize, sMaintenance, m_iMaintenanceCosts, speed, tmpSpeed, range, CLoc::GetString("SHORT"));break;
		case SHIP_RANGE::MIDDLE: s.Format("%s: %d  -  %s: %d  -  %s: %d  -  %s: %s",Size, m_nShipSize, sMaintenance, m_iMaintenanceCosts, speed, tmpSpeed, range, CLoc::GetString("MIDDLE")); break;
		case SHIP_RANGE::LONG: s.Format("%s: %d  -  %s: %d  -  %s: %d  -  %s: %s",Size, m_nShipSize, sMaintenance, m_iMaintenanceCosts, speed, tmpSpeed, range, CLoc::GetString("LONG")); break;
		}
	}
	else
	{
		switch (tmpRange)
		{
		case SHIP_RANGE::SHORT: s.Format("%s: %d  -  %s: %s",speed, tmpSpeed, range, CLoc::GetString("SHORT"));break;
		case SHIP_RANGE::MIDDLE: s.Format("%s: %d  -  %s: %s",speed, tmpSpeed, range, CLoc::GetString("MIDDLE")); break;
		case SHIP_RANGE::LONG: s.Format("%s: %d  -  %s: %s",speed, tmpSpeed, range, CLoc::GetString("LONG")); break;
		}
	}

	fontBrush.SetColor(clrNormal);
	g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	fontBrush.SetColor(clrMark);

	r.SetRect(rect.left,rect.top+62,rect.right,rect.top+84);
	s = CLoc::GetString("ARMAMENT");
	g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	// Waffen typenrein sammeln
	std::map<CString, int> mBeamWeapons;
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
	{
		s.Format("%s %d %s", CLoc::GetString("TYPE"), m_BeamWeapons.GetAt(i).GetBeamType(),m_BeamWeapons.GetAt(i).GetBeamName());
		mBeamWeapons[s] += m_BeamWeapons.GetAt(i).GetBeamNumber();
	}

	std::map<CString, int> mTorpedoWeapons;
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
	{
		s.Format("%s (%s)", m_TorpedoWeapons.GetAt(i).GetTupeName(), m_TorpedoWeapons.GetAt(i).GetTorpedoName());
		mTorpedoWeapons[s] += m_TorpedoWeapons.GetAt(i).GetNumberOfTupes();
	}

	// Waffenbeschreibungstext zusammenbauen
	CString sWeapons = "";
	for (map<CString, int>::const_iterator it = mBeamWeapons.begin(); it != mBeamWeapons.end(); ++it)
	{
		if (sWeapons != "")
			sWeapons += ", ";

		s.Format("%d x %s", it->second, it->first);
		sWeapons += s;
	}
	for (map<CString, int>::const_iterator it = mTorpedoWeapons.begin(); it != mTorpedoWeapons.end(); ++it)
	{
		if (sWeapons != "")
			sWeapons += ", ";

		s.Format("%d x %s", it->second, it->first);
		sWeapons += s;
	}

	if (sWeapons == "")
		sWeapons = CLoc::GetString("NONE");
	r.SetRect(rect.left,rect.top+84,rect.right,rect.top+143);
	fontBrush.SetColor(clrNormal);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(CComBSTR(sWeapons), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);

	// Wenn das Rechteck, welches wir an diese Funktion �bergeben haben breiter als 500 ist, dann wird der Abstand
	// zwischen Bewaffnung und "Schilde und H�lle" um eine Zeile verk�rzt
	USHORT sub = 0;
	if (rect.Width() > 500)
		sub = 20;
	r.SetRect(rect.left,rect.top+143-sub,rect.right,rect.top+165-sub);
	s = CLoc::GetString("SHIELDS")+" "+CLoc::GetString("AND")+" "+CLoc::GetString("HULL");
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontBrush.SetColor(clrMark);
	g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);

	r.SetRect(rect.left,rect.top+165-sub,rect.right,rect.top+185-sub);
	float shieldBoni = 1.0f;
	if (research->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		shieldBoni += (float)(research->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetBonus(1)) / 100;

	s.Format("%s %d %s: %s %d",CLoc::GetString("TYPE"),m_Shield.GetShieldType(),
		CLoc::GetString("SHIELDS"), CLoc::GetString("CAPACITY"), (UINT)(m_Shield.GetMaxShield() * shieldBoni));
	fontBrush.SetColor(clrNormal);
	g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	r.SetRect(rect.left,rect.top+185-sub,rect.right,rect.top+225-sub);
	CString material;
	switch (m_Hull.GetHullMaterial())
	{
	case TITAN: material = CLoc::GetString("TITAN");; break;
	case DURANIUM: material = CLoc::GetString("DURANIUM");; break;
	case IRIDIUM: material = CLoc::GetString("IRIDIUM");; break;
	default: material = "";
	}
	float hullBoni = 1.0f;
	if (research->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		hullBoni += (float)(research->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetBonus(2)) / 100;

	if (m_Hull.GetDoubleHull() == TRUE)
		s.Format("%s%s: %s %d",material, CLoc::GetString("DOUBLE_HULL_ARMOUR"),
			CLoc::GetString("INTEGRITY"), (int)(m_Hull.GetMaxHull()*hullBoni));
	else
		s.Format("%s%s: %s %d",material, CLoc::GetString("HULL_ARMOR"),
			CLoc::GetString("INTEGRITY"), (int)(m_Hull.GetMaxHull()*hullBoni));
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);

	if (rect.Width() >= 501)
	{
		// Man�vrierbarkeit anzeigen
		fontBrush.SetColor(clrMark);
		r.SetRect(rect.left,rect.top+266,rect.right,rect.top+288);
		g->DrawString(CComBSTR(CLoc::GetString("MANEUVERABILITY")), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
		switch (m_byManeuverability)
		{
		case 9: s = CLoc::GetString("PHENOMENAL");	break;
		case 8:	s = CLoc::GetString("EXCELLENT");	break;
		case 7:	s = CLoc::GetString("VERYGOOD");	break;
		case 6:	s = CLoc::GetString("GOOD");		break;
		case 5:	s = CLoc::GetString("NORMAL");		break;
		case 4:	s = CLoc::GetString("ADEQUATE");	break;
		case 3:	s = CLoc::GetString("BAD");			break;
		case 2:	s = CLoc::GetString("VERYBAD");		break;
		case 1:	s = CLoc::GetString("MISERABLE");	break;
		default:s = CLoc::GetString("NONE");
		}
		fontBrush.SetColor(clrNormal);
		r.SetRect(rect.left,rect.top+288,rect.right,rect.top+308);
		g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);

		// Scan anzeigen
		fontBrush.SetColor(clrMark);
		r.SetRect(rect.left,rect.top+311,rect.right,rect.top+333);
		g->DrawString(CComBSTR(CLoc::GetString("SENSORS")), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
		r.SetRect(rect.left,rect.top+333,rect.right,rect.top+353);
		CString Scanrange = CLoc::GetString("SCANRANGE");
		CString Scanpower = CLoc::GetString("SCANPOWER");
		BYTE ScanRange = m_iScanRange;
		USHORT ScanPower = m_iScanPower;
		s.Format("%s: %d  -  %s: %d",Scanrange, ScanRange, Scanpower, ScanPower);
		fontBrush.SetColor(clrNormal);
		g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
		fontBrush.SetColor(clrMark);

		// Spezialf�higkeiten anzeigen
		fontBrush.SetColor(clrMark);
		r.SetRect(rect.left,rect.top+356,rect.right,rect.top+378);
		g->DrawString(CComBSTR(CLoc::GetString("SPECIAL_ABILITIES")), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
		s = "";
		fontBrush.SetColor(clrNormal);
		if (HasSpecial(SHIP_SPECIAL::ASSULTSHIP))
			s += CLoc::GetString("ASSAULTSHIP")+" \n";
		if (HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
			s += CLoc::GetString("BLOCKADESHIP")+" \n";
		if (HasSpecial(SHIP_SPECIAL::COMMANDSHIP))
			s += CLoc::GetString("COMMANDSHIP")+" \n";
		if (HasSpecial(SHIP_SPECIAL::DOGFIGHTER))
			s += CLoc::GetString("DOGFIGHTER")+" \n";
		if (HasSpecial(SHIP_SPECIAL::DOGKILLER))
			s += CLoc::GetString("DOGKILLER")+" \n";
		if (HasSpecial(SHIP_SPECIAL::PATROLSHIP))
			s += CLoc::GetString("PATROLSHIP")+" \n";
		if (HasSpecial(SHIP_SPECIAL::RAIDER))
			s += CLoc::GetString("RAIDER")+" \n";
		if (HasSpecial(SHIP_SPECIAL::SCIENCEVESSEL))
			s += CLoc::GetString("SCIENCESHIP")+" \n";
		if (m_Shield.GetRegenerative())
			s += CLoc::GetString("REGENERATIVE_SHIELDS")+" \n";
		if (m_Hull.GetAblative())
			s += CLoc::GetString("ABLATIVE_ARMOR")+" \n";
		if (m_Hull.GetPolarisation())
			s += CLoc::GetString("HULLPOLARISATION")+" \n";
		if (CanCloak())
			s += CLoc::GetString("CAN_CLOAK")+" \n";
		if (s.IsEmpty())
			s = CLoc::GetString("NONE")+" \n";

		CString strStorageRoom;
		strStorageRoom.Format ("%i", m_iStorageRoom);
		MYTRACE("logging")(MT::LEVEL_INFO, "ShipInfo..Shipdesign: m_iStorageRoom: %i (Display only if > 1501)\n", strStorageRoom);
		if (m_iStorageRoom > 1501) {
			CString s_add;
			s_add = " \n" + CLoc::GetString("SHIP_STORAGE") + ": " + strStorageRoom + " \n";
			s += s_add;;
			}

		r.SetRect(rect.left,rect.top+378,rect.right,rect.top+478);
		fontBrush.SetColor(clrNormal);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		g->DrawString(CComBSTR(s), -1, font, RectF((REAL)r.left, (REAL)r.top, (REAL)r.Width(), (REAL)r.Height()), &fontFormat, &fontBrush);
	}
}

/// Funktion gibt zur�ck, ob das Schiff mit der aktuellen Forschung einer Rasse baubar ist.
/// @param researchLevels Forschungsstufen der Rasse
/// @return Wahrheitswert
bool CShipInfo::IsThisShipBuildableNow(const BYTE reserachLevels[6]) const
{
	// zuerstmal die Forschungsstufen checken
	if (reserachLevels[5] < this->GetWeaponTech())
		return false;
	if (reserachLevels[4] < this->GetConstructionTech())
		return false;
	if (reserachLevels[3] < this->GetPropulsionTech())
		return false;
	if (reserachLevels[2] < this->GetComputerTech())
		return false;
	if (reserachLevels[1] < this->GetEnergyTech())
		return false;
	if (reserachLevels[0] < this->GetBioTech())
		return false;

	return true;
}

USHORT CShipInfo::GetNeededResource(BYTE res) const
{
	switch (res)
	{
	case TITAN:		return this->GetNeededTitan();
	case DEUTERIUM: return this->GetNeededDeuterium();
	case DURANIUM:	return this->GetNeededDuranium();
	case CRYSTAL:	return this->GetNeededCrystal();
	case IRIDIUM:	return this->GetNeededIridium();
	case DERITIUM:	return this->GetNeededDeritium();
	default:		return NULL;
	}
}
