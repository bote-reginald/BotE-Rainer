/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"

class CTorpedoInfo : public CObject
{
private:
	/// Konstruktor
	CTorpedoInfo(void);

	/// Destruktor
	~CTorpedoInfo(void);

public:
	// Zugriffsfunktionen zum Lesen der Membervariablen
	/**
	 * Diese Funktionen gibt den Torepdonamen zur�ck.
	 */
	static const CString& GetName(BYTE type) {SetValues(type); return m_strTorpedoName;}

	/**
	 * Diese Funktionen gibt die Torepdost�rke zur�ck.
	 */
	static USHORT GetPower(BYTE type) {SetValues(type); return m_iTorpedoPower;}

	/**
	 * Diese Funktionen gibt zur�ck, ob es sich um einen schilddurchschlagenden Torpedo handelt.
	 */
	static BOOLEAN GetMicro(BYTE type) {SetValues(type); return m_bMicro;}

	/**
	 * Diese Funktionen gibt zur�ck, ob es sich um einen schilddurchschlagenden Torpedo handelt.
	 */
	static BOOLEAN GetPenetrating(BYTE type) {SetValues(type); return m_bPenetrating;}

	/**
	 * Diese Funktionen gibt zur�ck, ob der Torpedo doppelten Schaden an den Schilden macht.
	 */
	static BOOLEAN GetDoubleShieldDmg(BYTE type) {SetValues(type); return m_bDoubleShieldDmg;}

	/**
	 * Diese Funktionen gibt zur�ck, ob der Torpedo doppelten Schaden an der H�lle macht.
	 */
	static BOOLEAN GetDoubleHullDmg(BYTE type) {SetValues(type); return m_bDoubleHullDmg;}

	/**
	 * Diese Funktionen gibt zur�ck, ob der Torpedo jegliche Schildart ignoriert..
	 */
	static BOOLEAN GetIgnoreAllShields(BYTE type) {SetValues(type); return m_bIgnoreAllShields;}

	/**
	 * Diese Funktionen gibt zur�ck, ob durch einen Torpedotreffer m�glicherweise die Schilde zusammenbrechen.
	 */
	static BOOLEAN GetCollapseShields(BYTE type) {SetValues(type); return m_bCollapseShields;}

	/**
	 * Diese Funktionen gibt zur�ck, ob durch einen Torpedotreffer m�glicherweise die Man�vriebarkeit auf
	 * NULL verringert wird.
	 */
	static BOOLEAN GetReduceManeuver(BYTE type) {SetValues(type); return m_bReduceManeuver;}

private:
	/**
	 * Private Funktion, die die Variablen anhand des �bergebenen Torpedotypes setzt.
	 */
	static void SetValues(BYTE type);

	// Eigenschaften der Torpedos
	static BYTE	   m_byType;			///> letzter gesuchter Typ des Torpedos
	static CString m_strTorpedoName;	///> der Name des Torpedos
	static USHORT  m_iTorpedoPower;		///> die St�rke eines Torpedos
	static BOOLEAN m_bMicro;			///> sind es Microtorpedos
	static BOOLEAN m_bPenetrating;		///> sind es schilddurchschlagende Torpedos?
	static BOOLEAN m_bDoubleShieldDmg;	///> macht dieser Torpedo doppelten Schaden an den Schilden?
	static BOOLEAN m_bDoubleHullDmg;	///> macht dieser Torpedo doppelten Schaden an der H�lle?
	static BOOLEAN m_bIgnoreAllShields;	///> ignoriert der Torpedo alle Arten von Schilden?
	static BOOLEAN m_bCollapseShields;	///> Torpedotreffer kann Schilde kollabieren lassen
	static BOOLEAN m_bReduceManeuver;	///> bei einem Treffer besteht die Chance, die Man�vriebarkeit auf NULL zu verringern
};
