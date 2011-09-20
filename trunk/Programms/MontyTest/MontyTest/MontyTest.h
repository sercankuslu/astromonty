// MontyTest.h : главный файл заголовка для приложения MontyTest
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CMontyTestApp:
// О реализации данного класса см. MontyTest.cpp
//

class CMontyTestApp : public CWinApp
{
public:
	CMontyTestApp();


// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMontyTestApp theApp;