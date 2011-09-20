// MontyTestView.h : интерфейс класса CMontyTestView
//


#pragma once


class CMontyTestView : public CView
{
protected: // создать только из сериализации
	CMontyTestView();
	DECLARE_DYNCREATE(CMontyTestView)

// Атрибуты
public:
	CMontyTestDoc* GetDocument() const;

// Операции
public:

// Переопределение
public:
	virtual void OnDraw(CDC* pDC);  // переопределено для отрисовки этого представления
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Реализация
public:
	virtual ~CMontyTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // отладочная версия в MontyTestView.cpp
inline CMontyTestDoc* CMontyTestView::GetDocument() const
   { return reinterpret_cast<CMontyTestDoc*>(m_pDocument); }
#endif

