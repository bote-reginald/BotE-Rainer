/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// ShipInfo.h: Schnittstelle f�r die Klasse CShipInfo.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIPINFO_H__7830A54F_2D37_48B4_8B2E_C26482F39413__INCLUDED_)
#define AFX_SHIPINFO_H__7830A54F_2D37_48B4_8B2E_C26482F39413__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ship.h"

// forward declaration
class CResearch;

class CShipInfo : public CShip
{
public:
	DECLARE_SERIAL (CShipInfo)
// Konstruktion & Destruktion
	CShipInfo();
	virtual ~CShipInfo();

	// Kopierkonstruktor
	CShipInfo(const CShipInfo & rhs);
	// Zuweisungsoperatur
	CShipInfo & operator=(const CShipInfo &);
	// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	/// Funktion gibt eine Rassennummer zur�ck. Jede Rasse besitzt eine Nummer, welche Schiffe und Geb�ude diese
	/// bauen kann.
	/// @return Rassennummer (nicht Rassen-ID!)
	BYTE GetRace() const {return m_iRace;}
	BYTE GetBioTech() const {return m_iBioTech;}
	BYTE GetEnergyTech() const {return m_iEnergyTech;}
	BYTE GetComputerTech() const {return m_iCompTech;}
	BYTE GetPropulsionTech() const {return m_iPropulsionTech;}
	BYTE GetConstructionTech() const {return m_iConstructionTech;}
	BYTE GetWeaponTech() const {return m_iWeaponTech;}
	UINT GetNeededIndustry() const {return m_iNeededIndustry;}
	UINT GetBaseIndustry() const {return m_iBaseIndustry;}
	USHORT GetNeededTitan() const {return m_iNeededTitan;}
	USHORT GetNeededDeuterium() const {return m_iNeededDeuterium;}
	USHORT GetNeededDuranium() const {return m_iNeededDuranium;}
	USHORT GetNeededCrystal() const {return m_iNeededCrystal;}
	USHORT GetNeededIridium() const {return m_iNeededIridium;}
	USHORT GetNeededDeritium() const {return m_iNeededDeritium;}
	USHORT GetNeededResource(BYTE res) const;
	const CString& GetOnlyInSystem() const {return m_strOnlyInSystem;}
	const CString& GetObsoleteShipClass() const {return m_strObsoletesClass;}

	void SetRace(BYTE Race) {m_iRace = Race;}
	void SetBioTech(BYTE BioTech) {m_iBioTech = BioTech;}
	void SetEnergyTech(BYTE EnergyTech) {m_iEnergyTech = EnergyTech;}
	void SetComputerTech(BYTE CompTech) {m_iCompTech = CompTech;}
	void SetPropulsionTech(BYTE PropulsionTech) {m_iPropulsionTech = PropulsionTech;}
	void SetConstructionTech(BYTE ConstructionTech) {m_iConstructionTech = ConstructionTech;}
	void SetWeaponTech(BYTE WeaponTech) {m_iWeaponTech = WeaponTech;}
	// Hier werden auch gleich die Basiskosten mitgesetzt
	void SetNeededIndustry(int NeededIndustry) {m_iNeededIndustry = NeededIndustry; m_iBaseIndustry = NeededIndustry;}
	void SetNeededTitan(int NeededTitan) {m_iNeededTitan = NeededTitan; m_iBaseTitan = NeededTitan;}
	void SetNeededDeuterium(int NeededDeuterium) {m_iNeededDeuterium = NeededDeuterium; m_iBaseDeuterium = NeededDeuterium;}
	void SetNeededDuranium(int NeededDuranium) {m_iNeededDuranium = NeededDuranium; m_iBaseDuranium = NeededDuranium;}
	void SetNeededCrystal(int NeededCrystal) {m_iNeededCrystal = NeededCrystal; m_iBaseCrystal = NeededCrystal;}
	void SetNeededIridium(int NeededIridium) {m_iNeededIridium = NeededIridium; m_iBaseIridium = NeededIridium;}
	void SetNeededDeritium(int NeededDeritium) {m_iNeededDeritium = NeededDeritium; m_iBaseDeritium = NeededDeritium;}
	void SetOnlyInSystem(const CString& name) {m_strOnlyInSystem = name;}
	void SetObsoleteShipClass(const CString& className) {m_strObsoletesClass = className;}

	void DeleteWeapons();							// Funktion l�scht die Bewaffnung
	void CalculateFinalCosts();						// Funktion berechnet die finalen Kosten zum Bau des Schiffes
	void CalculateMaintenanceCosts();				// calculates MaintenanceCosts of Ships
	void SetStartOrder();							// Funktion bestimmt die 1. Order des Schiffs nach dem Bau anhand dessen Typs
	void SetStartTactic();							// Funktion bestimmt die 1. Taktik des Schiffs nach dem Bau anhand dessen Typs
	void DrawShipInformation(Graphics* g, CRect rect, Gdiplus::Font* font, Gdiplus::Color clrNormal, Gdiplus::Color clrMark, CResearch* research);	// Funktion zeichnet wichtige Informationen zu dem Schiff

	/// Funktion gibt zur�ck, ob das Schiff mit der aktuellen Forschung einer Rasse baubar ist.
	/// @param researchLevels Forschungsstufen der Rasse
	/// @return Wahrheitswert
	bool IsThisShipBuildableNow(const BYTE reserachLevels[6]) const;

private:
	BYTE m_iRace;					// welche Rasse kann das Schiff bauen
	// n�tige Forschung
	BYTE m_iBioTech;
	BYTE m_iEnergyTech;
	BYTE m_iCompTech;
	BYTE m_iPropulsionTech;
	BYTE m_iConstructionTech;
	BYTE m_iWeaponTech;
	// aktuell n�tige Rohstoffe zum Bau des Schiffes
	UINT m_iNeededIndustry;
	USHORT m_iNeededTitan;
	USHORT m_iNeededDeuterium;
	USHORT m_iNeededDuranium;
	USHORT m_iNeededCrystal;
	USHORT m_iNeededIridium;
	USHORT m_iNeededDeritium;
	// n�tige Rohstoffe zum Bau, so wie die Grundkosten sind, denn durch CalculateFinalCosts werden die n�tigen Rohstoffe
	// und Industrieleistung erh�ht. Beim n�chsten Aufruf brauchen wir aber wieder die urspr�nglichen Kosten
	UINT m_iBaseIndustry;
	USHORT m_iBaseTitan;
	USHORT m_iBaseDeuterium;
	USHORT m_iBaseDuranium;
	USHORT m_iBaseCrystal;
	USHORT m_iBaseIridium;
	USHORT m_iBaseDeritium;
	// n�tiger Systemname
	CString m_strOnlyInSystem;
	// Schiffsklasse, welche durch diese Schiffsklasse ersetzt wird
	CString m_strObsoletesClass;
};

typedef CArray<CShipInfo,CShipInfo> ShipInfoArray;


#endif // !defined(AFX_SHIPINFO_H__7830A54F_2D37_48B4_8B2E_C26482F39413__INCLUDED_)
