// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"

//---------------------------------------------------------------
class CMainDlg :  public CDialogImpl<CMainDlg>,
	            	public CMessageFilter 
{
public:
   enum { IDD = IDD_MAINDLG };
   virtual BOOL PreTranslateMessage( MSG* pMsg );

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_SEARCH, OnSearch)
      COMMAND_ID_HANDLER(IDCANCEL, OnCancel )
   END_MSG_MAP()

   LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ );
   LRESULT OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ );
   LRESULT OnSearch( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ );
   LRESULT OnCancel( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ );
}; //class CMainDlg
//---------------------------------------------------------------
