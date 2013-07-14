/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// TorpedoWeapons.h: Schnittstelle f�r die Klasse CTorpedoWeapons.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TORPEDOWEAPONS_H__53084731_CFE0_4000_873E_6980A76C69CC__INCLUDED_)
#define AFX_TORPEDOWEAPONS_H__53084731_CFE0_4000_873E_6980A76C69CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TorpedoInfo.h"
#include "FireArc.h"

class CTorpedoWeapons : public CObject
{
	friend class CShip;
public:
	DECLARE_SERIAL (CTorpedoWeapons)
	CTorpedoWeapons();
	virtual ~CTorpedoWeapons();
	CTorpedoWeapons(const CTorpedoWeapons & rhs);
	CTorpedoWeapons & operator=(const CTorpedoWeapons &);
// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	/**
	 * Diese Funktion gibt den Namen des Torpedowerfers zur�ck.
	 */
	const CString& GetTupeName() const {return m_strTupeName;}

	/**
	 * Diese Funktion gibt die Anzahl der abzufeuerten Torpedos zur�ck.
	 */
	BYTE GetNumber() const {return m_iNumber;}

	/**
	 * Diese Funktion gibt die Feuerrate des Torepdowerfers zur�ck.
	 */
	BYTE GetTupeFirerate() const {return m_iTupeFirerate;}

	/**
	 * Diese Funktion gibt die Anzahl dieser Torpedowerfer zur�ck, die sich auf dem Schiff befinden.
	 */
	BYTE GetNumberOfTupes() const {return m_iNumberOfTupes;}

	/**
	 * Diese Funktion gibt die Trefferwahrscheinlichkeit des Torpedowerfers zur�ck.
	 */
	BYTE GetAccuracy() const {return m_byAccuracy;}

	/**
	 * Diese Funktion gibt zur�ck, ob der Torpedo nur f�r Microphoton geeignet ist.
	 */
	BOOLEAN GetOnlyMicroPhoton() const {return m_bOnlyMicroPhoton;}

	/// Funktion gibt Zeiger auf das Schussfeld der Waffe zur�ck.
	/// @return Zeiger auf Schussfeld
	CFireArc* GetFirearc(void) {return &m_Firearc;}

	/// folgende Funktionen geben nur Eigenschaften eines Torpedos zur�ck
	BYTE GetTorpedoType() const {return m_iTorpedoType;}
	const CString& GetTorpedoName() const {return CTorpedoInfo::GetName(m_iTorpedoType);}
	USHORT GetTorpedoPower() const {return CTorpedoInfo::GetPower(m_iTorpedoType);}

	/**
	 * Diese Funktion setzt die Genauigkeit eines Torpedowerfers direkt fest.
	 */
	void SetAccuracy(BYTE acc) {m_byAccuracy = acc;}

	/**
	 * Diese Funktion setzt die Feuerrate des Torpedowerfers fest.
	 */
	void SetTubeFirerate(BYTE rate) {m_iTupeFirerate = rate;}

	void ModifyTorpedoWeapon(BYTE TorpedoType, BYTE Number, BYTE TupeFirerate, BYTE NumberOfTupes, const CString& TupeName,
		BOOLEAN OnlyMicroPhoton, BYTE Acc);

private:
	BYTE	m_iTorpedoType;				// Typ des/der Torpedos
	BYTE	m_iNumber;					// Anzahl der abzufeuernden Torpedos pro Sekunde/Zug
	BYTE	m_iTupeFirerate;			// aller wieviel Sekunden kann m_iNumber Torpedos abgefeuert werden
	BYTE	m_iNumberOfTupes;			// Anzahler dieser Abschu�anlagen
	BYTE	m_byAccuracy;				// Die Treffergenauigkeit des Torpedos
	CString m_strTupeName;				// Name der Torpedoabschu�einrichtung
	BOOLEAN	m_bOnlyMicroPhoton;			// kann der Werfer nur Microphotonentorpedos schie�en?
	CFireArc m_Firearc;				// m�glicher Feuerwinkel und Anbringung der Waffe
};

#endif // !defined(AFX_TORPEDOWEAPONS_H__53084731_CFE0_4000_873E_6980A76C69CC__INCLUDED_)
