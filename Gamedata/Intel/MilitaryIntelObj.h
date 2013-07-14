/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "intelobject.h"

/**
 * Diese Klasse stellt Informationen zu einem Geheimdiensteinsatz mit irgendeinem milit�rischen Ziel bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

class CMilitaryIntelObj : public CIntelObject
{
	DECLARE_SERIAL (CMilitaryIntelObj)
public:
	/// Konstruktor
	CMilitaryIntelObj(void);

	/// Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param ko Koordinaten des System
	/// @param id ID der betroffenen Geb�ude
	CMilitaryIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, USHORT number,
		BOOLEAN building, BOOLEAN ship, BOOLEAN troop);

	/// Destruktor
	~CMilitaryIntelObj(void);

	/// Kopierkonstruktor
	CMilitaryIntelObj(const CMilitaryIntelObj & rhs);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die Koordinaten des Sektors zur�ck, auf welchen sich die Geheimdienstaktion auswirkt.
	CPoint GetKO() const {return m_KO;}

	/// Funktion gibt die ID des Geb�udes, des Schiffes oder der Truppe zur�ck, auf welches sich die Geheimdienstaktion
	/// auswirkt.
	USHORT GetID() const {return m_nID;}

	/// Funktion gibt die Anzahl der Geb�ude, Schiffe oder Truppen zur�ck, auf welche sich die Geheimdienstaktion auswirkt.
	USHORT GetNumber() const {return m_nNumber;}

	/// Funktion gibt zur�ck, ob das Ziel dieser Milit�raktion ein Geb�ude ist.
	BOOLEAN GetIsBuilding() const {return m_bBuilding;}

	/// Funktion gibt zur�ck, ob das Ziel dieser Milit�raktion ein Schiff/Aussenposten ist.
	BOOLEAN GetIsShip() const {return m_bShip;}

	/// Funktion gibt zur�ck, ob das Ziel dieser Milit�raktion eine Truppe ist.
	BOOLEAN GetIsTroop() const {return m_bTroop;}

	// sonstige Funktionen
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, f�r den Ausl�ser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textm�glichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse �bergeben werden, von der das Opfer denkt angegriffen worden zu sein
	void CreateText(CBotEDoc* pDoc, BYTE n, const CString& param);

private:
	// Atribute
	CPoint m_KO;					///< Systemkoordinaten der Milit�rsspionage

	USHORT m_nID;					///< ID der zu sabotierenden/sabotierten Geb�ude, Truppen, Schiffe

	USHORT m_nNumber;				///< Anzahl der Geb�ude/Schiffe/Truppen

	BOOLEAN m_bTroop;				///< handelt es sich beim Ziel der Aktion um eine Truppe

	BOOLEAN m_bShip;				///< handelt es sich beim Ziel der Aktion um ein Schiff/Schiffe

	BOOLEAN m_bBuilding;			///< handelt es sich beim Ziel der Aktion um ein/mehrere Geb�ude
};
