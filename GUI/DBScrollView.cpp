// DBScrollView.cpp : implementation file
//

#include "stdafx.h"
#include "DBScrollView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBScrollView

IMPLEMENT_DYNCREATE(CDBScrollView, CScrollView)

CDBScrollView::CDBScrollView():m_bUpdateOnly(FALSE)
{
	// Standard-Hintergrundfarbe eines Fensters ermitteln (meist wei�)
	m_colorBackground = GetSysColor(COLOR_WINDOW);
}

CDBScrollView::~CDBScrollView()
{
}


BEGIN_MESSAGE_MAP(CDBScrollView, CScrollView)
	//{{AFX_MSG_MAP(CDBScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBScrollView drawing

void CDBScrollView::OnDraw(CDC* /*pDC*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CDBScrollView diagnostics

#ifdef _DEBUG
void CDBScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDBScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDBScrollView message handlers

BOOL CDBScrollView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// verhindern, dass der Hintergrund automatisch mit wei� gef�llt wird
	return FALSE;

//	return CScrollView::OnEraseBkgnd(pDC);
}

void CDBScrollView::OnPaint()
{
	CPaintDC dc(this);
	OnPrepareDC(&dc);

	CDC *pDC = &dc, *pMemDC = NULL;
	CBitmap *pMemBMP = NULL;

	// Gr��e des Client-Bereichs ermitteln
	CRect client;
	GetClientRect(&client);

	// Double Buffering nur, wenn alles neu gezeichnet werden soll; tritt beim Erzeugen des MemDC oder des
	// MemBMP ein Fehler auf, dann wird direkt auf den urspr�nglichen DC gezeichnet
	if (!m_bUpdateOnly)
	{
		// MemDC erzeugen
		pMemDC = new CDC();
		if (pMemDC->CreateCompatibleDC(pDC))
		{
			// Bitmap erzeugen; dieses ist nur so gro� wie der Client-Bereich (der aktuell sichtbare Ausschnitt
			// der gesamten View)
			pMemBMP = new CBitmap();
			if (pMemBMP->CreateCompatibleBitmap(pDC, client.right, client.bottom))
			{
				// Bitmap in MemDC w�hlen
				pMemDC->SelectObject(pMemBMP);
				// Hintergrund mit angegebener Farbe f�llen (standardm��ig meist wei�)
				pMemDC->FillSolidRect(0, 0, client.right, client.bottom, m_colorBackground);
				// Koordinatenurspr�nge so setzen, dass die Zeichenoperationen die aktuelle
				// Verschiebung beachten
				pMemDC->SetWindowOrg(0, 0);
				pMemDC->SetViewportOrg(-GetScrollPosition());

				// auf MemDC statt auf urspr�nglichen DC zeichnen
				pDC = pMemDC;
			}
			else
			{
				// bei Fehler alles freigeben
				delete pMemBMP;
				pMemBMP = NULL;
				delete pMemDC;
				pMemDC = NULL;
			}
		}
		else
		{
			// bei Fehler freigeben
			delete pMemDC;
			pMemDC = NULL;
		}
	}

	// auf urspr�nglichen oder gepufferten DC zeichnen
	OnDraw(pDC);

	// wenn Double Buffering aktiv, dann Inhalt aus MemDC in urspr�nglichen DC kopieren
	if (pMemDC)
	{
		// MapMode anpassen und Urspr�nge der View und des Bitmaps angleichen,
		// so dass bei BitBlt die linke obere Ecke des Bitmaps mit der sicht-
		// baren linken oberen Ecke der View �bereinstimmt
		pMemDC->SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		pMemDC->SetViewportOrg(0, 0);

		// Bitmap in View kopieren
		dc.BitBlt(0, 0, client.right, client.bottom, pMemDC, 0, 0, SRCCOPY);

		delete pMemDC;
		pMemDC = NULL;
		delete pMemBMP;
		pMemBMP = NULL;
	}

	// �nderungen wurden gezeichnet, wieder auf FALSE setzen
	m_bUpdateOnly = FALSE;

	// Do not call CScrollView::OnPaint() for painting messages
}
