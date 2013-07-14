#pragma once

#include "afxmt.h"

/**
 * Basisklasse f�r Klassen, deren Objekte mit Hilfe von <code>CReference</code> und
 * <code>CReferenceTransfer</code> referenzgez�hlt werden sollen.
 *
 * @author CBot
 * @version 0.0.1
 */
class CReferenceCounter
{
private:
	UINT m_nCounter;							///< Referenzz�hler
	CCriticalSection m_CriticalSection;

public:
	/**
	 * Erzeugt ein Objekt. Initialisiert den Referenzz�hler mit <code>0</code>. Anschlie�end
	 * wird mit der Konstruktion von <code>CReference</code> oder <code>CReferenceTransfer</code>
	 * <code>Pin()</code> aufgerufen, so dass ein sp�terer Aufruf von <code>Release()</code>
	 * dieses Objekt zerst�rt.
	 */
	CReferenceCounter();
	virtual ~CReferenceCounter();

	/**
	 * @return aktueller Referenzz�hler, nach der Konstruktion <code>0</code>, nach dem ersten
	 * Aufruf von <code>Pin()</code> immer <code>&gt;= 1</code>
	 */
	inline UINT GetReferenceCount() const {return m_nCounter;}

	/**
	 * Erh�ht den Referenzz�hler. Diese Methode ist synchronisiert.
	 */
	void Pin();
	/**
	 * Dekrementiert den Referenzz�hler. Zerst�rt dieses Objekt, wenn der Z�hler mit dem Aufruf
	 * dieser Methode auf <code>0</code> f�llt. Diese Methode ist synchronisiert.
	 */
	void Release();

};
