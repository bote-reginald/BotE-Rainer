/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BottomBaseView.h"

#include <cassert>

// CShipBottomView view

struct stDrawingContext {
	stDrawingContext() : g(NULL), gp(NULL), pDoc(NULL), r(0, 0, 0, 0), fontName(""), fontBrush(NULL),
		fontFormat(), fontSize(0), normalColor(0, 0, 0)
	{
		memset(researchLevels, 1, sizeof(researchLevels));
	}
	Graphics *g;
	CGraphicPool *gp;
	CBotEDoc* pDoc;
	CRect r;
	CString fontName;
	SolidBrush *fontBrush;
	StringFormat fontFormat;
	Gdiplus::REAL fontSize;
	Gdiplus::Color normalColor;
	BYTE researchLevels[6];
};

class CShipBottomView : public CBottomBaseView
{
	DECLARE_DYNCREATE(CShipBottomView)

protected:
	CShipBottomView();           // protected constructor used by dynamic creation
	virtual ~CShipBottomView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion f�hrt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgef�hrt werden m�ssen.
	virtual void OnNewRound(void);

	/// Funktion l�dt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

	/// Funktion gibt zur�ck, ob in der View die Station gezeigt werden soll.
	/// Ist dies nicht der Fall, so werden Schiffe gezeigt.
	/// @return <code>TRUE</code> wenn Station gezeigt wird, bei Schiffen <code>FALSE</code>
	static BOOLEAN IsShowStation(void) {return m_bShowStation;}

	/// Funktion legt fest, ob eine Station oder Schiffe in der View angezeigt werden sollen.
	/// @param is Wahrheitswert: <code>TRUE</code> f�r Stationen, <code>FALSE</code> f�r Schiffe
	static void SetShowStation(BOOLEAN is) {m_bShowStation = is;}

	/// Funktion setzt ein Schiff als aktives Schiff (Hauptbuttons einblenden, Schiffsbewegung aktivieren usw.)
	/// @param i Iterator des Schiffes in der Schiffsmap
	void ActivateShip(CShipMap::iterator i, bool bSetShipMove = true);

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

private:
	enum MAIN_BUTTON {
		MAIN_BUTTON_NONE,
		MAIN_BUTTON_COMBAT_BEHAVIOR,
		MAIN_BUTTON_IMMEDIATE_ORDER,
		MAIN_BUTTON_SINGLE_TURN_ORDER,
		MAIN_BUTTON_MULTI_TURN_ORDER,
		MAIN_BUTTON_CANCEL
	};
	// Funktionen
	/// Funktion ermittelt die Nummer des Schiffes im Array, �ber welches die Maus bewegt wurde.
	/// @param pt Mauskoordinate
	/// @return Nummer des Schiffes im Schiffsarray (<code>-1</code> wenn kein Schiff markiert wurde)
	//int GetMouseOverShip(CPoint& pt);

	const unsigned bw; //button width
	const unsigned bh; //button height
	const unsigned bd; //button distance (between buttons)
	const unsigned bt; //for vertical position for the buttons which are the top-nearest ones
	const unsigned bb; //for vertical position for drawing "from bottom up"

	// Attribute
	USHORT m_iPage;								///< bei mehr als 9 Schiffen k�nnen wir auf die n�chste Seite wechseln
	CPoint m_LastKO;							///< Hilfsvariable, die den zuletzt angeklickten Sektor sich merkt
	BOOLEAN m_bShowNextButton;
	Bitmap* m_pShipOrderButton;					///< kleiner Button f�r die Schiffbefehle

	struct SecondaryButtonInfo {
		SecondaryButtonInfo() : /*which(SHIP_ORDER::NONE),*/ rect(0, 0, 0, 0) {};
		SecondaryButtonInfo(/*SHIP_ORDER::Typ which_,*/ const CRect& rect_) :
			/*which(which_),*/ rect(rect_)
			{};
		//CString String() const {
		//	switch(which) {
		//		case SHIP_ORDER::AVOID: return "BTN_AVOID";
		//		case SHIP_ORDER::ATTACK: return "BTN_ATTACK";
		//		case SHIP_ORDER::DESTROY_SHIP: return "BTN_DESTROY_SHIP";
		//		case SHIP_ORDER::CREATE_FLEET: return "BTN_CREATE_FLEET";
		//		case SHIP_ORDER::FOLLOW_SHIP: assert(false); break;
		//		case SHIP_ORDER::TRAIN_SHIP: return "BTN_TRAIN_SHIP";
		//		case SHIP_ORDER::WAIT_SHIP_ORDER: return "BTN_WAIT_SHIP_ORDER";
		//		case SHIP_ORDER::SENTRY_SHIP_ORDER: return "BTN_SENTRY_SHIP_ORDER";
		//		case SHIP_ORDER::REPAIR: return "BTN_REPAIR_SHIP";
		//		case SHIP_ORDER::ASSIGN_FLAGSHIP: return "BTN_ASSIGN_FLAGSHIP";
		//		case SHIP_ORDER::ENCLOAK: return "BTN_CLOAK";
		//		case SHIP_ORDER::DECLOAK: return "BTN_DECLOAK";
		//		case SHIP_ORDER::COLONIZE: return "BTN_COLONIZE";
		//		case SHIP_ORDER::TERRAFORM: return "BTN_TERRAFORM";
		//		case SHIP_ORDER::BUILD_OUTPOST: return "BTN_BUILD_OUTPOST";
		//		case SHIP_ORDER::BUILD_STARBASE: return "BTN_BUILD_STARBASE";
		//		case SHIP_ORDER::BLOCKADE_SYSTEM: return "BTN_BLOCKADE_SYSTEM";
		//		case SHIP_ORDER::ATTACK_SYSTEM: return "BTN_ATTACK_SYSTEM";
		//		case SHIP_ORDER::TRANSPORT: return "BTN_TRANSPORT";
		//		case SHIP_ORDER::RAID_SYSTEM: assert(false); break;
		//		default:
		//			assert(false);
		//	}
		//	return "";
		//}
		//SHIP_ORDER::Typ which;
		CRect rect;
	};
	std::vector<SecondaryButtonInfo> m_vSecondaryShipOrders; //die ganzen Rechtecke f�r die Schiffsbefehlsbuttons

	struct MainButtonInfo {
		MainButtonInfo() : which(MAIN_BUTTON_NONE), rect(0, 0, 0, 0) {};
		MainButtonInfo(MAIN_BUTTON which_, const CRect& rect_) :
			which(which_), rect(rect_)
			{};
		CString String() const {
			switch(which) {
				case MAIN_BUTTON_COMBAT_BEHAVIOR: return "COMBAT_BEHAVIOR";
				case MAIN_BUTTON_IMMEDIATE_ORDER: return "BTN_IMMEDIATE_ORDER";
				case MAIN_BUTTON_SINGLE_TURN_ORDER: return "BTN_SINGLE_TURN_ORDER";
				case MAIN_BUTTON_MULTI_TURN_ORDER: return "BTN_MULTI_TURN_ORDER";
				case MAIN_BUTTON_CANCEL: return "BTN_CANCEL";
				default:
					assert(false);
			}
			return "";
		}
		MAIN_BUTTON which;
		CRect rect;
	};

	std::vector<const MainButtonInfo> m_vMainShipOrders;	///< die Hauptbuttons f�r die Schiffsbefehle
	short m_iTimeCounter;
	MAIN_BUTTON m_iWhichMainShipOrderButton;				///< welchen Hauptschiffsbefehlsbutton haben wir gedr�ckt
	static BOOLEAN m_bShowStation;
	vector<pair<CRect, CShips*> > m_vShipRects;				///< alle angezeigten Schiffe mit ihren Rechtecken zu draufklicken

	// OnDraw Helper Functions
	stDrawingContext m_dc;

	bool CheckDisplayShip(CShips *pShip, CSector *csec );
	void DrawShipContent();
	bool TimeDoDraw(short counter) const;
	void DrawMaincommandMenu();
	short DrawCombatMenu();
	short DrawImmediateOrderMenu();
	short DrawSingleTurnOrderMenu();
	short DrawMultiTurnOrderMenu();

	void DrawStationData();
	void DrawMenu();

	CPoint CalcSecondaryButtonTopLeft(short counter, bool top_down = true) const;

	bool DrawImage( const CString& resName, const CRect& r ) const;
	void DrawSmallButton( const CString& resString, const CPoint& coords, SHIP_ORDER::Typ shiporder = SHIP_ORDER::NONE );
	void SetupDrawing();
	void SetupMainButtons();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


