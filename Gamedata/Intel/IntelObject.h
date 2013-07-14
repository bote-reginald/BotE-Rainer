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

/**
 * Diese abstrakte Klasse stellt Informationen zu einem bestimmten Geheimdiensteinsatz bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */
class CBotEDoc;
class CIntelObject : public CObject
{
public:
	/// Standardkonstruktor
	CIntelObject(void);

	/// Konstruktor mit Paramter�bergabe.
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param type Typ der Aktion -> Wirtschaft == 0, Forschung == 1, Milit�r == 2, Diplomatie == 3
	CIntelObject(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, BYTE type);

	/// Destruktor
	virtual ~CIntelObject(void);

	/// Kopierkonstruktor
	CIntelObject(const CIntelObject & rhs);

	// Operator�berschreibung, der zum Sortieren des Feldes benuzt werden kann
	bool operator< (const CIntelObject& elem2) const { return m_nRound < elem2.m_nRound;}
	bool operator> (const CIntelObject& elem2) const { return m_nRound > elem2.m_nRound;}

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktion
	/// Funktion gibt den Besitzer/Ausl�ser der Geheimdienstaktion zur�ck.
	const CString& GetOwner() const {return m_sOwner;}

	/// Funktion gibt das Ziel der Geheimdienstaktion zur�ck.
	const CString& GetEnemy() const {return m_sEnemy;}

	/// Funktion gibt den speziellen Untertyp der Geheimdienstaktion zur�ck.
	/// @return Wirtschaft == 0, Forschung == 1, Milit�r == 2, Diplomatie == 3
	BYTE GetType() const {return m_byType;}

	/// Funktion gibt die Beschreibung der Geheimdienstaktion f�r den Ausl�ser der Aktion zur�ck.
	const CString* GetOwnerDesc() const {return &m_strOwnerDesc;}

	/// Funktion gibt die Beschreibung der Geheimdienstaktion f�r das Ziel der Aktion zur�ck.
	const CString* GetEnemyDesc() const {return &m_strEnemyDesc;}

	/// Funktion gibt zur�ck, ob es sich bei der Aktion um eine Spionageaktion oder eine Sabotageaktion
	/// handelt. Der R�ckgabewert ist <code>TRUE</code> wenn es sich um eine Spionageaktion handelt, ansonsten
	/// ist er <code>FALSE</code> und somit eine Sabotageaktion.
	BOOLEAN GetIsSpy() const {return m_bIsSpy;}

	/// Funktion gibt zur�ck, ob es sich bei der Aktion um eine Sabotageaktion oder eine Spionageaktion
	/// handelt. Der R�ckgabewert ist <code>TRUE</code> wenn es sich um eine Sabotageaktion handelt, ansonsten
	/// ist er <code>FALSE</code> und somit eine Spionageaktion.
	BOOLEAN GetIsSabotage() const {return !m_bIsSpy;}

	/// Funktion gibt die Runde zur�ck, in welcher das Objekt angelegt wurde.
	USHORT GetRound() const {return m_nRound;}

	// sonstige Funktionen
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, f�r den Ausl�ser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textm�glichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse �bergeben werden, von der das Opfer denkt angegriffen worden zu sein
	virtual void CreateText(CBotEDoc* pDoc, BYTE n, const CString& param) = 0;

	/// Funktion setzt den Text f�r den Ausl�ser der Geheimdienstaktion
	/// @param s neuer Text
	void SetOwnersDesc(const CString &s) {m_strOwnerDesc = s;}

	/// Funktion setzt den Text f�r das Opfer der Geheimdienstaktion
	/// @param s neuer Text
	void SetEnemyDesc(const CString &s) {m_strEnemyDesc = s;}

	// Sortierfunktionen
	template <class ELEM>
	static int sort_by_enemy_desc(const ELEM* elem1, const ELEM* elem2)
	{ return (*(CIntelObject**)elem1)->GetEnemy() < (*(CIntelObject**)elem2)->GetEnemy() ? -1 : 1; }
	template <class ELEM>
	static int sort_by_round_desc(const ELEM* elem1, const ELEM* elem2)
	{ return (*(CIntelObject**)elem1)->GetRound() < (*(CIntelObject**)elem2)->GetRound() ? -1 : 1; }
	template <class ELEM>
	static int sort_by_type_desc(const ELEM* elem1, const ELEM* elem2)
	{ return (*(CIntelObject**)elem1)->GetType() < (*(CIntelObject**)elem2)->GetType() ? -1 : 1; }
	template <class ELEM>
	static int sort_by_kind_desc(const ELEM* elem1, const ELEM* elem2)
	{ return (*(CIntelObject**)elem1)->GetIsSpy() < (*(CIntelObject**)elem2)->GetIsSpy() ? -1 : 1; }

	template <class ELEM>
	static int sort_by_enemy_asc(const ELEM* elem1, const ELEM* elem2)
	{ return (*(CIntelObject**)elem1)->GetEnemy() > (*(CIntelObject**)elem2)->GetEnemy() ? -1 : 1; }
	template <class ELEM>
	static int sort_by_round_asc(const ELEM* elem1, const ELEM* elem2)
	{ return (*(CIntelObject**)elem1)->GetRound() > (*(CIntelObject**)elem2)->GetRound() ? -1 : 1; }
	template <class ELEM>
	static int sort_by_type_asc(const ELEM* elem1, const ELEM* elem2)
	{ return (*(CIntelObject**)elem1)->GetType() > (*(CIntelObject**)elem2)->GetType() ? -1 : 1; }
	template <class ELEM>
	static int sort_by_kind_asc(const ELEM* elem1, const ELEM* elem2)
	{ return (*(CIntelObject**)elem1)->GetIsSpy() > (*(CIntelObject**)elem2)->GetIsSpy() ? -1 : 1; }

protected:
	// Attribute
	CString m_sOwner;		///< Rasse welche die Aktion gestartet hat

	CString m_sEnemy;		///< Rasse welche die Aktion treffen soll/hat

	BYTE m_byType;			///< spezielle Unterart der Geheimdienstaktion (Wirtschaft == 0, Wissenschaft == 1, Milit�r == 2, Diplomatie == 3)

	CString m_strOwnerDesc;	///< Aktionsbeschreibung f�r den Ausl�ser der Aktion

	CString m_strEnemyDesc;	///< Aktionsbeschreibung f�r das Ziel der Aktion

	BOOLEAN m_bIsSpy;		///< handelt es sich bei der Aktion um Spionage oder Sabotage

	USHORT m_nRound;		///< Runde in welcher dieses Objekt erstellt wurde
};
