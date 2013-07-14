/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "afxtempl.h"
#include "Constants.h"

class CTradeHistory : public CObject
{
public:
	DECLARE_SERIAL (CTradeHistory)
	// Standardkonstruktor
	CTradeHistory(void);

	// Destruktor
	virtual ~CTradeHistory(void);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	// Funktion gibt alle Preise aller Ressourcen zur�ck
	CArray<USHORT,USHORT>* GetHistoryPrices() {return m_Prices;}

	// Funktion gibt alle Preise einer bestimmten Ressource zur�ck
	CArray<USHORT,USHORT>* GetHistoryPriceFromRes(USHORT res) {return &m_Prices[res];}

	// zum Schreiben der Membervariablen

// sonstige Funktionen
	// Funktion f�gt die aktuelle Preisliste in das Feld ein
	void SaveCurrentPrices(USHORT* resPrices, float currentTax);

	// Funktion gibt den Minimumpreis einer bestimmten Ressource zur�ck
	USHORT GetMinPrice(USHORT res);

	// Funktion gibt den Maximumpreis einer bestimmten Ressource zur�ck. �bergeben wird die Art der Ressource
	// und optional der Bereich, aus dem das Maximum ermittelt werden soll
	USHORT GetMaxPrice(USHORT res, USHORT start = 0, USHORT end = 0);

	// Funktion gibt den Durchschnittspreis einer bestimmten Ressource zur�ck
	USHORT GetAveragePrice(USHORT res);

	// Resetfunktion f�r die Klasse CTradeHistory
	void Reset();

private:
	// Feld mit Eintr�gen (dynamisches Feld) der Preise aller Runden im Spiel
	CArray<USHORT,USHORT> m_Prices[5];
};
