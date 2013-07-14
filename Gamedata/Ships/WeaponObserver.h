/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "ShipInfo.h"

struct BeamWeaponsObserverStruct
{
	CString WeaponName;
	BYTE maxLevel;
};

struct TupeWeaponsObserverStruct
{
	CString TupeName;
	BYTE number;
	BYTE fireRate;
	BOOLEAN onlyMicro;
	USHORT fireAngle;
};

class CWeaponObserver :	public CObject
{
public:
	DECLARE_SERIAL (CWeaponObserver)
	// Standardkonstruktor
	CWeaponObserver(void);

	// Destruktor
	virtual ~CWeaponObserver(void);

	// Kopierkonstruktor
	CWeaponObserver(const CWeaponObserver & rhs);

	// Zuweisungsoperatur
	CWeaponObserver & operator=(const CWeaponObserver &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Funktion gibt uns den maximal baubaren Typ einer Beamwaffe zur�ck. Daf�r �bergeben wir der Funktion
	// den Namen der Beamwaffe. Ist der R�ckgabewert NULL, so k�nnen wir den Typ nicht bauen
	BYTE GetMaxBeamType(const CString& NameOfBeamWeapon) const;

	// Funktion sucht einen weiteren baubaren Torpedo und �bernimmt daf�r den aktuell angebauten Torpedotyp und
	// den aktuell angebauten Torpedowerfeer
	BYTE GetNextTorpedo(BYTE currentTorpedoType, BOOLEAN onlyMircoTupe) const;

	// Funktion sucht einen weiteren Torpedowerfer, denn wir an das Schiff montieren k�nnen und �bernimmt daf�r
	// den Namen des aktuell angebauten Werfers und den aktuell angebauten Torpedotyp
	TupeWeaponsObserverStruct GetNextTupe(const CString& currentTupeName, BYTE currentTorpedoType) const;

	// Funktion gibt den maximal baubaren Schildtyp des Imperiums zur�ck
	BYTE GetMaxShieldType() const {return m_MaxShieldLevel;}

	// Funktion checkt alle baubaren Beamwaffen und setzt alle Variablen auf den richtigen Wert.
	// Der Parameter <code>info</code> mu� ein derzeit baubares Schiff des Imperium sein. Die Funktion
	// checkt gleichzeitig auch den max. Schildtyp.
	void CheckBeamWeapons(CShipInfo* info);

	// Funktion checkt alle Torpedowaffen (Torpedowerfer, Torpedoart) die wir bauen k�nnen und �bernimmt daf�r einen
	// Zeiger auf ein Schiffsinfoobjekt. Das Schiff mu� zur gleichen Rasse geh�ren, zu der auch der auch das
	// Observerobjekt geh�rt. Der Parameter <code>info</code> mu� ein derzeit baubares Schiff des Imperium sein.
	void CheckTorpedoWeapons(CShipInfo* info);

	// Resetfunktion f�r diese Klasse, welche alle Werte wieder auf den Ausgangswert setzt
	void Reset();

private:
	// dynamisches Feld speichert alle baubaren Beamwaffen eines Imperiums
	CArray<BeamWeaponsObserverStruct,BeamWeaponsObserverStruct> m_BeamWeapons;

	// dynamisches Feld speichert alle baubaren Torpedowerfer eines Imperiums
	CArray<TupeWeaponsObserverStruct,TupeWeaponsObserverStruct> m_TupeWeapons;

	// Feld speichert welche Torpedos wir an Schiffen anbringen k�nnen
	BOOLEAN m_BuildableTorpedos[DIFFERENT_TORPEDOS];

	// Der maximale Schildtyp, den wir derzeit bauen k�nnen
	BYTE m_MaxShieldLevel;
};
