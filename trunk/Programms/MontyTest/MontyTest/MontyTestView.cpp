// MontyTestView.cpp : реализация класса CMontyTestView
//

#include "stdafx.h"
#include "MontyTest.h"

#include "MontyTestDoc.h"
#include "MontyTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMontyTestView

IMPLEMENT_DYNCREATE(CMontyTestView, CView)

BEGIN_MESSAGE_MAP(CMontyTestView, CView)
END_MESSAGE_MAP()

// создание/уничтожение CMontyTestView

CMontyTestView::CMontyTestView()
{
	// TODO: добавьте код создания

}

CMontyTestView::~CMontyTestView()
{
}

BOOL CMontyTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: изменить класс Window или стили посредством изменения
	//  CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// рисование CMontyTestView

void CMontyTestView::OnDraw(CDC* /*pDC*/)
{
	CMontyTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: добавьте здесь код отрисовки для собственных данных
}


// диагностика CMontyTestView

#ifdef _DEBUG
void CMontyTestView::AssertValid() const
{
	CView::AssertValid();
}

void CMontyTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMontyTestDoc* CMontyTestView::GetDocument() const // встроена неотлаженная версия
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMontyTestDoc)));
	return (CMontyTestDoc*)m_pDocument;
}
#endif //_DEBUG


// обработчики сообщений CMontyTestView
