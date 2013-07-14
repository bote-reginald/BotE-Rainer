#pragma once
#include "afx.h"

class CFireArc : public CObject
{
public:
	DECLARE_SERIAL (CFireArc)

	/// Standardkonstruktir
	CFireArc(void);

	/// Konstruktor mit Parameter�bergabe
	/// @param mountPos Position der Waffe auf dem Schiff (zwischen 0� und 360�)
	/// @param angle �ffnungswinkel (zwischen 0� und 360�)
	CFireArc(USHORT mountPos, USHORT angle);

	/// Standarddestruktor
	~CFireArc(void);

	/// Kopierkonstruktor
	CFireArc(const CFireArc & rhs);

	/// Zuweisungsoperator
	CFireArc & operator=(const CFireArc &);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen

	/// Funktion gibt die Position der Waffe auf dem Schiff zur�ck.
	/// @return Position der Waffe auf dem Schiff
	USHORT GetPosition(void) const {return m_nMountPos;}

	/// Funktion gibt den �ffnungswinkel der Waffe zur�ck.
	/// @return �ffnungswinkel in Grad
	USHORT GetAngle(void) const {return m_nAngle;}

	/// Funktion zum Setzen der Feuerwinkelwerte.
	/// @param nMountPos Position am Schiff (0 entspricht Front, 90 entspricht rechts, 180 Heck und 270 links)
	/// @param nAngle �ffnungswinkel in �
	void SetValues(USHORT nMountPos, USHORT nAngle);

private:
	/// Anbringung der Waffe auf dem Schiff <code>FRONT, RIGHT, BACK, LEFT</code>
	USHORT m_nMountPos;

	/// Schusswinkel der Waffe (zwischen 0� und 360�) kegelf�rmig
	USHORT m_nAngle;
};
