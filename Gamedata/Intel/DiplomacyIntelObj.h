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
 * Diese Klasse stellt Informationen zu einem Geheimdiensteinsatz mit irgendeinem Diplomatieziel bereit.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */

class CDiplomacyIntelObj :	public CIntelObject
{
	DECLARE_SERIAL (CDiplomacyIntelObj)
public:
	/// Konstruktor
	CDiplomacyIntelObj(void);

	/// Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param minorRaceKO die Koordinaten des Minorraceheimatsystems
	/// @param agreement Vertrag von <code>enemy</code> mit der Minorrace
	/// @param relationship Beziehung von <code>enemy</code> mit der Minorrace
	CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &minorRaceKO, DIPLOMATIC_AGREEMENT::Typ agreement, short relationship);

	/// Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param sMajorRaceID betroffene Majorrace mit der <code>sEnemyID</code> in Beziehung steht
	/// @param agreement Vertrag von <code>enemy</code> mit der Majorrace
	/// @param duration Vertragsdauer
	/// @param relationship Beziehung von <code>enemy</code> mit der Majorrace
	CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CString& sMajorRaceID, DIPLOMATIC_AGREEMENT::Typ agreement, short duration, short relationship);

	///  Konstruktor mit Parameter�bergabe
	/// @param sOwnerID Ausl�ser/Eigent�mer
	/// @param sEnemyID Ziel der Geheimdienstaktion
	/// @param round Runde in der die Geheimdienstaktion angelegt wurde
	/// @param isSpy handelt es sich um Spionage oder Sabotage
	/// @param minorRaceKO die Koordinaten des Minorraceheimatsystems
	CDiplomacyIntelObj(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, const CPoint &minorRaceKO);

	/// Destruktor
	~CDiplomacyIntelObj(void);

	/// Kopierkonstruktor
	CDiplomacyIntelObj(const CDiplomacyIntelObj & rhs);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt die Koordinate des Heimatsystems einer Minorrace zur�ck.
	const CPoint& GetMinorRaceKO() const {return m_MinorRaceKO;}

	/// Funktion gibt die ID der betroffenen Majorrace zur�ck.
	const CString& GetMajorRaceID() const {return m_sMajor;}

	/// Funktion gibt einen bestehenden Vertrag zur�ck
	DIPLOMATIC_AGREEMENT::Typ GetAgreement() const {return m_nAgreement;}

	/// Funktion gibt die Dauer eines m�glichen Vertrages zur�ck.
	short GetDuration() const {return m_nDuration;}

	/// Funktion gibt die Beziehung zu einer anderen Rasse zur�ck.
	short GetRelationship() const {return m_nRelationship;}

	// sonstige Funktion
	/// Funktion generiert einen Text, welcher eine Geheimdiestaktion beschreibt, f�r den Ausl�ser bzw. das Opfer
	/// dieser Aktion.
	/// @param pDoc Zeiger auf das Dokument
	/// @param n Nummer der verschiedenen Textm�glichkeiten, im Normalfall <code>NULL</code>
	/// @param param Hier kann die Rasse �bergeben werden, von der das Opfer denkt angegriffen worden zu sein
	void CreateText(CBotEDoc* pDoc, BYTE n, const CString& param);

private:
	// Attribute
	CPoint m_MinorRaceKO;		///< Heimatsystemkoordinaten der betroffenen Minorrace

	CString m_sMajor;			///< betroffene Majorrace, mit welcher das Geheimdienstopfer einen Vertrag hat (oder auch nicht)

	DIPLOMATIC_AGREEMENT::Typ m_nAgreement;	///< Vertrag zwischen den Rassen

	short m_nDuration;			///< Dauer des Vertrages

	short m_nRelationship;		///< Beziehung des Geheimdienstopfers zu einer anderen Major oder Minor
};
