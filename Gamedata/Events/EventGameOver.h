/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "Events/EventScreen.h"

class CEventGameOver : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventGameOver)

	/// Standardkonstruktor
	CEventGameOver(void) {};

	/// Standardkonstruktor mit Parameter�bergabe
	CEventGameOver(const CString& sPlayersRace);

	/// Standarddestruktor
	~CEventGameOver(void);

	virtual void Create(void);

	virtual void Close(void);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
