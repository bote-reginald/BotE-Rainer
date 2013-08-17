// ChooseRaceView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "resource.h"
#include "ChooseRaceView.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "MainFrm.h"
#include "Races/RaceController.h"
#include "General/Loc.h"

#include "BotEServer.h"
#include "LANServer.h"

#include "Graphic\memdc.h"
#include "GraphicPool.h"

#define BTN_STARTGEAME	WM_USERMSG + 1
#define	BTN_CANCEL		WM_USERMSG + 2

// CChooseRaceView

IMPLEMENT_DYNCREATE(CChooseRaceView, CFormView)

CChooseRaceView::CChooseRaceView() : CFormView(CChooseRaceView::IDD), m_bIsServer(false), m_nPlayerCount(0),
	m_bStopDrawing(false),
	m_pBkgndImg()
{
}

CChooseRaceView::~CChooseRaceView()
{
	for (unsigned int i = 0; i < m_vMajorBtns.size(); i++)
		delete m_vMajorBtns[i].first;
}

void CChooseRaceView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChooseRaceView, CFormView)
	ON_WM_ERASEBKGND()
	ON_CONTROL_RANGE(BN_CLICKED, (int)network::RACE_1, (int)network::RACE_6, OnRaceButtonClicked)
	ON_BN_CLICKED(BTN_STARTGEAME, OnBnStartGameClicked)
	ON_BN_CLICKED(BTN_CANCEL, OnBnCancelClicked)
	ON_EN_CHANGE(IDC_MSG, OnEnChangeMsg)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CChooseRaceView-Diagnose

#ifdef _DEBUG
void CChooseRaceView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CChooseRaceView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CChooseRaceView-Meldungshandler

void CChooseRaceView::OnDraw(CDC* dc)
{
	if(m_bStopDrawing)
		return;

	// TODO: F�gen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMyMemDC pDC(dc);
	CRect clientRect;
	GetClientRect(&clientRect);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());

	g.Clear(Color::Black);
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)clientRect.Width() / (REAL)m_TotalSize.cx, (REAL)clientRect.Height() / (REAL)m_TotalSize.cy);

	g.DrawImage(m_pBkgndImg, 0, 0, m_TotalSize.cx, m_TotalSize.cy);

	Gdiplus::Font font(L"Calibri", 10, FontStyleBold);
	StringFormat format;
	format.SetAlignment(StringAlignmentFar);
	format.SetLineAlignment(StringAlignmentFar);

	CString sVersion = "Birth of the Empires Alpha7 V";
	sVersion += VERSION_INFORMATION;
	sVersion += "\n� by Sir Pustekuchen 2013";
	g.DrawString(CComBSTR(sVersion), -1, &font, RectF(0, 0, m_TotalSize.cx, m_TotalSize.cy), &format, &SolidBrush(Color::WhiteSmoke));

	// Umrandung f�r die Buttons zeichnen
	Color clr(130, 0, 0, 0);
	g.FillRectangle(&Gdiplus::SolidBrush(clr), RectF(75, 225, 350, 440));
	g.DrawRectangle(&Pen(Color(200,200,200), 1), RectF(75, 225, 350, 440));

	// Rassenauswahl schreiben
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentNear);
	CString sChooseRace = CLoc::GetString("CHOOSEEMPIRE");
	g.DrawString(CComBSTR(sChooseRace), -1, &font, RectF(75, 235, 350, 25), &format, &SolidBrush(Color::WhiteSmoke));

	// Chat schreiben
	CString sChat = CLoc::GetString("CHAT");
	g.DrawString(CComBSTR(sChat), -1, &font, RectF(75, 675, 350, 25), &format, &SolidBrush(Color::WhiteSmoke));


	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	// Buttons aktivieren/deaktivieren, aus-/abw�hlen, Anzahl der Spieler z�hlen
	for (vector<pair<CRoundButton2*, CString> >::const_iterator it = m_vMajorBtns.begin(); it != m_vMajorBtns.end(); ++it)
	{
		CRoundButton2 *pButton = it->first;
		ASSERT(pButton);
		if (!pButton)
			continue;

		CString sPlayer = it->second;
		if (sPlayer.IsEmpty())
			continue;

		// Namen des Spielers zeichnen
		CRect rect;
		pButton->GetWindowRect(rect);
		rect.left /= (double)((double)clientRect.Width() / (double)m_TotalSize.cx);
		rect.right /= (double)((double)clientRect.Width() / (double)m_TotalSize.cx);
		rect.top /= (double)((double)clientRect.Height() / (double)m_TotalSize.cy);
		rect.bottom /= (double)((double)clientRect.Height() / (double)m_TotalSize.cy);

		g.DrawString(CComBSTR(sPlayer), -1, &font, RectF(rect.left, rect.top - 18, rect.Width(), 20), &format, &SolidBrush(Color::LightSkyBlue));

		network::RACE race = (network::RACE)(it->first->GetDlgCtrlID());
		if (!client.IsPlayer(race))
			continue;

		// Bild der Rasse anzeigen
		CString sRaceID = pDoc->GetRaceCtrl()->GetMappedRaceID(race);
		CMajor* pRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sRaceID));
		if (!pRace)
			continue;

		if (client.GetClientRace() != race)
			continue;

		Bitmap* graphic	= pDoc->GetGraphicPool()->GetGDIGraphic("Races\\" + pRace->GetGraphicFileName());
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\ImageMissing.boj");
		if (graphic)
			g.DrawImage(graphic, 475, 307, 300, 300);
		g.DrawRectangle(&Pen(Color(200,200,200), 1), 475, 307, 300, 300);

		g.FillRectangle(&Gdiplus::SolidBrush(clr), RectF(775, 307, 450, 300));
		g.DrawRectangle(&Pen(Color(200,200,200), 1), RectF(775, 307, 450, 300));
		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentCenter);
		Gdiplus::Font font(L"Calibri", 11, FontStyleBold);
		g.DrawString(CComBSTR(pRace->GetRaceDesc()), -1, &font, RectF(775, 307, 450, 300), &format, &SolidBrush(Color::WhiteSmoke));

		// Rassensymbol noch in die Ecke zeichnen
		graphic	= pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\" + pRace->GetRaceID() + ".bop");
		if (graphic)
			g.DrawImage(graphic, 575, 207, 100, 100);
	}

	g.ReleaseHDC(pDC->GetSafeHdc());
}

void CChooseRaceView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: F�gen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	m_pBkgndImg = pDoc->GetGraphicPool()->GetGDIGraphic("Events\\Startmenu.boj");
	m_TotalSize = CSize(1280, 1024);

	// Majorrassen einlesen um Rasseninformationen zu bekommen (Imperiumsname, Bild, Beschreibung...)
	pDoc->GetRaceCtrl()->Init();

	// Structure containing Style
	tButtonStyle tStyle;
	//////////////////////////////////////////////////////////////////////////

	// Get default Style
	m_tButtonStyle.GetButtonStyle(&tStyle);
	// Change Radius of Button
	tStyle.m_dRadius = 1.0;

	// Change Color Schema of Button
	tStyle.m_tColorFace.m_tEnabled		= RGB(100, 100, 100);
	tStyle.m_tColorBorder.m_tEnabled	= RGB(220, 220, 220);

	tStyle.m_tColorFace.m_tClicked		= RGB(75, 75, 75);
	tStyle.m_tColorBorder.m_tClicked	= RGB(25, 25, 25);

	tStyle.m_tColorFace.m_tPressed		= RGB(0x80, 0x80, 0xFF);
	tStyle.m_tColorBorder.m_tPressed	= RGB(0x40, 0x40, 0xFF);

	tStyle.m_tColorFace.m_tDisabled		= RGB(75, 75, 75);
	tStyle.m_tColorBorder.m_tDisabled	= RGB(25, 25, 25);

	tStyle.m_tColorFace.m_tHot			= RGB(0x80, 0x80, 0xFF);
	tStyle.m_tColorBorder.m_tHot		= RGB(0x40, 0x40, 0xFF);

	// Set Style again
	m_tButtonStyle.SetButtonStyle(&tStyle);

	// Create a push button.
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	int nRaceCount = 0;
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		network::RACE nRace = pDoc->GetRaceCtrl()->GetMappedClientID(it->first);
		if (nRace == network::RACE_NONE)
			continue;

		CRoundButton2* pBtn = new CRoundButton2();
		pBtn->Create(it->second->GetEmpiresName(), WS_CHILD|WS_VISIBLE|BS_PUSHLIKE, CRect(), this, nRace);
		m_vMajorBtns.push_back(pair<CRoundButton2*, CString>(pBtn, ""));
		nRaceCount++;
	}

	// Create Edit Control (Chat)
	m_edtChat.Create(WS_CHILD|WS_VISIBLE|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE|ES_READONLY|WS_BORDER|WS_VSCROLL, CRect(), this, IDC_CHAT);
	m_edtChatMsg.Create(WS_CHILD|WS_VISIBLE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_LEFT|ES_MULTILINE|ES_WANTRETURN|WS_BORDER, CRect(), this, IDC_MSG);

	// Buttons f�r Spiel starten und Cancel
	m_btStartGame.Create(CLoc::GetString("STARTGAME"), WS_CHILD|WS_VISIBLE|BS_PUSHLIKE, CRect(), this, BTN_STARTGEAME);
	m_btCancel.Create(CLoc::GetString("BTN_BACK"), WS_CHILD|WS_VISIBLE|BS_PUSHLIKE, CRect(), this, BTN_CANCEL);

	EnableRaceButtons();
}

BOOL CChooseRaceView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: F�gen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	return FALSE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void CChooseRaceView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: F�gen Sie hier Ihren Meldungsbehandlungscode ein.
	double dSizeModX = m_TotalSize.cx / (double)cx;
	double dSizeModY = m_TotalSize.cy / (double)cy;

	double nButtonSizeX = 300.0 / dSizeModX;
	double nButtonSizeY = 40.0 / dSizeModY;

	int nXPos = 100 / dSizeModX;
	int nYPos = 275 / dSizeModY;

	double nGab = nButtonSizeY + 25.0 / dSizeModY;

	// aufl�sungsabh�ngige Font erstellen
	CFont fnt;
	double dFontHeight = 15 / min(dSizeModX, dSizeModY);
	fnt.CreateFont(dFontHeight, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Calibri");

	// Buttons aktivieren/deaktivieren, aus-/abw�hlen, Anzahl der Spieler z�hlen
	int nCount = 0;
	for (vector<pair<CRoundButton2*, CString> >::const_iterator it = m_vMajorBtns.begin(); it != m_vMajorBtns.end(); ++it)
	{
		CRoundButton2 *pButton = it->first;
		ASSERT(pButton);
		if (!pButton)
			continue;

		// Gr��e der Buttons der Aufl�sung anpassen
		pButton->SetWindowPos(NULL, nXPos, nYPos + nGab * nCount, nButtonSizeX, nButtonSizeY, SWP_FRAMECHANGED);
		SetButtonStyle(pButton, dFontHeight);
		nCount++;
	}

	if (m_edtChat.m_hWnd)
	{
		m_edtChat.SetWindowPos(NULL, 75 / dSizeModX, 700 / dSizeModY, 350 / dSizeModX, 200 / dSizeModY, SWP_FRAMECHANGED);
		// Font dem Editcontrol zuweisen
		m_edtChat.SetFont(&fnt);
	}

	if (m_edtChatMsg.m_hWnd)
	{
		m_edtChatMsg.SetWindowPos(NULL, 75 / dSizeModX, 905 / dSizeModY, 350 / dSizeModX, 20 / dSizeModY, SWP_FRAMECHANGED);
		// Font dem Editcontrol zuweisen
		m_edtChatMsg.SetFont(&fnt);
	}

	if (m_btStartGame.m_hWnd)
	{
		m_btStartGame.SetWindowPos(NULL, 260 / dSizeModX, 940 / dSizeModY, 150 / dSizeModX, 30 / dSizeModY, SWP_FRAMECHANGED);
		SetButtonStyle(&m_btStartGame, dFontHeight);
	}

	if (m_btCancel.m_hWnd)
	{
		m_btCancel.SetWindowPos(NULL, 90 / dSizeModX, 940 / dSizeModY, 150 / dSizeModX, 30 / dSizeModY, SWP_FRAMECHANGED);
		SetButtonStyle(&m_btCancel, dFontHeight);
	}

	// Handle auf Font freigeben
	fnt.Detach();
}

void CChooseRaceView::SetButtonStyle(CRoundButton2* pBtn, double dFontHeight)
{
	if (!pBtn)
	{
		ASSERT(pBtn);
		return;
	}

	// Initialize the Buttons with global style
	pBtn->SetRoundButtonStyle(&m_tButtonStyle);

	LOGFONT tFont;
	pBtn->GetFont(&tFont);
	strcpy(tFont.lfFaceName, "Calibri");
	tFont.lfHeight = dFontHeight;
	tFont.lfWeight = FW_BLACK;
	tFont.lfQuality = CLEARTYPE_QUALITY;
	pBtn->SetFont(&tFont);

	tColorScheme tColor;
	pBtn->GetTextColor(&tColor);

	// Change Color
	tColor.m_tEnabled	= RGB(255, 255, 255);
	tColor.m_tClicked	= RGB(50, 50, 50);
	tColor.m_tPressed	= RGB(50, 50, 50);

	// Set Text-Color
	pBtn->SetTextColor(&tColor);

	pBtn->SetHotButton(true);
}

void CChooseRaceView::EnableRaceButtons()
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	m_nPlayerCount = 0;

	// Buttons aktivieren/deaktivieren, aus-/abw�hlen, Anzahl der Spieler z�hlen
	for (vector<pair<CRoundButton2*, CString> >::iterator it = m_vMajorBtns.begin(); it != m_vMajorBtns.end(); ++it)
	{
		CRoundButton2 *pButton = it->first;
		ASSERT(pButton);
		if (!pButton)
			continue;

		network::RACE race = (network::RACE)(pButton->GetDlgCtrlID());

		// Wenn ein Spiel geladen wurde und es sich hier um den Server handelt, dann
		// f�r ausgel�schte Rassen einstellen, dass diese vom Server gespielt werden.
		// Somit k�nnen sie nicht mehr von Spielern ausgew�hlt werden.
		if (pDoc->m_bGameLoaded && m_bIsServer && !server.IsPlayedByServer(race))
		{
			CString sRaceID = pDoc->GetRaceCtrl()->GetMappedRaceID(race);
			CMajor* pRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sRaceID));
			// Beim Neustart hat jede Rasse 1 System (die Variable wird so schon per Reset gesetzt)
			if (!pRace || pRace->GetEmpire()->CountSystems() == 0)
				server.SetPlayByServer(race, TRUE, TRUE);
		}

		if (client.IsPlayer(race) && !client.IsPlayedByServer(race))
		{
			m_nPlayerCount++;
			pButton->EnableWindow(race == client.GetClientRace());
			pButton->SetCheck(race == client.GetClientRace());
			it->second = client.GetUserName(race);
			if (it->second.IsEmpty())
				it->second = "?";
		}
		else if (client.IsPlayedByServer(race))
		{
			// Wenn Rasse nach Laden schon ausgel�scht, dann kann sie nicht mehr angew�hlt werden
			pButton->EnableWindow(FALSE);
			pButton->SetCheck(TRUE);
			it->second = CLoc::GetString("ELIMINATED");
		}
		else
		{
			pButton->EnableWindow(TRUE);
			pButton->SetCheck(FALSE);
			it->second = "";
		}
	}

	m_btStartGame.EnableWindow(CheckValues());

	CRect clientRect;
	GetClientRect(clientRect);

	double dSizeModX = m_TotalSize.cx / (double)clientRect.Width();
	double dSizeModY = m_TotalSize.cy / (double)clientRect.Height();

	CRect rect(70 / dSizeModX, 200 / dSizeModY, 1230 / dSizeModX, 610 / dSizeModY);
	InvalidateRect(rect, false);

	for (vector<pair<CRoundButton2*, CString> >::iterator it = m_vMajorBtns.begin(); it != m_vMajorBtns.end(); ++it)
	{
		CRoundButton2 *pButton = it->first;
		ASSERT(pButton);
		if (!pButton)
			continue;

		pButton->RedrawWindow();
	}
}

BOOL CChooseRaceView::CheckValues()
{
	// k�nnen das Spiel erstmal nur starten, wenn der Server-Spieler ein Volk gew�hlt hat
	return m_bIsServer && client.GetClientRace() != network::RACE_NONE;
}

void CChooseRaceView::OnRaceButtonClicked(UINT nID)
{
	CButton* pButton = (CButton*)(GetDlgItem(nID));
	if (!pButton)
		return;

	if (client.IsPlayer((network::RACE)nID))
		return;

	if (pButton->GetCheck())
	{
		client.RequestRace(network::RACE_NONE);
	}
	else
	{
		client.RequestRace((network::RACE)(nID));
		// Soundwiedergabe f�r die Rasse
		switch (nID)
		{
		case network::RACE_1:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_FED_RACESELECT); break;
		case network::RACE_2:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_FER_RACESELECT); break;
		case network::RACE_3:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_KLI_RACESELECT); break;
		case network::RACE_4:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_ROM_RACESELECT); break;
		case network::RACE_5:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_CAR_RACESELECT); break;
		case network::RACE_6:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_DOM_RACESELECT); break;
		}
	}

	EnableRaceButtons();
}

void CChooseRaceView::OnBnStartGameClicked()
{
	using namespace network;

	// Wechsel zur n�chsten Seite nur m�glich, wenn der Server l�uft
	if (!m_bIsServer)
		return;

	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);
	if (!pDoc)
		return;

	m_bStopDrawing = true;

	SetCursor(::LoadCursor(NULL, IDC_WAIT));

	// evtl. laufende Ver�ffentlichung des Spiels stoppen
	serverPublisher.StopPublishing();

	// die nicht ausgew�hlten V�lker vom Server spielen lassen
	for (int i = 0; i < RACE_COUNT; i++)
	{
		RACE race = (RACE)(RACE_FIRST + i);
		if (!server.IsPlayedByClient(race))
			server.SetPlayByServer(race,TRUE,FALSE);
	}

	// Spiel starten; veranlasst die Clients, auf die n�chste Seite zu wechseln
	pDoc->PrepareData();
	server.BeginGame(pDoc);

	SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

void CChooseRaceView::OnBnCancelClicked()
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);
	if (!pDoc)
		return;

	client.Disconnect();

	if (m_bIsServer)
	{
		// warten bis der Netzwerkthread die Nachricht f�r client.Disconnect() gesendet hat
		// st�rzt sonst manchmal ab wenn die MessageBox daf�r gerade eingeblendet werden soll und das Men�
		// umgeschaltet wird
		Sleep(500);

		serverPublisher.StopPublishing();
		server.Stop();
	}

	resources::pMainFrame->SelectMainView(START_VIEW);
}

void CChooseRaceView::AddChatMsg(const CString& name, const CString& msg)
{
	CString total;
	GetDlgItemText(IDC_CHAT, total);
	if (total != "")
		total += "\r\n";

	CString newtext;
	newtext.Format("%s: %s", name, msg);
	SetDlgItemText(IDC_CHAT, total + newtext);
	((CEdit *)GetDlgItem(IDC_CHAT))->SetSel(msg.GetLength(), -1);
}

void CChooseRaceView::OnEnChangeMsg()
{
	if (((CEdit *)GetDlgItem(IDC_MSG))->GetLineCount() > 1)
	{
		CString msg;
		GetDlgItemText(IDC_MSG, msg);
		msg.Replace("\r\n", "");
		msg.Trim();
		if (msg.GetLength() > 0)
		{
			client.Chat(msg);
			AddChatMsg(client.GetClientUserName(), msg);
		}
		SetDlgItemText(IDC_MSG, "");
	}
}
