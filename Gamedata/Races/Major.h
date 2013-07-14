/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Race.h"
#include "RaceDesign.h"
#include "MoralObserver.h"
#include "Empire.h"
#include "Trade\Trade.h"
#include "Ships\ShipHistory.h"
#include "Ships\WeaponObserver.h"

/// Klasse zur Abstraktion einer Hauptrasse
class CMajor : public CRace
{
	friend class CMajorAI;

	DECLARE_SERIAL(CMajor)
public:
	/// Standardkonstruktur
	CMajor(void);
	/// Standarddestruktor
	virtual ~CMajor(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen

	/// Funktion gibt das RaceDesign-Objekt zur�ck. Dieses beinhaltet alle Informationen zur Schriftfarben, Schriftgr��en und
	/// Schriftarten.
	/// @return Zeiger auf das Designobjekt der Rasse
	const CRaceDesign* GetDesign(void) const {return &m_RaceDesign;}

	CMoralObserver* GetMoralObserver(void) {return &m_MoralObserver;}

	CWeaponObserver* GetWeaponObserver(void) {return &m_WeaponObserver;}

	/// Funktion gibt einen Zeiger auf das Imperiumsobjekt (Reich) der Hauptrasse zur�ck.
	/// @return Zeiger auf Imperiumsobjekt
	CEmpire* GetEmpire(void) {return &m_Empire;}

	/// Funktion gibt einen Zeiger auf das Handelsobjekt der Rasse zur�ck.
	/// @return Zeiger auf Handelsobjekt
	CTrade* GetTrade(void) {return &m_Trade;}

	/// Funktion gibt einen Zeiger auf die Schiffshistory bzw. Informationen zu Schiffen zur�ck.
	/// @return Zeiger auf Schiffshistoryobjekt
	CShipHistory* GetShipHistory(void) {return &m_ShipHistory;}

	/// Funktion gibt die Starmap der Rasse zur�ck. Diese beinhaltet Reichweiteninformationen sowie KI wichtige Sachen.
	/// @return Zeiger auf Starmap der Rasse
	CStarmap* GetStarmap(void) const {return m_pStarmap;}

	/// Funktion erstellt eine neue Starmap f�r die Rasse.
	void CreateStarmap(void);

	/// Funktion gibt den Imperiumsnamen zur�ck.
	/// @return Imperiumsnamen inkl. Artikel
	const CString& GetEmpiresName(void) const {return m_sEmpireName;}

	/// Funktion gibt den Imperiumsnamen inkl. Artikel zur�ck.
	/// @return Imperiumsnamen inkl. Artikel
	const CString& GetEmpireNameWithArticle(void) const {return m_sEmpireWithArticle;}

	/// Funktion gibt den Imperiumsnamen inkl. bestimmten Artikel zur�ck.
	/// @return Imperiumsnamen inkl. bestimmten Artikel
	const CString& GetEmpireNameWithAssignedArticle(void) const {return m_sEmpireWithAssignedArticle;}

	/// Funktion gibt den Pr�fix der Rasse zur�ck.
	/// @return Pr�fix
	const CString& GetPrefix(void) const {return m_sPrefix;}

	/// Funktion gibt an, ob die Rasse von einem menschlichen Spieler oder vom Computer gespielt wird.
	/// @return <code>true</code> wenn von einem menschlichen Spieler, ansonsten <code>false</code>
	bool IsHumanPlayer(void) const {return m_bPlayer;}

	//in addition to that this major belongs to a human player, consider whether autoturns is active
	//if yes, let the ai us play
	bool AHumanPlays() const;

	bool CanBuildShip(SHIP_TYPE::Typ type,const BYTE researchLevels[6], const CShipInfo& info) const;

	//returns the ID of the ship in the shipinfoarray, which matches the given type and which
	//this major can build according to its current tech levels
	//@return -1 if none found
	short BestBuildableVariant(SHIP_TYPE::Typ type, const CArray<CShipInfo, CShipInfo>& shipinfoarray) const;

	/// Funktion legt fest, ob die Rasse von einem menschlichen Spieler oder vom Computer gespielt wird.
	/// @param bHumanPlaner ja/nein
	void SetHumanPlayer(bool bHumanPlayer) {m_bPlayer = bHumanPlayer;}

	/// Funktion gibt den Diplomatiebonus der Rasse zur�ck.
	/// @return Diplomatiebonus
	short GetDiplomacyBonus(void) {return m_nDiplomacyBonus;}

	/// Funktion gibt die noch verbleibenden Runden des diplomatischen Vertrages zur�ck.
	/// @param sRaceID Rassen-ID der anderen Rasse
	/// @return Dauer verbleibender Runden (<code>NULL</code> bedeutet unbegrenzt)
	short GetAgreementDuration(const CString& sRaceID)
	{
		map<CString, short>::const_iterator it = m_mAgrDuration.find(sRaceID);
		if (it != m_mAgrDuration.end())
			return it->second;
		else
			return 0;
	}

	/// Funktion setzt die noch verbleibenden Runden des diplomatischen Vertrages.
	/// @param sRaceID Rassen-ID der anderen Rasse
	/// @param nNewValue neue Rundenanzahl
	void SetAgreementDuration(const CString& sRaceID, short nNewValue);

	/// Funktion legt den diplomatischen Status zu einer anderes Rasse fest.
	/// @param sOtherRace andere Rasse
	/// @param nNewAgreement neuer Vertrag
	virtual void SetAgreement(const CString& sOtherRace, DIPLOMATIC_AGREEMENT::Typ nNewAgreement);

	virtual void Contact(const CRace& Race, const CPoint& p);

	/// Funktion f�gt ein Schiff zur Liste der verlorenen Schiffe in der Schiffshistory ein.
	/// @param sEvent Ereignis warum Schiff weg/zerst�rt/verschwunden ist
	/// @param sStatus Status des Schiffes (meist zerst�rt)
	virtual void AddToLostShipHistory(const CShips& Ship, const CString& sEvent,
		const CString& sStatus, unsigned short round);

	virtual void LostFlagShip(const CShip& ship);
	virtual void LostStation(SHIP_TYPE::Typ type);
	virtual void LostShipToAnomaly(const CShips& ship, const CString& anomaly);

	/// Funktion gibt zur�ck, ob die Hauptrasse einen Verteidigungspakt mit einer anderen Hauptrasse
	/// aufrechterh�lt.
	/// @param pRaceID Rassen-ID der anderen Hauptrasse
	/// @return <code>true</code> wenn ein Verteidigungspakt besteht, sonst <code>false</code>
	bool GetDefencePact(const CString& sRaceID) const;

	/// Funktion legt fest, ob ein Verteidigungspakt parallel zu den anderen Vertr�gen besteht
	/// @param sOtherRace andere Rasse
	/// @param bIs Verteidigungspakt, ja oder nein
	virtual void SetDefencePact(const CString& sOtherRace, bool nIs);

	/// Funktion gibt die noch verbleibenden Runden eines Verteidigungspaktes zur�ck.
	/// @param sRaceID Rassen-ID der anderen Rasse
	/// @return Dauer verbleibender Runden (<code>NULL</code> bedeutet unbegrenzt)
	short GetDefencePactDuration(const CString& sRaceID)
	{
		map<CString, short>::const_iterator it = m_mDefDuration.find(sRaceID);
		if (it != m_mDefDuration.end())
			return it->second;
		else
			return 0;
	}

	/// Funktion setzt die noch verbleibenden Runden des Verteidigungspaktes.
	/// @param sRaceID Rassen-ID der anderen Rasse
	/// @param nNewValue neue Rundenanzahl
	void SetDefencePactDuration(const CString& sRaceID, short nNewValue);

	// Funktionen
	/// Funktion verringert die Anzahl der noch verbleibenden Runden der laufenden Vertr�ge um eins.
	/// Au�er der Vertrag l�uft auf unbestimmte Zeit.
	/// @param pmMajors Zeiger auf Map mit allen Majors im Spiel
	bool DecrementAgreementsDuration(map<CString, CMajor*>* pmMajors);

	/// Funktion zum erstellen einer Rasse.
	/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
	/// @param saInfo Referenz auf Rasseninformationen
	/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
	virtual void Create(const CStringArray& saInfo, int& nPos);

	/// Funktion zum zur�cksetzen aller Werte auf Ausgangswerte.
	virtual void Reset(void);

protected:
	// Attribute
	bool				m_bPlayer;						///< wird die Rasse von einem menschlichen Spieler gespielt?

	CString				m_sEmpireName;					///<!!! l�ngerer Imperiumsname
	CString				m_sEmpireWithArticle;			///<!!! Artikel f�r Imperiumsnamen
	CString				m_sEmpireWithAssignedArticle;	///<!!! bestimmter Artikel f�r den Imperiumsnamen
	CString				m_sPrefix;						///<!!! Rassenprefix

	short				m_nDiplomacyBonus;				///<!!! Bonus bei diplomatischen Verhandlungen, NULL == kein Bonus/kein Malus

	map<CString, short>	m_mAgrDuration;					///< noch verbleibende Runden des Vertrags (NULL == unbegrenzt)
	vector<CString>		m_vDefencePact;					///< besitzt die Majorrace eines Verteidigungspakt mit einer anderen Majorrace (Rassen-ID)
	map<CString, short>	m_mDefDuration;					///< Dauer des Verteidigungspaktes, einzeln speichern, weil er separat zu anderen Vertr�gen abgeschlossen werden kann.

	CRaceDesign			m_RaceDesign;					///!!!< Objekt welches gestalterische Informationen zur Rasse beinhaltet

	// gr��ere Objekte
	CEmpire				m_Empire;						///< das Imperium (Reich) der Hauptrasse
	CTrade				m_Trade;						///< der B�rsenhandel f�r diese Rasse
	CShipHistory		m_ShipHistory;					///< alle statistischen Daten aller Schiffe sind hier zu finden

	// Observer-Objekte
	CMoralObserver		m_MoralObserver;				///< In den MoralObserver werden alle Events geschrieben, die die Moral der Majorrace irgendwie beeinflussen
	CWeaponObserver		m_WeaponObserver;				///< beobachtet die baubaren Waffen f�r Schiffe. Wird ben�tigt wenn wir Schiffe designen wollen

	// Starmap
	CStarmap*			m_pStarmap;						///< Die Starmap des Majors, beinhaltet Reichweiteninformationen (muss nicht serialisiert werden)
};
