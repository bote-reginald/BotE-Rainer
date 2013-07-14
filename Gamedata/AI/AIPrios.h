/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Constants.h"
#include "IntelAI.h"
#include <map>

class CBotEDoc;
class CSectorAI;

using namespace std;
/**
 * Diese Klasse beinhaltet zus�tzliche Priorit�ten f�r den Schiffsbau und Truppenbau, sowie den Geheimdienst.
 * Diese Priorit�ten werden dann bei der Ausf�hrung der System-KI mitbeachtet.
 */
class CAIPrios
{
public:
	/// Konstruktor
	CAIPrios(CBotEDoc* pDoc);

	/// Destruktor
	~CAIPrios(void);

	// Zugriffsfunktionen
	/// Funtkion gibt die Priorit�ten ein Kolonieschiff zu bauen zur�ck.
	BYTE GetColoShipPrio(const CString& sRaceID) {return m_mColoShipPrio[sRaceID];}

	/// Funtkion gibt die Priorit�ten ein Kolonieschiff zu bauen zur�ck.
	BYTE GetTransportShipPrio(const CString& sRaceID) {return m_mTransportPrio[sRaceID];}

	/// Funtkion gibt die Priorit�ten ein Kriegsschiff zu bauen zur�ck.
	BYTE GetCombatShipPrio(const CString& sRaceID) {return m_mCombatShipPrio[sRaceID];}

	/// Funktion gibt einen Zeiger auf das Geheimdienstobjekt f�r eine bestimmte Rasse zur�ck.
	/// @param race Rasse
	CIntelAI* GetIntelAI(void) {return &m_IntelAI;}

	// sonstige Funktionen
	/// Funktion sollte aufgerufen werden, wenn diese Priorit�t gew�hlt wurde. Denn dann verringert sich
	/// diese Priorit�t.
	void ChoosedColoShipPrio(const CString& sRaceID) {m_mColoShipPrio[sRaceID] /= 2;}

	/// Funktion sollte aufgerufen werden, wenn diese Priorit�t gew�hlt wurde. Denn dann verringert sich
	/// diese Priorit�t.
	void ChoosedTransportShipPrio(const CString& sRaceID) {m_mTransportPrio[sRaceID] /= 2;}

	/// Funktion sollte aufgerufen werden, wenn diese Priorit�t gew�hlt wurde. Denn dann verringert sich
	/// diese Priorit�t.
	void ChoosedCombatShipPrio(const CString& sRaceID) {m_mCombatShipPrio[sRaceID] /= 2;}

	/// Diese Funktion berechnet die Priorit�ten der einzelnen Majorrassen, wann sie ein Kolonieschiff in Auftrag
	/// geben sollen.
	void CalcShipPrios(CSectorAI* sectorAI);

	/// Funktion l�scht alle vorher berechneten Priorit�ten.
	void Clear(void);

private:
	// Attribute
	/// Beinhaltet die Priorit�t einer Rasse ein Kolonieschiff zu bauen.
	map<CString, BYTE> m_mColoShipPrio;

	/// Beinhaltet die Priorit�t Truppentransporter zu bauen.
	map<CString, BYTE> m_mTransportPrio;

	/// Beinhaltet die Priorit�t Kriegschiffe zu bauen.
	map<CString, BYTE> m_mCombatShipPrio;

	/// Beinhaltet Priorit�ten f�r den Geheimdienst und stellt Funktionen zur Steuerung der KI bereit
	CIntelAI m_IntelAI;

	/// Ein Zeiger auf das Document.
	CBotEDoc* m_pDoc;
};

/*
Schiffsangriff:

Wann wird ein Zielsektor ermittelt?
-----------------------------------
Genau dann, wenn wir angegriffen werden, einen Angriff vorbereiten wollen (noch kein Krieg) oder im Krieg mit einer Rasse
sind und sofort angreifen wollen.


Wie wird ein Zielsektor ermittelt?
----------------------------------
Wir m�ssen zwischen Offensive und Defensive unterscheiden. Wollen wir angreifen haben wir genug Zeit die Schiffe zu
sammeln. M�ssen wir uns verteidigen, so m�ssen wir auch sammeln, sollten aber auch angreifen, wenn unsere Flotte
insgesamt schw�cher ist. Einen Zielsektor in der Offensive sollte eine gegnerische Flotte oder ein gegnerischer
Aussenposten oder auch ein gegnerisches System sein.


Wie wird die Flotte gesammelt?
------------------------------
Zuerst wird der Sektor gesucht, in dem wir die meisten/st�rksten Schiffe stationiert haben. All diese Schiffe werden
zu einer Flotte zusammengefasst und fliegen zum Zielsektor. Doch sie fliegen noch nicht direkt zum Zielsektor, sondern
halten rand()%3+1 Felder davor an. Um dies rauszubekommen werden einfach die letzten rand()%3+1 Flugpfadeintr�ge der
Flotte entfernt. Dann wird gewartet, bis die anderen Schiffsflotten auf diesem Feld eintreffen. Ich schlage mal vor,
dass ca. 80% - 90% der Flotte auf diesem Feld eingetroffen sein muss oder die Schiffsst�rke in diesem Feld schon st�rker
als die Schiffsst�rke im Zielsektor ist. Trifft eine der beiden Bedingungen ein wird der Kurs zum Zielsektor gesetzt.

*/
