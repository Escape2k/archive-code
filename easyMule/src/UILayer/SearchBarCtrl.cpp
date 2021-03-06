/*
 * $Id: SearchBarCtrl.cpp 20835 2010-11-18 10:36:09Z dgkang $
 * 
 * this file is part of easyMule
 * Copyright (C)2002-2008 VeryCD Dev Team ( strEmail.Format("%s@%s", "emuledev", "verycd.com") / http: * www.easymule.org )
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
// SearchBar.cpp : 实现文件
//

#include "stdafx.h"
#include "emule.h"
#include "SearchBarCtrl.h"
#include "FaceManager.h"
#include "Util.h"
#include "SearchParams.h"
#include ".\searchbarctrl.h"
#include "MenuCmds.h"
#include "SearchButton.h"
#include "DlgMainTabSidePanel.h"
#include "emuledlg.h"

// CSearchBar

IMPLEMENT_DYNAMIC(CSearchBarCtrl, CWnd)
CSearchBarCtrl::CSearchBarCtrl()
{
	m_nCurrentIcon = 0;
	m_eType = SearchTypeVeryCD;
	m_Type = MP_SEARCHEMULE;

	m_isFocus = FALSE;

	m_bHover = FALSE;

	m_Font.CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, _T("宋体"));
}

CSearchBarCtrl::~CSearchBarCtrl()
{
	m_ImageList.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CSearchBarCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_MESSAGE(WM_ERASEBKGND, OnEraseBkgndEx)
	ON_WM_CREATE()
	ON_WM_INITMENUPOPUP()
	//ON_WM_DRAWITEM()
	//ON_WM_MEASUREITEM()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_SEARCHEDIT_CLEAR, OnClearBtClicked)
END_MESSAGE_MAP()



// CSearchBar 消息处理程序

BOOL CSearchBarCtrl::Create(CWnd* pParentWnd, CRect rect, DWORD dwStyle, UINT nID)
{
	if(!CreateEx(NULL, NULL, NULL, dwStyle, rect, pParentWnd, nID))
	{
		return FALSE;
	}

	if(!m_SearchEdit.Create(this, CRect(0, 0, 0, 0)))
	{
		return FALSE;
	}

	if(!m_pButtonClear.Create(_T(""), WS_CHILD, CRect(0,0,0,0), this, 0))
	{
		return FALSE;
	}
	else
	{
		m_pButtonClear.SetOwner(this);
		m_pButtonClear.SetClearButton(1);//标识按钮属性
		m_pButtonClear.SetTransparent(RGB(255,0,255));
		m_pButtonClear.SetBitmaps(_T("PNG_SEARCHEDIT_CLEAR"), 4, 1);
	}

	return TRUE;
}

void CSearchBarCtrl::OnPaint()
{
	CPaintDC dc(this);

	CRect rtTriangle;
	CRect rtBack;
	

	CClientRect	rtClient(this);
	CBufferDC	bufDC(dc.GetSafeHdc(), rtClient);
	rtTriangle.CopyRect(rtClient);

	SendMessage(WM_ERASEBKGND, (WPARAM) bufDC.GetSafeHdc(), 1);

	CSize	sizeIcon;
	CRect	rtIcon;
	CFaceManager::GetInstance()->GetImageSize(m_nCurrentIcon, sizeIcon);
	CenterRect(&rtIcon, rtClient, sizeIcon);

	rtIcon.left = rtClient.left + MARGIN_WIDTH;
	rtIcon.top = rtClient.top + MARGIN_WIDTH;
	rtIcon.right = rtIcon.left + sizeIcon.cx;

	rtBack.left = rtClient.left + 2;
	rtBack.top = rtClient.top + 2;
	rtBack.bottom = rtClient.bottom;
	rtBack.right = sizeIcon.cx - 8;


	if (m_bHover)
	{
		bufDC.FillSolidRect(rtBack, RGB(226,226,226));//RGB(238,236,221)
	}
	else
	{
		bufDC.FillSolidRect(rtBack, RGB(240,240,240));//RGB(238,236,221)
	}

	{
		CPenDC pen(bufDC.GetSafeHdc(), RGB(200,200,200));


		bufDC.MoveTo(rtBack.bottom + 6, rtBack.top - 1);
		bufDC.LineTo(rtBack.bottom + 6, rtBack.right - 5);

	}

	m_ImageList.Draw(&bufDC, m_nCurrentIcon, CPoint(rtIcon.left, rtIcon.top), ILD_NORMAL);

	rtTriangle.left = rtClient.left + MARGIN_WIDTH + 1;
	rtTriangle.bottom = rtClient.bottom - rtClient.Height()/2 + MARGIN_WIDTH;
	rtTriangle.right = rtTriangle.left + sizeIcon.cx;


	DrawTriangle(&bufDC, rtTriangle);

}

HBRUSH CSearchBarCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	if(m_SearchEdit.m_bTipinfo)
	{
		pDC->SetTextColor(RGB(194, 194, 194));
	}
	else
	{
		pDC->SetTextColor(RGB(0, 0, 0));
	}

	return hbr;
}

CSearchEdit* CSearchBarCtrl::GetEditor(void)
{
	return &m_SearchEdit;
}

void CSearchBarCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(rcClient);

	if (NULL != m_SearchEdit.GetSafeHwnd())
	{
		CRect rtEdit;
		rtEdit = rcClient;

		rtEdit.DeflateRect(MARGIN_WIDTH + IMAGE_WIDTH + 8, MARGIN_WIDTH + 2, MARGIN_WIDTH, MARGIN_WIDTH);


		m_SearchEdit.MoveWindow(&rtEdit, FALSE);
	}

	if (NULL != m_pButtonClear.GetSafeHwnd())
	{
		CRect rcClear(rcClient.right - 20, rcClient.top + 4, rcClient.right - 1, rcClient.top + 22);
		m_pButtonClear.MoveWindow(&rcClear, FALSE);
	}
}

LRESULT	CSearchBarCtrl::OnEraseBkgndEx(WPARAM wParam, LPARAM lParam)
{
	if (1 == lParam)
	{
		DrawParentBk(GetSafeHwnd(), (HDC) wParam);

		CRect rtClient;
		GetClientRect(&rtClient);

		CFaceManager::GetInstance()->DrawImageBar(IBI_SEARCHBAR_EDIT, (HDC) wParam, rtClient);
	}

	return 1;
}

LRESULT CSearchBarCtrl::OnClearBtClicked(WPARAM wParam, LPARAM lParam)
{
	ShowClearButton(FALSE);
	ClearEdit();
	return 0;
}

void CSearchBarCtrl::ClearEdit()
{
	m_SearchEdit.SetWindowText(_T(""));
	m_SearchEdit.SetFocus();
}

void CSearchBarCtrl::SwitchSearchEditInfo(int nTabType)
{
	m_SearchEdit.m_nTabType = nTabType;

	m_SearchEdit.SetWindowText(_T(""));
	m_SearchEdit.OnEnKillfocus();//设置默认info
}

void CSearchBarCtrl::ShowClearButton(BOOL bShow)
{
	if (m_pButtonClear.GetSafeHwnd() != NULL)
	{
		if (bShow)
			m_pButtonClear.ShowWindow(SW_SHOW);
		else
			m_pButtonClear.ShowWindow(SW_HIDE);

		m_pButtonClear.RedrawWindow();
	}
}

void CSearchBarCtrl::ShowImage(int nIconNumber)
{
	if (nIconNumber == m_nCurrentIcon)
	{
		return;
	}

	m_nCurrentIcon = nIconNumber;
	Invalidate();
	UpdateWindow();
}

BOOL CSearchBarCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case MP_SEARCHVERYCD:
		ShowImage(0);
		SetSearchType(SearchTypeVeryCD);
		SetType(MP_SEARCHEMULE);
		m_isFocus = TRUE;
		
		break;
	case MP_SEARCHEMULE:
		ShowImage(1);
		SetSearchType(SearchTypeEd2kGlobal);
		SetType(MP_SEARCHVERYCD);
		m_isFocus = TRUE;

		break;
	default:
		break;
	}
	
	if (m_isFocus)
	{
		m_SearchEdit.SetFocus();
		m_SearchEdit.SetSel(0, -1);
		m_isFocus = FALSE;
	}

	return CWnd::OnCommand(wParam, lParam);
}

void CSearchBarCtrl::SetSearchType(ESearchType eType)
{
	m_eType = eType;
}

ESearchType CSearchBarCtrl::GetSearchType(void)
{
	//根据当前的标签页 设置当前搜索方式
	if(theApp.emuledlg &&  theApp.emuledlg->m_mainTabWnd.GetActiveTab() == theApp.emuledlg->m_mainTabWnd.m_aposTabs[CMainTabWnd::TI_ADVANCE])
	{
		if(thePrefs.m_bStartShowHomePage)
			return SearchTypeVeryCD;
		else
			return SearchTypeEd2kGlobal;
	}
	else
	{
		if(theApp.emuledlg &&  theApp.emuledlg->m_mainTabWnd.GetActiveTab() == theApp.emuledlg->m_mainTabWnd.m_aposTabs[CMainTabWnd::TI_SEARCH])
			return SearchTypeEd2kGlobal;
		else
			return SearchTypeVeryCD;
	}
}

int CSearchBarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ImageList.Create( 16, 16, theApp.m_iDfltImageListColorFlags | ILC_MASK, 2,1);
	m_ImageList.Add(CTempIconLoader(IDI_ICON_VERYCD,16 ,16));
	m_ImageList.Add(CTempIconLoader(IDI_ICON_VERYCD,16 ,16));

	return 0;
}

void CSearchBarCtrl::DrawTriangle(CDC* pDC, CRect &rect)
{
	rect.left += rect.Width()/2 - 2;
	rect.top += rect.Height()/2 + 3;

	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left + 5, rect.top);

	pDC->MoveTo(rect.left + 1, rect.top + 1);
	pDC->LineTo(rect.left + 4, rect.top + 1);

	pDC->MoveTo(rect.left + 2, rect.top + 2);
	pDC->LineTo(rect.left + 3, rect.top + 2);
}

void CSearchBarCtrl::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	UINT nID = pPopupMenu->GetMenuItemID( 0 );
	if ( nID != SC_RESTORE )
	{
		MenuXP.AddMenu( pPopupMenu, TRUE );
	}
}

/* 因为与ButtonST类DrawItem冲突 加之现在不用选择搜索类型,所以暂时屏蔽 */
//void CSearchBarCtrl::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDrawItemStruct)
//{
//	MenuXP.DrawItem(lpDrawItemStruct);
//}
//
//void CSearchBarCtrl::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
//{
//	MenuXP.MeasureItem(lpMeasureItemStruct);
//}

void CSearchBarCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(point.x <= IMAGE_WIDTH + 5)
	{
		//创建空菜单
		CMenu menu;
		menu.CreatePopupMenu();

		//填充菜单项
		menu.AppendMenu(MF_STRING | MF_UNCHECKED, MP_SEARCHVERYCD, GetResString(IDS_SEARCHVERYCD));
		menu.AppendMenu(MF_STRING | MF_UNCHECKED, MP_SEARCHEMULE, GetResString(IDS_SEARCHEMULE));

		//放置菜单
		CRect editRect;
		GetClientRect(&editRect);
		CPoint pointMenu(0, editRect.bottom + 2);
		ClientToScreen(&pointMenu);
		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pointMenu.x, pointMenu.y, this);
		return;
	}

	CWnd::OnLButtonUp(nFlags, point);
}

BOOL CSearchBarCtrl::PreTranslateMessage(MSG* pMsg)
{
	CSearchButton* SearchButton = (CSearchButton*)(&((CDlgMainTabSidePanel*)GetParent())->m_SearchButton);

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if(SearchButton)
			{
				SearchButton->OnBnClicked();
			}
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CSearchBarCtrl::SetType(UINT type)
{
	m_Type = type;
}

UINT CSearchBarCtrl::GetType(void)
{
	return m_Type;
}

LRESULT CSearchBarCtrl::OnMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		Invalidate();
	}
	
	return 0;
}

void CSearchBarCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT		csTME;
	CWnd::OnMouseMove(nFlags, point);

	if(point.x <= IMAGE_WIDTH + 5)
	{
		if (!m_bHover)
		{
			m_bHover = TRUE;
			Invalidate();

			ZeroMemory(&csTME, sizeof(csTME));
			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = m_hWnd;
			::_TrackMouseEvent(&csTME);
		}
		
		if( AfxGetMainWnd()->GetActiveWindow() )
		{//如果当前应用程序有活动窗口，说明应用程序是活动的。我们就在鼠标滑过时弹出菜单提示用户选择搜索方式
			SendMessage( WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(point.x,point.y) );
		}
	}
}

LRESULT CSearchBarCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( WM_INPUTLANGCHANGEREQUEST == message )
	{
		if(GetKeyboardLayout(0) == (HKL)lParam)
			return 0;
		UINT count = GetKeyboardLayoutList(0,NULL);
		if(count == 0) return 0;
		HKL* lpList = new HKL[count];

		count = GetKeyboardLayoutList(count,lpList);
		BOOL bFound = FALSE;
		for(int i=0;i<count;i++)
		{
			if((HKL)lParam == lpList[i])
			{
				bFound = TRUE;
				break;
			}
		}
		if(lpList)
		{
			delete[] lpList;
			lpList = NULL;
		}
		if(!bFound)
		{
			CString sID;
			sID.Format( L"%08x",lParam );
			LoadKeyboardLayout(sID,KLF_ACTIVATE);
		}
		else
		{
			ActivateKeyboardLayout((HKL)lParam,KLF_REORDER);
		}

		return 0;
	}


	return CWnd::WindowProc(message, wParam, lParam);
}
