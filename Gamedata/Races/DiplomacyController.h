/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include <vector>

// forward declaration
class CBotEDoc;
class CRace;
class CMajor;
class CMinor;
class CDiplomacyInfo;

/// Klasse zur Verarbeitung aller diplomatischen Ereignisse.
class CDiplomacyController
{
protected:
	/// Standardkonstruktor
	CDiplomacyController(void);

public:
	/// Standarddestruktor
	~CDiplomacyController(void);

	/// Funktion zum Versenden von diplomatischen Angeboten
	static void Send(void);

	/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots.
	static void Receive(void);

private:
	// private Funktionen

	/// Funktion �berpr�ft die diplomatische Konsistenz und berechnet die direkten diplomatischen Auswirkungen.
	/// Sie sollte nach <func>Receive</func> aufgerufen werden.
	/// Stellt die Funktion Probleme fest, so werden diese automatisch behoben.
	static void CalcDiplomacyFallouts(CBotEDoc* pDoc);

	/// Funktion zum Versenden von diplomatischen Angeboten an eine Majorrace.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pToMajor Zeiger auf die Empf�ngerrasse
	/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
	static void SendToMajor(CBotEDoc* pDoc, CMajor* pToMajor, CDiplomacyInfo* pInfo);

	/// Funktion zum Versenden von diplomatischen Angeboten an eine Minorrace.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pToMinor Zeiger auf die Empf�ngerrasse
	/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
	static void SendToMinor(CBotEDoc* pDoc, CMinor* pToMinor, CDiplomacyInfo* pInfo);

	/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots f�r eine Majorrace.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pToMajor Zeiger auf die Empf�ngerrasse
	/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
	static void ReceiveToMajor(CBotEDoc* pDoc, CMajor* pToMajor, CDiplomacyInfo* pInfo);

	/// Funktion zum Empfangen und Bearbeiten eines diplomatischen Angebots f�r eine Minorrace.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pToMajor Zeiger auf die Empf�ngerrasse
	/// @param pInfo Zeiger auf aktuelles Diplomatieobjekt
	static void ReceiveToMinor(CBotEDoc* pDoc, CMinor* pToMinor, CDiplomacyInfo* pInfo);

	/// Funktion zum Berechnen aller betroffenen Rassen, welchen zus�tzlich der Krieg erkl�rt wird.
	/// Dies geschieht dadurch, wenn der Kriegsgegner B�ndnisse oder Verteidigungspakt mit anderen Rassen besitzt.
	/// @param pDoc Zeiger auf das Dokument
	/// @param pFromMajor Zeiger auf die kriegserkl�rende Majorrace
	/// @param pToRace Zeiger auf die Rasse, welcher der Krieg erkl�rt wurde
	/// @return Feld mit allen betroffenen Rassen, denen auch der Krieg erkl�rt werden muss
	static std::vector<CString> GetEnemiesFromContract(CBotEDoc* pDoc, CMajor* pFromMajor, CRace* pToRace);

	/// Funktion erkl�rt den Krieg zwischen zwei Rassen und nimmt dabei alle notwendigen Arbeiten vor.
	/// @param pFromRace Zeiger auf die kriegserkl�rende Rasse
	/// @param pEnemy Zeiger auf die Rasse, welcher Krieg erkl�rt wird
	/// @param pInfo Diplomatieobjekt
	/// @param bWithMoralEvent <code>true</code> wenn Moralevent mit eingeplant werden soll
	static void DeclareWar(CRace* pFromRace, CRace* pEnemy, CDiplomacyInfo* pInfo, bool bWithMoralEvent);
};
