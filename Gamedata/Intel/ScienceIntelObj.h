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
 * Diese Klasse stellt Informationen zu einem Geheimdiensteinsatz mit irgendeinem Forschungsziel bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

class CScienceIntelObj : public CIntelObject
{
	DECLARE_SERIAL (CScienceIntelObj)
public:
	/// Konstruktor
	CScienceIntelObj(void);

	/// Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param ko Koordinaten des System
	/// @param id ID der betroffenen Geb�ude
	/// @param number Anzahl der betroffenen Geb�ude
	CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &ko, USHORT id, BYTE number);

	/// Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param FP globale Forschungspunkte
	CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, UINT FP);

	/// Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param techLevel aktuelle ausspioniertes Techlevel
	/// @param techType Typ des ausspionierten Techlevel (Biotechnik, Energietechnik usw.)
	/// @param specialTechComplex ausspionierter Spezialforschungskomplex
	/// @param choosenSpecialTech gew�hlter Bonus aus der Spezialforschungs
	CScienceIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, short techLevel, short techType, RESEARCH_COMPLEX::Typ specialTechComplex, short choosenSpecialTech);

	/// Destruktor
	~CScienceIntelObj(void);

	/// Kopierkonstruktor
	CScienceIntelObj(const CScienceIntelObj & rhs);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die global produzierten Forschungspunkte des ausspionierten Imperiums zur�ck.
	UINT GetFP() const {return m_iFP;}

	/// Funktion gibt die Koordinaten des Sektors zur�ck, auf welchen sich die Geheimdienstaktion auswirkt.
	const CPoint& GetKO() const {return m_KO;}

	/// Funktion gibt die ID des Geb�udes zur�ck, auf welches sich die Geheimdienstaktion auswirkt.
	USHORT GetID() const {return m_nID;}

	/// Funktion gibt die Anzahl der Geb�ude zur�ck, auf welche sich die Geheimdienstaktion auswirkt.
	BYTE GetNumber() const {return m_byNumber;}

	/// Funktion gibt das ausspionierte Techlevel zur�ck.
	short GetTechLevel() const {return m_nTechlevel;}

	/// Funktion gibt den Typ des ausspionierten Techlevels zur�ck (also Biotechnik, Energietechnik usw.)
	short GetTechType() const {return m_nTechType;}

	/// Funktion gibt einen erforschten Spezialforschungskomplex zur�ck.
	RESEARCH_COMPLEX::Typ GetSpecialTechComplex() const {return m_nSpecialTechComplex;}

	/// Funktion gibt die gew�hlte Auswahl aus dem Spezialforschungskomplex zur�ck.
	short GetChoosenSpecialTech() const {return m_nChoosenSpecialTech;}

	// sonstige Funktion
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, f�r den Ausl�ser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textm�glichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse �bergeben werden, von der das Opfer denkt angegriffen worden zu sein
	void CreateText(CBotEDoc* pDoc, BYTE n, const CString& param);

private:
	// Attribute
	UINT m_iFP;						///< gesamte Forschungspunkte

	CPoint m_KO;					///< Systemkoordinaten der Forschungsspionage

	BYTE m_byNumber;				///< Anzahl der spionierten/sabotierten Geb�ude

	USHORT m_nID;					///< ID der zu sabotierenden/sabotierten Geb�ude

	short m_nTechlevel;				///< aktuelle ausspioniertes Techlevel

	short m_nTechType;				///< Typ des ausspionierten Techlevel (Biotechnik, Energietechnik usw.)

	RESEARCH_COMPLEX::Typ m_nSpecialTechComplex;	///< ausspionierter Spezialforschungskomplex

	short m_nChoosenSpecialTech;	///< gew�hlter Bonus aus der Spezialforschungs
};
