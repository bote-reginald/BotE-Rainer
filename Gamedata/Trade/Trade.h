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
#include "Trade\TradeHistory.h"
#include <map>
#include <vector>

using namespace std;
// forward declaration
class CMajor;
class CSystem;
class CSector;

struct TradeStruct {
	USHORT res;
	int number;
	CPoint system;
	int price;
	TradeStruct() : res(0), number(0), system(-1, -1), price(0) {}
};

class CTrade : public CObject
{
public:
	DECLARE_SERIAL (CTrade)
	// Standardkonstruktor
	CTrade(void);

	// Destruktor
	virtual ~CTrade(void);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen

	/// Funktion gibt die Kursgeschichte zur�ck.
	/// @return Zeiger auf ein CTradeHistory Objekt
	CTradeHistory* GetTradeHistory(void) {return &m_TradeHistory;}

	// Funktion gibt das Feld der aktuellen Preise der Ressourcen zur�ck
	USHORT* GetRessourcePrice() {return m_iRessourcePrice;}

	// Funktion gibt das Feld der aktuellen Preise der Ressourcen zur�ck, so wie sie am
	// Anfang der jeweiligen Runde waren
	USHORT* GetRessourcePriceAtRoundStart() {return m_iRessourcePriceAtRoundStart;}

	// Funktion gibt die aktuelle Steuer auf Handelsaktivit�ten zur�ck
	float GetTax() const {return m_fTax;}

	// Funktion gibt das Feld mit den Steuern, die wir nur durch den Sofortkauf bei Bauauftr�gen
	// machen, zur�ck
	USHORT* GetTaxesFromBuying() {return m_iTaxes;}

	// Funktion gibt die Menge, die wir bei einem Klick kaufen/verkaufen zur�ck
	USHORT GetQuantity() const {return m_iQuantity;}

	// Diese Klassenfunktion gibt den Besitzer eines Monopols f�r eine bestimmte Ressource zur�ck
	static CString GetMonopolOwner(USHORT res) {return m_sMonopolOwner[res];}

	/// Diese Klassenfunktion gibt das statische Feld mit den Monopolbesitzern zur�ck.
	const static CString* GetMonopolOwner() {return m_sMonopolOwner;}

	// Funktion gibt uns das Feld zur�ck, in dem steht ob wir ein Monopol gekauft haben
	double* GetMonopolBuying() {return m_dMonopolBuy;}

	// Funktion gibt das Feld der Handelsaktivit�ten f�r das komplette Imperium zur�ck
	CArray<TradeStruct,TradeStruct>* GetTradeActions() {return &m_TradeActions;}

	// zum Schreiben der Membervariablen

	// Funktion setzt den Preis f�r eine Ressource fest
	void SetRessourcePrice(USHORT res, USHORT price) {m_iRessourcePrice[res] = price;}

	// Funktion setzt die Steuer auf Handelsaktivit�ten fest
	void SetTax(float newTax) {m_fTax = newTax;}

	// Funktion setzt die Menge, die wir pro Handelsaktivit�t umsetzen fest
	void SetQuantity(USHORT newQuantity) {m_iQuantity = newQuantity;}

	// Diese Klassenfunktion setzt den Besitzer eines Monopols f�r eine bestimmte Ressource
	static void SetMonopolOwner(USHORT res, const CString& sOwnerID) {m_sMonopolOwner[res] = sOwnerID;}

	// Funktion setzt den veranschlagten Kaufpreis einer Ressource in das Feld m_dMonopolBuy
	void SetMonopolBuying(USHORT res, double costs) {m_dMonopolBuy[res] = costs;}

// sonstige Funktionen
	// Funktion kauft die Anzahl der jeweiligen Ressource f�r das System und f�gt den Auftrag in das Array
	// m_TradeActions ein. Danach berechnet sie den Preis der Ressource nach dem Kauf. Steuern
	// werden hier noch nicht in den Preis mit einbezogen. Der Preis inkl. Steuern wird zur�ckgegeben.
	// Falls bNotAtMarket == true, dann wird nix in das Feld m_TradeActions geschrieben. Nur die Preisauswirkungen des Kaufes
	// auf die B�rse werden beachtet
	int BuyRessource(USHORT res, ULONG number, CPoint system, long empires_credits, bool bNotAtMarket = false);

	// Funktion verkauft die Anzahl der jeweiligen Ressource aus dem System und f�gt den Auftrag in das Array
	// m_TradeActions ein. Danach berechnet sie den Preis der Ressource nach dem Verkauf. Steuern
	// werden hier noch nicht in den Preis mit einbezogen. Falls bNotAtMarket == true, dann wird nix in das Feld m_TradeActions
	// geschrieben. Nur die Preisauswirkungen des Verkaufes auf die B�rse werden beachtet
	void SellRessource(USHORT res, ULONG number, CPoint system, bool bNotAtMarket = false);

	// Funktion berechnet die ganzen Handelsaktionen, lagert also Ressourcen ein oder gibt das Credits, welches
	// wir durch den Verkauf bekommen haben an das jeweilige Imperium
	void CalculateTradeActions(CMajor* pMajor, std::vector<CSystem>& systems, const std::vector<CSector>& sectors, USHORT* taxes);

	/// Funktion berechnet den Preis der Ressourcen in Zusammenhang zu den anderen B�rsen.
	/// @param pmMajors Zeiger auf Map mit allen Majors des Spiels
	/// @param pCurMajor aktuelle Rasse, f�r die die Preise berechnet werden
	void CalculatePrices(map<CString, CMajor*>* pmMajors, CMajor* pCurMajor);

	// Funktion veranla�t, dass in der neuen Runde versucht wird ein Monopol zu erlangen.
	void BuyMonopol(USHORT res, double monopol_costs) {m_dMonopolBuy[res] = monopol_costs;}

	// Resetfunktion f�r die Klasse CTrade
	void Reset(void);

private:

	// Das Handelshistoryobjekt, welches alle vergangenen B�rsenkurse gespeichert hat
	CTradeHistory m_TradeHistory;

	// Der aktuelle Preis der jeweiligen Ressource an der globalen Handelsb�rse
	USHORT m_iRessourcePrice[IRIDIUM + 1];

	// Der Preis der Ressource zu Beginn einer Runde (wird genommen, wenn wir Bauauftr�ge kaufen, sonst k�nnte man
	// in der gleichen Runde den Preis dr�cken, dann billig kaufen und dann den Preis wieder hochtreiben)
	USHORT m_iRessourcePriceAtRoundStart[IRIDIUM + 1];

	// Die Anzahl der jeweiligen Ressource die wir kaufen oder verkaufen m�chten (negative Werte bedeuten verkaufen)
	CArray<TradeStruct,TradeStruct> m_TradeActions;

	// Die Menge die wir bei einem Klick kaufen bzw. Verkaufen
	USHORT m_iQuantity;

	// Variable die die aktuelle Steuer auf Handelsaktivit�ten festh�lt (rassenabh�ngig)
	float m_fTax;

	// Steuergelder auf Ressourcen nur durch Sofortkauf von Bauauftr�gen, nicht die Steuern, die wir durch normalen
	// Handel machen
	USHORT m_iTaxes[IRIDIUM + 1];

	// Welche Majorrace besitzt ein Monopol auf die jeweilige Ressource
	static CString m_sMonopolOwner[IRIDIUM + 1];

	// Wollen wir ein Monopol kaufen? Wird bei neuer Runde abgefragt. Ist der Wert darin ungleich NULL, dann wollen wir
	// eins kaufen. Der Wert gibt auch den Kaufpreis an
	double m_dMonopolBuy[IRIDIUM + 1];
};

