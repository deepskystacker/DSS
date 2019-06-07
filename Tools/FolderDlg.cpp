// -------------
// FolderDlg.cpp
// -------------
/**
* @file
* @brief Re: CFolderDlg
* @author Achim Klein
* @version 0.1
*/


// --------
// Includes
// --------
#include "stdafx.h"
#include "FolderDlg.h"


// --------------
// Static members
// --------------

// ---------------
// m_initialFolder
// ---------------
/**
 * The initial folder.
 *
 * @see
 * BrowseCallbackProc()
 */
CString CFolderDlg::m_initialFolder;


// ----------
// CFolderDlg
// ----------
/**
 * The constructor.
 */
CFolderDlg::CFolderDlg(BOOL HideNewFolderButton, const CString& InitialFolder, CWnd* pParent)
: m_parent(pParent)
{
	// show or hide 'New Folder' button
	m_hideNewFolderButton = HideNewFolderButton;

	// use passed folder
	m_initialFolder = InitialFolder;

	// empty string passed
	if (m_initialFolder.IsEmpty())
	{
		TCHAR szDir[MAX_PATH];

		// use current folder
		if (GetCurrentDirectory(sizeof(szDir) / sizeof(szDir[0]), szDir))
		{
			m_initialFolder = szDir;
		}
	}
}


// -----------
// ~CFolderDlg
// -----------
/**
 * Destructor without any task.
 */
CFolderDlg::~CFolderDlg()
{
	// nothing
}


// -------
// DoModal
// -------
/**
 * Call this member function to invoke the modal dialog box and return the dialog-box result when done.
 */
int CFolderDlg::DoModal()
{
	// reset
	m_selectedFolder = "";

	// A thread must call CoInitializeEx or CoInitialize before calling any other COM library function
	// except the CoGetMalloc function and other memory allocation calls. (here: BIF_NEWDIALOGSTYLE)
	HRESULT result = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

	// check result
	if ((result == S_OK) || (result == S_FALSE))
	{
		// interface pointer to the memory allocator
		LPMALLOC pMalloc;

		// Retrieves a pointer to the default OLE task memory allocator
		// (which supports the system implementation of the IMalloc interface)
		// so applications can call its methods to manage memory.
		if (CoGetMalloc(1, &pMalloc) == S_OK)
		{
			_TCHAR buffer[MAX_PATH];

			// initialize the BROWSEINFO struct
			BROWSEINFO bi;
			bi.hwndOwner      = m_parent->GetSafeHwnd();
			bi.pidlRoot       = 0;
			bi.pszDisplayName = buffer;
			bi.lpszTitle      = m_title;
			bi.ulFlags        = (BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_NEWDIALOGSTYLE);
			bi.lpfn           = BrowseCallbackProc;
			bi.lParam         = 0;

			// hide button if necessary
			if (m_hideNewFolderButton)
			{
				bi.ulFlags |= BIF_NONEWFOLDERBUTTON;
			}

			// Displays a dialog box enabling the user to select a Shell folder.
			// You must initialize Component Object Model (COM) using CoInitializeEx with the
			// COINIT_APARTMENTTHREADED flag set in the dwCoInit parameter prior to calling SHBrowseForFolder.
			LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);

			// cancel will return 0
			if (pidl != 0)
			{
				// Converts an item identifier list to a file system path.
				if (::SHGetPathFromIDList(pidl, buffer))
				{
					m_selectedFolder = buffer;
				}

				// Frees a previously allocated block of memory. (IMalloc::Free)
				pMalloc->Free(pidl);
			}

			// Decrements the reference count. (IUnknown::Release)
			pMalloc->Release();
		}

		// To close the COM library gracefully on a thread, each successful call to
		// CoInitialize or CoInitializeEx, including any call that returns S_FALSE,
		// must be balanced by a corresponding call to CoUninitialize.
		CoUninitialize();
	}

	// check selected folder
	return (m_selectedFolder.IsEmpty() ? IDCANCEL : IDOK);
}


// --------
// SetTitle
// --------
/**
 * Sets the text that is displayed above the tree view control.
 */
void CFolderDlg::SetTitle(const CString& Title)
{
	m_title = Title;
}


// -------------
// GetFolderName
// -------------
/**
 * Call this member function to get the name of the selected folder.
 */
CString CFolderDlg::GetFolderName() const
{
	return m_selectedFolder;
}


// ------------------
// BrowseCallbackProc
// ------------------
/**
 * The browse dialog box calls this function to notify it about events.
 *
 * @see
 * SHBrowseForFolder()
 */
INT CALLBACK CFolderDlg::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	TCHAR szDir[MAX_PATH];

	switch(uMsg)
	{
		case BFFM_INITIALIZED:

			if (!m_initialFolder.IsEmpty())
			{
				// WParam is TRUE since you are passing a path
				// it would be FALSE if you were passing a pidl
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)m_initialFolder);
			}

			break;

		case BFFM_SELCHANGED:

			// set the status window to the currently selected path
			if (SHGetPathFromIDList((LPITEMIDLIST)lp, szDir))
			{
				SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
			}

			break;
	}

	return 0;
}
