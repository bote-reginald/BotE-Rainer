#pragma once

class CShipMap;
class CMajor;
class CMinor;

enum GLOBALEVENTTYPE
{
	GLOBALEVENTSYSTEM = 0,
	GLOBALEVENTRESEARCH = 1,
	GLOBALEVENTMINOR = 2
};

enum SYSTEMEVENTTYPE
{
	SYSTEMEVENTMORALBOOST=0,// Moral bonus
	SYSTEMEVENTMORALMALUS=1,// Moral malus
	SYSTEMEVENTPLANETMOVEMENT=2,
	SYSTEMEVENTDEMOGRAPHIC=3
};
enum EXPLOREEVENTTYPE
{
	ALIENTEC=0,
	EVENTSHIPXP=1
};

/// Singleton-Klasse zum Berechnen der Random Events (Random Event Manager)
class CRandomEventCtrl : public CObject
{
private:
	/// Konstruktor private, damit man sich keine Instanzen holen kann.
	CRandomEventCtrl(void);

	/// Den Kopierkonstruktor sch�tzen um zu vermeiden, dass das Objekt unbeabsichtigt kopiert wird.
	CRandomEventCtrl(const CRandomEventCtrl& cc);

public:
	/// Standarddestruktor
	~CRandomEventCtrl(void);

	/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
	/// @return Instanz dieser Klasse
	static CRandomEventCtrl* GetInstance(void);

	/// Liest aus der Ini-Datei, ob Zufallsereignisse aktiviert sind
	/// @return <code>true</code> wenn Zufallsereignisse aktiviert, sonst <code>false</code>
	bool IsActivated() const { return m_bIsActivated; }

	///Berechnet Randomevents f�r die gegebene Majorrace
	void CalcEvents(CMajor* pRace);

	///Berechnet Events wenn ein Sektor erkundet wird
	void CalcExploreEvent(const CPoint& ko, CMajor* pRace, CShipMap* ships);

	/// Funktion berechnet Schiffsevents
	void CalcShipEvents() const;

private:
	// F�r Random Events die nur ein System betreffen
	bool SystemEvent(const CPoint &ko, CMajor* pRace);

	//F�r globale Events
	void GlobalEventResearch(CMajor* pRace);
	void GlobalEventMinor(CMajor* pRace, CMinor* pMinor);

	// Attribute
	const unsigned m_uiGlobalProb;//0-100
	const unsigned m_uiProbPerSystem;//0-1000
	const unsigned m_uiProbPerMinor;//0-1000

	bool m_bIsActivated;
};
