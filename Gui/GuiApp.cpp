#include "stdafx.h"
#include "GuiApp.h"
#include "MainFrame.h"

GuiApp app;

GuiApp::GuiApp()
{

}

BOOL GuiApp::InitInstance()
{
	CWinAppEx::InitInstance();
	MainFrame* frame;

	frame = new MainFrame;
	m_pMainWnd = frame;
	frame->ShowWindow(SW_SHOW);
	frame->UpdateWindow();

	return TRUE;
}

