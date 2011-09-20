// MontyTestDoc.cpp : реализаци€ класса CMontyTestDoc
//

#include "stdafx.h"
#include "MontyTest.h"

#include "MontyTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMontyTestDoc

IMPLEMENT_DYNCREATE(CMontyTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CMontyTestDoc, CDocument)
END_MESSAGE_MAP()


// создание/уничтожение CMontyTestDoc

CMontyTestDoc::CMontyTestDoc()
{
	// TODO: добавьте код дл€ одноразового вызова конструктора

}

CMontyTestDoc::~CMontyTestDoc()
{
}

BOOL CMontyTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: добавьте код повторной инициализации
	// (ƒокументы SDI будут повторно использовать этот документ)

	return TRUE;
}




// сериализаци€ CMontyTestDoc

void CMontyTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: добавьте код сохранени€
	}
	else
	{
		// TODO: добавьте код загрузки
	}
}


// диагностика CMontyTestDoc

#ifdef _DEBUG
void CMontyTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMontyTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// команды CMontyTestDoc
