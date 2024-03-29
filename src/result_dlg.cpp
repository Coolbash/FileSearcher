// implementation od CResultDlg class
#include "stdafx.h"
#include "result_dlg.h"

//some utility functions first

//---------------------------------------------------------------
/// string comparison routine   Compares 2 zero-ended strings, implementing * and ? wildcards. Case insensetive.
template<typename T>
int	isStringMatch( const T* s, const T* p ) 
{
   while( *p )
   {
      T cur_p = *p++;
      switch( cur_p )
      {
      case '?': ++s; continue;
      case '*':
         if( 0 == *p ) return 1;
         while( *s )
            if( isStringMatch( s++, p ) )
               return 1;
         return 0;
      case '\\':
         cur_p = *p++;
      default:
         if( 0 == *s || tolower( *s++ ) != tolower( cur_p ) )
            return 0;
      }
   }
   if( *s || *p )
      return 0;
   return 1;
}
//---------------------------------------------------------------
template <typename T> 
int compare( const T a, const T b )
{
   return a == b ? 0 :( a < b? -1 : 1);
}
//---------------------------------------------------------------
inline const ULONGLONG& ft2ULL( const FILETIME &ft ){ return *reinterpret_cast< const ULONGLONG* >(&ft ); };
//---------------------------------------------------------------

int CALLBACK ListViewCompareProc( LPARAM lparam1, LPARAM lparam2, LPARAM lParamSort )
{
   ATLASSERT( lParamSort != NULL && lparam1 != NULL && lparam2 != NULL );

   const CResultDlg::CItem* pItem1 = reinterpret_cast< CResultDlg::CItem* >( lparam1 );
   const CResultDlg::CItem* pItem2 = reinterpret_cast< CResultDlg::CItem* >( lparam2 );
   const CResultDlg::CSortData *sort = reinterpret_cast< CResultDlg::CSortData* >( lParamSort );

   int result = 0;
   switch( sort->m_nColumn )
   {
      case CResultDlg::nColumnName:result = pItem1->m_name.CompareNoCase( pItem2->m_name ); break;
      case CResultDlg::nColumnSize:result = compare( pItem1->m_size, pItem2->m_size ); break;
      case CResultDlg::nColumnType:result = pItem1->m_type.CompareNoCase( pItem2->m_type ); break;
      case CResultDlg::nColumnDate:result = compare( ft2ULL( pItem1->m_date ), ft2ULL( pItem2->m_date ) ); break;
      case CResultDlg::nColumnFldr:result = pItem1->m_folder.CompareNoCase( pItem2->m_folder ); break;
   };
   return sort->m_bReverse ? -result : result;
}
//---------------------------------------------------------------
const CResultDlg::CColumn CResultDlg::m_Columns[5] =
{
#ifdef _UNICODE
   {   nColumnName, _T( "Name" ),   _T( "↑ Name ↑" ),    _T( "↓ Name ↓" )     },
   {   nColumnSize, _T( "Size" ),   _T( "↑ Size ↑" ),    _T( "↓ Size ↓" )     },
   {   nColumnType, _T( "Type" ),   _T( "↑ Type ↑" ),    _T( "↓ Type ↓" )     },
   {   nColumnDate, _T( "Date" ),   _T( "↑ Date ↑" ),    _T( "↓ Date ↓" )     },
   {   nColumnFldr, _T( "Folder" ), _T( "↑ Folder ↑" ),  _T( "↓ Folder ↓" )   }
#else
   {   nColumnName, _T( "Name" ),   _T( "^ Name ^" ),    _T( "v Name v" )     },
   {   nColumnSize, _T( "Size" ),   _T( "^ Size ^" ),    _T( "v Size v" )     },
   {   nColumnType, _T( "Type" ),   _T( "^ Type ^" ),    _T( "v Type v" )     },
   {   nColumnDate, _T( "Date" ),   _T( "^ Date ^" ),    _T( "v Date v" )     },
   {   nColumnFldr, _T( "Folder" ), _T( "^ Folder ^" ),  _T( "v Folder v" )   }
#endif
};

//---------------------------------------------------------------

LRESULT CResultDlg::OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
   {//prepearing m_fm
      m_nf.NumDigits = 0;// We want no fractions 
      // But all the system defaults for the others 
      TCHAR szBuffer[5];
      GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_ILZERO, szBuffer, 5 );
      m_nf.LeadingZero = _ttoi( szBuffer );
      GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SGROUPING, szBuffer, 5 );
      m_nf.Grouping = _ttoi( szBuffer );
      GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, m_szDecSep, 5 );
      m_nf.lpDecimalSep = m_szDecSep;
      GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, m_szThousandsSep, 5 );
      m_nf.lpThousandSep = m_szThousandsSep;
      GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, szBuffer, 5 );
      m_nf.NegativeOrder = _ttoi( szBuffer );
   }
   m_hCursorWait = ::LoadCursor( nullptr, IDC_WAIT );
   m_hCursorArrow = ::LoadCursor( nullptr, IDC_ARROW );

   CenterWindow();
   // set icons
   HICON hIcon = AtlLoadIconImage( IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics( SM_CXICON ), ::GetSystemMetrics( SM_CYICON ) );
   SetIcon( hIcon, TRUE );
   HICON hIconSmall = AtlLoadIconImage( IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics( SM_CXSMICON ), ::GetSystemMetrics( SM_CYSMICON ) );
   SetIcon( hIconSmall, FALSE );

   DoDataExchange( false );

   m_threadSearch = std::thread( &CResultDlg::searchRoutine, this );
   return m_threadSearch.joinable();
}
//------------------------------------------------------------
LRESULT CResultDlg::OnBackCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/ )
{
   m_result = resultBack;
   if( m_trheadSearchIsRunning )
   {
      Yield();
      PostMessage( WM_COMMAND, ( wNotifyCode << 16 ) | wID, reinterpret_cast< LPARAM >( hWndCtl ) );
   }else
   {
      if( m_threadSearch.joinable() )
         m_threadSearch.join();
      EndDialog( wID );
   }
   return 0;
}
//------------------------------------------------------------

LRESULT CResultDlg::OnCloseCmd( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/ )
{
   m_result = resultCancel;
   if( m_trheadSearchIsRunning )
   {
      Yield();
      PostMessage( WM_COMMAND, ( wNotifyCode << 16 ) | wID, reinterpret_cast< LPARAM >( hWndCtl ) );
   }else
   {
      if( m_threadSearch.joinable() )
         m_threadSearch.join();
      EndDialog( wID );
   }
   return 0;
}
//------------------------------------------------------------
LRESULT CResultDlg::OnSetCursor( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
   if( m_trheadSearchIsRunning )
      SetCursor( m_hCursorWait );
   else
      SetCursor( m_hCursorArrow );
   return 1;
}
//------------------------------------------------------------
LRESULT CResultDlg::OnLVColumnClick( int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/ )
{
   LVCOLUMN clmn;
   clmn.mask = LVCF_TEXT;

   LPNMLISTVIEW lpLV = ( LPNMLISTVIEW )pnmh;
   const int newColumn = lpLV->iSubItem;
   if( m_Sort.m_nColumn != newColumn )
   {//restore normal name to old sorted column
      clmn.pszText = const_cast< LPTSTR >( m_Columns[m_Sort.m_nColumn].m_name );
      m_list.SetColumn( m_Sort.m_nColumn, &clmn );
      m_Sort.m_nColumn = newColumn;
      m_Sort.m_bReverse = false;
   }else 
      m_Sort.m_bReverse = !m_Sort.m_bReverse;

   //set new name for new sorted column
   if( m_Sort.m_bReverse )
      clmn.pszText = const_cast< LPTSTR >( m_Columns[newColumn].m_name_dn );
   else
      clmn.pszText = const_cast< LPTSTR >( m_Columns[newColumn].m_name_up );
   m_list.SetColumn( newColumn, &clmn );

   ATLASSERT( m_Sort.m_nColumn >= 0 && m_Sort.m_nColumn <= 4 );
   m_list.SortItems( ListViewCompareProc, ( LPARAM )& m_Sort );
   return 0;
}
//---------------------------------------------------------------
CResultDlg::CItem::CItem( const CFindFile& finder, LPCTSTR szPath )
   :m_name( finder.m_fd.cFileName ),
   m_size( finder.GetFileSize() ),
   m_date( finder.m_fd.ftLastWriteTime ),
   m_folder( szPath )
{
   if( finder.IsDirectory() )
      m_type = _T( "folder" );
   else
   {
      SHFILEINFO fileInfo;
      SHGetFileInfo( finder.GetFileURL(), 0, &fileInfo, sizeof( fileInfo ), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES );
      m_type = fileInfo.szTypeName;
   }
};
//---------------------------------------------------------------

void CResultDlg::addItem2list( const CItem& item, int num )
{
   //Name and LPARAM
   LVITEM listItem{LVIF_TEXT | LVIF_PARAM, num,0,0,0, const_cast<LPTSTR>(item.m_name.GetString()),0,0, reinterpret_cast<LPARAM>(&item)};
   m_list.InsertItem(&listItem);
   listItem.mask = LVIF_TEXT;//only text will be added to all othe subitmes

   const int bufSize = 254;  //this size should be enough for file size and for short date
   TCHAR buf[bufSize];
   
   //size
   listItem.iSubItem = nColumnSize;
   listItem.pszText = buf;
   if( item.m_size)
   {
      CString szFileSize; szFileSize.Format( _T( "%i64" ), item.m_size);
      if( GetNumberFormat( LOCALE_USER_DEFAULT, 0, szFileSize, &m_nf, buf, bufSize ) )
         m_list.SetItem( &listItem );
   }
   
   //date
   listItem.iSubItem = nColumnDate;
   SYSTEMTIME st;
   if( FileTimeToSystemTime( &item.m_date, &st ) )
   {
      GetDateFormat( LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, buf, bufSize );
      m_list.SetItem( &listItem );
   }

   //type
   listItem.iSubItem = nColumnType;
   listItem.pszText = const_cast< LPTSTR >( item.m_type.GetString() );
   m_list.SetItem( &listItem );

   //folder
   listItem.iSubItem = nColumnFldr;
   listItem.pszText = const_cast< LPTSTR >( item.m_folder.GetString() );
   m_list.SetItem(&listItem);
}
//------------------------------------------------------------
void CResultDlg::findFiles( LPCTSTR szPath, LPCTSTR szStringToSearch )
{
   CString szSearchPattern( szPath );
   szSearchPattern.Append( _T( "\\*" ) );
   CFindFile finder;
   if( finder.FindFile( szSearchPattern ) )
   {
      do
      {
         CString szFileName( finder.m_fd.cFileName );
         if( finder.IsDirectory() )
         {
            if( finder.IsDots() )
               continue;
            if( m_searchInFolderNames && isStringMatch( szFileName.GetString(), szStringToSearch ) )
            {
               m_items.push_back( { finder, szPath } );
               addItem2list( m_items.back(), ++m_itemCount );
            }
            CString szPathDeeper( szPath );
            szPathDeeper.Append( _T( "\\" ) );
            szPathDeeper.Append( szFileName );
            findFiles( szPathDeeper, szStringToSearch );
         }
         else
            if( m_searchInFileNames && isStringMatch( szFileName.GetString(), szStringToSearch ) )
            {
               m_items.push_back( { finder, szPath } );
               addItem2list( m_items.back(), ++m_itemCount );
            }
      } while( m_result == resultStillWorking && finder.FindNextFile() );
   }
   finder.Close();
}
//------------------------------------------------------------
void CResultDlg::searchRoutine()
{
   //CWaitCursor waitCursor;
   HRESULT r = SetThreadDescription( GetCurrentThread(), L"searchRoutine" );
   m_trheadSearchIsRunning = true;

   m_list.AddColumn( m_Columns[nColumnName].m_name, nColumnName );
   m_list.AddColumn( m_Columns[nColumnSize].m_name, nColumnSize, -1, LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_RIGHT );   //left-side text for better file size visualisation.
   m_list.AddColumn( m_Columns[nColumnType].m_name, nColumnType );
   m_list.AddColumn( m_Columns[nColumnDate].m_name, nColumnDate );
   m_list.AddColumn( m_Columns[nColumnFldr].m_name, nColumnFldr );
   m_list.SetColumnWidth( nColumnName, 180 ); 
   m_list.SetColumnWidth( nColumnSize, 80 );
   m_list.SetColumnWidth( nColumnType, 77 );
   m_list.SetColumnWidth( nColumnDate, 70 );
   m_list.SetColumnWidth( nColumnFldr, 152 );

   m_itemCount = -1;
   if( m_result == resultStillWorking )
   {
      PWSTR pszFolder;
      if( S_OK == SHGetKnownFolderPath( FOLDERID_Documents, 0, NULL, &pszFolder ) )
      {
         findFiles( CString( pszFolder ), _T( '*' ) + m_szPattern + _T( '*' ) );//the search
         //findFiles( _T( "c:" ), _T( '*' ) + m_szPattern + _T( '*' ) );  //test search for threading debug
         CoTaskMemFree( pszFolder );
      }
   }
   m_trheadSearchIsRunning = false;
   PostMessage( WM_SETCURSOR );//restoring standard cursor
   ATLTRACE( _T( "searchRoutine ended...\n" ) );
}
//------------------------------------------------------------
