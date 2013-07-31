// ResearchMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "MainFrm.h"
#include "ResearchMenuView.h"
#include "ResearchBottomView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"
#include "HTMLStringBuilder.h"
#include "General/Loc.h"
#include "GraphicPool.h"

// CResearchMenuView

IMPLEMENT_DYNCREATE(CResearchMenuView, CMainBaseView)

CResearchMenuView::CResearchMenuView() :
	bg_researchmenu(),
	bg_urmenu(),
	bg_emptyur(),
	m_bySubMenu(0)
{

}

CResearchMenuView::~CResearchMenuView()
{
	for (int i = 0; i < m_ResearchMainButtons.GetSize(); i++)
	{
		delete m_ResearchMainButtons[i];
		m_ResearchMainButtons[i] = 0;
	}
	m_ResearchMainButtons.RemoveAll();
}

BEGIN_MESSAGE_MAP(CResearchMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CResearchMenuView drawing

void CResearchMenuView::OnDraw(CDC* dc)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	// TODO: add draw code here
	// Doublebuffering wird initialisiert
	CMyMemDC pDC(dc);
	CRect client;
	GetClientRect(&client);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());

	g.Clear(Color::Black);
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);

	// ***************************** DIE FORSCHUNGSANSICHT ZEICHNEN **********************************
	if (m_bySubMenu == 0)
		DrawResearchMenue(&g);
	else if (m_bySubMenu == 1)
		DrawUniqueResearchMenue(&g);
	else if (m_bySubMenu == 3)
		DrawResearchMenue3(&g);
	else if (m_bySubMenu == 4)
		DrawResearchMenue4(&g);
	else if (m_bySubMenu == 5)
		DrawResearchMenue5(&g);
	// ************** DIE FORSCHUNGSANSICHT ZEICHNEN ist hier zu Ende **************
	g.ReleaseHDC(pDC->GetSafeHdc());
}

// CResearchMenuView diagnostics

#ifdef _DEBUG
void CResearchMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CResearchMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CResearchMenuView message handlers

void CResearchMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	// Zuweisungsbalken in der Forschungs�bersicht
	// Die Koodinaten der Rechtecke f�r die ganzen Buttons
	CRect r;
	GetClientRect(&r);
	// i == 0->Bio, 1->Energy, 2->Computer, 3->Bautechnik, 4->Antrieb, 5->Waffen, 6->Unique
	for (int i = 0; i < 3; i++)	// Unique Forschung mu� separat initialisiert werden, daher nur bis 5
		for (int j = 0; j < 101; j++)
			// Wir haben ne Breite von 200 f�r die 100 Striche zur Auswahl -> jeder Strich 2 breit
			ResearchTimber[i][j].SetRect(r.left+33+j*2+i*260,r.top+350,r.left+33+j*2+2+i*260,r.top+350+20);
	// Jetzt die Striche f�r die unteren Forschungsgebiete
	for (int i = 3; i < 6; i++)	// Unique Forschung mu� seperat initialisiert werden, daher nur bis 5
		for (int j = 0; j < 101; j++)
			// Wir haben ne Breite von 200 f�r die 100 Striche zur Auswahl -> jeder Strich 2 breit
			ResearchTimber[i][j].SetRect(r.left+33+j*2+(i-3)*260,r.top+693,r.left+33+j*2+2+(i-3)*260,r.top+693+20);
	// Nun alles f�r die Unique Forschung
	for (int i = 0; i < 101; i++)
		// Wir haben ne Breite von 200 f�r die 100 Striche zur Auswahl -> ieder Strich 2 breit
		ResearchTimber[6][i].SetRect(r.left+33+i*2,r.top+350,r.left+33+i*2+2,r.top+350+20);
	LockStatusRect[6].SetRect(r.left+75,r.top+120,r.left+195,r.top+290);

	// Die kleinen Rechtecke f�r den Status, ob die Forschung gelocked ist oder nicht
	for (int i = 0; i < 3; i++)
		LockStatusRect[i].SetRect(r.left+75+i*260,r.top+170,r.left+195+i*260,r.top+290);
	for (int i = 3; i < 6; i++)
		LockStatusRect[i].SetRect(r.left+75+(i-3)*260,r.top+510,r.left+195+(i-3)*260,r.top+633);
	m_bySubMenu = 0;

	// View bei den Tooltipps anmelden
	resources::pMainFrame->AddToTooltip(this);
}

/// Funktion l�dt die rassenspezifischen Grafiken.
void CResearchMenuView::LoadRaceGraphics()
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// Alle Buttons in der View erstellen
	CreateButtons();
	// alle Hintergrundgrafiken laden
	CString sPrefix = pMajor->GetPrefix();
	bg_researchmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "researchmenu.boj");
	bg_urmenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "urmenu.boj");
	bg_emptyur		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "emptyur.boj");
}

void CResearchMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CResearchMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Forschungsmen�s
/////////////////////////////////////////////////////////////////////////////////////////
void CResearchMenuView::DrawResearchMenue(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	StringFormat fontFormatTop;
	fontFormatTop.SetAlignment(StringAlignmentCenter);
	fontFormatTop.SetLineAlignment(StringAlignmentNear);
	fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);

	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormatCenter.SetTrimming(StringTrimmingEllipsisCharacter);

	if (bg_researchmenu)
		g->DrawImage(bg_researchmenu, 0, 0, 1075, 750);

	CString s;
	USHORT level;
	USHORT techPercentage[7];

	// gro�en Rechtecke f�r die einzelnen Forschungsgebiete zeichnen
	// Biogenetik
	level = pMajor->GetEmpire()->GetResearch()->GetBioTech();
	s.Format("%s - %s %d",CLoc::GetString("BIOTECH"),CLoc::GetString("LEVEL"),level);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(0);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,110,250,25), &fontFormatCenter, &fontBrush);
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\biotech.bop");
	if (graphic)
		g->DrawImage(graphic, 60,140,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetBioFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetBio(level))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[0] = pMajor->GetEmpire()->GetResearch()->GetBioPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[0]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,315,250,25), &fontFormatCenter, &fontBrush);

	// Bautechnik
	level = pMajor->GetEmpire()->GetResearch()->GetConstructionTech();
	s.Format("%s - %s %d",CLoc::GetString("CONSTRUCTIONTECH"),CLoc::GetString("LEVEL"),level);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(4);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\constructiontech.bop");
	if (graphic)
		g->DrawImage(graphic, 60,480,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetConstructionFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetConstruction(level))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[3] = pMajor->GetEmpire()->GetResearch()->GetConstructionPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[3]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,655,250,25), &fontFormatCenter, &fontBrush);

	// Energietechnik
	level = pMajor->GetEmpire()->GetResearch()->GetEnergyTech();
	s.Format("%s - %s %d",CLoc::GetString("ENERGYTECH"),CLoc::GetString("LEVEL"),level);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(1);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,110,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\energytech.bop");
	if (graphic)
		g->DrawImage(graphic, 320,140,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetEnergyFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetEnergy(level))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[1] = pMajor->GetEmpire()->GetResearch()->GetEnergyPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[1]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,315,250,25), &fontFormatCenter, &fontBrush);

	// Antriebstechnik
	level = pMajor->GetEmpire()->GetResearch()->GetPropulsionTech();
	s.Format("%s - %s %d",CLoc::GetString("PROPULSIONTECH"),CLoc::GetString("LEVEL"),level);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(3);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\propulsiontech.bop");
	if (graphic)
		g->DrawImage(graphic, 320,480,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetPropulsionFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetPropulsion(level))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[4] = pMajor->GetEmpire()->GetResearch()->GetPropulsionPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[4]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,655,250,25), &fontFormatCenter, &fontBrush);

	// Computertechnik
	level = pMajor->GetEmpire()->GetResearch()->GetCompTech();
	s.Format("%s - %s %d",CLoc::GetString("COMPUTERTECH"),CLoc::GetString("LEVEL"),level);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(2);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,110,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\computertech.bop");
	if (graphic)
		g->DrawImage(graphic, 580,140,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetComputerFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetComp(level))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[2] = pMajor->GetEmpire()->GetResearch()->GetCompPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[2]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,315,250,25), &fontFormatCenter, &fontBrush);

	// Waffentechnik
	level = pMajor->GetEmpire()->GetResearch()->GetWeaponTech();
	s.Format("%s - %s %d",CLoc::GetString("WEAPONTECH"),CLoc::GetString("LEVEL"),level);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(5);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\weapontech.bop");
	if (graphic)
		g->DrawImage(graphic, 580,480,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetWeaponFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetWeapon(level))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[5] = pMajor->GetEmpire()->GetResearch()->GetWeaponPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[5]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,655,250,25), &fontFormatCenter, &fontBrush);


	// Die gesamten Forschungspunkte rechts anzeigen
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCHPOINTS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,80,240,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CLoc::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);

	s.Format("%d %s",pMajor->GetEmpire()->GetFP(),CLoc::GetString("FP"));
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCHPOINTS (TOTAL): %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	// Die einzelnen Forschungsboni anzeigen
	fontBrush.SetColor(normalColor);
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH_BONI")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,145,245,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	RectF rect(855,180,190,25);
	s = CLoc::GetString("BIOTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetBioTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI BIOTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("ENERGYTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI ENERGYTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("COMPUTERTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetCompTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI COMPUTERTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetConstructionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI CONSTRUCTIONTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetPropulsionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI PROPULSIONTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("WEAPONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetWeaponTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI WEAPONTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);

	fontBrush.SetColor(normalColor);

	Color penMark;
	penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 1; j < 101; j += 5)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf dr�cken k�nnen
		{
			RectF timber(ResearchTimber[i][j].left, ResearchTimber[i][j].top, ResearchTimber[i][j].Width() + 6, ResearchTimber[i][j].Height());
			if (j <= techPercentage[i])
				g->FillRectangle(&SolidBrush(Color(250-j*2.5,50+j*2,0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(75,75,75,75)), timber);
		}
		// farbige Kante um Zuweisungsbalken zeichnen
		RectF markRect(ResearchTimber[i][1].left, ResearchTimber[i][1].top, ResearchTimber[i][100].left - ResearchTimber[i][1].left, ResearchTimber[i][1].Height());
		g->DrawRectangle(&Gdiplus::Pen(penMark), markRect);
	}

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentFar);
	for (int i = 0; i < 6; i++)
	{
		const BOOL lockstatus = pMajor->GetEmpire()->GetResearch()->GetLockStatus(i); 
		if (lockstatus == FALSE)
		{
			s = CLoc::GetString("UNLOCKED");
			fontBrush.SetColor(normalColor);
		}
		else
		{
			s = CLoc::GetString("LOCKED");
			fontBrush.SetColor(Color(255,0,0));
		}
		RectF lock(LockStatusRect[i].left, LockStatusRect[i].top, LockStatusRect[i].Width(), LockStatusRect[i].Height());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), lock, &fontFormatCenter, &fontBrush);
	}

	// Die einzelnen Men�buttons f�r Spezialforschung, Datenbank und Schiffskonstruktion darstellen
	// Schriftfarbe w�hlen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	fontBrush.SetColor(btnColor);
	DrawGDIButtons(g, &m_ResearchMainButtons, m_bySubMenu, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);

	// "Forschungs�bersicht" in der Mitte zeichnen
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);

	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH_MENUE")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormatCenter, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Uniqueforschungsmen�s
/////////////////////////////////////////////////////////////////////////////////////////
void CResearchMenuView::DrawUniqueResearchMenue(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	StringFormat fontFormatTop;
	fontFormatTop.SetAlignment(StringAlignmentCenter);
	fontFormatTop.SetLineAlignment(StringAlignmentNear);
	fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);

	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormatCenter.SetTrimming(StringTrimmingEllipsisCharacter);

	// Wenn wir eine Unique Forschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	{
		if (bg_urmenu)
			g->DrawImage(bg_urmenu, 0, 0, 1075, 750);
	}
	else
	{
		if (bg_emptyur)
			g->DrawImage(bg_emptyur, 0, 0, 1075, 750);
	}

	// Die gesamten Forschungspunkte rechts anzeigen
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCHPOINTS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,80,240,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CLoc::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);

	s.Format("%d %s",pMajor->GetEmpire()->GetFP(),CLoc::GetString("FP"));
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	// Die einzelnen Forschungsboni anzeigen
	fontBrush.SetColor(normalColor);
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH_BONI")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,145,245,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	RectF rect(855,180,190,25);
	s = CLoc::GetString("BIOTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetBioTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("ENERGYTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("COMPUTERTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetCompTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetConstructionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetPropulsionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("WEAPONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetWeaponTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	fontBrush.SetColor(normalColor);

	// gro�en Rechtecke f�r die einzelnen Forschungsgebiete zeichnen
	// Uniqueforschung
	// Wenn wir eine Unique Forschung zur Auswahl haben, dann auch anzeigen
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	{
		USHORT techPercentage;
		RectF TechRect(10,80,250,25);
		USHORT level = pMajor->GetEmpire()->GetResearch()->GetNumberOfUniqueResearch();
		// UniqueForschung braucht ja soviele FP wie alle anderen des Level zusammen, deswegen die lange Anweisung
		ULONG allOthers = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetBio(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetEnergy(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetComp(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetPropulsion(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetConstruction(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetWeapon(level);
		// man ben�tigt etwas weniger Punkte als alle Techs zusammen
		allOthers /= SPECIAL_RESEARCH_DIV;
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), TechRect, &fontFormatTop, &fontBrush);

		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\specialtech.bop");
		if (graphic)
			g->DrawImage(graphic, 45, 100, 180, 150);

		s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetUniqueFP()*100)/(allOthers)));
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH PROGRESS %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,290,250,25), &fontFormatCenter, &fontBrush);

		techPercentage = pMajor->GetEmpire()->GetResearch()->GetUniquePercentage();
		s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage);
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH ASSIGNMENT %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,315,250,25), &fontFormatCenter, &fontBrush);

		Color penMark;
		penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
		for (int j = 1; j < 101; j += 5)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf dr�cken k�nnen
		{
			RectF timber(ResearchTimber[6][j].left, ResearchTimber[6][j].top, ResearchTimber[6][j].Width() + 6, ResearchTimber[6][j].Height());
			if (j <= techPercentage)
				g->FillRectangle(&SolidBrush(Color(250-j*2.5,50+j*2,0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(75,75,75,75)), timber);
		}
		// farbige Kante um Zuweisungsbalken zeichnen
		RectF markRect(ResearchTimber[6][1].left, ResearchTimber[6][1].top, ResearchTimber[6][100].left - ResearchTimber[6][1].left, ResearchTimber[6][1].Height());
		g->DrawRectangle(&Gdiplus::Pen(penMark), markRect);

		BOOL lockstatus;
		fontFormatCenter.SetAlignment(StringAlignmentCenter);
		fontFormatCenter.SetLineAlignment(StringAlignmentFar);

		lockstatus = pMajor->GetEmpire()->GetResearch()->GetLockStatus(6);
		if (lockstatus == FALSE)
		{
			s = CLoc::GetString("UNLOCKED");
			fontBrush.SetColor(normalColor);
		}
		else
		{
			s = CLoc::GetString("LOCKED");
			fontBrush.SetColor(Color(255,0,0));
		}
		RectF lock(LockStatusRect[6].left, LockStatusRect[6].top, LockStatusRect[6].Width(), LockStatusRect[6].Height());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), lock, &fontFormatCenter, &fontBrush);
		fontFormatCenter.SetLineAlignment(StringAlignmentCenter);

		// Den Namen des Unique Komplexes hinschreiben
		fontBrush.SetColor(markColor);
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH Name %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(325,75,500,25), &fontFormatTop, &fontBrush);
		fontBrush.SetColor(normalColor);

		// Die Beschreibung des Unique Complexes hinschreiben
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexDescription();
		fontFormatTop.SetFormatFlags(!StringFormatFlagsNoWrap);
		fontFormatTop.SetTrimming(StringTrimmingEllipsisCharacter);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(385,120,380,260), &fontFormatTop, &fontBrush);
		fontFormatTop.SetTrimming(StringTrimmingNone);

		// hier die 3 Wahlm�glichkeiten
		fontBrush.SetColor(markColor);
		// 1. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(1);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 2. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(2);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 3. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(3);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,445,250,300), &fontFormatTop, &fontBrush);
		}

		// ab hier die Beschreibungen zu den einzelnen Wahlm�glichkeiten
		fontBrush.SetColor(normalColor);
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(1);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(2);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(3);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,490,250,205), &fontFormatTop, &fontBrush);
		}
		fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);
	}

	// Die einzelnen Men�buttons f�r Spezialforschng, Datenbank und Schiffskonstruktion darstellen
	// Schriftfarbe w�hlen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	fontBrush.SetColor(btnColor);
	DrawGDIButtons(g, &m_ResearchMainButtons, m_bySubMenu, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
	// Nun die Buttons zur Auswahl einer Uniqueforschung
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE && pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
	{
		CString sFile = pMajor->GetPrefix() + "button.bop";
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + sFile);
		if (graphic)
		{
			g->DrawImage(graphic, 55, 698, 160, 40);
			g->DrawImage(graphic, 315, 698, 160, 40);
			g->DrawImage(graphic, 575, 698, 160, 40);
		}
		s = CLoc::GetString("BTN_SELECT");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(55,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(315,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(575,698,160,40), &fontFormatCenter, &fontBrush);
	}

	// "Spezialforschung" in der Mitte zeichnen + wenn mgl. den Namen des Forschungskomplexes
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);

	// Wenn wir eine Uniqueforschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		s.Format("%s - %s",CLoc::GetString("SPECIAL_RESEARCH"),	pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName());
	// Wenn wir keine Uniqueforschung zur Auswahl haben
	else
		s = CLoc::GetString("SPECIAL_RESEARCH");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,m_TotalSize.cx,55), &fontFormatCenter, &fontBrush);

	// Wenn wir keine Unique Forschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() != FALSE)
	{
		fontFormatCenter.SetLineAlignment(StringAlignmentNear);
		fontFormatCenter.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CLoc::GetString("NO_SPECIAL_RESEARCH_AVAILABLE");
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "NO_SPECIAL_RESEARCH_AVAILABLE: %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190,200,500,100), &fontFormatCenter, &fontBrush);

		// darunter die schon erforschten Spezialforschungen anzeigen
		CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);

		fontBrush.SetColor(markColor);
		s = CLoc::GetString("RESEARCHED_SPECIALTECHS");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,300,680,30), &fontFormatCenter, &fontBrush);
		fontBrush.SetColor(normalColor);

		int nCount = 0;
		for (int i = 0; i < NoUC; i++)
		{
			RESEARCH_COMPLEX::Typ nComplex = (RESEARCH_COMPLEX::Typ)i;
			// Bereich ermitteln
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
			{
				// Unterbereich bestimmen
				for (int j = 1; j <= 3; j++)
				{
					if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldStatus(j) == RESEARCH_STATUS::RESEARCHED)
					{
						s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexName() + ": " +
							pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldName(j);
						MYTRACE("logdata")(MT::LEVEL_DEBUG, "SPECIAL_RESEARCH research yet: %s \n", s);
						g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,350 + nCount * 30 , 680, 30), &fontFormatCenter, &fontBrush);
						nCount++;
						break;
					}
				}
			}
		}
		if (nCount == 0)
		{
			s = CLoc::GetString("NONE");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,350,680,30), &fontFormatCenter, &fontBrush);
		}
	}
}

void CResearchMenuView::DrawResearchMenue3(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	StringFormat fontFormatTop;
	fontFormatTop.SetAlignment(StringAlignmentCenter);
	fontFormatTop.SetLineAlignment(StringAlignmentNear);
	fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);

	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormatCenter.SetTrimming(StringTrimmingEllipsisCharacter);

	// Wenn wir eine Unique Forschung zur Auswahl haben
	// if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	// {
	// 	if (bg_urmenu)
	// 		g->DrawImage(bg_urmenu, 0, 0, 1075, 750);
	// }
	// else
	// {
	// 	if (bg_emptyur)
			g->DrawImage(bg_emptyur, 0, 0, 1075, 750);
	// }

	// Die gesamten Forschungspunkte rechts anzeigen
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCHPOINTS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,80,240,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CLoc::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);

	s.Format("%d %s",pMajor->GetEmpire()->GetFP(),CLoc::GetString("FP"));
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	// Die einzelnen Forschungsboni anzeigen
	fontBrush.SetColor(normalColor);
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH_BONI")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,145,245,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	RectF rect(855,180,190,25);
	s = CLoc::GetString("BIOTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetBioTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("ENERGYTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("COMPUTERTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetCompTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetConstructionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetPropulsionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("WEAPONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetWeaponTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	fontBrush.SetColor(normalColor);

	// gro�en Rechtecke f�r die einzelnen Forschungsgebiete zeichnen
	// Uniqueforschung
	// Wenn wir eine Unique Forschung zur Auswahl haben, dann auch anzeigen
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	{
		USHORT techPercentage;
		RectF TechRect(10,80,250,25);
		USHORT level = pMajor->GetEmpire()->GetResearch()->GetNumberOfUniqueResearch();
		// UniqueForschung braucht ja soviele FP wie alle anderen des Level zusammen, deswegen die lange Anweisung
		ULONG allOthers = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetBio(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetEnergy(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetComp(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetPropulsion(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetConstruction(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetWeapon(level);
		// man ben�tigt etwas weniger Punkte als alle Techs zusammen
		allOthers /= SPECIAL_RESEARCH_DIV;
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), TechRect, &fontFormatTop, &fontBrush);

		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\specialtech.bop");
		if (graphic)
			g->DrawImage(graphic, 45, 100, 180, 150);

		s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetUniqueFP()*100)/(allOthers)));
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH PROGRESS %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,290,250,25), &fontFormatCenter, &fontBrush);

		techPercentage = pMajor->GetEmpire()->GetResearch()->GetUniquePercentage();
		s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage);
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH ASSIGNMENT %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,315,250,25), &fontFormatCenter, &fontBrush);

		Color penMark;
		penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
		for (int j = 1; j < 101; j += 5)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf dr�cken k�nnen
		{
			RectF timber(ResearchTimber[6][j].left, ResearchTimber[6][j].top, ResearchTimber[6][j].Width() + 6, ResearchTimber[6][j].Height());
			if (j <= techPercentage)
				g->FillRectangle(&SolidBrush(Color(250-j*2.5,50+j*2,0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(75,75,75,75)), timber);
		}
		// farbige Kante um Zuweisungsbalken zeichnen
		RectF markRect(ResearchTimber[6][1].left, ResearchTimber[6][1].top, ResearchTimber[6][100].left - ResearchTimber[6][1].left, ResearchTimber[6][1].Height());
		g->DrawRectangle(&Gdiplus::Pen(penMark), markRect);

		BOOL lockstatus;
		fontFormatCenter.SetAlignment(StringAlignmentCenter);
		fontFormatCenter.SetLineAlignment(StringAlignmentFar);

		lockstatus = pMajor->GetEmpire()->GetResearch()->GetLockStatus(6);
		if (lockstatus == FALSE)
		{
			s = CLoc::GetString("UNLOCKED");
			fontBrush.SetColor(normalColor);
		}
		else
		{
			s = CLoc::GetString("LOCKED");
			fontBrush.SetColor(Color(255,0,0));
		}
		RectF lock(LockStatusRect[6].left, LockStatusRect[6].top, LockStatusRect[6].Width(), LockStatusRect[6].Height());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), lock, &fontFormatCenter, &fontBrush);
		fontFormatCenter.SetLineAlignment(StringAlignmentCenter);

		// Den Namen des Unique Komplexes hinschreiben
		fontBrush.SetColor(markColor);
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH Name %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(325,75,500,25), &fontFormatTop, &fontBrush);
		fontBrush.SetColor(normalColor);

		// Die Beschreibung des Unique Complexes hinschreiben
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexDescription();
		fontFormatTop.SetFormatFlags(!StringFormatFlagsNoWrap);
		fontFormatTop.SetTrimming(StringTrimmingEllipsisCharacter);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(385,120,380,260), &fontFormatTop, &fontBrush);
		fontFormatTop.SetTrimming(StringTrimmingNone);

		// hier die 3 Wahlm�glichkeiten
		fontBrush.SetColor(markColor);
		// 1. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(1);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 2. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(2);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 3. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(3);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,445,250,300), &fontFormatTop, &fontBrush);
		}

		// ab hier die Beschreibungen zu den einzelnen Wahlm�glichkeiten
		fontBrush.SetColor(normalColor);
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(1);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(2);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(3);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,490,250,205), &fontFormatTop, &fontBrush);
		}
		fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);
	}

	// Die einzelnen Men�buttons f�r Spezialforschng, Datenbank und Schiffskonstruktion darstellen
	// Schriftfarbe w�hlen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	fontBrush.SetColor(btnColor);
	DrawGDIButtons(g, &m_ResearchMainButtons, m_bySubMenu, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
	// Nun die Buttons zur Auswahl einer Uniqueforschung
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE && pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
	{
		CString sFile = pMajor->GetPrefix() + "button.bop";
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + sFile);
		if (graphic)
		{
			g->DrawImage(graphic, 55, 698, 160, 40);
			g->DrawImage(graphic, 315, 698, 160, 40);
			g->DrawImage(graphic, 575, 698, 160, 40);
		}
		s = CLoc::GetString("BTN_SELECT");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(55,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(315,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(575,698,160,40), &fontFormatCenter, &fontBrush);
	}

	// "Spezialforschung" in der Mitte zeichnen + wenn mgl. den Namen des Forschungskomplexes
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);

	// Wenn wir eine Uniqueforschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		s.Format("%s - %s",CLoc::GetString("SPECIAL_RESEARCH"),	pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName());
	// Wenn wir keine Uniqueforschung zur Auswahl haben
	else
		s = CLoc::GetString("SPECIAL_RESEARCH");
	//g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,m_TotalSize.cx,55), &fontFormatCenter, &fontBrush);
	g->DrawString(CComBSTR("Menue3"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,m_TotalSize.cx,55), &fontFormatCenter, &fontBrush);

	// Wenn wir keine Unique Forschung zur Auswahl haben
	//if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() != FALSE)
	//{
		/*
		fontFormatCenter.SetLineAlignment(StringAlignmentNear);
		fontFormatCenter.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CLoc::GetString("NO_SPECIAL_RESEARCH_AVAILABLE");
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "NO_SPECIAL_RESEARCH_AVAILABLE: %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190,200,500,100), &fontFormatCenter, &fontBrush);

		// darunter die schon erforschten Spezialforschungen anzeigen
		CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);

		fontBrush.SetColor(markColor);
		s = CLoc::GetString("RESEARCHED_SPECIALTECHS");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,300,680,30), &fontFormatCenter, &fontBrush);
		fontBrush.SetColor(normalColor);
		*/

		int nCount = 0;
		for (int i = 0; i < NoUC; i++)
		{
			RESEARCH_COMPLEX::Typ nComplex = (RESEARCH_COMPLEX::Typ)i;
			// Bereich ermitteln
			//if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
			//{
				// Unterbereich bestimmen
				//for (int j = 1; j <= 3; j++)
				//{
					//if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldStatus(j) == RESEARCH_STATUS::RESEARCHED)
					//{
				s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexName(); // + ": " +
						//s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexName() + ": " +
							//pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldName(j);
						//MYTRACE("logdata")(MT::LEVEL_DEBUG, "SPECIAL_RESEARCH research yet: %s \n", s);
						g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,350 + nCount * 30 , 680, 30), &fontFormatCenter, &fontBrush);
						nCount++;
						break;
					//}
				//}
			//}
		}
		if (nCount == 0)
		{
			s = CLoc::GetString("NONE");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,350,680,30), &fontFormatCenter, &fontBrush);
		}
	//}
}
void CResearchMenuView::DrawResearchMenue4(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	StringFormat fontFormatTop;
	fontFormatTop.SetAlignment(StringAlignmentCenter);
	fontFormatTop.SetLineAlignment(StringAlignmentNear);
	fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);

	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormatCenter.SetTrimming(StringTrimmingEllipsisCharacter);

	// Wenn wir eine Unique Forschung zur Auswahl haben
	// if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	// {
	// 	if (bg_urmenu)
	// 		g->DrawImage(bg_urmenu, 0, 0, 1075, 750);
	// }
	// else
	// {
	// 	if (bg_emptyur)
			g->DrawImage(bg_emptyur, 0, 0, 1075, 750);
	// }

	// Die gesamten Forschungspunkte rechts anzeigen
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCHPOINTS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,80,240,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CLoc::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);

	s.Format("%d %s",pMajor->GetEmpire()->GetFP(),CLoc::GetString("FP"));
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	// Die einzelnen Forschungsboni anzeigen
	fontBrush.SetColor(normalColor);
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH_BONI")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,145,245,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	RectF rect(855,180,190,25);
	s = CLoc::GetString("BIOTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetBioTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("ENERGYTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("COMPUTERTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetCompTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetConstructionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetPropulsionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("WEAPONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetWeaponTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	fontBrush.SetColor(normalColor);

	// gro�en Rechtecke f�r die einzelnen Forschungsgebiete zeichnen
	// Uniqueforschung
	// Wenn wir eine Unique Forschung zur Auswahl haben, dann auch anzeigen
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	{
		USHORT techPercentage;
		RectF TechRect(10,80,250,25);
		USHORT level = pMajor->GetEmpire()->GetResearch()->GetNumberOfUniqueResearch();
		// UniqueForschung braucht ja soviele FP wie alle anderen des Level zusammen, deswegen die lange Anweisung
		ULONG allOthers = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetBio(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetEnergy(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetComp(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetPropulsion(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetConstruction(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetWeapon(level);
		// man ben�tigt etwas weniger Punkte als alle Techs zusammen
		allOthers /= SPECIAL_RESEARCH_DIV;
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), TechRect, &fontFormatTop, &fontBrush);

		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\specialtech.bop");
		if (graphic)
			g->DrawImage(graphic, 45, 100, 180, 150);

		s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetUniqueFP()*100)/(allOthers)));
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH PROGRESS %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,290,250,25), &fontFormatCenter, &fontBrush);

		techPercentage = pMajor->GetEmpire()->GetResearch()->GetUniquePercentage();
		s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage);
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH ASSIGNMENT %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,315,250,25), &fontFormatCenter, &fontBrush);

		Color penMark;
		penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
		for (int j = 1; j < 101; j += 5)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf dr�cken k�nnen
		{
			RectF timber(ResearchTimber[6][j].left, ResearchTimber[6][j].top, ResearchTimber[6][j].Width() + 6, ResearchTimber[6][j].Height());
			if (j <= techPercentage)
				g->FillRectangle(&SolidBrush(Color(250-j*2.5,50+j*2,0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(75,75,75,75)), timber);
		}
		// farbige Kante um Zuweisungsbalken zeichnen
		RectF markRect(ResearchTimber[6][1].left, ResearchTimber[6][1].top, ResearchTimber[6][100].left - ResearchTimber[6][1].left, ResearchTimber[6][1].Height());
		g->DrawRectangle(&Gdiplus::Pen(penMark), markRect);

		BOOL lockstatus;
		fontFormatCenter.SetAlignment(StringAlignmentCenter);
		fontFormatCenter.SetLineAlignment(StringAlignmentFar);

		lockstatus = pMajor->GetEmpire()->GetResearch()->GetLockStatus(6);
		if (lockstatus == FALSE)
		{
			s = CLoc::GetString("UNLOCKED");
			fontBrush.SetColor(normalColor);
		}
		else
		{
			s = CLoc::GetString("LOCKED");
			fontBrush.SetColor(Color(255,0,0));
		}
		RectF lock(LockStatusRect[6].left, LockStatusRect[6].top, LockStatusRect[6].Width(), LockStatusRect[6].Height());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), lock, &fontFormatCenter, &fontBrush);
		fontFormatCenter.SetLineAlignment(StringAlignmentCenter);

		// Den Namen des Unique Komplexes hinschreiben
		fontBrush.SetColor(markColor);
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH Name %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(325,75,500,25), &fontFormatTop, &fontBrush);
		fontBrush.SetColor(normalColor);

		// Die Beschreibung des Unique Complexes hinschreiben
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexDescription();
		fontFormatTop.SetFormatFlags(!StringFormatFlagsNoWrap);
		fontFormatTop.SetTrimming(StringTrimmingEllipsisCharacter);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(385,120,380,260), &fontFormatTop, &fontBrush);
		fontFormatTop.SetTrimming(StringTrimmingNone);

		// hier die 3 Wahlm�glichkeiten
		fontBrush.SetColor(markColor);
		// 1. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(1);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 2. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(2);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 3. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(3);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,445,250,300), &fontFormatTop, &fontBrush);
		}

		// ab hier die Beschreibungen zu den einzelnen Wahlm�glichkeiten
		fontBrush.SetColor(normalColor);
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(1);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(2);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(3);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,490,250,205), &fontFormatTop, &fontBrush);
		}
		fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);
	}

	// Die einzelnen Men�buttons f�r Spezialforschng, Datenbank und Schiffskonstruktion darstellen
	// Schriftfarbe w�hlen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	fontBrush.SetColor(btnColor);
	DrawGDIButtons(g, &m_ResearchMainButtons, m_bySubMenu, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
	// Nun die Buttons zur Auswahl einer Uniqueforschung
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE && pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
	{
		CString sFile = pMajor->GetPrefix() + "button.bop";
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + sFile);
		if (graphic)
		{
			g->DrawImage(graphic, 55, 698, 160, 40);
			g->DrawImage(graphic, 315, 698, 160, 40);
			g->DrawImage(graphic, 575, 698, 160, 40);
		}
		s = CLoc::GetString("BTN_SELECT");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(55,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(315,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(575,698,160,40), &fontFormatCenter, &fontBrush);
	}

	// "Spezialforschung" in der Mitte zeichnen + wenn mgl. den Namen des Forschungskomplexes
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);

	// Wenn wir eine Uniqueforschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		s.Format("%s - %s",CLoc::GetString("SPECIAL_RESEARCH"),	pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName());
	// Wenn wir keine Uniqueforschung zur Auswahl haben
	else
		s = CLoc::GetString("SPECIAL_RESEARCH");
	//g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,m_TotalSize.cx,55), &fontFormatCenter, &fontBrush);
	g->DrawString(CComBSTR("Menue4"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,m_TotalSize.cx,55), &fontFormatCenter, &fontBrush);


	// Wenn wir keine Unique Forschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() != FALSE)
	{
		fontFormatCenter.SetLineAlignment(StringAlignmentNear);
		fontFormatCenter.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CLoc::GetString("NO_SPECIAL_RESEARCH_AVAILABLE");
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "NO_SPECIAL_RESEARCH_AVAILABLE: %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190,200,500,100), &fontFormatCenter, &fontBrush);

		// darunter die schon erforschten Spezialforschungen anzeigen
		CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);

		fontBrush.SetColor(markColor);
		s = CLoc::GetString("RESEARCHED_SPECIALTECHS");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,300,680,30), &fontFormatCenter, &fontBrush);
		fontBrush.SetColor(normalColor);

		int nCount = 0;
		for (int i = 0; i < NoUC; i++)
		{
			RESEARCH_COMPLEX::Typ nComplex = (RESEARCH_COMPLEX::Typ)i;
			// Bereich ermitteln
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
			{
				// Unterbereich bestimmen
				for (int j = 1; j <= 3; j++)
				{
					if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldStatus(j) == RESEARCH_STATUS::RESEARCHED)
					{
						s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexName() + ": " +
							pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldName(j);
						MYTRACE("logdata")(MT::LEVEL_DEBUG, "SPECIAL_RESEARCH research yet: %s \n", s);
						g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,350 + nCount * 30 , 680, 30), &fontFormatCenter, &fontBrush);
						nCount++;
						break;
					}
				}
			}
		}
		if (nCount == 0)
		{
			s = CLoc::GetString("NONE");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,350,680,30), &fontFormatCenter, &fontBrush);
		}
	}
}
void CResearchMenuView::DrawResearchMenue5(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	StringFormat fontFormatTop;
	fontFormatTop.SetAlignment(StringAlignmentCenter);
	fontFormatTop.SetLineAlignment(StringAlignmentNear);
	fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);

	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormatCenter.SetTrimming(StringTrimmingEllipsisCharacter);

	if (bg_researchmenu)
		g->DrawImage(bg_researchmenu, 0, 0, 1075, 750);

	CString s;
	USHORT leveldatabase;
	USHORT techPercentage[7];

	// gro�en Rechtecke f�r die einzelnen Forschungsgebiete zeichnen
	// Biogenetik
	leveldatabase = pMajor->GetEmpire()->GetResearch()->GetBioTech();
	s.Format("%s - %s %d",CLoc::GetString("BIOTECH"),CLoc::GetString("LEVEL"),leveldatabase);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(0);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,110,250,25), &fontFormatCenter, &fontBrush);
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\biotech.bop");
	if (graphic)
		g->DrawImage(graphic, 60,140,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetBioFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetBio(leveldatabase))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[0] = pMajor->GetEmpire()->GetResearch()->GetBioPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[0]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,315,250,25), &fontFormatCenter, &fontBrush);

	// Bautechnik
	leveldatabase = pMajor->GetEmpire()->GetResearch()->GetConstructionTech();
	s.Format("%s - %s %d",CLoc::GetString("CONSTRUCTIONTECH"),CLoc::GetString("LEVEL"),leveldatabase);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(4);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\constructiontech.bop");
	if (graphic)
		g->DrawImage(graphic, 60,480,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetConstructionFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetConstruction(leveldatabase))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[3] = pMajor->GetEmpire()->GetResearch()->GetConstructionPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[3]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,655,250,25), &fontFormatCenter, &fontBrush);

	// Energietechnik
	leveldatabase = pMajor->GetEmpire()->GetResearch()->GetEnergyTech();
	s.Format("%s - %s %d",CLoc::GetString("ENERGYTECH"),CLoc::GetString("LEVEL"),leveldatabase);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(1);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,110,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\energytech.bop");
	if (graphic)
		g->DrawImage(graphic, 320,140,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetEnergyFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetEnergy(leveldatabase))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[1] = pMajor->GetEmpire()->GetResearch()->GetEnergyPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[1]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,315,250,25), &fontFormatCenter, &fontBrush);

	// Antriebstechnik
	leveldatabase = pMajor->GetEmpire()->GetResearch()->GetPropulsionTech();
	s.Format("%s - %s %d",CLoc::GetString("PROPULSIONTECH"),CLoc::GetString("LEVEL"),leveldatabase);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(3);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\propulsiontech.bop");
	if (graphic)
		g->DrawImage(graphic, 320,480,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetPropulsionFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetPropulsion(leveldatabase))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[4] = pMajor->GetEmpire()->GetResearch()->GetPropulsionPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[4]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,655,250,25), &fontFormatCenter, &fontBrush);

	// Computertechnik
	leveldatabase = pMajor->GetEmpire()->GetResearch()->GetCompTech();
	s.Format("%s - %s %d",CLoc::GetString("COMPUTERTECH"),CLoc::GetString("LEVEL"),leveldatabase);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(2);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,110,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\computertech.bop");
	if (graphic)
		g->DrawImage(graphic, 580,140,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetComputerFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetComp(leveldatabase))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[2] = pMajor->GetEmpire()->GetResearch()->GetCompPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[2]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,315,250,25), &fontFormatCenter, &fontBrush);

	// Waffentechnik
	leveldatabase = pMajor->GetEmpire()->GetResearch()->GetWeaponTech();
	s.Format("%s - %s %d",CLoc::GetString("WEAPONTECH"),CLoc::GetString("LEVEL"),leveldatabase);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(5);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\weapontech.bop");
	if (graphic)
		g->DrawImage(graphic, 580,480,150,125);
	s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetWeaponFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetWeapon(leveldatabase))));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[5] = pMajor->GetEmpire()->GetResearch()->GetWeaponPercentage();
	s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage[5]);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,655,250,25), &fontFormatCenter, &fontBrush);


	// Die gesamten Forschungspunkte rechts anzeigen
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCHPOINTS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,80,240,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CLoc::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);

	s.Format("%d %s",pMajor->GetEmpire()->GetFP(),CLoc::GetString("FP"));
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCHPOINTS (TOTAL): %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	// Die einzelnen Forschungsboni anzeigen
	fontBrush.SetColor(normalColor);
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH_BONI")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,145,245,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	RectF rect(855,180,190,25);
	s = CLoc::GetString("BIOTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetBioTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI BIOTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("ENERGYTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI ENERGYTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("COMPUTERTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetCompTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI COMPUTERTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetConstructionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI CONSTRUCTIONTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetPropulsionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI PROPULSIONTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("WEAPONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetWeaponTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	// (ok) MYTRACE("logdata")(MT::LEVEL_DEBUG, "RESEARCH_BONI WEAPONTECH: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);

	fontBrush.SetColor(normalColor);

	Color penMark;
	penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 1; j < 101; j += 5)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf dr�cken k�nnen
		{
			RectF timber(ResearchTimber[i][j].left, ResearchTimber[i][j].top, ResearchTimber[i][j].Width() + 6, ResearchTimber[i][j].Height());
			if (j <= techPercentage[i])
				g->FillRectangle(&SolidBrush(Color(250-j*2.5,50+j*2,0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(75,75,75,75)), timber);
		}
		// farbige Kante um Zuweisungsbalken zeichnen
		RectF markRect(ResearchTimber[i][1].left, ResearchTimber[i][1].top, ResearchTimber[i][100].left - ResearchTimber[i][1].left, ResearchTimber[i][1].Height());
		g->DrawRectangle(&Gdiplus::Pen(penMark), markRect);
	}

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentFar);
	for (int i = 0; i < 6; i++)
	{
		const BOOL lockstatus = pMajor->GetEmpire()->GetResearch()->GetLockStatus(i); 
		if (lockstatus == FALSE)
		{
			s = CLoc::GetString("UNLOCKED");
			fontBrush.SetColor(normalColor);
		}
		else
		{
			s = CLoc::GetString("LOCKED");
			fontBrush.SetColor(Color(255,0,0));
		}
		RectF lock(LockStatusRect[i].left, LockStatusRect[i].top, LockStatusRect[i].Width(), LockStatusRect[i].Height());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), lock, &fontFormatCenter, &fontBrush);
	}

	// Die einzelnen Men�buttons f�r Spezialforschung, Datenbank und Schiffskonstruktion darstellen
	// Schriftfarbe w�hlen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	fontBrush.SetColor(btnColor);
	DrawGDIButtons(g, &m_ResearchMainButtons, m_bySubMenu, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);

	// "Forschungs�bersicht" in der Mitte zeichnen
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);

	//g->DrawString("MENUE5", -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormatCenter, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH_MENUE")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormatCenter, &fontBrush);
}
void CResearchMenuView::DrawResearchMenue6(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	StringFormat fontFormatTop;
	fontFormatTop.SetAlignment(StringAlignmentCenter);
	fontFormatTop.SetLineAlignment(StringAlignmentNear);
	fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);

	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormatCenter.SetTrimming(StringTrimmingEllipsisCharacter);

	// Wenn wir eine Unique Forschung zur Auswahl haben
	//if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	//{
	//	if (bg_urmenu)
	//		g->DrawImage(bg_urmenu, 0, 0, 1075, 750);
	//}
	//else
	//{
	//	if (bg_emptyur)
			g->DrawImage(bg_emptyur, 0, 0, 1075, 750);
	//}

	// Die gesamten Forschungspunkte rechts anzeigen
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCHPOINTS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,80,240,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CLoc::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);

	s.Format("%d %s",pMajor->GetEmpire()->GetFP(),CLoc::GetString("FP"));
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	// Die einzelnen Forschungsboni anzeigen
	fontBrush.SetColor(normalColor);
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH_BONI")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(835,145,245,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);

	RectF rect(855,180,190,25);
	s = CLoc::GetString("BIOTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetBioTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("ENERGYTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("COMPUTERTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetCompTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetConstructionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetPropulsionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CLoc::GetString("WEAPONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetWeaponTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	fontBrush.SetColor(normalColor);

	// gro�en Rechtecke f�r die einzelnen Forschungsgebiete zeichnen
	// Uniqueforschung
	// Wenn wir eine Unique Forschung zur Auswahl haben, dann auch anzeigen
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	{
		USHORT techPercentage;
		RectF TechRect(10,80,250,25);
		USHORT level = pMajor->GetEmpire()->GetResearch()->GetNumberOfUniqueResearch();
		// UniqueForschung braucht ja soviele FP wie alle anderen des Level zusammen, deswegen die lange Anweisung
		ULONG allOthers = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetBio(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetEnergy(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetComp(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetPropulsion(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetConstruction(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetWeapon(level);
		// man ben�tigt etwas weniger Punkte als alle Techs zusammen
		allOthers /= SPECIAL_RESEARCH_DIV;
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), TechRect, &fontFormatTop, &fontBrush);

		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\specialtech.bop");
		if (graphic)
			g->DrawImage(graphic, 45, 100, 180, 150);

		s.Format("%s: %i%%",CLoc::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetUniqueFP()*100)/(allOthers)));
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH PROGRESS %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,290,250,25), &fontFormatCenter, &fontBrush);

		techPercentage = pMajor->GetEmpire()->GetResearch()->GetUniquePercentage();
		s.Format("%s: %i%%",CLoc::GetString("ASSIGNMENT"),techPercentage);
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH ASSIGNMENT %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,315,250,25), &fontFormatCenter, &fontBrush);

		Color penMark;
		penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
		for (int j = 1; j < 101; j += 5)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf dr�cken k�nnen
		{
			RectF timber(ResearchTimber[6][j].left, ResearchTimber[6][j].top, ResearchTimber[6][j].Width() + 6, ResearchTimber[6][j].Height());
			if (j <= techPercentage)
				g->FillRectangle(&SolidBrush(Color(250-j*2.5,50+j*2,0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(75,75,75,75)), timber);
		}
		// farbige Kante um Zuweisungsbalken zeichnen
		RectF markRect(ResearchTimber[6][1].left, ResearchTimber[6][1].top, ResearchTimber[6][100].left - ResearchTimber[6][1].left, ResearchTimber[6][1].Height());
		g->DrawRectangle(&Gdiplus::Pen(penMark), markRect);

		BOOL lockstatus;
		fontFormatCenter.SetAlignment(StringAlignmentCenter);
		fontFormatCenter.SetLineAlignment(StringAlignmentFar);

		lockstatus = pMajor->GetEmpire()->GetResearch()->GetLockStatus(6);
		if (lockstatus == FALSE)
		{
			s = CLoc::GetString("UNLOCKED");
			fontBrush.SetColor(normalColor);
		}
		else
		{
			s = CLoc::GetString("LOCKED");
			fontBrush.SetColor(Color(255,0,0));
		}
		RectF lock(LockStatusRect[6].left, LockStatusRect[6].top, LockStatusRect[6].Width(), LockStatusRect[6].Height());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), lock, &fontFormatCenter, &fontBrush);
		fontFormatCenter.SetLineAlignment(StringAlignmentCenter);

		// Den Namen des Unique Komplexes hinschreiben
		fontBrush.SetColor(markColor);
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "Unique=Special RESEARCH Name %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(325,75,500,25), &fontFormatTop, &fontBrush);
		fontBrush.SetColor(normalColor);

		// Die Beschreibung des Unique Complexes hinschreiben
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexDescription();
		fontFormatTop.SetFormatFlags(!StringFormatFlagsNoWrap);
		fontFormatTop.SetTrimming(StringTrimmingEllipsisCharacter);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(385,120,380,260), &fontFormatTop, &fontBrush);
		fontFormatTop.SetTrimming(StringTrimmingNone);

		// hier die 3 Wahlm�glichkeiten
		fontBrush.SetColor(markColor);
		// 1. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(1);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 2. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(2);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 3. M�glichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(3);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,445,250,300), &fontFormatTop, &fontBrush);
		}

		// ab hier die Beschreibungen zu den einzelnen Wahlm�glichkeiten
		fontBrush.SetColor(normalColor);
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(1);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(10,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(2);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(270,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE ||
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(3);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(530,490,250,205), &fontFormatTop, &fontBrush);
		}
		fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);
	}

	// Die einzelnen Men�buttons f�r Spezialforschng, Datenbank und Schiffskonstruktion darstellen
	// Schriftfarbe w�hlen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	fontBrush.SetColor(btnColor);
	DrawGDIButtons(g, &m_ResearchMainButtons, m_bySubMenu, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
	// Nun die Buttons zur Auswahl einer Uniqueforschung
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE && pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
	{
		CString sFile = pMajor->GetPrefix() + "button.bop";
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + sFile);
		if (graphic)
		{
			g->DrawImage(graphic, 55, 698, 160, 40);
			g->DrawImage(graphic, 315, 698, 160, 40);
			g->DrawImage(graphic, 575, 698, 160, 40);
		}
		s = CLoc::GetString("BTN_SELECT");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(55,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(315,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(575,698,160,40), &fontFormatCenter, &fontBrush);
	}

	// "Spezialforschung" in der Mitte zeichnen + wenn mgl. den Namen des Forschungskomplexes
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);

	// Wenn wir eine Uniqueforschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		s.Format("%s - %s",CLoc::GetString("SPECIAL_RESEARCH"),	pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName());
	// Wenn wir keine Uniqueforschung zur Auswahl haben
	else
		s = CLoc::GetString("SPECIAL_RESEARCH");
	//g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,m_TotalSize.cx,55), &fontFormatCenter, &fontBrush);
	g->DrawString(CComBSTR("Menue6"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,m_TotalSize.cx,55), &fontFormatCenter, &fontBrush);

	// Wenn wir keine Unique Forschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() != FALSE)
	{
		fontFormatCenter.SetLineAlignment(StringAlignmentNear);
		fontFormatCenter.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CLoc::GetString("NO_SPECIAL_RESEARCH_AVAILABLE");
		MYTRACE("logdata")(MT::LEVEL_DEBUG, "NO_SPECIAL_RESEARCH_AVAILABLE: %s \n", s);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190,200,500,100), &fontFormatCenter, &fontBrush);

		// darunter die schon erforschten Spezialforschungen anzeigen
		CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);

		fontBrush.SetColor(markColor);
		s = CLoc::GetString("RESEARCHED_SPECIALTECHS");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,300,680,30), &fontFormatCenter, &fontBrush);
		fontBrush.SetColor(normalColor);

		int nCount = 0;
		for (int i = 0; i < NoUC; i++)
		{
			RESEARCH_COMPLEX::Typ nComplex = (RESEARCH_COMPLEX::Typ)i;
			// Bereich ermitteln
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
			{
				// Unterbereich bestimmen
				for (int j = 1; j <= 3; j++)
				{
					if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldStatus(j) == RESEARCH_STATUS::RESEARCHED)
					{
						s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetComplexName() + ": " +
							pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(nComplex)->GetFieldName(j);
						MYTRACE("logdata")(MT::LEVEL_DEBUG, "SPECIAL_RESEARCH research yet: %s \n", s);
						g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,350 + nCount * 30 , 680, 30), &fontFormatCenter, &fontBrush);
						nCount++;
						break;
					}
				}
			}
		}
		if (nCount == 0)
		{
			s = CLoc::GetString("NONE");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(100,350,680,30), &fontFormatCenter, &fontBrush);
		}
	}
}
void CResearchMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

	// Hier �berpr�fen auf welchen Men�button wir in den Forschungsansichten (normal und unique) geklickt haben
	// wechsel zwischen normaler <--> unique Forschung
	int temp = m_bySubMenu;
	//AfxMessageBox(temp); 
	if (ButtonReactOnLeftClick(point, &m_ResearchMainButtons, temp, FALSE))
	{
		m_bySubMenu = temp;
		// Haben wir auf den Schiffsdesign Button geklickt
		if (m_bySubMenu == 2)
		{
			resources::pMainFrame->SelectMainView(9, pMajor->GetRaceID());	// Schiffsdesignansicht zeichnen
			Invalidate(FALSE);
			m_bySubMenu = 0;
			return;
		}
		else
		{
			Invalidate(FALSE);
			return;
		}
	}
	// Wenn wir uns im normalen Forschungsmenue befinden
	if (m_bySubMenu == 0)
	{
		// haben wir in die Balken geklickt, um den Forschungsanteil zu �ndern
		for (int i = 0; i < 6; i++)
			for (int j = 0; j < 101; j++)
			{
				// Rechteck leicht in der Gr��e modifizieren, damit man es besser anklicken kann
				ResearchTimber[i][j].SetRect(ResearchTimber[i][j].left,ResearchTimber[i][j].top,ResearchTimber[i][j].right+1,ResearchTimber[i][j].bottom);
				if (ResearchTimber[i][j].PtInRect(point))
				{
					pMajor->GetEmpire()->GetResearch()->SetPercentage(i,j);
					Invalidate();
				}
				// modifizerte Rechteckgr��e wieder r�ckg�ngig machen
				ResearchTimber[i][j].SetRect(ResearchTimber[i][j].left,ResearchTimber[i][j].top,ResearchTimber[i][j].right-1,ResearchTimber[i][j].bottom);
			}

			// Haben wir auf ein Bild gedr�ckt, um es zu locken oder unlocken
			for (int i = 0; i < 6; i++)
				if (LockStatusRect[i].PtInRect(point))
				{
					pMajor->GetEmpire()->GetResearch()->SetLock(i,!pMajor->GetEmpire()->GetResearch()->GetLockStatus(i));
					CRect r = LockStatusRect[i];
					CalcDeviceRect(r);
					InvalidateRect(r, FALSE);
					break;
				}
	}
	// Wenn wir uns im Uniqueforschungsmenue befinden
	else if (m_bySubMenu == 1)
	{
		// Ist die Uniqueforschung m�glich?
		if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		{
			// 6 steht hier �berall f�r Unique Forschung!!
			for (int j = 0; j < 101; j++)
			{
				// Rechteck leicht in der Gr��e modifizieren, damit man es besser anklicken kann
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right+1,ResearchTimber[6][j].bottom);
				if (ResearchTimber[6][j].PtInRect(point))
				{
					pMajor->GetEmpire()->GetResearch()->SetPercentage(6,j);
					Invalidate();
				}
				// modifizerte Rechteckgr��e wieder r�ckg�ngig machen
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right-1,ResearchTimber[6][j].bottom);
			}
			// Pr�fen ob wir auf ein Rechteck gedr�ckt haben, um eine der 3 M�glichkeiten auszuw�hlen
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
			{
				CRect r[3];
				for (int i = 0; i < 3; i++)
				{
					r[i].SetRect(55+i*260,680,215+i*260,720);
					if (r[i].PtInRect(point))
					{
						pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(i+1);
						Invalidate();
					}
				}
			}
			// Wollen wir den Lockstatus �ndern?
			if (LockStatusRect[6].PtInRect(point))
			{
				pMajor->GetEmpire()->GetResearch()->SetLock(6,!pMajor->GetEmpire()->GetResearch()->GetLockStatus(6));
				CRect r = LockStatusRect[6];
				CalcDeviceRect(r);
				InvalidateRect(r, FALSE);
			}
		}

			// Wenn wir uns im Menue 3 befinden
	else if (m_bySubMenu == 3)
	{
	//AfxMessageBox("SubMenu3"); 
		// Ist die Uniqueforschung m�glich?
		if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		{
			// 6 steht hier �berall f�r Unique Forschung!!
			for (int j = 0; j < 101; j++)
			{
				// Rechteck leicht in der Gr��e modifizieren, damit man es besser anklicken kann
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right+1,ResearchTimber[6][j].bottom);
				if (ResearchTimber[6][j].PtInRect(point))
				{
					pMajor->GetEmpire()->GetResearch()->SetPercentage(6,j);
					Invalidate();
				}
				// modifizerte Rechteckgr��e wieder r�ckg�ngig machen
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right-1,ResearchTimber[6][j].bottom);
			}
			// Pr�fen ob wir auf ein Rechteck gedr�ckt haben, um eine der 3 M�glichkeiten auszuw�hlen
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
			{
				CRect r[3];
				for (int i = 0; i < 3; i++)
				{
					r[i].SetRect(55+i*260,680,215+i*260,720);
					if (r[i].PtInRect(point))
					{
						pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(i+1);
						Invalidate();
					}
				}
			}
			// Wollen wir den Lockstatus �ndern?
			if (LockStatusRect[6].PtInRect(point))
			{
				pMajor->GetEmpire()->GetResearch()->SetLock(6,!pMajor->GetEmpire()->GetResearch()->GetLockStatus(6));
				CRect r = LockStatusRect[6];
				CalcDeviceRect(r);
				InvalidateRect(r, FALSE);
			}
		}
			// Wenn wir uns im menue 4 befinden
	else if (m_bySubMenu == 4)
	{
		//AfxMessageBox("SubMenu4"); 
		// Ist die Uniqueforschung m�glich?
		if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		{
			// 6 steht hier �berall f�r Unique Forschung!!
			for (int j = 0; j < 101; j++)
			{
				// Rechteck leicht in der Gr��e modifizieren, damit man es besser anklicken kann
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right+1,ResearchTimber[6][j].bottom);
				if (ResearchTimber[6][j].PtInRect(point))
				{
					pMajor->GetEmpire()->GetResearch()->SetPercentage(6,j);
					Invalidate();
				}
				// modifizerte Rechteckgr��e wieder r�ckg�ngig machen
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right-1,ResearchTimber[6][j].bottom);
			}
			// Pr�fen ob wir auf ein Rechteck gedr�ckt haben, um eine der 3 M�glichkeiten auszuw�hlen
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
			{
				CRect r[3];
				for (int i = 0; i < 3; i++)
				{
					r[i].SetRect(55+i*260,680,215+i*260,720);
					if (r[i].PtInRect(point))
					{
						pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(i+1);
						Invalidate();
					}
				}
			}
			// Wollen wir den Lockstatus �ndern?
			if (LockStatusRect[6].PtInRect(point))
			{
				pMajor->GetEmpire()->GetResearch()->SetLock(6,!pMajor->GetEmpire()->GetResearch()->GetLockStatus(6));
				CRect r = LockStatusRect[6];
				CalcDeviceRect(r);
				InvalidateRect(r, FALSE);
			}
		}
			// Wenn wir uns im menue 5 befinden
	else if (m_bySubMenu == 5)
	{
		//AfxMessageBox("SubMenu5"); 
	//	if (m_bySubMenu == 0)
	//{
		// haben wir in die Balken geklickt, um den Forschungsanteil zu �ndern
		for (int i = 0; i < 6; i++)
			for (int j = 0; j < 101; j++)
			{
				// Rechteck leicht in der Gr��e modifizieren, damit man es besser anklicken kann
				ResearchTimber[i][j].SetRect(ResearchTimber[i][j].left,ResearchTimber[i][j].top,ResearchTimber[i][j].right+1,ResearchTimber[i][j].bottom);
				if (ResearchTimber[i][j].PtInRect(point))
				{
					//pMajor->GetEmpire()->GetResearch()->SetPercentage(i,j);
					Invalidate();
				}
				// modifizerte Rechteckgr��e wieder r�ckg�ngig machen
				ResearchTimber[i][j].SetRect(ResearchTimber[i][j].left,ResearchTimber[i][j].top,ResearchTimber[i][j].right-1,ResearchTimber[i][j].bottom);
			} 

			
	}
		
			
		}
		

	CMainBaseView::OnLButtonDown(nFlags, point);
}
} 
}

void CResearchMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CResearchBottomView* pView = dynamic_cast<CResearchBottomView*>(resources::pMainFrame->GetView(RUNTIME_CLASS(CResearchBottomView)));
	if (!pView)
		return;

	CalcLogicalPoint(point);

	ButtonReactOnMouseOver(point, &m_ResearchMainButtons);
	if (m_bySubMenu == 0)
	{
		if (pView->GetCurrentTech() != 0 && CRect(10,80,260,380).PtInRect(point))
		{
			pView->SetCurrentTech(0);
			pView->Invalidate(FALSE);
		}
		else if (pView->GetCurrentTech() != 1 && CRect(270,80,520,380).PtInRect(point))
		{
			pView->SetCurrentTech(1);
			pView->Invalidate(FALSE);
		}
		else if (pView->GetCurrentTech() != 2 && CRect(530,80,780,380).PtInRect(point))
		{
			pView->SetCurrentTech(2);
			pView->Invalidate(FALSE);
		}
		else if (pView->GetCurrentTech() != 3 && CRect(270,425,520,725).PtInRect(point))
		{
			pView->SetCurrentTech(3);
			pView->Invalidate(FALSE);
		}
		else if (pView->GetCurrentTech() != 4 && CRect(10,425,260,725).PtInRect(point))
		{
			pView->SetCurrentTech(4);
			pView->Invalidate(FALSE);
		}
		else if (pView->GetCurrentTech() != 5 && CRect(530,425,780,725).PtInRect(point))
		{
			pView->SetCurrentTech(5);
			pView->Invalidate(FALSE);
		}
	}
	else if (m_bySubMenu == 1)
	{
		if (pView->GetCurrentTech() != 6)
		{
			CMajor* pMajor = m_pPlayersRace;
			ASSERT(pMajor);
			if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
			{
				pView->SetCurrentTech(6);
				pView->Invalidate(FALSE);
			}
		}
	}

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CResearchMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Unique Forschung einstellen
	if (nChar == VK_NUMPAD1)
	{
		pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(1);
	}
	else if (nChar == VK_NUMPAD2)
	{
		pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(2);
	}
	else if (nChar == VK_NUMPAD3)
	{
		pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(3);
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CResearchMenuView::CreateButtons()
{
	ASSERT((CBotEDoc*)GetDocument());

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// alle Buttons in der View anlegen und Grafiken laden
	CString sPrefix = pMajor->GetPrefix();
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	// Buttons in den Forschungsansichten
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,450) , CSize(160,40), CLoc::GetString("BTN_NORMAL"), fileN, fileI, fileA));
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,500) , CSize(160,40), CLoc::GetString("BTN_SPECIAL"), fileN, fileI, fileA));
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,550) , CSize(160,40), CLoc::GetString("BTN_SHIPDESIGN"), fileN, fileI, fileA));
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,600) , CSize(160,40), "Menue3", fileN, fileI, fileA)); //later put into Stringtable
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,650) , CSize(160,40), "Menue4", fileN, fileI, fileA)); //later put into Stringtable
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,700) , CSize(160,40), "Menue5", fileN, fileI, fileA)); //later put into Stringtable
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CResearchMenuView::CreateTooltip(void)
{
	// nur im normalen Forschungsmen�
	if (m_bySubMenu != 0)
		return "";

	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	// �ber welches Tech wurde die Maus gehalten?
	int nTech = -1;
	if (CRect(10,80,260,380).PtInRect(pt))
		nTech = 0;
	else if (CRect(270,80,520,380).PtInRect(pt))
		nTech = 1;
	else if (CRect(530,80,780,380).PtInRect(pt))
		nTech = 2;
	else if (CRect(270,425,520,725).PtInRect(pt))
		nTech = 3;
	else if (CRect(10,425,260,725).PtInRect(pt))
		nTech = 4;
	else if (CRect(530,425,780,725).PtInRect(pt))
		nTech = 5;
	if (nTech == -1)
		return "";

	// mit dieser Forschung nun baubare Geb�ude zeichnen
	BYTE researchLevels[6] =
		{
			pMajor->GetEmpire()->GetResearch()->GetBioTech(),
			pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
			pMajor->GetEmpire()->GetResearch()->GetCompTech(),
			pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
			pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
			pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
		};
	// aktuelle Forschung so umstellen, als w�re die markierte Tech schon erforscht
	researchLevels[nTech] += 1;

	// dann erforschbare neue Objekte ermitteln
	std::vector<CBuildingInfo*> vNewBuildings;
	std::vector<CShipInfo*>		vNewShips;
	std::vector<CTroopInfo*>	vNewTroops;
	for (int i = 0; i < pDoc->BuildingInfo.GetSize(); i++)
		if (pDoc->BuildingInfo[i].GetOwnerOfBuilding() == pMajor->GetRaceBuildingNumber())
			if (pDoc->BuildingInfo[i].IsBuildingBuildableNow(researchLevels))
			{
				int nTechLevel = -1;
				switch (nTech)
				{
				case 0: nTechLevel = pDoc->BuildingInfo[i].GetBioTech();			break;
				case 1: nTechLevel = pDoc->BuildingInfo[i].GetEnergyTech();			break;
				case 2: nTechLevel = pDoc->BuildingInfo[i].GetCompTech();			break;
				case 3: nTechLevel = pDoc->BuildingInfo[i].GetPropulsionTech();		break;
				case 4: nTechLevel = pDoc->BuildingInfo[i].GetConstructionTech();	break;
				case 5: nTechLevel = pDoc->BuildingInfo[i].GetWeaponTech();			break;
				}
				if (nTechLevel != -1 && nTechLevel == researchLevels[nTech])
					vNewBuildings.push_back(&pDoc->BuildingInfo[i]);
			}

	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
		if (pDoc->m_ShipInfoArray[i].GetRace() == pMajor->GetRaceShipNumber())
			if (pDoc->m_ShipInfoArray[i].IsThisShipBuildableNow(researchLevels))
			{
				int nTechLevel = -1;
				switch (nTech)
				{
				case 0: nTechLevel = pDoc->m_ShipInfoArray[i].GetBioTech();				break;
				case 1: nTechLevel = pDoc->m_ShipInfoArray[i].GetEnergyTech();			break;
				case 2: nTechLevel = pDoc->m_ShipInfoArray[i].GetComputerTech();		break;
				case 3: nTechLevel = pDoc->m_ShipInfoArray[i].GetPropulsionTech();		break;
				case 4: nTechLevel = pDoc->m_ShipInfoArray[i].GetConstructionTech();	break;
				case 5: nTechLevel = pDoc->m_ShipInfoArray[i].GetWeaponTech();			break;
				}
				if (nTechLevel != -1 && nTechLevel == researchLevels[nTech])
					vNewShips.push_back(&pDoc->m_ShipInfoArray[i]);
			}

	for (int i = 0; i < pDoc->m_TroopInfo.GetSize(); i++)
		if (pDoc->m_TroopInfo[i].GetOwner() == pMajor->GetRaceID())
			if (pDoc->m_TroopInfo[i].IsThisTroopBuildableNow(researchLevels))
			{
				int nTechLevel = pDoc->m_TroopInfo[i].GetNeededTechlevel(nTech);
				if (nTechLevel != -1 && nTechLevel == researchLevels[nTech])
					vNewTroops.push_back(&pDoc->m_TroopInfo[i]);
			}

	// neue Geb�ude zeichnen
	CString sBuildings(CLoc::GetString("RESEARCHEVENT_NEWBUILDINGS"));
	sBuildings = CHTMLStringBuilder::GetHTMLColor(sBuildings, _T("silver"));
	sBuildings = CHTMLStringBuilder::GetHTMLHeader(sBuildings, _T("h3"));
	sBuildings += CHTMLStringBuilder::GetHTMLStringNewLine();
	sBuildings += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sBuildings += CHTMLStringBuilder::GetHTMLStringNewLine();
	CString s = "";
	for (vector<CBuildingInfo*>::const_iterator it = vNewBuildings.begin(); it != vNewBuildings.end(); ++it)
	{
		s += (*it)->GetBuildingName();
		s = CHTMLStringBuilder::GetHTMLColor(s);
		s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
		s += CHTMLStringBuilder::GetHTMLStringNewLine();
	}
	// gibt es keine neuen Geb�ude
	if (vNewBuildings.empty())
	{
		s = CLoc::GetString("NONE");
		s = CHTMLStringBuilder::GetHTMLColor(s);
		s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
		s += CHTMLStringBuilder::GetHTMLStringNewLine();
	}
	sBuildings += s;


	// neue Schiffe und Truppen zeichnen
	CString sShips = "";
	sShips += CHTMLStringBuilder::GetHTMLStringNewLine();
	sShips += CLoc::GetString("RESEARCHEVENT_NEWSHIPS_AND_TROOPS");
	sShips = CHTMLStringBuilder::GetHTMLColor(sShips, _T("silver"));
	sShips = CHTMLStringBuilder::GetHTMLHeader(sShips, _T("h3"));
	sShips += CHTMLStringBuilder::GetHTMLStringNewLine();
	sShips += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sShips += CHTMLStringBuilder::GetHTMLStringNewLine();
	s = "";
	for (vector<CShipInfo*>::const_iterator it = vNewShips.begin(); it != vNewShips.end(); ++it)
	{
		s += (*it)->GetShipClass() + "-" + CLoc::GetString("CLASS") + " (" + (*it)->GetShipTypeAsString() + ")";
		s = CHTMLStringBuilder::GetHTMLColor(s);
		s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
		s += CHTMLStringBuilder::GetHTMLStringNewLine();
	}
	for (vector<CTroopInfo*>::const_iterator it = vNewTroops.begin(); it != vNewTroops.end(); ++it)
	{
		s += (*it)->GetName();
		s = CHTMLStringBuilder::GetHTMLColor(s);
		s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
		s += CHTMLStringBuilder::GetHTMLStringNewLine();
	}
	// gibt es keine neuen Schiffe und Truppen
	if (vNewShips.empty() && vNewTroops.empty())
	{
		s = CLoc::GetString("NONE");
		s = CHTMLStringBuilder::GetHTMLColor(s);
		s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
		s += CHTMLStringBuilder::GetHTMLStringNewLine();
	}
	sShips += s;

	CString sHeader = CLoc::GetString("NEW_TECHNOLOGY_ALLOWS");
	sHeader = CHTMLStringBuilder::GetHTMLColor(sHeader);
	sHeader = CHTMLStringBuilder::GetHTMLHeader(sHeader, _T("h3"));
	sHeader += CHTMLStringBuilder::GetHTMLStringNewLine();
	sHeader += CHTMLStringBuilder::GetHTMLStringNewLine();

	return CHTMLStringBuilder::GetHTMLCenter(sHeader + sBuildings + sShips);
}
