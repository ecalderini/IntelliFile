/* This file is part of IntelliFile application developed by Stefan-Mihai MOGA.

IntelliFile is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliFile is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliFile. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// QuickAccessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliFile.h"
#include "QuickAccessDlg.h"
#include "FileDialogST.h"

// CQuickAccessDlg dialog

IMPLEMENT_DYNAMIC(CQuickAccessDlg, CDialogEx)

CQuickAccessDlg::CQuickAccessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_QuickAccessDlg, pParent)
{
}

CQuickAccessDlg::~CQuickAccessDlg()
{
}

void CQuickAccessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FAVORITE_FOLDERS, m_listFavoriteFolders);
	DDX_Control(pDX, IDC_ACCESS, m_ctrlQuickAccess);
	DDX_Control(pDX, IDC_ADD, m_ctrlAddFolder);
	DDX_Control(pDX, IDC_REMOVE, m_ctrlRemoveFolder);
}

BEGIN_MESSAGE_MAP(CQuickAccessDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_FAVORITE_FOLDERS, &CQuickAccessDlg::OnDblclkFolderList)
	ON_BN_CLICKED(IDC_ACCESS, &CQuickAccessDlg::OnBnClickedAccess)
	ON_BN_CLICKED(IDC_ADD, &CQuickAccessDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_REMOVE, &CQuickAccessDlg::OnBnClickedRemove)
END_MESSAGE_MAP()

// CQuickAccessDlg message handlers

CString GetQuickAcessFilePath()
{
	TCHAR lpszDrive[_MAX_DRIVE];
	TCHAR lpszDirectory[_MAX_DIR];
	TCHAR lpszFilename[_MAX_FNAME];
	TCHAR lpszExtension[_MAX_EXT];
	TCHAR lpszExeFilePath[_MAX_PATH];
	TCHAR lpszGetQuickAcess[_MAX_PATH];

	const int nLength = GetModuleFileName(NULL, lpszExeFilePath, _MAX_PATH);
	ASSERT(nLength > 0);
	VERIFY(0 == _tsplitpath_s(lpszExeFilePath, lpszDrive, _MAX_DRIVE, lpszDirectory, _MAX_DIR, lpszFilename, _MAX_FNAME, lpszExtension, _MAX_EXT));
	VERIFY(0 == _tmakepath_s(lpszGetQuickAcess, _MAX_PATH, lpszDrive, lpszDirectory, lpszFilename, _T(".txt")));
	return lpszGetQuickAcess;
}

BOOL CQuickAccessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strItemText;
	const UINT nTextFileFlags = CFile::modeRead | CFile::typeText;
	try {
		CStdioFile pTextFile(GetQuickAcessFilePath(), nTextFileFlags);
		while (pTextFile.ReadString(strItemText))
		{
			m_listFavoriteFolders.AddString(strItemText);
		}
		pTextFile.Close();
	}
	catch (CFileException *pFileException) {
		const int nErrorLength = 0x100;
		TCHAR lpszErrorMessage[nErrorLength] = { 0, };
		pFileException->GetErrorMessage(lpszErrorMessage, nErrorLength);
		TRACE(_T("%s\n"), lpszErrorMessage);
		pFileException->Delete();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuickAccessDlg::OnDestroy()
{
	CString strFileLine;
	CString strItemText;
	const UINT nTextFileFlags = CFile::modeCreate | CFile::modeWrite | CFile::typeText;
	try {
		CStdioFile pTextFile(GetQuickAcessFilePath(), nTextFileFlags);
		const int nListSize = m_listFavoriteFolders.GetCount();
		for (int nListItem = 0; nListItem < nListSize; nListItem++)
		{
			m_listFavoriteFolders.GetText(nListItem, strItemText);
			strFileLine.Format(_T("%s\n"), static_cast<LPCWSTR>(strItemText));
			pTextFile.WriteString(strFileLine);
		}
		pTextFile.Close();
	}
	catch (CFileException *pFileException) {
		const int nErrorLength = 0x100;
		TCHAR lpszErrorMessage[nErrorLength] = { 0, };
		pFileException->GetErrorMessage(lpszErrorMessage, nErrorLength);
		TRACE(_T("%s\n"), lpszErrorMessage);
		pFileException->Delete();
	}

	CDialogEx::OnDestroy();
}

void CQuickAccessDlg::OnDblclkFolderList()
{
	OnBnClickedAccess();
}

void CQuickAccessDlg::OnBnClickedAccess()
{
	const int nSelectedItem = m_listFavoriteFolders.GetCurSel();
	if (nSelectedItem != LB_ERR)
	{
		m_listFavoriteFolders.GetText(nSelectedItem, m_strSelectedFolder);
		CDialog::EndDialog(IDOK);
	}
}

void CQuickAccessDlg::OnBnClickedAdd()
{
	const UINT nFolderFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_VALIDATE;
	CFileDialogST dlgFileDialog;
	if (dlgFileDialog.SelectFolder(NULL, m_strSelectedFolder, nFolderFlags, this) == IDOK)
	{
		m_strSelectedFolder = dlgFileDialog.GetSelectedFolder();
		const int nRootFolder = m_strSelectedFolder.GetLength();
		if (nRootFolder > 0)
		{
			if (m_strSelectedFolder.GetAt(nRootFolder - 1) != _T('\\'))
				m_strSelectedFolder += _T("\\");
			m_listFavoriteFolders.AddString(m_strSelectedFolder);
		}
	}
}

void CQuickAccessDlg::OnBnClickedRemove()
{
	const int nSelectedItem = m_listFavoriteFolders.GetCurSel();
	if (nSelectedItem != LB_ERR)
	{
		m_listFavoriteFolders.DeleteString(nSelectedItem);
	}
}
