// MontyTestDoc.h : интерфейс класса CMontyTestDoc
//


#pragma once


class CMontyTestDoc : public CDocument
{
protected: // создать только из сериализации
	CMontyTestDoc();
	DECLARE_DYNCREATE(CMontyTestDoc)

// Атрибуты
public:

// Операции
public:

// Переопределение
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Реализация
public:
	virtual ~CMontyTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()
};


