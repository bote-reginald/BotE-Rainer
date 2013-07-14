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
class CMajor;

class CFontLoader
{
public:
	/**
	 * Diese Funktion generiert die passende Schriftart. Daf�r wird die jeweilige Rasse im Parameter <code>playersRace
	 * </code>, die allgemeine Schriftgr��e (0: sehr klein, 1: klein, 2: normal, 3: gro�, 4: sehr gro�, 5: kolossal)
	 * mittels <code>size</code> und ein Zeiger auf ein Fontobjekt <code>font</code>, in welches die generierte Schrift
	 * gespeichert wird, �bergeben.
	 */
	static void CreateFont(const CMajor* pPlayersRace, BYTE size, CFont* font);

	/**
	 * Diese Funktion generiert die passende Schriftart. Daf�r wird die jeweilige Rasse im Parameter <code>playersRace
	 * </code>, die allgemeine Schriftgr��e (0: sehr klein, 1: klein, 2: normal, 3: gro�, 4: sehr gro�, 5: kolossal)
	 * mittels <code>size</code>, die gew�nschte Schriftfarbe mittels <code>color</code> und ein Zeiger auf ein
	 * Fontobjekt <code>font</code>, in welches die generierte Schrift gespeichert wird, �bergeben. Der R�ckgabewert
	 * der Funktion ist die passende Schriftfarbe.
	 */
	static COLORREF CreateFont(const CMajor* pPlayersRace, BYTE size, BYTE color, CFont* font);

	/**
	 * Diese Funktion gibt eine bestimmte Farbe f�r die Schrift einer Rasse zur�ck. Daf�r wird die jeweilige Rasse im
	 * Parameter <code>playersRace</code> und die gew�nschte Schriftfarbe im Paramter <code>color</code> �bergeben.
	 * Jede Rasse hat f�nfs verschiedene Farben f�r Schriften.
	 */
	static COLORREF GetFontColor(const CMajor* pPlayersRace, BYTE colorType);

	static void CreateGDIFont(const CMajor* pPlayersRace, BYTE size, CString &family, Gdiplus::REAL &fontSize);

	static void GetGDIFontColor(const CMajor* pPlayersRace, BYTE colorType, Gdiplus::Color &color);

private:
	/// Konstruktor
	CFontLoader(void);

	/// Destruktor
	~CFontLoader(void);
};
