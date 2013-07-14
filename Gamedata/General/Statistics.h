/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Statistcs.h: Schnittstelle f�r die Klasse CStatistcs.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <map>
#include <list>
#include "Constants.h"

// forward declaration
class CBotEDoc;

class CStatistics : public CObject
{
public:
	DECLARE_SERIAL (CStatistics)

	/// Standardkonstruktor
	CStatistics(void);
	/// Standarddestruktor
	virtual ~CStatistics(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen

	/// Funktion gibt das universumweite Durchschnittstechlevel zur�ck.
	/// @return durchschnittliches Techlevel aller Hauptrassen
	BYTE GetAverageTechLevel(void) const {return m_byAverageTechLevel;}

	/// Funktion gibt Feld mit durchschnittlichen Ressourcenlagern zur�ck.
	/// @return Feld der durchschnittliches Menge im Ressourcenlager
	UINT* GetAverageResourceStorages(void) {return m_nAverageResourceStorages;}

	/// Funktion gibt Map mit den Schiffsst�rken aller Rassen zur�ck.
	/// @param sRaceID Rassen-ID f�r die die Schiffsst�rke erfragt werden soll
	/// @return Schiffsst�rke der Kriegsschiffe
	UINT GetShipPower(const CString& sRaceID) const;

	// zum Schreiben der Membervariablen
	/// Funktion zum Berechnen aller Statistiken.
	/// @param pDoc Zeiger auf das Dokument
	void CalcStats(CBotEDoc* pDoc);

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gew�nschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsBSP(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const;

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gew�nschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsProductivity(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const;

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gew�nschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsMilitary(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const;

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gew�nschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsResearch(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const;

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gew�nschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsMoral(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const;

	/// Funktion gibt die aktuellen Spielpunkte einer Rasse zur�ck.
	/// @param sRace ID gew�nschte Rasse
	/// @param sCurrentRound aktuelle Runde
	/// @param fDifficultyLevel Schwierigkeitsgradswert
	/// @return Punktzahl
	int GetGamePoints(const CString& sRaceID, int nCurrentRound, float fDifficultyLevel) const;

	/// Funktion errechnet eine sortierte Liste der aktuellen Topsysteme.
	/// @param nLimit Anzahl der zu errechnenden Topsystem (z.B. Top 5)
	/// @param [out] lSystems Liste in welche die Koordinaten der Topsysteme abgelegt werden.
	void GetTopSystems(int nLimit, std::list<CPoint>& lSystems) const;

	/// Funktion zum zur�cksetzen aller Werte auf Ausgangswerte.
	void Reset(void);

private:
	// private Funktionen

	/// Funktion zum Berechnen des universumweiten Techdurchschnittlevels.
	/// @param pDoc Zeiger auf das Dokument
	void CalcAverageTechLevel(CBotEDoc* pDoc);

	/// Funktion zum Berechnen der durchschnittlichen Bef�llung der Ressourcenlager.
	/// @param pDoc Zeiger auf das Dokument
	void CalcAverageResourceStorages(CBotEDoc* pDoc);

	/// Funktion zum Berechnen der gesamten milit�rischen Schiffsst�rken aller Rassen.
	/// @param pDoc Zeiger auf das Dokument
	void CalcShipPowers(CBotEDoc* pDoc);

	/// Funktion zum Berechnen des Milit�rs aller Rassen.
	/// @param pDoc Zeiger auf das Dokument
	void CalcMilitary(CBotEDoc* pDoc);

	/// Funktion berechnet die einzelnen Demographiewerte.
	/// @param sRaceID ID der gew�nschten Rasse
	/// @param pmMap auszuwertende Map
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void CalcDemoValues(const CString& sRaceID, const std::map<CString, float>* pmMap, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const;

	// Attribute
	BYTE m_byAverageTechLevel;						///< Durchschnittliches Techlevel aller Rassen

	UINT m_nAverageResourceStorages[DERITIUM + 1];	///< Durschschnittlicher Inhalt der Ressourcenlager

	std::map<CString, UINT> m_mShipPowers;			///< Schiffsst�rken aller Rassen
};
