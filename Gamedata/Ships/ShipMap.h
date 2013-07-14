
/*
 *@file
 * object to hold ships
 * The point of this file is to try as muhc as possible to keep iterators valid. All adding/remocing of ships
 * should be done here. Whenever a parameter is called "key", it refers to the key holding a CShips object
 * in the internal std::map; if it's called index, it refers to an array-like index starting with 0 at the begin()
 * iterator of the internal map.
 */

#pragma once

#include <map>

class CShips;

class CShipMap
{
public:
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

	CShipMap(bool bDestroyWhenDestructed = true);
	virtual ~CShipMap(void);
	CShipMap(const CShipMap& o);
	CShipMap& operator=(const CShipMap& o);

//////////////////////////////////////////////////////////////////////
// iterators
//////////////////////////////////////////////////////////////////////

	typedef std::map<unsigned, CShips*>::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	typedef std::map<unsigned, CShips*>::iterator iterator;
	iterator begin();
	iterator end();

	//get iterator to the CShips with the given key
	//returns end iterator if key not found
	//invalidates if element it points to is erased
	//complexity: logarithmic
	const_iterator find(unsigned key) const;

	//get iterator to the CShips with the given key
	//returns end iterator if key not found
	//invalidates if element it points to is erased
	//complexity: logarithmic
	iterator find(unsigned key);

	//get iterator to the CShips with the given index
	//invalidates if element it points to is erased
	//complexity: linear, avoid usage
	const_iterator iterator_at(int index) const;

	//get iterator to the CShips with the given index
	//invalidates if element it points to is erased
	//complexity: linear, avoid usage
	iterator iterator_at(int index);

//////////////////////////////////////////////////////////////////////
// adding elements
//////////////////////////////////////////////////////////////////////

	//adds the passed CShips at the end of this shiparray
	//@param ship: the ship to add
	//@return an iterator to the newly inserted element
	//complexity: constant
	iterator Add(CShips* ship);

	//appends the passed CShipMap at the end of this shiparray
	//@param other: the CShipMap to append
	void Append(const CShipMap& other);

//////////////////////////////////////////////////////////////////////
// removing elements
//////////////////////////////////////////////////////////////////////

	//@param destroy: Needs to be true in case the erased ships are removed from the game at all.
	//On the other hand, needs to be false in case these ships are just moved into or out of
	//a fleet and thus the memory where they are allocated must not be deleted.
	//If in doubt, better take false. Wrong false should cause a memory leak, wrong true dangling pointers.
	void Reset(bool destroy);

	//removes the element pointed to by the passed iterator from this shipmap
	//@param index: will be updated and point to the new position of the element which followed the erased one
	//@param destroy: Needs to be true in case the erased ship is removed from the game at all because it is
	//destroyed. On the other hand, needs to be false in case the removed ship is just moved into or out of
	//a fleet and thus the memory where it's allocated must not be deleted.
	//If in doubt, better take false. Wrong false should cause a memory leak, wrong true a dangling ship pointer.
	//The shipmap must not be empty before erasing.
	//In case it is afterwards, it is reset as if it was freshly constructed.
	//complexity: constant
	void EraseAt(CShipMap::iterator& index, bool destroy);

//////////////////////////////////////////////////////////////////////
// getting elements
//////////////////////////////////////////////////////////////////////

private:
	//get CShips at the given array-like index
	//complexity: linear, avoid usage
	const CShips& GetAt(int index) const;
public:

	//get the CShips with the given key
	//complexity: logarithmic
	const CShips& at(unsigned key) const;

	//get the CShips with the given key
	//complexity: logarithmic
	CShips& at(unsigned key);

//////////////////////////////////////////////////////////////////////
// getting info
//////////////////////////////////////////////////////////////////////

	int GetSize() const;

	//get highest valid index
	//complexity: constant
	int GetUpperBound() const;

	bool empty() const;

private:
	//don't make this public please, as it runs with O(shipmap size)
	//use the ship's key in the map instead
	friend class CTest;//In that class is the only call to index_of(), which we need to
	//test iterator_at against it.

	//get array-index of the given iterator
	//complexity: linear, avoid usage; use the m_Key member of the CShips class
	//to get and remember a CShips object in this shipmap
	int index_of(const CShipMap::const_iterator& position) const;
public:

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktionen
//////////////////////////////////////////////////////////////////////

	void Serialize(CArchive& ar);

	//special purpose serialization function
	void SerializeEndOfRoundData(CArchive& ar, const CString& sMajorID);

	//special purpose serialization function
	void SerializeNextRoundData(CArchive& ar, const CPoint& ptCurrentCombatSector);

//////////////////////////////////////////////////////////////////////
// special ships
//////////////////////////////////////////////////////////////////////

	void SetCurrentShip(unsigned key);
	void SetCurrentShip(const CShipMap::iterator& position);

	//Get a reference to the iterator pointing to the currently marked ship.
	//This iterator is only kept valid, but not semantically correct, so expect it to change in a way
	//which can make your code buggy.
	const CShipMap::iterator& CurrentShip() const;

	void SetFleetShip(unsigned key);
	void SetFleetShip(const CShipMap::iterator& position);

	//Get a reference to the iterator pointing to the currently marked ship.
	//This iterator is only kept valid, but not semantically correct, so expect it to change in a way
	//which can make your code buggy.
	const CShipMap::iterator& FleetShip() const;

	//Get leader (of fleet) of given ship (it can be the ship itself)
	CShips* GetLeader(const CShips* pShip) const;

private:
	void UpdateSpecialShip(CShipMap::iterator& ship, const CShipMap::const_iterator& to_erase);

//////////////////////////////////////////////////////////////////////
// debugging helper
//////////////////////////////////////////////////////////////////////

	//CString ToString() const;

	unsigned NextKey();

	std::map<unsigned, CShips*> m_Ships;// internal container with the CShips
	unsigned m_NextKey;// used to give newly added CShips their keys
	//This counter is incremented each time a ships is added, and only resetted when the shipmap is completely
	//empty, when loading a savegame and at turn change.
	bool m_bDestroyWhenDestructed;// Once that this shipmap is destructed, should it delete the memory allocated
	//to the ships it (still) contains ?
	//Set this to false in case this shipmap is a local variable and you want to reuse the ships it contains
	//later on.

	CShipMap::iterator m_CurrentShip;// Hilfsvariable, mit der auf ein spezielles Schiff im Array zugekriffen werden kann
	CShipMap::iterator m_FleetShip;// Das Schiff welches sozusagen die Flotte anf�hrt
	//These iterator variables are tried to keep always valid; the only case when they are not safely
	//dereferencable should be if this shipmap is completely empty. This can be checked for with the empty()
	//member function.

};
