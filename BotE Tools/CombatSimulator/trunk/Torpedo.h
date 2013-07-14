/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Vec3.h"


#include <irrlicht.h>

using namespace irr;


using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


class CCombatShip;
class CTorpedo : public CObject
{
	friend class CCombatShip;
	friend class CCombatSimulatorView;

public:
	/// Konstruktor
	CTorpedo(void);
	
	/// Destruktor
	~CTorpedo(void);

	/// Kopierkonstruktor
	CTorpedo(const CTorpedo & rhs);
	
	/// Zuweisungsoperatur
	CTorpedo & operator=(const CTorpedo &);

	/// Gebe Szenenmanager zur�ck
	ISceneManager* GetSceneManager() const {return p_smgr;};
	/// Gebe Node f�r Torpedo zur�ck
	ISceneNode* GetNode() const {return p_node;};

	///Setze Sezemanager
	void SetSceneManager(ISceneManager* smgr) {	p_smgr = smgr;};

	///Setze Node f�r Torpedo
	void SetNode(ISceneNode* node) {p_node = node;};
	
	

	/**
	* Funktion berechnet die Flugbahn des Torpedos und macht die Kollisionsabfrage mit allen anderen Schiffen
	* im Kampf. Wenn der Torpedo trifft, dann wird auch der Schaden am Schiff verrechnet und die Funktion gibt
	* TRUE zur�ck. Wenn wir keinen Treffer landen, aber MAX_TORPEDO_RANGE �berschritten haben gibt die Funktion
	* auch ein TRUE zur�ck. Ansonsten gibt die Funktion immer FALSE zur�ck
	*/
	BOOLEAN Fly(CArray<CCombatShip*,CCombatShip*>* CS);

private:
	/// aktuelle Koordinate im Raum
	vec3i m_KO;

	/// Zielkoordinate im Raum
	vec3i m_TargetKO;

	/// Wenn wir mehrere Torpedos auf einmal auf ein und das selbe Ziel verschie�en, so setzen wir diese Variable immer
	/// mit der Anzahl der Torpedos, die sich auf dieser Koordinate befinden. Im Normalfall ist dieser Wert eins, wenn
	/// unser Werfer aber 10 Torpedos verschie�t und alle das gleiche Ziel haben, dann setzen wird diesen Wert auf 10.
	BYTE m_iNumber;
	
	/// Die Distanz, die der Torpedo schon zur�ckgelegt hat. Wenn MAX_TORPEDO_RANGE erreicht wurde wird der Torpedo
	/// automatisch vernichtet
	USHORT m_iDistance;

	/// Schaden des Torpedos bei Einschlag in das Ziel
	USHORT m_iPower;

	/// Der Torepdotyp
	BYTE m_byType;

	/// Welche Rasse hat den Torpedo abgefeuert. Also wer ist der Besitzer des Torpedos
	CString m_sOwner;

	/// Die Man�vriebarkeit des Schiffes, welches den Torepdo abgefeuert hat
	BYTE m_byManeuverability;

	/// Szenenmanager 
	ISceneManager* p_smgr;	
	ISceneNode* p_node;

	/**
	* Trefferwahrscheinlichkeit des Torepdowerfers + Zielgenauigkeit durch Erfahrung der Mannschaft des Schiffes,
	* welches diesen Torpedo abgeschossen hat
	*/
	short m_iModi;

	/**
	* Diese private Funktion berechnet den Schaden, den ein Torpedo am gegnerischen Schiff anrichtet. Sie �bernimmt
	* daf�r den Zeiger auf das gegnerische Schiff <code>CS<code>.
	*/
	void MakeDamage(CCombatShip* CS);

	/**
	* Diese private Funktion setzt den Torpedo auf den n�chsten Punkt seiner Flugbahn. Diese Funktion nur aufrufen,
	* wenn der Torpedo keinen Schaden an irgendeinem Schiff gemacht hat. Hat der Torpedo aus irgendeinem Grund
	* seine maximale Lebensdauer erreicht, gibt die Funktion ein TRUE zur�ck, anonsten FALSE.
	*/
	BOOLEAN FlyToNextPosition();

	/**
	* Diese private Funktion gibt ein TRUE zur�ck, wenn der Torpedo sein Ziel getroffen hat, andernfalls wird ein
	* FALSE zur�ckgegeben. Ob ein Torpedo trifft h�ngt von mehreren Parametern ab, welche diese Funktion alle
	* ber�cksichtigt. Wenn er nicht trifft ruft die Funktion selbstst�ndig <code>FlyToNextPosition<code> auf. Als
	* Parameter wird ein Zeiger auf das Schiff �bergeben, welches vielleicht getroffen wird und die Distanz zwischen
	* unserem Torepdo und dem Zielschiff.
	*/
	BOOLEAN PerhapsImpact(CCombatShip* CS, USHORT minDistance);
};