// ValuePropagationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ValuePropagation.h"
#include "ValuePropagationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CValuePropagationDlg dialog




CValuePropagationDlg::CValuePropagationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CValuePropagationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iLineHigh = 20;
	m_strFile = "";
}

void CValuePropagationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CValuePropagationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CValuePropagationDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CValuePropagationDlg message handlers

BOOL CValuePropagationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	OnSize_Setting();
	IsInitFile();
	Threshold_Setting();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CValuePropagationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CValuePropagationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CValuePropagationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CValuePropagationDlg::OnSize_Setting()
{
	const int iControlHigh = 18;
	const int iHorzLineHigh = 3;
	const int iGapHigh = 5;
	const int iLeft = 20;

	GetDlgItem(IDC_STATIC_TEST_ONE)->MoveWindow(iLeft, m_iLineHigh, 200, iControlHigh);
	m_iLineHigh += iControlHigh;

	//1
	m_iLineHigh += iGapHigh;
	GetDlgItem(IDC_STATIC_MAX1)->MoveWindow(iLeft + 220, m_iLineHigh, 120, iControlHigh);
	GetDlgItem(IDC_COMBOX_MAX1)->MoveWindow(iLeft + 345, m_iLineHigh, 60, iControlHigh);

	GetDlgItem(IDC_STATIC_MIN1)->MoveWindow(iLeft + 415, m_iLineHigh, 120, iControlHigh);
	GetDlgItem(IDC_COMBOX_MIN1)->MoveWindow(iLeft + 540, m_iLineHigh, 60, iControlHigh);
	m_iLineHigh+= iControlHigh;


	//2
	m_iLineHigh += iGapHigh;
	GetDlgItem(IDC_STATIC_MAX2)->MoveWindow(iLeft + 220, m_iLineHigh, 120, iControlHigh);
	GetDlgItem(IDC_COMBOX_MAX2)->MoveWindow(iLeft + 345, m_iLineHigh, 60, iControlHigh);

	GetDlgItem(IDC_STATIC_MIN2)->MoveWindow(iLeft + 415, m_iLineHigh, 120, iControlHigh);
	GetDlgItem(IDC_COMBOX_MIN2)->MoveWindow(iLeft + 540, m_iLineHigh, 60, iControlHigh);
	m_iLineHigh+= iControlHigh;

	//3
	m_iLineHigh += iGapHigh;
	GetDlgItem(IDC_STATIC_MAX3)->MoveWindow(iLeft + 220, m_iLineHigh, 120, iControlHigh);
	GetDlgItem(IDC_COMBOX_MAX3)->MoveWindow(iLeft + 345, m_iLineHigh, 60, iControlHigh);

	GetDlgItem(IDC_STATIC_MIN3)->MoveWindow(iLeft + 415, m_iLineHigh, 120, iControlHigh);
	GetDlgItem(IDC_COMBOX_MIN3)->MoveWindow(iLeft + 540, m_iLineHigh, 60, iControlHigh);
	m_iLineHigh+= iControlHigh;

	m_iLineHigh += iGapHigh + iHorzLineHigh;
	GetDlgItem(IDC_STATIC_HOR)->MoveWindow(iLeft, m_iLineHigh, 600, 1);
	m_iLineHigh += iHorzLineHigh; 
}

bool CValuePropagationDlg::IsInitFile()
{
	CFileFind finder;
	BOOL bFlag = FALSE;

	TCHAR path[MAX_PATH];

	ZeroMemory(path, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, path);

	m_strFile.Format(_T("%s"), path);
	m_strFile.Append(_T("\\ValuePropagationTest.ini"));
	bFlag = finder.FindFile(m_strFile);

	return bFlag;
}

void CValuePropagationDlg::Threshold_Setting()
{

	CString str = NULL;
	for(int i = 30; i <=150; i++)
	{
		str.Format(_T("%d"), i);
		((CComboBox *)GetDlgItem(IDC_COMBOX_MIN1))->AddString(str);
		((CComboBox *)GetDlgItem(IDC_COMBOX_MIN2))->AddString(str);
		((CComboBox *)GetDlgItem(IDC_COMBOX_MIN3))->AddString(str);		
	}

	for(int i = 150; i <=256; i++)
	{
		str.Format(_T("%d"), i);
		((CComboBox *)GetDlgItem(IDC_COMBOX_MAX1))->AddString(str);
		((CComboBox *)GetDlgItem(IDC_COMBOX_MAX2))->AddString(str);
		((CComboBox *)GetDlgItem(IDC_COMBOX_MAX3))->AddString(str);
	}


	::GetPrivateProfileStringW(_T("Test One"),_T("Max1"),_T("200"), str.GetBuffer(MAX_PATH), MAX_PATH, m_strFile);
	((CComboBox *)GetDlgItem(IDC_COMBOX_MAX1))->SetWindowText(str);
	::GetPrivateProfileStringW(_T("Test One"),_T("Min1"),_T("20"), str.GetBuffer(MAX_PATH), MAX_PATH, m_strFile);
	((CComboBox *)GetDlgItem(IDC_COMBOX_MIN1))->SetWindowText(str);

	::GetPrivateProfileStringW(_T("Test One"),_T("Max2"),_T("200"), str.GetBuffer(MAX_PATH), MAX_PATH, m_strFile);
	((CComboBox *)GetDlgItem(IDC_COMBOX_MAX2))->SetWindowText(str);
	::GetPrivateProfileStringW(_T("Test One"),_T("Min2"),_T("20"), str.GetBuffer(MAX_PATH), MAX_PATH, m_strFile);
	((CComboBox *)GetDlgItem(IDC_COMBOX_MIN2))->SetWindowText(str);

	::GetPrivateProfileStringW(_T("Test One"),_T("Max3"),_T("200"),str.GetBuffer(MAX_PATH), MAX_PATH, m_strFile);
	((CComboBox *)GetDlgItem(IDC_COMBOX_MAX3))->SetWindowText(str);
	::GetPrivateProfileStringW(_T("Test One"),_T("Min3"),_T("20"), str.GetBuffer(MAX_PATH), MAX_PATH, m_strFile);
	((CComboBox *)GetDlgItem(IDC_COMBOX_MIN3))->SetWindowText(str);
}


void CValuePropagationDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString str = NULL;
	((CComboBox *)GetDlgItem(IDC_COMBOX_MAX1))->GetWindowText(str);
	::WritePrivateProfileStringW(_T("Test One"),_T("Max1"), str, m_strFile);  
	((CComboBox *)GetDlgItem(IDC_COMBOX_MIN1))->GetWindowText(str);
	::WritePrivateProfileStringW(_T("Test One"),_T("Min1"), str, m_strFile);  

	((CComboBox *)GetDlgItem(IDC_COMBOX_MAX2))->GetWindowText(str);
	::WritePrivateProfileStringW(_T("Test One"),_T("Max2"), str, m_strFile);  
	((CComboBox *)GetDlgItem(IDC_COMBOX_MIN2))->GetWindowText(str);
	::WritePrivateProfileStringW(_T("Test One"),_T("Min2"), str, m_strFile);  

	((CComboBox *)GetDlgItem(IDC_COMBOX_MAX3))->GetWindowText(str);
	::WritePrivateProfileStringW(_T("Test One"),_T("Max3"), str, m_strFile);  
	((CComboBox *)GetDlgItem(IDC_COMBOX_MIN3))->GetWindowText(str);
	::WritePrivateProfileStringW(_T("Test One"),_T("Min3"), str, m_strFile);  

}
