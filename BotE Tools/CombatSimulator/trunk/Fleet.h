/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Fleet.h: Schnittstelle f�r die Klasse CFleet.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLEET_H__EBC5B38C_84F8_4C71_B3CE_03B4ECFA1D1E__INCLUDED_)
#define AFX_FLEET_H__EBC5B38C_84F8_4C71_B3CE_03B4ECFA1D1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ship.h"

class CFleet : public CObject  
{	
public:
	DECLARE_SERIAL (CFleet)
	// Standardkonstruktor
	CFleet();
	
	// Destruktor
	virtual ~CFleet();
	
	// Kopierkonstruktor
	CFleet(const CFleet & rhs);
	
	// Zuweisungsoperatur
	CFleet & operator=(const CFleet &);

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Funktion gibt einen Zeiger auf ein Schiff aus der Flotte zur�ck
	CShip* GetShipFromFleet(int n) { return &m_vShips.ElementAt(n); }
	
	// Funktion um ein Schiff zur Flotte hinzuzuf�gen
	void AddShipToFleet(const CShip* pShip) { m_vShips.Add(*pShip); }
	
	// Funktion um ein Schiff aus der Flotte zu entfernen.
	void RemoveShipFromFleet(UINT nIndex);
	
	// Funktion liefert die Anzahl der Schiffe in der Flotte
	USHORT GetFleetSize() const { return m_vShips.GetSize(); }
	
	// Funktion berechnet die Geschwindigkeit der Flotte. Der Parameter der hier �bergeben werden sollte
	// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
	BYTE GetFleetSpeed(const CShip* ship = NULL) const;
	
	// Funktion berechnet die Reichweite der Flotte. Der Parameter der hier �bergeben werden sollte
	// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
	BYTE GetFleetRange(const CShip* ship = NULL) const;
	
	// Funktion berechnet den Schiffstyp der Flotte. Wenn hier nur der selbe Schiffstyp in der Flotte vorkommt,
	// dann gibt die Funktion diesen Schiffstyp zur�ck. Wenn verschiedene Schiffstypen in der Flotte vorkommen,
	// dann liefert und die Funktion ein -1. Der Parameter der hier �bergeben werden sollte ist der this-Zeiger
	// des Schiffsobjektes, welches die Flotte besitzt
	short GetFleetShipType(const CShip* ship) const;

	// Funktion berechnet die minimale Stealthpower der Flotte. Der Parameter der hier �bergeben werden sollte
	// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
	BYTE GetFleetStealthPower(const CShip* ship = NULL) const;
	
	// Funktion �bernimmt die Befehle des hier als Zeiger �bergebenen Schiffsobjektes an alle Mitglieder der Flotte
	void AdoptCurrentOrders(const CShip* ship);

	// Diese Funktion liefert TRUE wenn die Flotte den "order" ausf�hren kann. Als Schiffszeiger mu� das Schiff
	// �bergeben werden, welches die Flotte beinhaltet. Kann die Flotte den Befehl nicht befolgen liefert die
	// Funktion FALSE zur�ck
	BOOLEAN CheckOrder(const CShip* ship, BYTE order) const;
	
	// Funktion l�scht die gesamte Flotte
	void DeleteFleet(void) { m_vShips.RemoveAll(); }

private:
	ShipArray m_vShips;		// Die Schiffe in der Flotte, au�er Schiff selbst, welches das Flottenobjekt besitzt
};

#endif // !defined(AFX_FLEET_H__EBC5B38C_84F8_4C71_B3CE_03B4ECFA1D1E__INCLUDED_)
