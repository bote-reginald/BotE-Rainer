/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Starmap.h: interface for the CStarmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STARMAP_H__28F3FFEA_139A_45A5_AFF3_76F41E48496B__INCLUDED_)
#define AFX_STARMAP_H__28F3FFEA_139A_45A5_AFF3_76F41E48496B__INCLUDED_

// nur zu Demozwecken, sollte auskommentiert werden!
// #define DEBUG_AI_BASE_DEMO

#include "Constants.h"
#include "SortedList.h"
#include <list>
#include <set>
#include <map>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// die verschiedenen Reichweiten; je gr��er der Wert, desto n�her ist ein Au�enposten
#define SM_RANGE_SPACE		0		///< au�erhalb aller Reichweiten
#define SM_RANGE_FAR		1		///< gro�e Entfernung
#define SM_RANGE_MIDDLE		2		///< mittlere Entfernung
#define SM_RANGE_NEAR		3		///< kurze Entfernung


/// Struktur, die die Koordinaten eines Sektors aufnehmen kann
struct Sector
{
	Sector() : x(-1), y(-1) {}
	Sector(char _x, char _y) : x(_x), y(_y) {}
	Sector(const Sector &other) : x(other.x), y(other.y) {}
	Sector(const CPoint& p) : x((char)p.x), y((char)p.y) {}
	int operator==(const Sector &other) const {return (other.x == x && other.y == y);}
	Sector& operator=(const Sector &other) {x = other.x; y = other.y; return *this;}
	Sector operator+(const Sector &other) {return Sector(x + other.x, y + other.y);}
	int operator!=(const Sector &other) const {return (other.x != x || other.y != y);}

	/// Funktion, welche pr�ft, ob dies ein g�ltiger Sektor ist
	bool on_map() const {
		return 0 <= x && x < STARMAP_SECTORS_HCOUNT && 0 <= y && y < STARMAP_SECTORS_VCOUNT;
	}

	char x, y;
};

/// Struktur, die Informationen eines Sektors f�r die Berechnung eines k�rzesten Weges aufnimmt
struct PathSector
{
	bool used;			///< <code>true</code>, wenn der Knoten bereits gew�hlt wurde, sonst <code>false</code>
	double distance;	///< Entfernung vom Start
	int hops;			///< Anzahl der Hops vom Start-Sektor
	Sector parent;		///< Verweis auf Elternknoten

	Sector position;	///< Koordinaten des Sektors
};

/// Komparator f�r PathSector-Objekte (vergleicht die distance-Werte)
class PathSectorComparator
{
public:
	/// liefert -1 wenn a &lt; b, 0 wenn a == b und 1 wenn a &gt; b
	int operator()(const PathSector *a, const PathSector *b) const
	{
		if (a->distance < b->distance)
			return -1;
		else if (a->distance == b->distance)
			return 0;
		else
			return 1;
	}
};

/// Liste, die Nachbar-Sektoren mit Abst�nden aufnehmen kann (f�r CalcPath)
typedef CSortedList<PathSector, PathSectorComparator> LeafList;

/// Doppelt verkettete List mit Sector-Eintr�gen
typedef std::list<Sector> SECTORLIST;

/// Nimmt Informationen �ber Sektoren und deren Bewertung auf.
struct BaseSector
{
	Sector position;		///< Koordinaten des Sektors
	short points;			///< Bewertung des Sektors

	BaseSector() : position(Sector()), points(-1) {}

	bool operator<(const BaseSector &other) const
	{
		// wollen absteigend sortieren
		return points > other.points;
	}

};

typedef struct RangeMap
{
	unsigned char *range;		///< Array, das die Reichweitenmatrix zeilenweise enth�lt
	char w;						///< Anzahl der Spalten der Matrix
	char h;						///< Anzahl Zeilen der Matrix

	/// nullbasierter Index einer Spalte; die Matrix wird so ausgerichtet, dass diese Spalte �ber dem Sektor eines Au�enpostens steht
	char x0;
	/// nullbasierter Index einer Zeile; die Matrix wird so ausgerichtet, dass diese Zeile �ber dem Sektor eines Au�enpostens steht
	char y0;
}
RangeMap;

/**
 * Klasse, die f�r einen Spieler s�mtliche Infos der Starmap enth�lt. Enth�lt Methoden zur Berechnung von Reichweiten,
 * Flugrouten (k�rzester Weg) und Sektoren zum Au�enpostenbau durch die KI.
 *
 * @author CBot
 * @version 0.0.2
 */
class CSector;
class CStarmap
{
	friend class CGalaxyMenuView;

protected:
	/// Map, die die Entfernung vom n�chsten Au�enposten aufnimmt; der erste Index ist die x-,
	/// der zweite die y-Koordinate eines Sektors (modelliert Einfluss, den der Au�enposten hat)
	std::vector<std::vector<unsigned char>> m_Range;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];
	/// Koordinaten des aktuell ausgew�hlten Sektors; (-1, -1) wenn keiner ausgew�hlt ist
	// (orange rectangle visible in galaxy view)
	Sector m_Selection;

	/// Liste der Sektoren, in denen sich ein Au�enposten befindet
	SECTORLIST m_lBases;

	/// zu verwendende lokale RangeMap, f�r nachfolgend hinzugef�gte Au�enposten
	RangeMap m_RangeMap;

	/// bestimmte Sektoren sind gef�hrlich und sollten daher nicht prim�r angeflogen werden
	static double **m_BadMapModifiers;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

	/// Array, das Informationen zur Berechnung der k�rzesten Wege aufnimmt
	std::vector<std::vector<PathSector>> pathMap;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];
	/// Liste der Nachbarn, die von den aktuellen Bl�ttern aus erreichbar sind
	/// (werden als n�chstes zu Bl�ttern des Baumes)
	LeafList leaves;

	Sector pathStart;				///< Start-Sektor beim letzten Aufruf von CalcPath(), {-1, -1} wenn nicht gesetzt
	unsigned char pathRange;		///< Reichweite beim letzten Aufruf von CalcPath(), 0 wenn nicht gesetzt


	BOOL m_bAICalculation;			///< zus�tzliche Berechnungen f�r den automatischen Au�enpostenbau aktiviert?
	char m_nAIRange;				///< maximale Reichweite, innerhalb derer Au�enposten gebaut werden sollen und die zusammenh�ngen soll
	SECTORLIST m_lAITargets;		///< Ziele, zu denen sich bevorzugt ausgebreitet werden soll
	SECTORLIST m_lAIKnownSystems;	///< Liste von der KI bekannten Systemen (durch Scannen)

	/// Gebietszuwachs f�r Sektor (x, y), wenn in diesem Sektor ein Au�enposten gebaut w�rde
	std::vector<std::vector<short>> m_AIRangePoints;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

	/// f�r Sektoren au�erhalb der gegebenen Reichweite die Anzahl der Nachbarn innerhalb der Reichweite
	std::vector<std::vector<unsigned char>> m_AINeighbourCount;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];
	/// Bewertung f�r den Zusammenhang eines Gebiets, um L�cken zu vermeiden und nicht zusammenh�ngende Gebiete zu verbinden
	std::vector<std::vector<short>> m_AIConnectionPoints;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

	/// Bewertung f�r bevorzugte Ausbreitungsrichtungen
	std::vector<std::vector<short>> m_AITargetPoints;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

	/// negative Bewertung f�r ein Gebiet durch feindliche Grenzen. Man sollte keinen Aussenposten auf einen freien
	/// Fleck inmitten des gegnerischen Gebiets bauen.
	std::vector<std::vector<short>> m_AIBadPoints;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

public:
	/**
	 * @param bAICalculation aktiviert zus�tzliche Berechnungen, die den automatischen Au�enpostenbau erm�glichen
	 * @param nAIRange maximale Reichweite, innerhalb derer Au�enposten gebaut werden sollen und die zusammenh�ngen soll
	 */
	CStarmap(BOOL bAICalculation = FALSE, char nAIRange = SM_RANGE_NEAR);
	virtual ~CStarmap();

	/**
	 * Zeiger der statischen Variablen l�schen
	 */
	static void DeleteStatics();

	/**
	 * Funktion gibt den Wert aus der RangeMap <code>m_Range</code> an einer bestimmten Stelle <code>p</p> auf der
	 * Karte zur�ck.
	 */
	unsigned char GetRange(const CPoint &p) const {return (3 - m_Range[p.x][p.y]);}

	/**
	 * Ermittelt die Koordinaten eines angeklickten Sektors. <code>pt</code> gibt die ungezoomten Mauskoordinaten
	 * bez�glich der linken oberen Ecke der gesamten Starmap an.
	 */
	static Sector GetClickedSector(const CPoint& pt);
	/**
	 * Liefert die Koordinaten der linken oberen Ecke des angegebenen Sektors.
	 */
	static CPoint GetSectorCoords(const Sector& sector);

	/**
	 * W�hlt den angegebenen Sektor aus. Bei Angabe von (-1, -1) hat diese Methode keine Wirkung. Zum Entfernen der
	 * Markierung muss Deselect() verwendet werden.
	 */
	void Select(const Sector &sector);
	/**
	 * Entfernt die Markierung.
	 */
	void Deselect();
	/**
	 * Liefert die Koordinaten des ausgew�hlten Sektors. Liefert {-1, -1}, wenn kein Sektor ausgew�hlt ist.
	 */
	Sector GetSelection() const {return m_Selection;}

	/**
	 * Setzt die f�r nachfolgend hinzugef�gte Au�enposten zu verwendende RangeMap.
	 */
	void SetRangeMap(unsigned char rangeMap[], char w, char h, char x0, char y0);

	/**
	 * F�gt im angegebenen Sektor einen Au�enposten hinzu.
	 */
	void AddBase(const Sector& sector, BYTE propTech);

	/**
	 * @return <code>TRUE</code>, wenn im angegebenen Sektor ein Au�enposten hinzugef�gt wurde, sonst <code>FALSE</code>.
	 */
	BOOL IsBase(const Sector &sector);

	/**
	 * Schaltet die komplette Map frei, so dass �berall mit kurzer Reichweite hingeflogen werden kann
	 */
	void SetFullRangeMap(int nRange = SM_RANGE_NEAR, const std::vector<Sector>& vExceptions = std::vector<Sector>());

	/**
	 * Gleicht die komplette Rangemap mit der Galaxiemap ab. Gebiete, welche einer Rasse geh�ren mit der wir einen
	 * Nichtangriffspakt haben, werden von der Rangemap entfernt. �bergeben werden daf�r ein Zeiger auf alle
	 * Sektoren <code>sectors</code> und ein Wahrheitswert <code>races</code> f�r alle Rassen, ob wir einen
	 * Nichtangriffspakt mit dieser Rasse haben.
	 */
	void SynchronizeWithMap(const std::vector<CSector>& sectors, const std::set<CString>* races);

	/**
	 * F�hrt f�r gef�hrliche Anomalien mathematische Gewichte hinzu, so dass dieser Sektor bei der automatischen
	 * Wegsuche nicht �berflogen wird.
	 */
	static void SynchronizeWithAnomalies(const std::vector<CSector>& sectors);

	/**
	 * L�scht alle Basen und setzt die Eintr�ge in der Rangemap wieder auf Ausgangswert
	 */
	void ClearAll();

	/**
	 * Berechnet den k�rzesten Weg von position nach target innerhalb der durch range angegebenen Reichweite. Liefert den
	 * kompletten Weg in path zur�ck. R�ckgabewert ist der entsprechend speed als n�chstes zu erreichende Punkt auf
	 * dem berechneten Weg. Tritt ein Fehler auf (z.B. wenn das Ziel nicht erreichbar ist), liefert die Funktion ein
	 * Sector-Objekt, dessen Koordinaten auf -1 gesetzt sind. Die in path bereits vorhandenen Eintr�ge werden gel�scht,
	 * die Gr��e des Arrays wird auf die L�nge des gelieferten Weges gesetzt.
	 */
	Sector CalcPath(const Sector &position, const Sector &target, unsigned char range,
		unsigned char speed, CArray<Sector> &path);

	/**
	 * F�gt der Liste der bevorzugten Ausbreitungsrichtungen ein weiteres Ziel hinzu. Dient der Ermittlung eines Sektors,
	 * in dem ein Au�enposten gebaut werden sollte, um sich dem Ziel zu n�hern; auch bei weiter entfernten Zielen.
	 */
	void AddTarget(const Sector &target);

	/**
	 * @return <code>TRUE</code>, wenn der angegebene Sektor als Ziel hinzugef�gt wurde, sonst <code>FALSE</code>.
	 */
	BOOL IsTarget(const Sector &sector);

	/**
	 * F�gt der Liste der der KI bekannten Systeme ein weiteres System hinzu. Es wird bevorzugt versucht, diese Systeme
	 * in die gegebene Reichweite aufzunehmen, wenn sie dem bereits erschlossenen Gebiet nahe sind (etwas weniger aggresiv
	 * als die Ausbreitung zu einem Ziel).
	 */
	void AddKnownSystem(const Sector &sector);

	/// @return <code>TRUE</code>, wenn sich im angegebenen Sektor ein bekanntes System befindet, sonst <code>FALSE</code>.
	BOOL IsKnownSystem(const Sector &sector);

	/**
	 * Funktion erstellt anhand von Sektoren eine Matrix mit Werten, welche zur Berechnung der besten Position f�r
	 * einen Au�enposten genutzt werden soll. �bergeben werden daf�r ein Zeiger auf alle
	 * Sektoren <code>sectors</code> und die Rasse, zu welcher das Starmap Objekt geh�rt <code>race</code>.
	 */
	void SetBadAIBaseSectors(const std::vector<CSector>& sectors, const CString& race);

	/**
	 * Ermittelt einen Sektor, in dem g�nstig ein Au�enposten gebaut werden k�nnte. Liefert <code>Sector(-1, -1)</code> im
	 * Feld <code>position</code> und <code>-1</code> f�r <code>points</code>, wenn nirgendwo ein Au�enposten gebaut werden
	 * kann oder dieses Objekt mit <code>bAICalculation == FALSE</code> erzeugt wurde.
	 *
	 * @param variance maximal m�gliche Abweichung gegen�ber der besten Bewertung; aus [0, 1], 0: keine Abweichung,
	 * 1: 100% Abweichung m�glich
	 */
	BaseSector CalcAIBaseSector(double variance = 0.1);

	/**
	 * @return Gesamtbewertung des angegebenen Sektors. Zuvor m�ssen die Funktionen zur Neuberechnung der Bewertung aufgerufen worden sein.
	 */
	short GetPoints(const Sector &sector) const;

private:
	/// Diese Funktion berechnet die Reichweitenkarte anhand der aktuellen Techstufe <code>propTech</code> und schreibt
	/// das Ergebnis in den Parameter <code>rangeMap</code>. Zus�tzlich werden Referenzen auf paar Hilfsvariablen
	/// �bergeben.
	void CalcRangeMap(BYTE propTech);

	/// @return Eintrag der gesetzten RangeMap an der Stelle (x, y), wobei (0, 0) die Au�enpostenposition ist
	unsigned char GetRangeMapValue(char x, char y);

	/// Berechnet die Gebietszuw�chse neu.
	void RecalcRangePoints();

	/// Berechnet die Bewertungen zum Verbinden benachbarter Gebiete und Verhindern von L�cken.
	void RecalcConnectionPoints();

	/// Berechnet Bewertungen f�r bevorzugte Ausbreitungsrichtungen.
	void RecalcTargetPoints();

	//(Re)Initializes some of the vector members
	void InitSomeMembers();
};

#endif // !defined(AFX_STARMAP_H__28F3FFEA_139A_45A5_AFF3_76F41E48496B__INCLUDED_)
