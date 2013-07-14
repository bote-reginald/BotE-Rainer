/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include <vector>

/// Enum aller m�glichen Torpedospezialeigenschaften
enum TORPEDO_SPECIALS
{
	NO_SPECIAL			= 0,
	MICROTORPEDO		= 1,	///< Microtorpedo (nur f�r Microwerfer)
	PENETRATING			= 2,	///< schilddurchschlagender Torpedo
	DOUBLESHIELDDMG		= 4,	///< Torpedo macht doppelten Schaden an den Schilden
	DOUBLEHULLDMG		= 8,	///< Torpedo macht doppelten Schaden an der H�lle
	IGNOREALLSHIELDS	= 16,	///< Torpedo ignoriert alle Arten von Schilden
	COLLAPSESHIELDS		= 32,	///< Torpedotreffer kann Schilde kollabieren lassen
	REDUCEMANEUVER		= 64	///< Bei einem Treffer besteht die Chance, die Man�vrierbarkeit des Zieles auf NULL zu verringern.
};

// Eigenschaften eines Torpedos
struct TORPEDOINFO
{
	// Attribute
	CString		sName;			///< der Name des Torpedos
	USHORT		nDmg;			///< die St�rke eines Torpedos
	int			nSpecials;		///< Spezialeigenschaften (bitweise gespeichert)

	TORPEDOINFO(const CString& _sName, USHORT _nDmg, int _nSpecials = NO_SPECIAL)
	{
		sName		= _sName;
		nDmg		= _nDmg;
		nSpecials	= _nSpecials;
	}
};

/// Klasse enth�lt feste Informationen zu bestimmten Torpedotypen
class CTorpedoInfo
{
private:
	/// Standardkonstruktor
	CTorpedoInfo(void);

	/// Kopierkonstruktor
	CTorpedoInfo(const CTorpedoInfo&) {};

public:
	/// Destruktor
	~CTorpedoInfo(void);

	// Zugriffsfunktionen zum Lesen der Membervariablen

	/// Diese Funktionen gibt den Torepdonamen zur�ck.
	static const CString& GetName(BYTE type);

	/// Diese Funktionen gibt die Torepdost�rke zur�ck.
	static USHORT GetPower(BYTE type) { return GetInstance()->m_vInfos[type].nDmg; }

	/// Diese Funktionen gibt zur�ck, ob es sich um einen schilddurchschlagenden Torpedo handelt.
	static bool GetMicro(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & MICROTORPEDO) == MICROTORPEDO; }

	/// Diese Funktionen gibt zur�ck, ob es sich um einen schilddurchschlagenden Torpedo handelt.
	static bool GetPenetrating(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & PENETRATING) == PENETRATING; }

	///Diese Funktionen gibt zur�ck, ob der Torpedo doppelten Schaden an den Schilden macht.
	static bool GetDoubleShieldDmg(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & DOUBLESHIELDDMG) == DOUBLESHIELDDMG; }

	/// Diese Funktionen gibt zur�ck, ob der Torpedo doppelten Schaden an der H�lle macht.
	static bool GetDoubleHullDmg(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & DOUBLEHULLDMG) == DOUBLEHULLDMG; }

	/// Diese Funktionen gibt zur�ck, ob der Torpedo jegliche Schildart ignoriert..
	static bool GetIgnoreAllShields(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & IGNOREALLSHIELDS) == IGNOREALLSHIELDS; }

	/// Diese Funktionen gibt zur�ck, ob durch einen Torpedotreffer m�glicherweise die Schilde zusammenbrechen.
	static bool GetCollapseShields(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & COLLAPSESHIELDS) == COLLAPSESHIELDS; }

	/// Diese Funktionen gibt zur�ck, ob durch einen Torpedotreffer m�glicherweise die Man�vriebarkeit auf
	/// NULL verringert wird.
	static bool GetReduceManeuver(BYTE type) { return (GetInstance()->m_vInfos[type].nSpecials & REDUCEMANEUVER) == REDUCEMANEUVER; }

private:
	// Funktionen
	/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
	/// @return Instanz dieser Klasse
	static CTorpedoInfo const* const GetInstance(void);

	// Attribute
	std::vector<TORPEDOINFO> m_vInfos;	///< alle m�glichen Torpedotypen
};
