/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "Constants.h"
#include "System\System.h"

#define EVENTNUMBER 61

class CMoralObserver : public CObject
{
public:
	DECLARE_SERIAL (CMoralObserver)

	/// Konstruktor
	CMoralObserver(void);

	/// Destruktor
	~CMoralObserver(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	/// Serialisierungsfunktion f�r statische Variable
	static void SerializeStatics(CArchive &ar);

	/**
	 * Funktion f�gt das vorgekommene Ereignis dem MoralObserver-Objekt hinzu, so dass es sp�ter
	 * bei der Moralberechnung mit beachtet werden kann. Als Parameter wird dabei die Nummer des
	 * <code>event</code> und der jeweiligen Majorrace <code>major</code> �bergeben. Welche Nummer
	 * zu welchem Event geh�rt, kann in der Funktion <code>CalculateEvents</code> nachgelesen werde.
	 * Als R�ckgabewert gibt die Funktion den jeweiligen Text zur�ck, der in der Ereignis�bersicht
	 * angezeigt werden kann. Optional muss auch ein Textstring <code>param</code> �bergeben werden,
	 * der in bestimmte Nachrichten eingebaut wird.
	 */
	CString AddEvent(unsigned short Event, BYTE major, const CString& param = "") {
		m_iEvents.Add(Event); return GenerateText(Event, major, param);
	}
	/**
	 * Funktion berechnet die Moralauswirkung auf alle Systeme, abh�ngig von den vorgekommenen Events und
	 * der jeweiligen Majorrace. �bergeben werden daf�r alle Systeme <code>systems</code>, die RaceID und die
	 * gemappte Nummer der Majorrace <code>byMappedRaceNumber</code>, auf welche Moralwerte sich bezogen werden soll.
	 */
	void CalculateEvents(std::vector<CSystem>& systems, const CString& sRaceID, BYTE byMappedRaceNumber);

	/**
	 * Funktion erstellt einen Text, der in der Ereignisansicht angezeigt werden kann, aufgrund eines speziellen
	 * Events. Dieser Text ist rassenabh�ngig und wird von der Funktion zur�ckgegeben. Als Parameter werden an diese
	 * Funktion das Event <code>event</code> und die Nummer der Majorrace <code>major<code> �bergeben. Optional muss
	 * auch ein Textstring <code>param</code> �bergeben werden, der in bestimmte Nachrichten eingebaut wird.
	 */
	CString GenerateText(unsigned short Event, BYTE major, const CString& param = "") const;

	/**
	 * Funktion l�dt die zu Beginn die ganzen Moralwerte f�r alle verschiedenen Ereignisse in eine Moralmatrix.
	 */
	static void InitMoralMatrix();

	/// Funktion gibt einen bestimmten Moralwert zur�ck.
	/// @param byMappedRaceNumber gemappte Rassennummer
	/// @param Event Eventnummer
	/// @return Moralwert
	static short GetMoralValue(BYTE byMappedRaceNumber, unsigned short Event);

private:
	/// Das Feld, in dem alle Vertragsannahmen stehen.
	CArray<short,short> m_iEvents;

	/// Matrix in der die Moralwerte f�r die unterschiedlichen Ereignisse abgelegt werden.
	static short m_iMoralMatrix[EVENTNUMBER][MAJOR6];

	/// Matrix in der die Textnachrichten f�r die unterschiedlichen Ereignisse abgelegt werden.
	static CString m_strTextMatrix[EVENTNUMBER][MAJOR6];
};
