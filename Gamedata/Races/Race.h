/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "afx.h"
#include <map>
#include <vector>
#include <set>
#include "Constants.h"
#include "AI\DiplomacyAI.h"

class CShips;
class CShip;

using namespace std;

enum RaceProberties
{
	RACE_AGRARIAN				= 1,
	RACE_FINANCIAL				= 2,
	RACE_HOSTILE				= 4,
	RACE_INDUSTRIAL				= 8,
	RACE_PACIFIST				= 16,
	RACE_PRODUCER				= 32,
	RACE_SCIENTIFIC				= 64,
	RACE_SECRET					= 128,
	RACE_SNEAKY					= 256,
	RACE_SOLOING				= 512,
	RACE_WARLIKE				= 1024
};

enum RaceSpecialAbilities
{
	SPECIAL_NEED_NO_FOOD		= 1,	///< Rasse braucht keine Nahrung
	SPECIAL_NO_DIPLOMACY		= 2,	///< Rasse kann keine Diplomatie (kein First Contact)
	SPECIAL_ALIEN_DIPLOMACY		= 4		///< nur Alien-Diplomatie m�glich (NAP, Freundschaft und Krieg)
};

/// Basisklasse f�r alle Rassen.
class CRace : public CObject
{
public:

	DECLARE_SERIAL(CRace)

	enum RACE_TYPE {
		RACE_TYPE_MAJOR				=	0,	// Hauptrasse
		RACE_TYPE_MINOR				=	1	// kleine Rasse (keine Ausbreitung)
	};

	/// Standardkonstruktor
	CRace(void);
	/// Standarddestruktor
	virtual ~CRace(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);
	// Vergleichsopertoren
	bool operator< (const CRace& race) const { return m_sName < race.m_sName;}
	bool operator> (const CRace& race) const { return m_sName > race.m_sName;}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funktionen
	/// Funktion zum Erfragen der Rasseneigenschaften
	/// @param nProp Rasseneigenschaft
	/// @return <code>true</code>, wenn die Rasse die Eigenschaft besitzt, sonst <code>false</code>
	bool IsRaceProperty(RACE_PROPERTY::Typ nProp) const;

	/// Funktion gibt zur�ck, ob eine andere Rasse bekannt ist.
	/// @param sRaceID ID der anderen Rasse
	/// @return <code>true</code> wenn Kontakt zu der anderen Rasse besteht, ansonsten <code>false</code>
	bool IsRaceContacted(const CString& sRaceID) const;

	/// Funktion gibt die ID der Rasse zur�ck
	/// @return Rassen-ID
	const CString& GetRaceID(void) const {return m_sID;}

	/// Funktion gibt den Namen des Heimatsystems zur�ck
	/// @return Name des Heimatsystems
	const CString& GetHomesystemName(void) const {return m_sHomeSystem;}

	/// Funktion gibt den Rassennamen zur�ck
	/// @return Rassenname
	const CString& GetRaceName(void) const {return m_sName;}

	/// Funktion gibt die Rassenbeschreibung zur�ck
	/// @return Rassenbeschreibung
	const CString& GetRaceDesc(void) const {return m_sDesc;}

	/// Funktion gibt den Rassennamen inkl. vorangestelltem Artikel zur�ck
	/// @return Rassenname inkl Artikel
	CString GetRaceNameWithArticle(void) const {return m_sNameArticle + " " + m_sName;}

	/// Funktion gibt den Rassentyp zur�ck (MAJOR, MINOR).
	/// @return Rassentyp
	RACE_TYPE GetType(void) const {return m_RaceType;}
	bool IsMajor() const {
		return m_RaceType == RACE_TYPE_MAJOR;
	}
	bool IsMinor() const {
		return m_RaceType == RACE_TYPE_MINOR;
	}

	/// Funktion gibt zur�ck, ob es sich um eine Alienrasse (kein Heimatsystem) handelt
	/// @return <code>true</code> wenn es eine Alienrasse ist, sonst <code>false</code>
	bool IsAlienRace() const { return m_sHomeSystem == ""; }

	/// Funktion gibt die Nummer zur�ck, welche auf bestimmte Schiffe gemappt werden kann.
	/// @return Rassennummer
	BYTE GetRaceShipNumber(void) const {return m_byShipNumber;}

	/// Funktion gibt die Nummer zur�ck, welche auf bestimmte Geb�ude gemappt werden kann.
	/// @return Rassennummer
	BYTE GetRaceBuildingNumber(void) const {return m_byBuildingNumber;}

	/// Funktion gibt die Nummer zur�ck, welche auf bestimmte Moralwerte gemappt werden kann.
	/// @return Rassennummer
	BYTE GetRaceMoralNumber(void) const {return m_byMoralNumber;}

	/// Funktion gibt Beziehungswert zu einer anderes Rasse zur�ck.
	/// @param sOtherRace Beziehung zu der anderen Rasse
	/// @return Beziehungswert (0-100)
	BYTE GetRelation(const CString& sOtherRace)
	{
		map<CString, BYTE>::const_iterator it = m_mRelations.find(sOtherRace);
		if (it != m_mRelations.end())
			return it->second;
		else
			return 0;
	}

	/// Funktion setzt die neue Beziehung zur Rasse.
	/// @param sRaceID Rassen-ID zu der die Beziehung ge�ndert werden soll
	/// @param nAdd Wert der zur alten Beziehung addiert werden soll
	void SetRelation(const CString& sRaceID, short nAdd);

	/// Funktion gibt den diplomatischen Status zu einer anderes Rasse zur�ck.
	/// @param sOtherRace andere Rasse
	/// @return VertragsID (anscheinend -5 bis 6, Grenzen eingeschlossen)
	DIPLOMATIC_AGREEMENT::Typ GetAgreement(const CString& sOtherRace) const
	{
		map<CString, DIPLOMATIC_AGREEMENT::Typ>::const_iterator it = m_mAgreement.find(sOtherRace);
		if (it != m_mAgreement.end())
			return it->second;
		else
			return DIPLOMATIC_AGREEMENT::NONE;
	}

	/// Funktion gibt das Feld der ausgehenden Nachrichten zur�ck.
	/// @return Feld der ausgehenden diplomatischen Nachrichten
	vector<CDiplomacyInfo>* GetOutgoingDiplomacyNews(void) {return &m_vDiplomacyNewsOut;}

	/// Funktion gibt das Feld der eingehenden Nachrichten zur�ck.
	/// @return Feld der eingehenden diplomatischen Nachrichten
	vector<CDiplomacyInfo>* GetIncomingDiplomacyNews(void) {return &m_vDiplomacyNewsIn;}

	/// Funktion gibt das zuletzt gemachte Angebot an eine bestimmte Rasse aus den letzten beiden Runden zur�ck.
	/// @param sToRace Rasse an welche das Angebot ging
	/// @return Angebot oder <code>NULL</code>, wenn kein Angebot aus den letzten beiden Runden vorliegt.
	const CDiplomacyInfo* GetLastOffer(const CString& sToRace) const
	{
		map<CString, CDiplomacyInfo>::const_iterator it = m_mLastOffers.find(sToRace);
		if (it != m_mLastOffers.end())
			return &it->second;
		else
			return NULL;
	}

	/// Funktion gibt den zu nutzenden Grafikdateinamen inkl. Dateiendung zur�ck.
	/// @return Name der Grafikdatei
	const CString& GetGraphicFileName(void) const {return m_sGraphicFile;}

	/// Funktion erstellt eine Tooltipinfo der Rasse.
	/// @return	der erstellte Tooltip-Text
	virtual CString GetTooltip(void) const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funktionen zum Schreiben der Membervariablen
	/// Funktion legt fest, ob die Rasse eine andere Rasse kennt.
	/// @param sRace andere Rasse
	/// @param bKnown <code>TRUE</code> wenn sie sie kennenlernt, ansonsten <code>FALSE</code>
	void SetIsRaceContacted(const CString& sRace, bool bKnown);

	/// Funktion legt den diplomatischen Status zu einer anderes Rasse fest.
	/// @param sOtherRace andere Rasse
	/// @param nNewAgreement neuer Vertrag
	virtual void SetAgreement(const CString& sOtherRace, DIPLOMATIC_AGREEMENT::Typ nNewAgreement);

	/// Funktion zum Setzen von Rasseneigenschaften.
	/// @param nProp Rasseneigenschaft
	/// @param is <code>true</code> oder <code>false</code>
	void SetRaceProperty(RACE_PROPERTY::Typ nProp, bool is);

	/// Funktion setzt die Schiffsnummer f�r die Rasse.
	/// @param nNumber Nummer des Schiffssets
	void SetRaceShipNumber(BYTE nNumber) {m_byShipNumber = nNumber;}

	/// Funktion zum Erfragen der Rassenspezialeigenschaften
	/// @param ability Rassenspezialeigenschaft
	/// @return <code>true</code>, wenn die Rasse die Spezialeigenschaft besitzt, sonst <code>false</code>
	bool HasSpecialAbility(int ability) const;

	bool CanBeContactedBy(const CString& sRaceID) const;
	virtual void Contact(const CRace& Race, const CPoint& p);

	virtual void AddToLostShipHistory(const CShips& Ship, const CString& sEvent,
		const CString& sStatus, unsigned short round);

	virtual void LostFlagShip(const CShip& ship);
	virtual void LostStation(SHIP_TYPE::Typ type);
	virtual void LostShipToAnomaly(const CShips& ship, const CString& anomaly);

	/// Funktion zum Setzen von Spezialeigenschaften der Rasse.
	/// @param ability Spezialeigenschaft
	/// @param is <code>true</code> oder <code>false</code>
	void SetSpecialAbility(int ability, bool is);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Funktion zum erstellen einer Rasse.
	/// Die Funktion liest einen entsprechenden Eintrag aus einer data Datei.
	/// @param saInfo Referenz auf Rasseninformationen
	/// @param nPos Referenz auf Position im Array, ab wann die Informationen gelten
	virtual void Create(const CStringArray&, int&) {};

	/// Funktion l�sst die Diplomatie-KI der Rasse Angebote an andere Rassen erstellen.
	void MakeOffersAI(void);

	/// Funktion l�sst die KI auf diplomatische Angebote reagieren.
	/// @param pOffer diplomatisches Angebot
	void ReactOnOfferAI(CDiplomacyInfo* pOffer);

	/// Funktion zum zur�cksetzen aller Werte auf Ausgangswerte.
	virtual void Reset(void);

protected:
	// Attribute
	// Beschreibende Attribute
	CString				m_sID;			///<!!! Rassen-ID
	CString				m_sHomeSystem;	///<!!! Name des Heimatsystems
	CString				m_sName;		///<!!! Rassenname
	CString				m_sNameArticle;	///<!!! Artikel f�r Rassenname
	CString				m_sDesc;		///<!!! Rassenbeschreibung
	RACE_TYPE			m_RaceType;		///<!!! Rassentyp (Major, Medior, Minor)
	int					m_nProperty;	///<!!! Rasseneigenschaften
	BYTE				m_byShipNumber;	///<!!! zugewiesene Nummer, welche Schiffe verwendet werden sollen
	BYTE				m_byBuildingNumber;	///<!!! zugewiesene Nummer, welche Geb�ude verwendet werden sollen
	BYTE				m_byMoralNumber;	///<!!! zugewiesene Nummer, welche Moralwerte verwendet werden sollen
	int					m_nSpecialAbility;	///< Spezialf�higkeiten der Rasse

	// Ingame-Attribute (Rassenwechselwirkung)
	map<CString, BYTE>	m_mRelations;	///< Beziehungsmap (Rassen-ID, Beziehungswert)
	map<CString, DIPLOMATIC_AGREEMENT::Typ>	m_mAgreement;	///< Diplomatischer Status gegen�ber anderen Rassen (Rassen-ID, Status)
	set<CString>		m_vInContact;	///< kennt die Rasse eine andere Rasse (Rassen-ID)
	// diplomatische Nachrichten
	vector<CDiplomacyInfo>	m_vDiplomacyNewsIn;	///< Vektor mit allen eingehenden diplomatischen Nachrichten
	vector<CDiplomacyInfo>	m_vDiplomacyNewsOut;///< Vektor mit allen ausgehenden diplomatischen Nachrichten
	map<CString, CDiplomacyInfo> m_mLastOffers;	///< Mep mit den von dieser Rasse gemachten Angeboten der letzten 2 Runden.

	// grafische Attribute
	CString				m_sGraphicFile;			///<!!! Name der zugeh�rigen Grafikdatei

	// Rassen Diplomatie-KI
	CDiplomacyAI*		m_pDiplomacyAI;			///< Diplomatie-KI (muss nicht serialisiert werden)
};
