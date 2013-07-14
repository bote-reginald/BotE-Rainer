/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "CombatShip.h"
#include <map>
#include <vector>
#include <set>
#include <list>

using namespace std;

// forward declaration
class CRace;
class CAnomaly;

class CCombat :	public CObject
{
	friend class CCombatSimulatorDoc;
	friend class CCombatSimulatorView;

public:
	/// Konstruktor
	CCombat(void);
	
	/// Destruktor
	~CCombat(void);

	// Zugriffsfunktionen
	BOOLEAN GetReadyForCombat() const {return m_bReady;}

	/// Schiffskampfberechnungsfunktion
	/**
	* Diese Funktion verlangt beim Aufruf einen Zeiger auf ein Feld, welches Zeiger auf Schiffe beinhaltet
	* <code>ships<code>. Diese Schiffe werden dann am Kampf teilnehmen. Kommt es zu einem Kampf, so mu� 
	* diese Funktion zu allererst aufgerufen werden.
	*/
	void SetInvolvedShips(CArray<CShip*>* pvShips);
	
	/**
	* Diese Funktion setzt die gew�hlte Schiffsformation der Rasse <code>race<code> fest.
	* Sie sollte vor dem Aufruf der Funktion <code>PreCombatCalculation()<code> aufgerufen werden.
	*/
	void ApplyShipFormation(int race);
	
	/**
	* Diese Funktion setzt die gew�hlte Taktik <code>tactic<code> der Rasse <code>race<code> fest.
	* Sie sollte vor dem Aufruf der Funktion <code>PreCombatCalculation()<code> aufgerufen werden.
	*/
	void ApplyShipTactic(int race, BYTE tactic);
	
	/**
	* Diese Funktion mu� vor der Funktion <code>CalculateCombat()<code> aufgerufen werden. Sie stellt alle
	* Berechnungen an, welche f�r den sp�teren Kampfverlauf n�tig sind. Wird diese Funktion nicht ordnungsgem��
	* durchgef�hrt, kann die Funktion <code>CalculateCombat()<code> nicht durchgef�hrt werden.
	*/
	void PreCombatCalculation();
	
	/** 
	* Diese Funktion ist das Herzst�ck der CCombat-Klasse. Sie f�hrt die ganzen Kampfberechnungen durch.
	*/
	void CalculateCombat(std::map<CString, BYTE>& winner);

	/**
	* Funktion �berpr�ft, ob die Rassen in einem Kampf sich gegeneinander aus diplomatischen Gr�nden
	* �berhaupt attackieren. Die Funktion gibt <code>TRUE</code> zur�ck, wenn sie sich angreifen k�nnen,
	* ansonsten gibt sie <code>FALSE</code> zur�ck.
	*/
	static bool CheckDiplomacyStatus(const CRace* raceA, const CRace* raceB);

	/**
	* Diese Funktion setzt alle Variablen des Combat-Objektes wieder auf ihre Ausgangswerte
	*/
	void Reset();

//public:
private:
	/// Das dynamische Feld in denen alle am Kampf beteiligten Schiffe mit allen
	/// dazugh�rigen Informationen abegelegt sind
	CArray<CCombatShip*, CCombatShip*> m_InvolvedShips;

	/// Das Feld mit den Referenzen auf die Schiffe, welche im Kampf beteiligt sind. Dieses Feld wird zur Iteration
	/// verwendet und auch manipuliert.
	CArray<CCombatShip*, CCombatShip*> m_CS;

	/// In diesem Array werdem alle Gegner des jeweiligen Imperiums initialisiert.
	std::map<CString, std::vector<CCombatShip*> > m_mEnemies;

	/// Das dynamische Feld in denen alle am Kampf abgefeuerten und noch vorhandenen Torpedos
	/// mit allen dazugh�rigen Informationen abegelegt sind
	std::list<CTorpedo*> m_CT;
	
	/// Sind alle Vorbereitungen f�r eine Kampfberechnungen abgeschlossen
	BOOLEAN m_bReady;

	/// Diese Variable beinhaltet die aktuelle Zeit (Runde) im Kampf
	UINT m_iTime;

	/// Speichert ob in diesem Kampf irgendwer irgendwen attackiert hat.
	BOOLEAN m_bAttackedSomebody;

	/// Speichert die Nummer der beteiligten Rassen.
	std::set<CString> m_mInvolvedRaces;

	/**
	* Diese Funktion versucht dem i-ten Schiff im Feld <code>m_CS<code> ein Ziel zu geben. Wird dem Schiff ein Ziel
	* zugewiesen gibt die Funktion TRUE zur�ck, findet sich kein Ziel mehr gibt die Funktion FALSE zur�ck.
	*/	
	bool SetTarget(int i);		

	/**
	* Diese private Funktion �berpr�ft, ob das Schiff an der Stelle <code>i<code> im Feld <code>m_CS<code> weiterhin
	* am Kampf teilnehmen kann. Ist das Schiff entweder zerst�rt oder hat sich erfolgreich Zur�ckgezogen, so kann es
	* nicht weiter am Kampf teilnehmen. In diesem Fall unternimmt diese Funktion alle Arbeiten die anfallen,
	* um dieses Schiff aus dem Feld zu entfernen. D.h. m�gliche Ziele werden ver�ndert, Zeiger neu zugeweisen usw.
	* Wenn das Schiff nicht mehr im Kampf ist gibt die Funktion FALSE zur�ck, ansonsten TRUE.
	*/
	bool CheckShipStayInCombat(int i);	
};
