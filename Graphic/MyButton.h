/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

// forward declaration
class CGraphicPool;

namespace BUTTON_STATE
{
	enum Typ
	{
		NORMAL = 0,
		ACTIVATED = 1,
		DEACTIVATED = 2
	};
}

/**
 * Buttonklasse welche verschiedene Zust�nde eines Buttons darstellen kann. Gleichzeitig bietet diese Klasse
 * eine gute Abstraktion, um einfach Klicks abzufangen und Zeichenoperationen durchf�hren zu k�nnen.
 *
 * @author Sir Pustekuchen
 * @version 0.0.2
 */
class CMyButton
{
public:
	/// Konstruktor mit kompletter Parameter�bergabe.
	/// @param point linke obere Koordinate des Buttons
	/// @param size Gr��e des Buttons in x- und y-Richtung
	/// @param text der Text auf dem Button
	/// @param normGraphicName Name des Grafikfiles f�r die normale Darstellung
	/// @param inactiveGraphicName Name des Grafikfiles f�r die inaktive Darstellung
	/// @param activeGraphicName Name des Grafikfiles f�r die aktive Darstellung
	CMyButton(CPoint point, CSize size, const CString& text,
		const CString& normGraphicName, const CString& inactiveGraphicName, const CString& activeGraphicName);

	/// Destruktor
	~CMyButton(void);

	// Zugriffsfunktionen
	/// Funktion gibt den aktuellen Zustand des Buttons zur�ck.
	BUTTON_STATE::Typ GetState() const {return m_nStatus;}

	/// Funktion gibt das ben�tigte Rechteck des Buttons zur�ck.
	CRect GetRect() const {return CRect(m_ptKO.x, m_ptKO.y, m_ptKO.x+m_szSize.cx, m_ptKO.y+m_szSize.cy);}

	/// Funktion gibt den Text des Buttons zur�ck
	const CString& GetText() const {return m_sText;}

	/// Funktion setzt den Status/Zustand des Buttons.
	/// @param newState neuer Status f�r den Button (0 -> normal, 1 -> aktiviert, sonst deaktiviert)
	void SetState(BUTTON_STATE::Typ newState) {m_nStatus = newState;}

	/// Funktion �ndert den Text auf dem Button.
	/// @param text Text auf dem Button
	void SetText(const CString& text) {m_sText = text;}

	// Funktionen
	/// Funktion �berpr�ft, ob der �bergebene Punkt (z.B. Mausklick) in dem Feld des Buttons liegt.
	/// @param pt der zur �berpr�fende Punkt
	bool ClickedOnButton(const CPoint& pt) const;

	/// Diese Funktion zeichnet den Button in den �bergebenen Ger�tekontext.
	/// @param g Referenz auf Graphics Objekt
	/// @param graphicPool Zeiger auf die Sammlung aller Grafiken
	/// @param font Referenz auf zu benutzende Schrift
	/// @param brush Referenz auf Farbepinsel f�r Font (Schriftfarbe)
	void DrawButton(Gdiplus::Graphics &g, CGraphicPool* graphicPool, Gdiplus::Font &font, Gdiplus::SolidBrush &brush) const;

	/// Funktion aktiviert den Button. Wenn dieser inaktiv ist, kann er nicht aktiviert werden.
	bool Activate();

	/// Funktion deaktiviert den Button. Dieser kehrt dann zu seinem alten Zustand zur�ck.
	bool Deactivate();

private:
	// Attribute
	BUTTON_STATE::Typ m_nStatus;	///< Der Status des Buttons, also ob normal, inaktiv oder aktiv.
	CString m_sText;				///< Der Text auf dem Button.
	const CPoint m_ptKO;			///< Koordinate der linken oberen Ecke des Buttons.
	const CSize m_szSize;			///< Die Gr��e in x und y Richtung des Buttons.
	const CString m_sNormal;		///< Die Grafik f�r den Button im normalen Zustand.
	const CString m_sInactive;		///< Die Grafik f�r den Button im deaktivierten Zustand.
	const CString m_sActive;		///< Die Grafik f�r den Button im aktivierten Zustand.
};
