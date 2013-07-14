/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen & Plotnik
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include <irrlicht.h>
#include "BeamSceneNode.h"


using namespace irr;


using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#include "Ship.h"
#include "Torpedo.h"
#include <deque>
#include <list>

/// Eine Struktur f�r die Zeit wann wieder Beam- und Torpedowaffen abgefeuert werden k�nnen.
/// Da Folgende gilt glaube nicht mehr!
/// Die Zeiten in dieser Struktur entsprechen nicht den wahren Sekunden. Ich habe zur Umrechnung
/// 1sek == 10 Zeiteinheiten angesetzt. Somit bedeutet z.B. 5sek ein Wert von 50.
struct ShootTime
{
	CArray<BYTE,BYTE> phaser;		///< Gibt an wann wir wieder die Phaserwaffe abfeuern k�nnen
	CArray<BYTE,BYTE> torpedo;		///< Gibt an wann wir wieder die Torpedowaffe abfeuern k�nnen
	BOOLEAN phaserIsShooting;		///< Feuert ein Phaser gerade?
};

class CCombatShip :	public CObject
{
	friend class CCombat;
	friend class CTorpedo;
	friend class CCombatSimulatorView;

public:
	/// Konstruktor
	CCombatShip(void);

	/// Destruktor
	~CCombatShip(void);	

	/**
	 * Diese Funktion setzt die Man�vriebarkeit des Schiffes. Sie muss direkt nach anlegen des CombatSchiffes aufgerufen
	 * werden.
	 */
	void SetManeuverability(BYTE man) {m_byManeuverability = man;}

	/**
	 * Diese Funktion gibt den m�glichen Bonus durch die Man�vriebarkeit bei einem Angriff zur�ck. �bergeben m�ssen
	 * daf�r die Man�vrierbarkeit des Angreifers und die Man�vrierbarkeit des Verteidigers werden.
	 */
	static BYTE GetToHitBoni(BYTE Att, BYTE Def);

	/**
	 * Diese Funktion gibt den m�glichen Malus durch die Man�vriebarkeit bei einem Angriff zur�ck. �bergeben m�ssen
	 * daf�r die Man�vrierbarkeit des Angreifers und die Man�vrierbarkeit des Verteidigers werden.
	 */
	static BYTE GetToHitMali(BYTE Att, BYTE Def);

	/**
	 * Diese Funktion stellt die vorhandenen regenerativen Schilde auf die feindliche schilddurchschlagende Waffe ein.
	 */
	void ActRegShield();

	/**
	* Diese Funktion berechnet den n�chsten Punkt auf dem Weg zum Ziel, den das Schiff erreichen wird.
	*/
	void CalculateNextPosition();

	/**
	* Diese Funktion setzt das Schiff auf den n�chsten Punkt seiner vorher berechneten Flugroute.
	*/
	void GotoNextPosition();

	/**
	* Diese Funktion f�hrt einen Beamangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget<code>
	* gespeichert ist. Als Parameter wird dabei die Beamnummer �bergeben werden, ab welcher die Berechung durchgef�hrt
	* werden soll. Wenn w�hrend des Angriff das Ziel vernichtet wird, dann gibt die Funktion die aktuelle Beamnummer
	* zur�ck. Ansonsten gibt sie immer (-1/-1) zur�ck. Der R�ckgabewert ist eine Struktur mit 2 Variablen. Die erste
	* Variable gibt die Nummer des Beams im Feld an, die zweite die Nummer der Anzahl des Beams.
	*/
	CPoint AttackEnemyWithBeam(const CPoint& beamStart);

	/**
	* Diese Funktion f�hrt einen Torpedoangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget<code>
	* gespeichert ist. Als Parameter wird dabei ein Zeiger auf das Feld aller Torpedos im Kampf <code>CT<code>
	* �bergeben. Diese Funktion generiert dann automatisch die entsprechenden Torpedoobjekte und f�gt diese
	* in <code>pCT<code> ein. Wenn w�hrend des Angriff das Ziel vernichtet wird, dann gibt die Funktion die 
	* aktuelle Torpedonummer zur�ck. Ansonsten gibt sie immer (-1/-1) zur�ck. Der R�ckgabewert ist eine Struktur
	* mit 2 Variablen. Die erste Variable gibt die Nummer der Art des Launchers im Feld an, die zweite
	* die Nummer der Anzahl dieses Launchers.
	*/
	CPoint AttackEnemyWithTorpedo(std::list<CTorpedo*>* pCT, const CPoint& torpedoStart);

	/**
	* Diese Funktion gibt den Modifikator, den wir durch die Crewerfahrung erhalten zur�ck
	*/
	BYTE GetCrewExperienceModi();

	/**
	 * Diese Funktion gibt einen Wahrheitswert zur�ck, ob sich die Schilde schon auf eine schilddurchschlagende
	 * Waffe eingestellt haben.
	 */
	bool GetActRegShields() const {return m_bRegShieldStatus;}

	/// Funktion berechnet, ob ein Feuersystem aufgrund der Position des Schiffes, der Position des Systems auf dem Schiff und
	/// dessen Feuerwinkel auch feuern kann.
	/// @param arc Zeiger auf Schussfeld
	/// @return Wahrheitswert
	bool AllowFire(const CFireArc* arc);	

	IMeshSceneNode* GetNode() const {return m_pNode;};

	IMesh* GetMesh() const {return m_pMesh;};
	ISceneManager* GetSceneManager() const {return p_smgr;};
	int GetOrientationconst() const {return m_iOrientation;}
	ISceneNode* GetBulletParent() const {return p_BulletParent;};
	ITexture* GetTexture() const {return p_Texture;};
	void hideShip(){m_pNode->setVisible(false);};

	void SetTexture(ITexture* texture) { p_Texture = texture;};
	void SetNode(IMeshSceneNode* node) {m_pNode = node;};
	void SetMesh(IMesh* mesh)  {m_pMesh = mesh;};
	void SetOrientation (int i_o)  {m_iOrientation = i_o;}

	/// Diese Funktion setzt den Szenenmanager und erstellt einen Container f�r die Laserstahlen
	/// @param beamWeapon smgr Pointer zum Szenenmanager
	void SetSceneManager(ISceneManager* smgr) {
    	// logical parent for the bullets
			p_BulletParent = smgr->addEmptySceneNode();
			if ( p_BulletParent )
		p_BulletParent->setName ( "Bullet Container" );			
		p_smgr = smgr;}
	
private:
	// private Funktionen
	
	/// Diese Funktion berechnet den Angriffsbonus, den Schiffe auf Grund ihrer Spezialeigenschaften wom�glich erhalten.
	/// @return Angriffsbonus durch Schiffseienschaften
	BYTE GetAccBoniFromSpecials(void);

	/// Diese Funktion berechnet die Trefferwahrscheinlichkeit des Beams und f�gt dem Feindschiff wenn m�glich Schaden zu.
	/// @param beamWeapon Nummer der Beamwaffe
	/// @param distance Distanz zwischen unserem Schiff und dem Gegner
	/// @param boni Bonus durch Schiffseigenschaften
	void FireBeam(int beamWeapon, int distance, BYTE boni);

	/// Diese Funktion berechnet die Trefferwahrscheinlichkeit des Beams und f�gt dem Feindschiff wenn m�glich Schaden zu.
	/// @param pCT Feld der Torpedos im Kampf
	/// @param beamWeapon Nummer der Beamwaffe
	/// @param targetKO Zielkoordinate des Torpedos
	/// @param boni Bonus durch Schiffseigenschaften
	/// @return maximal zu erwartender Schaden
	UINT FireTorpedo(std::list<CTorpedo*>* pCT, int torpedoWeapon, const vec3i& targetKO, BYTE boni);

	/// Funktion berechnet die Route zum Ziel
	/// @param ptTarget Zielkoordinate im Raum
	/// @param nMinDistance Liegt die Zielkoordinate weniger als dieser Wert vom Startpunkt entfernt, so wird ein zuf�lliges neues Ziel angesteuert
	void CalcRoute(const vec3i& ptTarget, int nMinDistance = 0);

	// Attribute
	
	/// Zeiger auf das Schiff, welches hier im Kampf ist
	CShip*	m_pShip;
	
	/// Aktuelle Position (Koordinate im Raum) des Schiffes
	vec3i m_KO;					
	
	/// Flugroute des Schiffes, welche die folgenden Koordinaten beinhaltet
	std::deque<vec3i> m_lRoute;	
	
	/// Zeit bis das Schiff wieder seine Waffen abfeuern kann. Wenn dieser Wert NULL erreicht hat, dann kann es
	/// die Waffen wieder feuern.
	ShootTime m_Fire;
	
	/// werden Pulsebeams geschossen? Nur f�r den CombatSimulator
	bool m_bPulseFire;
	
	/// Die Man�vrierbarkeit des Schiffes im Kampf.
	BYTE m_byManeuverability;

	/// Der Schadens- und Verteidigungsbonus/malus der Schiffe der Rasse
	USHORT m_iModifier;

	/// Zeiger auf das Schiff, welches es als Ziel erfasst hat
	CCombatShip* m_pTarget;

	/// Status der regenerativen Schilde, angepa�t oder nicht 
	bool m_bRegShieldStatus;

	/// Ist das Schiff noch getarnt oder nicht. Nach dem Feuern hat das Schiff noch 50 bis 70 Ticks Zeit,
	/// bis es wirklich angegriffen werden kann. Solange m_byCloak gr��er als NULL ist, ist das Schiff getarnt.
	BYTE m_byCloak;

	/// Wenn dieser Counter auf 255 hochgez�hlt wurde, dann kann sich das Schiff wieder tarnen
	BYTE m_byReCloak;

	/// Hat das Schiff schonmal getarnt geschossen?
	bool m_bShootCloaked;

	// Auswirkungen durch Anomalien im Sektor
	/// K�nnen Schilde verwendet werden
	bool m_bCanUseShields;

	/// k�nnen Torpedos abgefeuert werden
	bool m_bCanUseTorpedos;

	/// Laden sich Schilde schneller auf
	bool m_bFasterShieldRecharge;

	/// Beim R�ckzugbefehl muss dieser Counter runtergez�hlt sein
	BYTE m_byRetreatCounter;

	IMeshSceneNode* m_pNode;		    // Animation Schiffsmodell 
	IMesh* m_pMesh;						// Schiffsmodell
	ISceneManager* p_smgr;				// Szenenmanager
	int	m_iOrientation;					// Ausrichtung im 2D Raum (0 = Postitive X-Achse  360 Grad)
	ISceneNode* p_BulletParent;			//Container f�r Laserbeams
	ITexture* p_Texture;				//Die Textur
	/// Der �ber den Schiff angezeigte Infosting wird aktualisiert.
	void updateInfoString();

	int m_Race;							//Num�mer der Rasse

	float SCAL;
	float LIFE;
	float SHIELDS;
};

typedef CArray<CCombatShip,CCombatShip> CombatShips;