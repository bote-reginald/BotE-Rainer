/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Constants.h"

// forward declaration
class CBotEDoc;
class CMajor;
class CBuildingInfo;
class CSystemAI
{
public:
	/// Konstruktor
	CSystemAI(CBotEDoc* pDoc);

	/// Destruktor
	~CSystemAI(void);

	/// Diese Funktion f�hrt die Berechnungen f�r die k�nstliche Intelligenz in einem System aus und macht
	/// alle m�glichen Eintr�ge und Ver�nderungen. Als Paramter wird daf�r die Koordinate <code>ko</code>
	/// des Systems �bergeben.
	void ExecuteSystemAI(CPoint ko);

private:
	//AI does not know how to use troops. Most of the time they are only harmfull (reduce moral, maintenance costs).
	//So just remove any start troops.
	//A human would pack them into a transporter and use it up to build an outpost.
	void KillTroops();

	/// Diese Funktion kauft unter Umst�nden den aktuellen Bauauftrag. Somit kommt der Ausbaue schneller voran.
	void PerhapsBuy();

	/// Diese Funktion legt die Priorit�ten an, mit welcher ein bestimmtes Arbeitergeb�ude gebaut werden soll.
	void CalcPriorities();


	bool CheckMoral(const CBuildingInfo& bi, bool build) const;

	/// Diese Funktion w�hlt ein zu bauendes Geb�ude aus der Liste der baubaren Geb�ude. Es werden nur Geb�ude
	/// ausgew�hlt, welche in die Priorit�tenliste passen. Der R�ckgabewert ist die ID des Bauauftrages.
	/// Wird <code>0</code> zur�ckgegeben, so wurde kein Geb�ude gefunden, welches in das Anforderungsprofil passt.
	int ChooseBuilding();

	/// Diese Funktion w�hlt ein zu bauendes Schiff aus der Liste der baubaren Schiffe. Der R�ckgabewert ist die ID
	/// des Schiffes (also gr��er 10000!). Wird <code>0</code> zur�ckgegeben, so wurde kein baubares Schiffe gefunden.
	/// �bergeben wird daf�r die Priorit�t <code>prio</code> des zu bauenden Schiffes (mittels Funktion <function>
	/// GetShipBuildPrios</function> ermitteln) und Wahrheitswertr, in denen steht welcher Schiffstyp gebaut werden soll.
	int ChooseShip(int prio, BOOLEAN chooseCombatship, BOOLEAN chooseColoship, BOOLEAN chooseTransport);

	/// Funktion �berpr�ft ob das Geb�ude mit der ID <code>id</code> in die Bauliste gesetzt werden kann.
	/// Wenn es in die Bauliste gesetzt werden konnte gibt die Funktion ein <code>TRUE</code> zur�ck.
	BOOLEAN MakeEntryInAssemblyList(short id);

	/// Diese Funktion besetzt die Geb�ude mit Arbeitern.
	void AssignWorkers();

	/// Diese Funktion versucht Arbeiter aus Bereichen abzuziehen. Dabei werden aber keine Arbeiter aus dem Nahrungs-
	/// und Energiebereich abgezogen. Die Funktion berechnet die neue Arbeiterverteilung, auch die freien Arbeiter, und
	/// gibt ein <code>TRUE</code> zur�ck, wenn Arbeiter abgezogen werden konnten. Ansonsten gibt sie <code>FALSE</code>
	/// zur�ck.
	BOOLEAN DumpWorker();

	/// Funktion rei�t �berfl�ssige Nahrungs- und Energiegeb�ude im System ab.
	void ScrapBuildings();

	/// Diese Funktion berechnet die neue Nahrungs-, Industrie- und Energieproduktion im System. Sie sollte aufgerufen
	/// werden, wenn Arbeiter aus diesen Gebieten ver�ndert wurden. Dabei werden aber alle anderen Produktionen
	/// gel�scht.
	void CalcProd();

	/// Diese Funktion legt m�glicherweise eine Handelsroute zu einem anderen System an. Dadurch wird halt auch die
	/// Beziehung zu Minorraces verbessert.
	void ApplyTradeRoutes();

	/// Diese Funktion berechnet die Priorit�t, mit welcher ein gewisser Schiffstyp gebaut werden soll. �bergeben werden
	/// dabei drei Referenzen auf Booleanvariablen, in welcher dann ein <code>TRUE</code> steht wurde ausgew�hlt.
	/// @return ist die Priorit�t, mit welcher der Schiffstyp gebaut werden soll.
	int GetShipBuildPrios(BOOLEAN &chooseCombatship, BOOLEAN &chooseColoship, BOOLEAN &chooseTransport);

	/// Funktion �berpr�ft, ob f�r einen bestimmten Arbeitertyp auch ein Geb�ude in der Liste der baubaren
	/// Geb�ude exisitiert. Wenn dies nicht der Fall ist, so ist auch die Priorit�t solch ein Geb�ude zu bauen
	/// gleich Null.
	/// @param nWorker Arbeitertyp (FOOD_WORKER, INDUSTRY_WORKER usw.)
	/// @return <code>TRUE</code> wenn ein Geb�ude baubar ist, ansonsten <code>FALSE</code>
	bool CheckBuilding(WORKER::Typ nWorker) const;

	/// Funktion berechnet die Priorit�t der Nahrungsmittelproduktion im System
	/// @param dMaxHab maximale Bev�lkerungszahl im System
	/// @return Priotit�t der Nahrungsmittelproduktion
	int GetFoodPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorit�t der Industrieproduktion im System
	/// @param dMaxHab maximale Bev�lkerungszahl im System
	/// @return Priotit�t der Industrieproduktion
	int GetIndustryPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorit�t der Industrieproduktion im System
	/// @param dMaxHab maximale Bev�lkerungszahl im System
	/// @return Priotit�t der Industrieproduktion
	int GetEnergyPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorit�t der Geheimdienstproduktion im System
	/// @param dMaxHab maximale Bev�lkerungszahl im System
	/// @return Priotit�t der Geheimdienstproduktion
	int GetIntelPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorit�t der Forschungsproduktion im System
	/// @param dMaxHab maximale Bev�lkerungszahl im System
	/// @return Priotit�t der Forschungsproduktion
	int GetResearchPrio(double dMaxHab) const;

	/// Funktion berechnet die Priorit�t einer bestimmten Ressourcenproduktion im System
	/// @param nWorker Arbeiter f�r eine bestimmte Ressource
	/// @param dMaxHab maximale Bev�lkerungszahl im System
	/// @return Priotit�t der Forschungsproduktion
	int GetResourcePrio(WORKER::Typ nWorker, double dMaxHab) const;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// Ein Zeiger auf das Document.
	CBotEDoc* m_pDoc;

	/// Die Koordinate des Systems, in welchem gerade die KI-Berechnungen durchgef�hrt werden sollen
	CPoint m_KO;

	/// Die Priorit�ten der einzelnen Arbeitergeb�ude
	int m_iPriorities[10];

	/// Was sollte am ehesten gebaut werden? 0 - Geb�ude, 1 - Upgrade, 2 - Schiff, 3 - Truppen
	BYTE m_bBuildWhat;

	/// Wurden die Priorit�ten berechnet, so nimmt diese Variable den Wert <code>TRUE</code> an. Ansonsten ist
	/// ihr Wert <code>FALSE</code>.
	BOOLEAN m_bCalcedPrio;

	/// auf diese Variable wird w�hrend der Berechnung die Majorrace gespeichert, der das System geh�rt.
	CMajor* m_pMajor;
};
