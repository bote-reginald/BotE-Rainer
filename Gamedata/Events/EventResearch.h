/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Events\eventscreen.h"
#include <vector>

// forward declaration
class CBuildingInfo;
class CShipInfo;
class CTroopInfo;

class CEventResearch : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventResearch)

	/// Standardkonstruktor
	CEventResearch(void);

	/// Standardkonstruktor mit Parameter�bergabe
	CEventResearch(const CString &sPlayersRaceID, const CString &headline, BYTE byTech);

	/// Standarddestruktor
	virtual ~CEventResearch(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	virtual void Create(void);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString GetTooltip(const CPoint &pt) const;

private:
	// Attribute
	BYTE m_byTech;		///< erforschte Tech

	// tempor�re Attribute zum Zeichnen -> m�ssen nicht serialisiert werden
	Gdiplus::Bitmap* _graphic;
	BYTE m_byTechlevel;			///< erforschtes Techlevel

	std::vector<CBuildingInfo*> m_vNewBuildings;
	std::vector<CBuildingInfo*> m_vNewUpgrades;
	std::vector<CShipInfo*>		m_vNewShips;
	std::vector<CTroopInfo*>	m_vNewTroops;
};
