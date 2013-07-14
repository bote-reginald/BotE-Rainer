/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Constants.h"
#include <map>

/**
 * Diese Klasse beinhaltet bestimmte Priorit�ten, welche im Zusammenhang des Geheimdienstes stehen. Zus�tzlich finden
 * hier die Berechnungen der KI f�r den Geheimdienst statt.
 */
class CBotEDoc;
class CIntelAI
{
public:
	/// Konstruktor
	CIntelAI(void);

	/// Destruktor
	~CIntelAI(void);

	// Zugriffsfunktionen
	/// Funktion gibt die Priorit�t f�r den Geheimdienst f�r eine Rasse zur�ck.
	/// @param race gew�nschte Rasse
	BYTE GetIntelPrio(const CString& sRace) {return m_byIntelPrio[sRace];}

	// sonstige Funktionen
	/// Funktion berechnet den Umgang mit dem Geheimdienst f�r die KI. Sie trifft Zuteilungen f�r die einzelnen Rassen.
	/// @param pDoc Zeiger auf das Dokument
	void CalcIntelligence(CBotEDoc* pDoc);

	/// Resetfunktion f�r ein CIntelAI-Objekt.
	void Reset(void);

private:
	// Attribute
	std::map<CString, BYTE> m_byIntelPrio;		///< beinhaltet die Priorit�t zum Bau von Geheimdienstgeb�uden


};
