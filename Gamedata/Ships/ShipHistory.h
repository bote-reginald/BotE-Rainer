/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "array_sort.h"
class CShips;

/// Deklaration einer Struktur zum Speichern aller relevanten Daten
struct CShipHistoryStruct {
	/// Der Schiffsname des Schiffes
	CString m_strShipName;

	/// Der Schiffstyp des Schiffes
	CString m_strShipType;

	/// Die Bezeichnung der Schiffsklasse des Schiffes
	CString m_strShipClass;

	/// Der Name des Sektors, in dem das Schiff gebaut bzw. zerst�rt wurde.
	CString m_strSectorName;

	/// Der Name des aktuellen Sektors, in dem sich das Schiff befindet
	CString m_strCurrentSector;

	/// Der aktuelle Auftrag des Schiffes, wenn es nicht mehr vorhanden ist, dann steht
	/// hier der Status des Schiffes, z.B. zuerst�rt, vermisst usw.
	CString m_strCurrentTask;

	/// Art der Zerst�rung des Schiffes, z.B. Kampf, Abwracken, Wurmloch usw.
	CString m_strKindOfDestroy;

	//Momentanes Ziel
	CString m_strTarget;

	/// Die Runde in der das Schiff gebaut wurde
	short m_iBuildRound;

	/// Die Runde, in der das Schiff zerst�rt wurde
	short m_iDestroyRound;

	/// Die aktuelle Erfahrung des Schiffes
	USHORT m_iExperiance;

	// Custom ascending ordering function
	static int _cdecl sort_by_shipname(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_strShipName < elem2->m_strShipName ? -1 : 1; }
	static int _cdecl sort_by_shiptype(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_strShipType < elem2->m_strShipType ? -1 : 1; }
	static int _cdecl sort_by_shipclass(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_strShipClass < elem2->m_strShipClass ? -1 : 1; }
	static int _cdecl sort_by_sectorname(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_strSectorName < elem2->m_strSectorName ? -1 : 1; }
	static int _cdecl sort_by_currentsector(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_strCurrentSector < elem2->m_strCurrentSector ? -1 : 1; }
	static int _cdecl sort_by_task(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_strCurrentTask < elem2->m_strCurrentTask ? -1 : 1; }
	static int _cdecl sort_by_destroytype(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_strKindOfDestroy < elem2->m_strKindOfDestroy ? -1 : 1; }
	static int _cdecl sort_by_roundbuild(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_iBuildRound < elem2->m_iBuildRound ? -1 : 1; }
	static int _cdecl sort_by_rounddestroy(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_iDestroyRound < elem2->m_iDestroyRound ? -1 : 1; }
	static int _cdecl sort_by_exp(const CShipHistoryStruct* elem1,const CShipHistoryStruct* elem2)
	{ return elem1->m_iExperiance < elem2->m_iExperiance ? -1 : 1; }
};

class CShipHistory : public CObject
{
public:
	DECLARE_SERIAL (CShipHistory)

	/// Konstruktor
	CShipHistory(void);

	/// Destruktor
	~CShipHistory(void);

	/// Kopierkonstruktor
//	CShipHistory(const CShipHistory & rhs);

	/// Zuweisungsoperatur
//	CShipHistory & operator=(const CShipHistory &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	/**
	 * Funktion f�gt das Schiff, auf welches der �bergebene Zeiger zeigt dem Feld hinzu. Alle angaben werden
	 * dabei automatisch gemacht. Es wird ebenfalls �berpr�ft, dass dieses Schiff nicht schon hinzugef�gt wurde.
	 * Zus�tzlich m�ssen als Parameter noch der Name des Systems �bergeben werden, in dem das Schiff gebaut wurde,
	 * sowie die aktuelle Runde.
	 */
	void AddShip(const CShips* ship, const CString& buildsector, short round);

	/**
	 * Funktion modifiziert den Eintrag in dem Feld <code>m_ShipHistory<code>. Dabei wird das Schiff �bergeben, dessen
	 * Werte modifiziert werden sollen. Wenn ein Schiff aus irgendeinem Grund (Kampf, Kolonisierung usw.) zerst�rt
	 * wurde, wird f�r den Parameter <code>destroyRound<code> die aktuelle Runde der Zerst�rung �bergeben und
	 * f�r den Parameter <code>destroyType<code> die Art der Zerst�rung als CString �bergeben. Au�erdem wird der neue
	 * Status des Schiffes im Parameter <code>status<code> �bergeben, z.B. zerst�rt, vermisst usw.
	 * Konnte das Schiff modifiziert werden, so gibt die Funktion <code>true</code> zur�ck, sonst <code>false</code>
	 */
	bool ModifyShip(const CShips* ship, const CString& sector, short destroyRound = 0, const CString& destroyType = "", const CString& status = "");

	/// Funktion entfernt ein bestimmtes Schiff aus der Schiffshistory.
	/// @param ship Zeiger auf das zu entfernende Schiff.
	void RemoveShip(const CShips* ship);

	/**
	 * Funktion gibt einen Zeiger auf ein CShipHistory Objekt zur�ck, und zwar das, welches an <code>i<code>-ter
	 * Stelle in dem Feld der Schiffsobjekte steht.
	 */
	CShipHistoryStruct* GetShipHistory(unsigned short i) {return &m_ShipHistory[i];}

	/**
	 * Funktion gibt einen Zeiger auf das komplette Feld zur�ck, in dem die Daten der Schiffe stehen
	 */
	CArray<CShipHistoryStruct,CShipHistoryStruct>* GetShipHistoryArray(){return &m_ShipHistory;}

	/**
	 * Funktin gibt die Gr��e des <code>m_ShipHistory<code> Arrays zur�ck.
	 */
	UINT GetSizeOfShipHistory() const {return m_ShipHistory.GetSize();}

	/**
	 * Funktion gibt einen Wahrheitswert zur�ck, ob das Schiff noch im Dienst ist, oder schon zerst�rt wurde.
	 * Ist das Schiff noch im Dienst, dann gibt die Funktion <code>TRUE<code> zur�ck, ansonsten gibt sie
	 * <code>FALSE<code> zur�ck. �bergeben wird dabei die Nummer <code>i<code>, an dessen Stelle sich das
	 * Schiff im Array <code>m_ShipHistory<code> befindet.
	 */
	BOOLEAN IsShipAlive(unsigned short i) const {return m_ShipHistory.GetAt(i).m_iDestroyRound == 0;}

	/**
	 * Funktion gibt die Anzahl der noch lebenden Schiffe zur�ck, wenn der Parameter <code>shipAlive</code> wahr ist.
	 * Ansonsten gibt die Funktion die Anzahl der zerst�rten Schiffe zur�ck.
	 */
	UINT GetNumberOfShips(BOOLEAN shipAlive) const;

	// Resetfunktion f�r die Klasse CShipHistory
	void Reset(void);

private:
	/// Das Feld mit der gesamten Schiffshistorie eines Imperiums
	CArray<CShipHistoryStruct,CShipHistoryStruct> m_ShipHistory;
};
