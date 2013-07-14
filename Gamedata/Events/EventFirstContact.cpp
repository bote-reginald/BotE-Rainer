#include "stdafx.h"
#include "EventFirstContact.h"
#include "FontLoader.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "General/Loc.h"
#include "GraphicPool.h"

IMPLEMENT_SERIAL (CEventFirstContact, CObject, 1)

CEventFirstContact::CEventFirstContact(const CString &sPlayersRaceID, const CString &sRaceID) : m_sRaceID(sRaceID),
	CEventScreen(sPlayersRaceID, "FirstContact")
{
}

CEventFirstContact::CEventFirstContact(void) : m_sRaceID("")
{
}

CEventFirstContact::~CEventFirstContact(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEventFirstContact::Serialize(CArchive &ar)
{
	__super::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sRaceID;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_sRaceID;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CEventFirstContact::Create(void)
{
	CEventScreen::Create();
}

void CEventFirstContact::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	CEventScreen::Draw(g, graphicPool);

	if (m_sRaceID == "")
		return;

	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);
	CRace* pContactedRace = pDoc->GetRaceCtrl()->GetRace(m_sRaceID);
	if (!pContactedRace)
		return;

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sRace));
	ASSERT(pMajor);

	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);

	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	Gdiplus::Color color;
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrNormalText);
	fontBrush.SetColor(color);

	// �berschrift zeichnen
	s = CLoc::GetString("FIRSTCONTACTEVENT_HEADLINE");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(730,500,540,75), &fontFormat, &fontBrush);

	CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	s = CLoc::GetString("FIRSTCONTACTEVENT_TEXT", FALSE, pContactedRace->GetRaceName());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(800,610,470,100), &fontFormat, &fontBrush);

	// Bild der Rasse zeichnen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\" + pContactedRace->GetGraphicFileName());
	if (graphic == NULL)
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\ImageMissing.boj");
	if (graphic)
		g->DrawImage(graphic, 6, 399, 301, 300);
	// Name der Rasse zeichnen
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	s = pContactedRace->GetRaceName();
	MYTRACE("logevent")(MT::LEVEL_INFO, "NewContact - Race: %s \n", s);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(5,717,304,50), &fontFormat, &fontBrush);

	// Eigenschaften zeichnen
	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	fontBrush.SetColor(markColor);
	s = CLoc::GetString("PROPERTIES");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(730,140,540,75), &fontFormat, &fontBrush);

	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontBrush.SetColor(color);
	vector<CString> sProperties;
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::FINANCIAL))
		sProperties.push_back(CLoc::GetString("FINANCIAL"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::WARLIKE))
		sProperties.push_back(CLoc::GetString("WARLIKE"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::AGRARIAN))
		sProperties.push_back(CLoc::GetString("AGRARIAN"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::INDUSTRIAL))
		sProperties.push_back(CLoc::GetString("INDUSTRIAL"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::SECRET))
		sProperties.push_back(CLoc::GetString("SECRET"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::SCIENTIFIC))
		sProperties.push_back(CLoc::GetString("SCIENTIFIC"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::PRODUCER))
		sProperties.push_back(CLoc::GetString("PRODUCER"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::PACIFIST))
		sProperties.push_back(CLoc::GetString("PACIFIST"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::SNEAKY))
		sProperties.push_back(CLoc::GetString("SNEAKY"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::SOLOING))
		sProperties.push_back(CLoc::GetString("SOLOING"));
	if (pContactedRace->IsRaceProperty(RACE_PROPERTY::HOSTILE))
		sProperties.push_back(CLoc::GetString("HOSTILE"));
	if (sProperties.empty())
		sProperties.push_back(CLoc::GetString("NONE"));

	for (UINT i = 0; i < sProperties.size(); i++)
	{
		MYTRACE("logevent")(MT::LEVEL_INFO, "NewContact - sProperties: %s, Relation:%d\n",sProperties[i],pContactedRace->GetRelation(pMajor->GetRaceID()));
		g->DrawString(CComBSTR(sProperties[i]), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(730,210 + i*30,540,30), &fontFormat, &fontBrush);
	}

	//Beziehungsbalken zeichnen
	if (pContactedRace->IsMinor() || pContactedRace->IsMajor() && ((CMajor*)pContactedRace)->IsHumanPlayer() == false)
	{
		// see above MYTRACE("logevent")(MT::LEVEL_INFO, "NewContact - Relation: %d \n", pContactedRace->GetRelation(pMajor->GetRaceID()));
		for (int t = 0; t < 20; t++)
		{
			RectF timber(7+t*15,360,13,30);
			if (pContactedRace->GetRelation(pMajor->GetRaceID()) * 2 / 10 > t)
				g->FillRectangle(&SolidBrush(Color(200,250-t*12, 0+t*12, 0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(100,100,100,100)), timber);
		}
	}
	fontBrush.SetColor(color);
	// Technischen Fortschritt zeichnen
	CString sProgress;
	if (pContactedRace->IsMinor())
	{
		CMinor* pMinor = dynamic_cast<CMinor*>(pContactedRace);
		if (!pMinor->IsAlienRace())
		{
			switch (pMinor->GetTechnologicalProgress())
			{
			case 0: sProgress = CLoc::GetString("VERY_UNDERDEVELOPED");	break;
			case 1: sProgress = CLoc::GetString("UNDERDEVELOPED");		break;
			case 2: sProgress = CLoc::GetString("NORMAL_DEVELOPED");	break;
			case 3: sProgress = CLoc::GetString("DEVELOPED");			break;
			case 4: sProgress = CLoc::GetString("VERY_DEVELOPED");		break;
			}
		}
	}
	else if (pContactedRace->IsMajor())
	{
		MYTRACE("logevent")(MT::LEVEL_INFO, "NewContact - Relation: %d \n", pContactedRace->GetRelation(pMajor->GetRaceID()));
		// erforschte Techs vergleichen
		double dOurLevel =		pMajor->GetEmpire()->GetResearch()->GetBioTech() +
								pMajor->GetEmpire()->GetResearch()->GetEnergyTech() +
								pMajor->GetEmpire()->GetResearch()->GetCompTech() +
								pMajor->GetEmpire()->GetResearch()->GetPropulsionTech() +
								pMajor->GetEmpire()->GetResearch()->GetConstructionTech() +
								pMajor->GetEmpire()->GetResearch()->GetWeaponTech();
		dOurLevel /= 6.0;

		double dTheirLevel =	((CMajor*)pContactedRace)->GetEmpire()->GetResearch()->GetBioTech() +
								((CMajor*)pContactedRace)->GetEmpire()->GetResearch()->GetEnergyTech() +
								((CMajor*)pContactedRace)->GetEmpire()->GetResearch()->GetCompTech() +
								((CMajor*)pContactedRace)->GetEmpire()->GetResearch()->GetPropulsionTech() +
								((CMajor*)pContactedRace)->GetEmpire()->GetResearch()->GetConstructionTech() +
								((CMajor*)pContactedRace)->GetEmpire()->GetResearch()->GetWeaponTech();
		dTheirLevel /= 6.0;

		// ist der Durchschnitt nur um einen halben Techlevel verschieden, so wird ein "normal entwickelt angezeigt
		double dDiff = dOurLevel - dTheirLevel;
		if (fabs(dDiff) <= 0.5)
			sProgress = CLoc::GetString("NORMAL_DEVELOPED");
		// ist die getroffene Rasse technologisch hinter uns?
		else if (dDiff > 0.0)
		{
			// ist der Durchschnitt der getroffenen Rasse zwei Techlevels hinter dem unseren, so gilt sie als sehr r�ckst�ndig
			if (dDiff >= 2.0)
				sProgress = CLoc::GetString("VERY_UNDERDEVELOPED");
			// sonst als r�ckst�ndig
			else
				sProgress = CLoc::GetString("UNDERDEVELOPED");
		}
		// ist die getroffene Rasse technologisch fortschrittlicher als wir?
		else
		{
			// ist der Durchschnitt der getroffenen Rasse zwei Techlevels vor dem unseren, so gilt sie als sehr fortschrittlich
			if (dDiff <= -2.0)
				sProgress = CLoc::GetString("VERY_DEVELOPED");
			// sonst als fortschrittlich
			else
				sProgress = CLoc::GetString("DEVELOPED");
		}
	}

	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	MYTRACE("logevent")(MT::LEVEL_INFO, "NewContact - relative progress: %s \n", sProgress);
	g->DrawString(CComBSTR(sProgress), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(730,100,540,40), &fontFormat, &fontBrush);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	// Spezialgeb�ude und -schiffe der Rasse zeichnen, sofern es sich um eine Minorrace handelt
	if (pContactedRace->IsMinor())
	{
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		fontFormat.SetLineAlignment(StringAlignmentNear);
		CFontLoader::CreateGDIFont(pMajor, 1, fontName, fontSize);
		graphic = NULL;
		int nCount = 0;
		// Spezialgeb�ude zeichnen
		for (int i = 0; i < pDoc->BuildingInfo.GetSize(); i++)
			if (pDoc->BuildingInfo[i].GetOwnerOfBuilding() == NOBODY)
				if (pDoc->BuildingInfo[i].GetOnlyMinorRace() == TRUE)
					if (pDoc->BuildingInfo[i].GetOnlyInSystemWithName() == pContactedRace->GetHomesystemName())
					{
						graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\" + pDoc->BuildingInfo[i].GetGraphikFileName());
						if (graphic == NULL)
							graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
						if (graphic)
							g->DrawImage(graphic, 15 + nCount * 165, 810, 150, 113);
						s = pDoc->BuildingInfo[i].GetBuildingName();
						MYTRACE("logevent")(MT::LEVEL_INFO, "NewContact - Building: %s \n", s);
						g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(nCount * 165, 935, 180, 50), &fontFormat, &fontBrush);
						nCount++;
					}
		// Schiffe des Minors zeichnen
		for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
			if (pDoc->m_ShipInfoArray[i].GetRace() == MINORNUMBER)
				if (pDoc->m_ShipInfoArray[i].GetOnlyInSystem() == pContactedRace->GetHomesystemName() && pContactedRace->GetHomesystemName() != "")
				{
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\" + pDoc->m_ShipInfoArray[i].GetShipClass() + ".bop");
					if (graphic == NULL)
						graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
					if (graphic)
						g->DrawImage(graphic, 15 + nCount * 165, 810, 150, 113);
					s = pDoc->m_ShipInfoArray[i].GetShipClass() + "-" + CLoc::GetString("CLASS") + " (" + pDoc->m_ShipInfoArray[i].GetShipTypeAsString() + ")";
					MYTRACE("logevent")(MT::LEVEL_INFO, "NewContact - Shiplist: %s \n", s);
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(nCount * 165, 935, 180, 50), &fontFormat, &fontBrush);
					nCount++;
				}
				MYTRACE("logevent")(MT::LEVEL_INFO, "NewContact -------------------\n");
	}

	// Buttons zeichnen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontBrush.SetColor(color);
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, graphicPool, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CEventFirstContact::GetTooltip(const CPoint &pt) const
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);
	CRace* pContactedRace = pDoc->GetRaceCtrl()->GetRace(m_sRaceID);
	if (!pContactedRace)
		return "";

	// wurde auf das Rassenbild gehalten, dann Beschreibung der Rasse anzeigen
	if (CRect(6, 399, 301, 767).PtInRect(pt))
	{
		CString sTip = pContactedRace->GetRaceDesc();
		sTip = CHTMLStringBuilder::GetHTMLColor(sTip);
		sTip = CHTMLStringBuilder::GetHTMLHeader(sTip, _T("h4"));

		return sTip;
	}

	if (pContactedRace->IsMinor())
	{
		// Spezialgeb�ude pr�fen
		int nCount = 0;
		for (int i = 0; i < pDoc->BuildingInfo.GetSize(); i++)
			if (pDoc->BuildingInfo[i].GetOwnerOfBuilding() == NOBODY)
				if (pDoc->BuildingInfo[i].GetOnlyMinorRace() == TRUE)
					if (pDoc->BuildingInfo[i].GetOnlyInSystemWithName() == pContactedRace->GetHomesystemName())
					{
						if (CRect(15 + nCount * 165, 810, 15 + nCount * 165 + 150, 960).PtInRect(pt))
						{
							int nID = pDoc->BuildingInfo[i].GetRunningNumber();

							CString sName = pDoc->GetBuildingInfo(nID).GetBuildingName();
							sName = CHTMLStringBuilder::GetHTMLColor(sName);
							sName = CHTMLStringBuilder::GetHTMLHeader(sName, _T("h3"));
							sName = CHTMLStringBuilder::GetHTMLCenter(sName);
							sName += CHTMLStringBuilder::GetHTMLStringNewLine();
							sName += CHTMLStringBuilder::GetHTMLStringNewLine();

							CString sProd = pDoc->GetBuildingInfo(nID).GetProductionAsString();
							sProd = CHTMLStringBuilder::GetHTMLColor(sProd);
							sProd = CHTMLStringBuilder::GetHTMLHeader(sProd, _T("h5"));
							sProd += CHTMLStringBuilder::GetHTMLStringNewLine();
							sProd += CHTMLStringBuilder::GetHTMLStringHorzLine();
							sProd += CHTMLStringBuilder::GetHTMLStringNewLine();

							CString sDesc = pDoc->GetBuildingInfo(nID).GetBuildingDescription();
							sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc, _T("silver"));
							sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));

							return sName + sProd + sDesc;
						}
						nCount++;
					}

		// Schiffe des Minors zeichnen
		for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
			if (pDoc->m_ShipInfoArray[i].GetRace() == MINORNUMBER)
				if (pDoc->m_ShipInfoArray[i].GetOnlyInSystem() == pContactedRace->GetHomesystemName())
				{
					if (CRect(15 + nCount * 165, 810, 15 + nCount * 165 + 150, 960).PtInRect(pt))
						return pDoc->m_ShipInfoArray[i].GetTooltip();
					nCount++;
				}
	}
	return "";
}
