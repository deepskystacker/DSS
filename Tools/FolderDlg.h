// -----------
// FolderDlg.h
// -----------
/**
* @file
* @brief Re: CFolderDlg
* @author Achim Klein
* @version 0.1
*/


// -------------------
// One-Definition-Rule
// -------------------
#ifndef FOLDERDLG_H_INCLUDE_NR1
#define FOLDERDLG_H_INCLUDE_NR1


// ----------------------------------
// Definition of the CFolderDlg class
// ----------------------------------
/**
 * This class displays a dialog box that enables the user to select a shell folder.
 */
class CFolderDlg
{

public:

	// ------------
	// Construction
	// ------------

	/// constructor
	CFolderDlg(BOOL HideNewFolderButton, const CString& InitialFolder, CWnd* pParent);

	/// virtual destructor
	virtual ~CFolderDlg();


	// --------
	// Handling
	// --------

	/// call this member function to invoke the modal dialog box and return the dialog-box result when done
	virtual int DoModal();


	// --------------
	// Setter methods
	// --------------

	/// sets the text that is displayed above the tree view control
	void SetTitle(const CString& Title);


	// --------------
	// Getter methods
	// --------------

	/// call this member function to get the name of the selected folder
	CString GetFolderName() const;


protected:

	// ----------------
	// Internal methods
	// ----------------

	/// the browse dialog box calls this function to notify it about events
	static INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData);


private:

	// ----------
	// Attributes
	// ----------

	/// the initial folder (used in the static callback function)
	static CString m_initialFolder;

	/// show or hide 'New Folder' button
	BOOL m_hideNewFolderButton;

	/// the parent window
	CWnd* m_parent;

	/// the text that is displayed above the tree view control
	CString m_title;

	/// the selected folder
	CString m_selectedFolder;
};


#endif	// FOLDERDLG_H_INCLUDE_NR1
