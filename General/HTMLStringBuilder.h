/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once

/// Klasse bietet funktionen an, durch welche HTML String zusammengebaut werden k�nnen
class CHTMLStringBuilder
{
private:
	/// Standardkonstruktor
	CHTMLStringBuilder(void) {}

	/// Standarddestruktor
	~CHTMLStringBuilder(void) {}

public:

	/// Funktion zentriert den �bergebenen String.
	/// @param str zu zentrierender Text
	/// @return zentrierter Text
	static inline CString GetHTMLCenter(const CString& str)
	{
		CString msg = "";

		msg = _T("<center>");
		msg += str;
		msg += _T("</center>");

		return msg;
	};

	/// Funktion macht den �bergebenen String fett.
	/// @param str fettzumachender Text
	/// @return fetter Text
	static inline CString GetHTMLStringBold(const CString& str)
	{
		CString msg = "";

		msg = _T("<b>");
		msg += str;
		msg += _T("</b>");

		return msg;
	}

	/// Funktion gibt ein NewLine HTML Kommando zur�ck.
	/// @return NewLine HTML Kommando als String
	static inline CString GetHTMLStringNewLine(void)
	{
		CString msg = "";
		msg = _T("<br>");

		return msg;
	}

	/// Funktion gibt ein Zeichne-Horizontale-Linie-HTML-Kommando zur�ck.
	/// @return Zeichne-Horizontale-Linie-HTML-Kommando
	static inline CString GetHTMLStringHorzLine(void)
	{
		CString msg = "";
		msg = _T("<font color=white><hr></font>");

		return msg;
	}

	/// Funktion macht aus dem �bergebenen String eine HTML �berschrift
	/// @param str Text f�r �berschrift
	/// @param head HTML �berschriftsart
	/// @return Text als �berschrift
	static inline CString GetHTMLHeader(const CString& str, const CString& head = "h1")
	{
		CString msg = "";
		msg = _T("<" + head + ">");
		msg += str;
		msg += _T("</" + head + ">");

		return msg;
	}

	/// Funktion wei�t dem �bergebenen String eine HTML-Farbe zu.
	/// @param str Text f�r Farbe
	/// @param clr HTML-Farbe
	/// @return Text mit HTML-Farbkommando
	static inline CString GetHTMLColor(const CString& str, const CString& clr = "white")
	{
		CString msg = "";
		msg = _T("<font color=" + clr + ">");
		msg += str;
		msg += _T("</font>");

		return msg;
	}
};
