//CMainDlg implementation
#include "stdafx.h"
#include "main_dlg.h"
#include "result_dlg.h"

//---------------------------------------------------------------
BOOL CMainDlg::PreTranslateMessage( MSG* pMsg )
{
   return CWindow::IsDialogMessage( pMsg );
}
//---------------------------------------------------------------
LRESULT CMainDlg::OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
   // center the dialog on the screen
   CenterWindow();

   // set icons
   HICON hIcon = AtlLoadIconImage( IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics( SM_CXICON ), ::GetSystemMetrics( SM_CYICON ) );
   SetIcon( hIcon, TRUE );
   HICON hIconSmall = AtlLoadIconImage( IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics( SM_CXSMICON ), ::GetSystemMetrics( SM_CYSMICON ) );
   SetIcon( hIconSmall, FALSE );

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT( pLoop != NULL );
   pLoop->AddMessageFilter( this );

   CButton( GetDlgItem( IDC_BUTTON_FILE_NAME ) ).SetCheck( true );
   CButton( GetDlgItem( IDC_BUTTON_FOLDER_NAME ) ).SetCheck( true );

   return TRUE;
}//CMainDlg::OnInitDialog
//---------------------------------------------------------------
LRESULT CMainDlg::OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT( pLoop != NULL );
   pLoop->RemoveMessageFilter( this );
   return 0;
}
//---------------------------------------------------------------
LRESULT CMainDlg::OnSearch( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
   const bool searchInFileNames = CButton( GetDlgItem( IDC_BUTTON_FILE_NAME ) ).GetCheck();
   const bool searchInFolderNames = CButton( GetDlgItem( IDC_BUTTON_FOLDER_NAME ) ).GetCheck();
   if( searchInFileNames || searchInFolderNames )
   {
      CResultDlg dlg;
      GetDlgItem( IDC_EDIT1 ).GetWindowText( dlg.m_szPattern );
      dlg.m_searchInFileNames = searchInFileNames;
      dlg.m_searchInFolderNames = searchInFolderNames;

      ShowWindow( SW_HIDE );
      dlg.DoModal();
      if( dlg.m_result != dlg.resultBack )
      {
         DestroyWindow();
         ::PostQuitMessage( wID );
      }
      else
         ShowWindow( SW_SHOW );
   }else 
      MessageBox( _T( "Choose where to search" ) );
   return 0;
}//CMainDlg::OnSearch
//---------------------------------------------------------------
LRESULT CMainDlg::OnCancel( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
   DestroyWindow();
   ::PostQuitMessage( wID );
   return 0;
}
//---------------------------------------------------------------
