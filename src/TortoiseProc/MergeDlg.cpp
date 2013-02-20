// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2007-2013 - TortoiseGit

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

// MergeDlg.cpp : implementation file
//

#include "stdafx.h"

#include "Git.h"
#include "TortoiseProc.h"
#include "MergeDlg.h"
#include "AppUtils.h"

#include "Messagebox.h"
// CMergeDlg dialog

IMPLEMENT_DYNAMIC(CMergeDlg, CResizableStandAloneDialog)

CMergeDlg::CMergeDlg(CWnd* pParent /*=NULL*/)
	: CResizableStandAloneDialog(CMergeDlg::IDD, pParent),
	CChooseVersion(this)
{
	m_pDefaultText = MAKEINTRESOURCE(IDS_PROC_AUTOGENERATEDBYGIT);
	m_bNoFF=false;
	m_bSquash=false;
	m_bNoCommit=false;
	m_bLog = FALSE;
	CString mergeLog = g_Git.GetConfigValue(_T("merge.log"));
	int nLog = _ttoi(mergeLog);
	m_nLog = nLog > 0 ? nLog : 20;
}

CMergeDlg::~CMergeDlg()
{
}

void CMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	CHOOSE_VERSION_DDX;

	DDX_Check(pDX,IDC_CHECK_NOFF,this->m_bNoFF);
	DDX_Check(pDX,IDC_CHECK_SQUASH,this->m_bSquash);
	DDX_Check(pDX,IDC_CHECK_NOCOMMIT,this->m_bNoCommit);
	DDX_Check(pDX, IDC_CHECK_MERGE_LOG, m_bLog);
	DDX_Text(pDX, IDC_EDIT_MERGE_LOGNUM, m_nLog);
	DDX_Text(pDX, IDC_COMBO_MERGESTRATEGY, m_MergeStrategy);
	DDX_Text(pDX, IDC_COMBO_STRATEGYOPTION, m_StrategyOption);
	DDX_Text(pDX, IDC_EDIT_STRATEGYPARAM, m_StrategyParam);
	DDX_Control(pDX, IDC_LOGMESSAGE, m_cLogMessage);
}


BEGIN_MESSAGE_MAP(CMergeDlg, CResizableStandAloneDialog)
	CHOOSE_VERSION_EVENT
	ON_BN_CLICKED(IDOK, &CMergeDlg::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_MERGE_LOG, &CMergeDlg::OnBnClickedCheckMergeLog)
	ON_CBN_SELCHANGE(IDC_COMBO_MERGESTRATEGY, &CMergeDlg::OnCbnSelchangeComboMergestrategy)
	ON_CBN_SELCHANGE(IDC_COMBO_STRATEGYOPTION, &CMergeDlg::OnCbnSelchangeComboStrategyoption)
END_MESSAGE_MAP()


BOOL CMergeDlg::OnInitDialog()
{
	CResizableStandAloneDialog::OnInitDialog();
	CAppUtils::MarkWindowAsUnpinnable(m_hWnd);

	CHOOSE_VERSION_ADDANCHOR;

	AddAnchor(IDC_GROUP_OPTION, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_MERGE_MESSAGE,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_LOGMESSAGE,TOP_LEFT,BOTTOM_RIGHT);

	AddAnchor(IDOK,BOTTOM_RIGHT);
	AddAnchor(IDCANCEL,BOTTOM_RIGHT);
	AddAnchor(IDHELP, BOTTOM_RIGHT);

	this->AddOthersToAnchor();

	AdjustControlSize(IDC_RADIO_BRANCH);
	AdjustControlSize(IDC_RADIO_TAGS);
	AdjustControlSize(IDC_RADIO_VERSION);
	AdjustControlSize(IDC_CHECK_SQUASH);
	AdjustControlSize(IDC_CHECK_NOFF);
	AdjustControlSize(IDC_CHECK_NOCOMMIT);
	AdjustControlSize(IDC_CHECK_MERGE_LOG);

	CheckRadioButton(IDC_RADIO_BRANCH,IDC_RADIO_VERSION,IDC_RADIO_BRANCH);
	this->SetDefaultChoose(IDC_RADIO_BRANCH);

	CString sWindowTitle;
	GetWindowText(sWindowTitle);
	CAppUtils::SetWindowTitle(m_hWnd, g_Git.m_CurrentDir, sWindowTitle);

	Init(true);

	m_ProjectProperties.ReadProps(CTGitPath(g_Git.m_CurrentDir));

	m_cLogMessage.Init(m_ProjectProperties);
	m_cLogMessage.SetFont((CString)CRegString(_T("Software\\TortoiseGit\\LogFontName"), _T("Courier New")), (DWORD)CRegDWORD(_T("Software\\TortoiseGit\\LogFontSize"), 8));
	m_cLogMessage.RegisterContextMenuHandler(this);

	m_cLogMessage.SetText(m_pDefaultText);

	((CComboBox *)GetDlgItem(IDC_COMBO_MERGESTRATEGY))->AddString(_T("resolve"));
	((CComboBox *)GetDlgItem(IDC_COMBO_MERGESTRATEGY))->AddString(_T("recursive"));
	((CComboBox *)GetDlgItem(IDC_COMBO_MERGESTRATEGY))->AddString(_T("ours"));
	((CComboBox *)GetDlgItem(IDC_COMBO_MERGESTRATEGY))->AddString(_T("subtree"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("ours"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("theirs"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("patience"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("ignore-space-change"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("ignore-all-space"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("ignore-space-at-eol"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("renormalize"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("no-renormalize"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("rename-threshold"));
	((CComboBox *)GetDlgItem(IDC_COMBO_STRATEGYOPTION))->AddString(_T("subtree"));

	EnableSaveRestore(_T("MergeDlg"));
	GetDlgItem(IDOK)->SetFocus();

	return FALSE;
}

// CMergeDlg message handlers


void CMergeDlg::OnBnClickedOk()
{
	this->UpdateData(TRUE);

	this->UpdateRevsionName();

	this->m_strLogMesage = m_cLogMessage.GetText() ;
	if( m_strLogMesage == CString(this->m_pDefaultText) )
	{
		m_strLogMesage.Empty();
	}
	if (m_MergeStrategy != _T("recursive"))
		m_StrategyOption = _T("");
	if (m_StrategyOption != _T("rename-threshold") && m_StrategyOption != _T("subtree"))
		m_StrategyParam = _T("");

	OnOK();
}

void CMergeDlg::OnDestroy()
{
	WaitForFinishLoading();
	__super::OnDestroy();
}

void CMergeDlg::OnBnClickedCheckMergeLog()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_MERGE_LOGNUM)->EnableWindow(m_bLog);
}

void CMergeDlg::OnCbnSelchangeComboMergestrategy()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_COMBO_STRATEGYOPTION)->EnableWindow(m_MergeStrategy == _T("recursive"));
	GetDlgItem(IDC_EDIT_STRATEGYPARAM)->EnableWindow(m_MergeStrategy == _T("recursive") ? m_StrategyOption == _T("rename-threshold") || m_StrategyOption == _T("subtree") : FALSE);
}

void CMergeDlg::OnCbnSelchangeComboStrategyoption()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_STRATEGYPARAM)->EnableWindow(m_StrategyOption == _T("rename-threshold") || m_StrategyOption == _T("subtree"));
}
