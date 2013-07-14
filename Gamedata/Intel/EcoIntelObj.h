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
 * Diese Klasse stellt Informationen zu einem Geheimdiensteinsatz mit irgendeinem Wirtschaftsziel bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */
class CEcoIntelObj : public CIntelObject
{
	DECLARE_SERIAL (CEcoIntelObj)
public:
	/// Konstruktor
	CEcoIntelObj(void);

	/// Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param ko Koordinaten des System
	/// @param id ID der betroffenen Geb�ude
	/// @param number Anzahl der betroffenen Geb�ude
	CEcoIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, BYTE number);

	/// Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param credits Credits
	CEcoIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, int credits);

	/// Destruktor
	~CEcoIntelObj(void);

	/// Kopierkonstruktor
	CEcoIntelObj(const CEcoIntelObj & rhs);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen

	/// Funktion gibt die Koordinaten des Sektors zur�ck, auf welchen sich die Geheimdienstaktion auswirkt.
	const CPoint& GetKO() const {return m_KO;}

	/// Funktion gibt die ID des Geb�udes zur�ck, auf welches sich die Geheimdienstaktion auswirkt.
	USHORT GetID() const {return m_nID;}

	/// Funktion gibt die Anzahl der Geb�ude zur�ck, auf welche sich die Geheimdienstaktion auswirkt.
	BYTE GetNumber() const {return m_byNumber;}

	/// Funktion gibt die Creditsmenge zur�ck
	int GetCredits() const {return m_iCredits;}

	// sonstige Funktion
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, f�r den Ausl�ser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textm�glichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse �bergeben werden, von der das Opfer denkt angegriffen worden zu sein
	void CreateText(CBotEDoc* pDoc, BYTE n, const CString& param);

private:
	// Attribute
	CPoint m_KO;				///< Systemkoordinaten des Wirtschaftsanschlages

	BYTE m_byNumber;			///< Anzahl der spionierten/sabotierten Geb�ude

	USHORT m_nID;				///< ID der zu sabotierenden/sabotierten Geb�ude

	int m_iCredits;				///< Creditsmenge
};
