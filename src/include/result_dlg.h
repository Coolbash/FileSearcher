// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <thread>
#include <list>
#include "resource.h"


int CALLBACK ListViewCompareProc( LPARAM lparam1, LPARAM lparam2, LPARAM lParamSort );
class CItem;


class CResultDlg :   public CDialogImpl<CResultDlg>,
                     public CWinDataExchange<CResultDlg>
{
public:     //interface
   volatile enum       //result of the dialog
   {
      resultStillWorking,     //user is still working with the dialog
      resultBack,             //user have pressed the "back" button
      resultCancel            //user have pressed the "cancel" button
   } m_result = resultStillWorking;

   CString  m_szPattern;                     //the string, that we need to find in file names
   bool     m_searchInFileNames=true;        //search m_szPattern in file names
   bool     m_searchInFolderNames = true;    //search m_szPattern in folder names

	enum { IDD = IDD_DIALOG_RESULT   };

private:    //end of the interface
   BEGIN_DDX_MAP( CResultDlg )
      DDX_CONTROL_HANDLE( IDC_LIST_RESULT, m_list)
   END_DDX_MAP()

	BEGIN_MSG_MAP(CResultDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor )
      COMMAND_ID_HANDLER( ID_BACK, OnBackCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
      NOTIFY_CODE_HANDLER( LVN_COLUMNCLICK, OnLVColumnClick )
   END_MSG_MAP()
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   CListViewCtrl  m_list;
   int            m_itemCount=-1;
   std::thread    m_threadSearch;
   volatile bool  m_trheadSearchIsRunning=false;
   HCURSOR        m_hCursorWait=0;
   HCURSOR        m_hCursorArrow=0;
   
   struct CColumn //data for each column
   {
      int      m_num;      //number of the column
      LPCTSTR  m_name;     //normal name of the column
      LPCTSTR  m_name_up;  //name of sorted column
      LPCTSTR  m_name_dn;  //name of reverse sorted column
   };

   enum  //numbers of column
   {
      nColumnName,
      nColumnSize,
      nColumnType,
      nColumnDate,
      nColumnFldr,
   };
   static const CColumn m_Columns[5];

   struct CItem
   {
      CString     m_name;
      ULONGLONG   m_size;
      CString     m_type;
      FILETIME    m_date;
      CString     m_folder;
      CItem( const CFindFile& finder, LPCTSTR szPath );
      //CString getSizeString();
   };

   std::list<CItem>   m_items;
   struct CSortData
   {
      int		m_nColumn = 0;
      bool	m_bReverse = false;
   }  m_Sort;


   NUMBERFMT      m_nf = {0};
   TCHAR          m_szDecSep[5] = { 0 };
   TCHAR          m_szThousandsSep[5] = { 0 };

   //------------------------------------------------------------
   LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ );
   LRESULT OnLVColumnClick( int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/ );
   LRESULT OnSetCursor( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ );
   LRESULT OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/ );
   LRESULT OnBackCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/ );

   void findFiles( LPCTSTR szPath, LPCTSTR szStringToSearch );
   void addItem2list(const CItem& item, int num);
   void searchRoutine();
};//class CResultDlg
//------------------------------------------------------------
