/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "DiplomacyAI.h"

/// Klasse f�r die Diplomatie KI einer Majorrace
class CMajorAI : public CDiplomacyAI
{
public:
	/// Standardkonstruktor
	/// @param pRace Zeiger auf Rasse, zu welcher das Diplomatieobjekt geh�rt
	CMajorAI(CRace* pRace);
	/// Standarddestruktor
	virtual ~CMajorAI(void);

	// Funktionen

	/// Diese Funktion berechnet, wie eine computergesteuerte Rasse, auf ein Angebot reagiert
	/// Das ist die komplette KI der Reaktion auf irgendwelche Angebote.
	/// @param info Information des diplomatischen Angebots
	/// @return <code>ACCEPTED</code> f�r Annahme
	///			<code>DECLINED</code> f�r Ablehnung
	///			<code>NOT_REACTED</code> f�r keine Reaktion
	virtual ANSWER_STATUS::Typ ReactOnOffer(const CDiplomacyInfo& info);

	/// Funktion zur Erstellung eines diplomatischen Angebots.
	/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
	virtual bool MakeOffer(CString& sRaceID, CDiplomacyInfo& info);

	/// Rasse bestimmt einen Vektor mit den Lieblingsrassen der Majorrace. Auf diese wird sich
	/// beim Geschenkegeben und bei Mitgiften konzentriert.
	void CalcFavoriteMinors(void);

protected:
	// private Funktionen

	/// Diese Funktion berechnet, wie eine computergesteuerte Rasse, auf ein Angebot eines Minors reagiert.
	/// @param info Information des diplomatischen Angebots
	/// @return <code>ACCEPTED</code> f�r Annahme
	///			<code>DECLINED</code> f�r Ablehnung
	///			<code>NOT_REACTED</code> f�r keine Reaktion
	static ANSWER_STATUS::Typ ReactOnMinorOffer(const CDiplomacyInfo& info);

	/// Diese Funktion berechnet, wie eine computergesteuerte Rasse, auf ein Angebot eines Majors reagiert.
	/// @param info Information des diplomatischen Angebots
	/// @return <code>ACCEPTED</code> f�r Annahme
	///			<code>DECLINED</code> f�r Ablehnung
	///			<code>NOT_REACTED</code> f�r keine Reaktion
	ANSWER_STATUS::Typ ReactOnMajorOffer(const CDiplomacyInfo& info);

	/// Diese Funktion berechnet wie eine Majorrace auf eine Forderung reagiert.
	/// @param info Information des diplomatischen Angebots
	/// @return <code>ACCEPTED</code> f�r Annahme
	///			<code>DECLINED</code> f�r Ablehnung
	///			<code>NOT_REACTED</code> f�r keine Reaktion
	ANSWER_STATUS::Typ CalcDiplomacyRequest(const CDiplomacyInfo& info);

	/// Funktion zur Erstellung eines diplomatischen Angebots an einen Minor.
	/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
	bool MakeMinorOffer(const CString& sRaceID, CDiplomacyInfo& info);

	/// Funktion zur Erstellung eines diplomatischen Angebots an einen Major.
	/// @param sRaceID RassenID der Rasse, an die das Angebot gemacht werden soll
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return <code>true</code> wenn ein Angebot erzeugt wurde, ansonsten <code>false</code>
	bool MakeMajorOffer(CString& sRaceID, CDiplomacyInfo& info);

	/// Funktion gibt den ben�tigten Wert zur�ck, der erreicht werden muss, damit diese Rasse
	/// das diplomatische Angebot abgibt. Der Wert ist abh�ngig von den Rasseneigenschaften.
	/// @param nOfferType Typ des Angebots
	/// @return ben�tigter Wert
	int GetMinOfferValue(DIPLOMATIC_AGREEMENT::Typ nOfferType) const;

	/// Funktion berechnet wieviel und welche Mitgifte die KI bei einem Angebot mitgibt.
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return <code>true</code> wenn eine Mitgift gegeben wurde, ansonsten <code>false</code>
	bool GiveDowry(CDiplomacyInfo& info);

	/// Funktion berechnet ob und was f�r eine Forderung gestellt wird.
	/// @param info Referenz auf eine Diplomatieinformation (darin wird das Angebot abgelegt)
	/// @return <code>true</code> wenn eine Forderung gestellt wurde, ansonsten <code>false</code>
	bool ClaimRequest(CDiplomacyInfo& info);

	/// Funktion berechnet die Beziehungsverbesserungen durch die �bergabe von Credits und Ressourcen.
	/// Die Credits werden hier jedoch nicht gutgeschrieben, sondern nur die Beziehung zur Majorrace verbessert.
	/// @param info Referenz auf eine Diplomatieinformation
	virtual void ReactOnDowry(const CDiplomacyInfo& info);

protected:
	// Attribute
	CString m_sFavoriteMinor;		///< die Lieblingsminorrace des Majors
};
