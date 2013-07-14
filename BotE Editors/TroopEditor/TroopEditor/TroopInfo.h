/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "troop.h"

class CTroopInfo :	public CTroop
{
public:
	/// Konstruktor
	CTroopInfo(void);

	/// Konstruktor mit kompletter Parameter�bergabe
	CTroopInfo(const CString& name, const CString& desc,const CString& file, BYTE offense,BYTE defense, BYTE costs, BYTE techs[6], USHORT res[5], USHORT ip,
		BYTE ID, const CString& sOwner, USHORT size, BYTE moralValue);
	
	/// Destruktor
	~CTroopInfo(void);
	
	// Serialisierungsfunktion; n�tig f�r CBotf2Doc::SerializeBeginGameData
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen zum Lesen der Membervariablen
	/**
	 * Diese Funktion gibt den Namen der Truppe zur�ck.
	 */
	const CString GetName() const {return m_strName;}

	const CString GetName2() const {return m_strName2;}


	/**
	 * Dieser Funktion gibt die Beschreibung der Truppe zur�ck.
	 */
	const CString GetDescription() const {return m_strDescription;}

	const CString GetDescription2() const {return m_strDescription2;}

	const CString GetGraphicfile() const {return m_strGraphicfile;}

	/**
	 * Diese Funktion gibt die Unterhaltskosten der Truppe zur�ck.
	 */
	BYTE GetMaintenanceCosts() const {return m_byMaintenanceCosts;}

	/**
	 * Diese Funktion gibt einen Zeiger auf die ben�tigten Techlevel dieser Einheit zur�ck.
	 */
	BYTE* GetNeededTechlevels() {return m_byNeededTechs;}

	/**
	 * Diese Funktion gibt ein ben�tigtes Techlevel zur�ck, welches man mit dem Paramter <code>tech</code>
	 * erfragt hat.
	 */
	BYTE GetNeededTechlevel(BYTE tech) const {return m_byNeededTechs[tech];}

	/**
	 * Diese Funktion gibt einen Zeiger auf die ben�tigten Ressourcen dieser Truppe zur�ck
	 */
	USHORT* GetNeededResources() {return m_iNeededResources;}

	USHORT GetNeededRessource(BYTE res) {return m_iNeededResources[res];}

	/**
	 * Diese Funktion gibt die ben�tigte Industrie zum Bau dieser Einheit zur�ck.
	 */
	USHORT GetNeededIndustry() const {return m_iNeededIndustry;}

	/**
	 * Diese Funktion gibt die Truppengr��e, somit den ben�tigten Platz bei einem Transport zur�ck.
	 */
	USHORT GetSize() const {return m_iSize;}

	/**
	 * Diese Funktion gibt den Moralwert dieser Truppe zur�ck.
	 */
	BYTE GetMoralValue() const {return m_byMoralValue;}
	
	// Zugriffsfunktionen zum Schreiben der Membervariablen
	/**
	 * Diese Funktion setzt den Namen der Einheit.
	 */
	void SetName(const CString& name) {m_strName = name;}

	void SetName2(const CString& name) {m_strName2 = name;}

	void SetOwner(const CString& name) {m_sOwner= name;}

	/**
	 * Diese Funktion setzt die Beschreibung der Truppe.
	 */
	void SetDescription(const CString& desc) {m_strDescription = desc;}

	void SetDescription2(const CString& desc) {m_strDescription2 = desc;}

	/**
	 * Diese Funktion setzt die zum Bau ben�tigte Industrie der Truppe.
	 */
	void SetNeededIndustry(USHORT ip) {m_iNeededIndustry = ip;}

	void SetID(BYTE nID) {m_byID=nID;}

	void SetGraphicfile(const CString& file) {m_strGraphicfile=file;}

	void SetSize(USHORT nSize) {m_iSize=nSize;}

	void SetMoralValue(BYTE nMoralValue) {m_byMoralValue=nMoralValue;}

	void SetMaintenanceCosts(BYTE nMC) {m_byMaintenanceCosts=nMC;}

	void SetNeededRessource(BYTE res,USHORT nNeeded) {m_iNeededResources[res]=nNeeded;}

	void SetNeededTechlevel(BYTE tech,BYTE nNeeded) {m_byNeededTechs[tech]=nNeeded;}

	/// Funktion gibt zur�ck, ob die Truppe mit der aktuellen Forschung einer Rasse baubar ist.
	/// @param researchLevels Forschungsstufen der Rasse
	/// @return Wahrheitswert
	bool IsThisTroopBuildableNow(const BYTE reserachLevels[6]) const;

	bool operator< (const CTroopInfo& elem2) const { return m_byID < elem2.GetID();}
	bool operator> (const CTroopInfo& elem2) const { return m_byID > elem2.GetID();}

private:
	/// Der Name der Einheit
	CString m_strName;

	/// Die Beschreibung der Einheit
	CString m_strDescription;

	CString m_strName2;

	CString m_strDescription2;

	CString m_strGraphicfile;

	/// Diese Variable beinhaltet die Unterhaltskosten der Einheit
	BYTE m_byMaintenanceCosts;

	/// Dieses Feld beinhaltet die ben�tigten Forschungslevel zum Bau dieser Einheit
	BYTE m_byNeededTechs[6];

	/// Dieses Feld beinhaltet die ben�tigten Ressourcen zum Bau dieser Einheit
	USHORT m_iNeededResources[5];

	/// Die ben�tigte Industrie um eine Einheit fertigzustellen
	USHORT m_iNeededIndustry;

	/// Die Anzahl der Soldaten bzw. der ben�tigte Platz bei einem Transport f�r diese Einheit
	USHORT m_iSize;

	/// Der Moralwert der Truppe. Um dieses Wert wird die Moral bei unter 100% im System erh�ht bzw.
	/// bei �ber 100% in dem System verringert
	BYTE m_byMoralValue;
};
