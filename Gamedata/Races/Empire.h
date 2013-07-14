/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "General\EmpireNews.h"
#include "Research.h"
#include "Intel\Intelligence.h"
#include "System\GlobalStorage.h"
#include "Events\EventColonization.h"
#include "Events\EventBlockade.h"
#include "Events\EventBombardment.h"
#include "Events\EventResearch.h"
#include "Events\EventFirstContact.h"
#include "Events\EventGameOver.h"
#include "Events\EventRaceKilled.h"
#include "Events\EventVictory.h"
#include "Events\EventAlienEntity.h"
#include "Events\EventRandom.h"

/**
 * Struktur, die die wichtigsten Informationen eines Systems aufnehmen kann. Dies wird ben�tigt, wenn wir in einer
 * View eine Liste der Systeme aufnehmen m�chten. Ich m�chte da nur die wichtigsten Infos haben
 */
struct EMPIRE_SYSTEMS
{
	bool operator< (const EMPIRE_SYSTEMS& elem2) const { return name < elem2.name;}
	bool operator> (const EMPIRE_SYSTEMS& elem2) const { return name > elem2.name;}

	EMPIRE_SYSTEMS& operator=(const EMPIRE_SYSTEMS &other) {name = other.name; ko = other.ko; return *this;}
	EMPIRE_SYSTEMS(const CString& _name, CPoint _ko) : name(_name), ko(_ko) {};
	EMPIRE_SYSTEMS() : name(""), ko(CPoint(-1,-1)) {};

	CString name;
	CPoint ko;
};

class CSystem;
class CSector;
class CEmpire : public CObject
{
public:
	DECLARE_SERIAL (CEmpire)

	/// Konstruktor
	CEmpire();

	/// Destruktor
	~CEmpire();

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/// Funktion gibt die Anzahl der Systeme des Imperiums zur�ck.
	USHORT CountSystems() const {return m_nNumberOfSystems;}

	/// Funktion gibt einen Zeiger auf die Liste der zum Imperium geh�renden Systeme zur�ck.
	CArray<EMPIRE_SYSTEMS>* GetSystemList() {return &m_SystemList;}

	/// Funktion gibt die Nummer des Imperiums zur�ck.
	const CString& GetEmpireID() const {return m_sEmpireID;}

	/// Funktion gibt den aktuellen Bestand an Credits zur�ck.
	long GetCredits() const {return m_iCredits;}

	/// Funktion gibt die gesamte Credits�nderung seit der letzten Runde zur�ck.
	long GetCreditsChange() const {return m_iCreditsChange;}

	/// Funktion gibt die Schiffsunterst�tzungskosten zur�ck.
	UINT GetShipCosts() const {return m_iShipCosts;}

	/// Funktion gibt die Schiffunterst�tzungkosten durch die Bev�lkerung zur�ck.
	UINT GetPopSupportCosts() const {return m_iPopSupportCosts;}

	/// Funktion gibt die aktuell produzierten Forschungspunkte zur�ck.
	UINT GetFP() const {return m_lFP;}

	/// Funktion gibt die aktuell produtierten Geheimdienstpunkte zur�ck.
	UINT GetSP() const {return m_Intelligence.GetSecurityPoints();}

	/// Funktion gibt ein Feld mit der Menge aller Ressourcen auf allen Systemen des Imperiums zur�ck.
	const UINT* GetStorage() const {return m_lResourceStorages;}

	/// Funktion gibt einen Zeiger auf alle Nachrichten an das Imperium zur�ck.
	EmpiresNews* GetMsgs() {return &m_vMessages;}

	/// Funktion gibt einen Zeiger auf das Feld mit den Eventnachrichten f�r das Imperium zur�ck.
	/// @return Pointer auf <code>CObArray</code>
	CObArray* GetEvents() {return &m_Events;}

	/// Funktion gibt einen Zeiger auf das Forschungsobjekt des Imperiums zur�ck.
	CResearch* GetResearch(void) {return &m_Research;}
	const CResearch* GetResearch(void) const {return &m_Research;}

	/// Funktion gibt einen Zeiger auf das Geheimdienstobjekt des Imperiums zur�ck.
	CIntelligence* GetIntelligence(void) {return &m_Intelligence;}

	/// Funktion liefert einen Zeiger auf das globale Lager eines Imperiums.
	CGlobalStorage* GetGlobalStorage(void) {return &m_GlobalStorage;}

	// zum Schreiben der Membervariablen
	/// Funktion addiert die im Parameter <code>add</code> �bergebene Menge zu der Anzahl der Systeme
	/// des Imperiums.
	/// @param n Anzahl der Systene
	void SetNumberOfSystems(USHORT n) {m_nNumberOfSystems = n;}

	/// Funktion legt die zugeh�rige Rassennummer des Imperiums fest.
	/// @param empireNumber Rassennummer
	void SetEmpireID(const CString& sRaceID) {m_sEmpireID = sRaceID; m_Intelligence.SetRaceID(sRaceID);}

	/// Funktion addiert die �bergebene Menge zum Creditsbestand.
	/// @param add Anzahl des zu addierenden Creditss
	void SetCredits(int add) {m_iCredits += add;}

	/// Funktion legt die Credits�nderung zur vorherigen Runde fest.
	/// @param change �nderung des Creditss gegn�ber der vorherigen Runde.
	void SetCreditsChange(int change) {m_iCreditsChange = change;}

	/// Funktion setzt die Schiffsunterst�tzungskosten auf den Wert von <code>costs</code>.
	void SetShipCosts(UINT costs) {m_iShipCosts = costs;}

	/// Funktion addiert die Schiffsunterst�tzungskosten, �bergeben durch den Parameter <code>add</code>, zu den
	/// aktuellen Schiffsunterst�tzungskosten des Imperiums.
	void AddShipCosts(UINT add) {m_iShipCosts += add;}

	/// Funktion setzt die Bev�lkerungsunterst�tzungskosten auf den Wert von <code>supportCosts</code>.
	void SetPopSupportCosts(UINT supportCosts) {m_iPopSupportCosts = supportCosts;}

	/// Funktion addiert die Bev�lkerungsunterst�tzungskosten, �bergeben durch den Parameter <code>add</code>, zu den
	/// aktuellen Bev�lkerungsunterst�tzungskosten des Imperiums.
	void AddPopSupportCosts(UINT add) {m_iPopSupportCosts += add;}

	/// Funktion addiert die �bergebene Anzahl an Forschungspunkten zu den Forschungspunkten des Imperiums.
	/// @param add Anzahl der zu addierenden Forschungspunkte
	void AddFP(int add);

	/// Funktion addiert die �bergebene Anzahl an Geheimdienstpunkten zu den Geheimdienstpunkten des Imperiums.
	/// @param add Anzahl der zu addierenden Geheimdienstpunkte
	void AddSP(UINT add) {m_Intelligence.AddSecurityPoints(add);}

	/// Funktion addiert die �bergebene Menge an Ressourcen zum zugeh�rigen Ressourcenlager.
	/// @param res Ressource
	/// @param add zu addierende Menge
	void SetStorageAdd(USHORT res, UINT add) {m_lResourceStorages[res] += add;}

	/// Funktion f�gt eine �bergebene Nachricht dem Nachrichtenfeld des Imperiums hinzu.
	/// @param message Nachricht
	void AddMsg(const CEmpireNews &msg) {m_vMessages.Add(msg);}

	// Sonstige Funktionen
	/// Resetfunktion f�r das CEmpire-Objekt.
	void Reset(void);

	/// Funktion generiert die Liste der Systeme f�r das Imperium anhand aller Systeme.
	/// @param systems Feld aller Systeme
	void GenerateSystemList(const std::vector<CSystem>& systems, const std::vector<CSector>& sectors);

	/// Funktion setzt die Lager und aktuell prodzuierten Forschungs- und Geheimdienstpunkte wieder auf NULL
	/// damit man diese sp�ter wieder f�llen kann. Die wirklichen Lager und Punkte in den einzelnen Systemen
	/// werden dabei nicht anger�hrt
	void ClearAllPoints(void);

	/// Funktion l�scht alle Nachrichten und Antworten an das Imperiums.
	void ClearMessagesAndEvents(void);

private:
	long m_iCredits;				///< Credits des Imperiums

	long m_iCreditsChange;			///< Gewinn bzw Creditsverlust zur letzten Runde

	UINT m_iShipCosts;				///< die Schiffsunterst�tzungskosten des Imperiums

	UINT m_iPopSupportCosts;		///< Unterst�tzungskosten aufgrund der Bev�lkerung

	UINT m_lFP;						///< aktuelle FP des Imperiums

	UINT m_lResourceStorages[DERITIUM+1];	///< die gesamte Menge auf allen Systemen der jeweiligen Ressource

	CString m_sEmpireID;			///< gibt die ID der Rasse des Imperiums zur�ck

	USHORT m_nNumberOfSystems;		///< Anzahl Systeme des Imperiums

	EmpiresNews m_vMessages;		///< alle Nachrichten an das Imperium

	CObArray m_Events;				///< alle Events f�r das Imperium

	CResearch m_Research;			///< die Forschung des Imperiums

	CIntelligence m_Intelligence;	///< der Geheimdienst des Imperiums

	CGlobalStorage m_GlobalStorage;	///< das globale Lager des Imperiums

	CArray<EMPIRE_SYSTEMS> m_SystemList;	///< Zeiger auf die zum Imperium geh�renden Systeme
};
