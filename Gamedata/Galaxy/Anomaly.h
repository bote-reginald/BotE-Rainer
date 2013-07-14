/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"

// forward declaration
class CShips;

// Eigenschaften aller Anomalien
/*
NEUTRONENSTERNE:
Neutronenstern				-> Ausfall aller Scanner, mindestens 500 Schildschaden, max. 75% Schildschaden, ohne Schilde Schiff zerst�rt
Radiopulsar					-> wie Neutronenstern + Scannst�rke im Umkreis niedriger
R�ntgenpulsar				-> wie Radiopulsar + Verlust aller Crewerfahrung
Magnetar					-> wie R�ntgenpulsar + Schiff strandet bei zu niedriger Antriebstech


VERZERRUNGEN:
Gravimetrische Verzerrung   -> mindestens 50 H�llenschaden, max. 50% H�llenschaden
Kontinuum-Riss				-> Schiff zerst�rt
Schwarzes Loch				-> Scannerst�rke im Umkreis niedriger

NEBEL:
Meatureanischer Nebel		-> Deaktivierung der Schilde im Kampf
Deuterium-Nebel				-> Ausfall der Schilde und aller Scanner
Ionensturm					-> Maximale Schildst�rke erh�ht sich um 3% pro Runde, Verlust aller Crewerfahrung, Ausfall aller Scanner
Biphasen-Nebel				-> Schilde laden 100% schneller auf
Toriongas-Nebel				-> Deaktivierung der Schilde im Kampf, Deaktivierung der Torpedos im Kampf
Radioaktiver Nebel			-> Verlust aller Crewerfahrung

SONSTIGES:
Quasar						-> Verst�rkung der Scanner und Scannerreichweite um 50%
*/


typedef enum ANOMALYTYP
{
	// Nebel
	RADIONEBULA,
	METNEBULA,
	DEUTNEBULA,
	IONSTORM,
	BINEBULA,
	TORIONGASNEBULA,

	// Neutronensterne
	NEUTRONSTAR,
	RADIOPULSAR,
	XRAYPULSAR,
	MAGNETAR,

	// Verzerrungen
	GRAVDISTORTION,
	CONTINUUMRIP,
	BLACKHOLE,

	// sonstiges
	QUASAR,
	WORMHOLE
};
/// Klasse abstrahiert eine Anomalie im Weltraum
class CAnomaly : public CObject
{
public:
	// Klasse serialisierbar machen
	DECLARE_SERIAL (CAnomaly)

	/// Standardkonstruktor
	CAnomaly(void);

	/// Standarddestruktor
	virtual ~CAnomaly(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, const CPoint& ptSector) const;

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString GetTooltip(void) const {return "";}

	// Zugriffsfunktionen
	/// Funktion gibt den Typ der Anomalie zur�ck.
	/// @return Typ der Anomalie
	ANOMALYTYP GetType(void) const { return m_byType; }

	/// Funktion gibt den Namen der Grafikdatei zur�ck.
	/// @return Name der Grafikdatei
	CString GetImageFileName(void) const { return m_sImageFile; }

	/// Funktion gibt Auskunft ob die Grafik horizontal gespiegelt ist.
	/// @return <code>true</code> wenn Grafik gespiegelt, sonst <code>false</code>
	bool GetImageFlipHorz(void) const { return m_bFlipHorz; }

	/// Funktion gibt den Namen der Anomalie zur�ck
	/// @param pt Koordinate auf der Map
	/// @return Mapname der Anomalie
	CString GetMapName(const CPoint& pt) const;

	/// Funktion gibt eine physikalische Beschreibung der Anomalie zur�ck
	/// @return physikalische Beschreibung der Anomalie
	CString GetPhysicalDescription(void) const;

	/// Funktion gibt die spieltechnischen Auswirkungen der Anomalie in Textform zur�ck
	/// @return physikalische Beschreibung der Anomalie
	CString GetGameplayDescription(void) const;

	/// Funktion gibt ein mathematisches Gewicht zur�ck, um welches bei der Wegsuche der
	/// Sektor mit der Anomalie beschwert wird. Dadurch wird nicht unbedingt �ber die Anomalie
	/// geflogen sondern ein weg herum gesucht.
	double GetWaySearchWeight(void) const;

	/// Funktion reduziert die Scanpower in diesem und umliegenden Sektoren
	/// @param pt Koordinate auf der Map
	void ReduceScanPower(const CPoint &pt) const;

	bool IsShipScannerDeactivated(void) const;

	void CalcShipEffects(CShips* pShip) const;

protected:
	// Attribute
	CString m_sImageFile;		///< Name der Grafikdatei

	bool m_bFlipHorz;			///< Grafik ist horizontal gespiegelt

	ANOMALYTYP m_byType;		///< Typ der Anomalie

	// Funktionen
	void MakeShieldDmg(int nMinDmgValue, int nMaxDmgPercent, CShips* pShip) const;

	void MakeHullDmg(int nMinDmgValue, int nMaxDmgPercent, CShips* pShip) const;

	void PerhabsStrand(CShips* pShip) const;
};
