// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTestDlg dialog


CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bControl = true;
	m_pStuTouchData = NULL;
	m_bRawData = false;
	m_bEndRawData = false;
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, NumEdit);
	DDX_Control(pDX, IDC_EDIT2, AddrEdit);
	DDX_Control(pDX, IDC_EDIT3, DataEdit);
	DDX_Control(pDX, IDC_LIST2, m_RawDataList);
	DDX_Control(pDX, IDC_PDRAW, m_Picture);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTestDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &CTestDlg::OnEnChangeEdit1)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestDlg::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestDlg::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BSTART, &CTestDlg::OnBnClickedBstart)
	ON_BN_CLICKED(IDC_BSTOP, &CTestDlg::OnBnClickedBstop)
	ON_MESSAGE(ON_MYEVENT_SNED,  &CTestDlg::OnCANMsgIncoming)
	ON_MESSAGE(WM_TOUCH_MESSAGE, &CTestDlg::OnReadTouchData)
END_MESSAGE_MAP()


// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//m_RawDataList.SetItemText(&M_)

	//初始化按钮，置为不可用
// 	CButton *pButtonStart=(CButton*)GetDlgItem(IDC_BSTART);
// 	CButton *pButtonEnd=(CButton*)GetDlgItem(IDC_BSTOP);
// 
// 	if(pButtonStart->IsWindowEnabled())
// 	{
// 		pButtonStart->EnableWindow(FALSE);
// 	}
// 
// 	if(pButtonEnd->IsWindowEnabled())
// 	{
// 		pButtonEnd->EnableWindow(FALSE);
// 	}

	m_Picture.ShowWindow(FALSE);

	m_Operator.SetDlgHWnd(m_hWnd);

	 m_DeviceInfo = m_Operator.GetDevicesInfo();

	//进入工厂模式
	if(ERROR_CODE_OK != m_Operator.EnterFactory())
	{
		MessageBox(_T("Enter factory pattern unsuccessfully!"));
	}

	unsigned char pPanelRows;
	unsigned char pPanelCols;
	m_Operator.ReadReg(0x02, &pPanelRows);
	m_Operator.ReadReg(0x03, &pPanelCols);

	m_Operator.SetChannelNum(pPanelRows, pPanelCols);	

	CString strColumn;
	CString strRow;

	for(int i = 0 ; i < pPanelCols; ++i)
	{
		if(0 == i)
		{
			strColumn = _T(" ");
		}
		else if(i>0 && i<10)
		{
			strColumn.Format(_T("CHY0%d"), i);
			strRow.Format(_T("CHX0%d"), i);
		}
		else
		{
			strColumn.Format(_T("CHY%d"), i);
			strRow.Format(_T("CHX%d"), i);
		}

		m_RawDataList.InsertColumn(i, strColumn, LVCFMT_CENTER, 50, i);
		if(i < pPanelRows)
		{
			m_RawDataList.InsertItem(i - 1, strColumn);
		}
		
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CTestDlg::PreTranslateMessage(MSG* pMsg)
{
	//转移停止RawData控件的焦点
	if(m_bEndRawData)
	{
		GetDlgItem(IDC_STATIC)->SetFocus();
		m_bEndRawData = false;
	}
	
	//是否捕获空格消息
	BOOL bReturn = FALSE;

	CButton *pButtonStart=(CButton*)GetDlgItem(IDC_BSTART);
	CButton *pButtonEnd=(CButton*)GetDlgItem(IDC_BSTOP);

	if (pMsg->message==WM_KEYDOWN)
	{
 		switch(pMsg->wParam)
		{ 
		case VK_SPACE:
			{	
				//RawData功能没有启动
				if(!m_bRawData)
				{
					m_bControl = !m_bControl;
					if(m_bControl)
					{
						m_Picture.ShowWindow(FALSE);
						m_RawDataList.ShowWindow(TRUE);

						CRect rRawDataList;
						m_RawDataList.GetWindowRect(&rRawDataList);
						ScreenToClient(rRawDataList);
						InvalidateRect(&rRawDataList);				//刷新区域

						m_Operator.EndTouchData();					//结束线程

						//进入工厂模式
						if(ERROR_CODE_OK != m_Operator.EnterFactory())
						{
							MessageBox(_T("Enter factory pattern unsuccessfully!"));
							return FALSE;
						}
						

						if(!pButtonStart->IsWindowEnabled())
						{
							pButtonStart->EnableWindow(TRUE);
						}

						if(!pButtonEnd->IsWindowEnabled())
						{
							pButtonEnd->EnableWindow(TRUE);
						}
					}
					else
					{
						if(0 == m_Operator.GetDevicesNum())
						{
							MessageBox(_T("No device!"));
							return FALSE;
						}

						if(pButtonStart->IsWindowEnabled())
						{
							pButtonStart->EnableWindow(FALSE);
						}

						if(pButtonEnd->IsWindowEnabled())
						{
							pButtonEnd->EnableWindow(FALSE);
						}

						//进入工作模式
 						if(ERROR_CODE_OK != m_Operator.EnterWork())
						{
							MessageBox(_T("Enter work pattern error!"));
							return FALSE;
						}
						m_RawDataList.ShowWindow(FALSE);
						m_Picture.ShowWindow(TRUE);

						CRect rPicture;
						m_Picture.GetWindowRect(&rPicture);		 //获取控件的屏幕坐标
						ScreenToClient(rPicture);				 //转换为对话框上的客户坐标

						CRect rBackground(rPicture.left + 250, rPicture.top, rPicture.right - 250, rPicture.bottom);
						m_rBackground = rBackground;

						CBrush brush(RGB(0,0,0));
						CClientDC dc(this);
						dc.FillRect(rBackground, &brush);

						m_Operator.StartTouchData();	  //开启线程
					}
				}
 				
				bReturn = TRUE;
			}
		default:
			{
				break;
			}
		}	
	}

	//是空格消息就自己处理不是就交由系统处理
	return bReturn ? bReturn : CDialog::PreTranslateMessage(pMsg);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

/*#include <cstring>*/
void CTestDlg::OnPaint()
{
	CString str;
	LPCTSTR lpszString;

	//HIDD_ATTRIBUTES DeviceInfo = m_Operator.GetDevicesInfo();
	
	PAINTSTRUCT ps;  
	HDC hdc = ::BeginPaint (m_hWnd, &ps);

	lpszString = _T("VendorID:");
	TextOut(hdc, 0,0, lpszString, sizeof(lpszString)+5);
	str.Format(_T("%x"), m_DeviceInfo.VendorID);
	TextOut(hdc, 65,0, str, sizeof(str));

	lpszString = _T("ProductID:");
	TextOut(hdc, 0,16, lpszString, sizeof(lpszString)+6);
	str.Format(_T("%x"), m_DeviceInfo.ProductID);
	TextOut(hdc, 65,16, str, sizeof(str));

	str.Format(_T("%x"), m_DeviceInfo.VendorID);


	CString str1;
	str1.Format(_T("%d"), m_Operator.GetDevicesNum());
	NumEdit.SetWindowText(str1);

// 	if(NULL != m_pStuTouchData)
// 	{
// 		for ( int iCount = 0; iCount < MAX_NUM_POINT; ++iCount )
// 		{
// 			if(2 == m_pStuTouchData->bEvent[iCount])
// 			{
// 				CClientDC dc(this);
// 				CPen pen(PS_SOLID,1,RGB(255, 0, 0));
// 				dc.SelectObject(&pen);
// 				CBrush Brush(RGB(255, 0, 0));
// 				dc.SelectObject(Brush);
// 				CPoint pc;
// 				CPoint& pt = m_pStuTouchData->Points[iCount];
// 		
// 				ScreenXYChange(pt.x, pt.y, m_rBackground, pc.x, pc.y);
// 
// 				if(pc.x - 10 >= m_rBackground.left && pc.x + 10 <= m_rBackground.right\
// 					&& pc.y- 10 >= m_rBackground.top && pc.y + 10 <= m_rBackground.bottom)
// 				{
// 					CBrush bBackground(RGB(0,0,0));
// 					CClientDC dc(this);
// 					dc.FillRect(m_rBackground, &bBackground);
// 
// 					dc.Ellipse(pc.x-10,pc.y-10,pc.x+10,pc.y+10);
// 				}	
// 			}
// 			else if(1 == m_pStuTouchData->bEvent[iCount])
// 			{
// 				CBrush bBackground(RGB(0,0,0));
// 				CClientDC dc(this);
// 				dc.FillRect(m_rBackground, &bBackground);
// 			}
// 		}
// 	}

	
	::EndPaint (m_hWnd, &ps);

	

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
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}

void CTestDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CTestDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//UpdateWindow();
	//CDialog::OnMouseMove(nFlags, point);
}

LRESULT CTestDlg::OnCANMsgIncoming(WPARAM wParam,LPARAM lParam)
{
	CString strBuf[TX_NUM_MAX][TX_NUM_MAX];

	int (*RawData)[RX_NUM_MAX] = (int (*)[RX_NUM_MAX])wParam;
	for(int i = 0; i < 18; ++i)
	{
		for(int j = 1; j < 32; ++j)
		{
			strBuf[i][j].Format(_T("%d"), RawData[i][j]);
			m_RawDataList.SetItemText(i, j , strBuf[i][j]);
		}
	}

	return 0;
}

LRESULT CTestDlg::OnReadTouchData(WPARAM wparam, LPARAM lparam)
{
	//m_pStuTouchData = (TouchDataStruct*)wparam;
	TouchDataStruct *pStuTouchData = (TouchDataStruct*)wparam;

	CDC *pDC = GetDC();
	//Draw picture

	CBrush bBackground(RGB(0,0,0));
	CClientDC dc(this);
	dc.FillRect(m_rBackground, &bBackground);

	for ( int iCount = 0; iCount < MAX_NUM_POINT; ++iCount )
	{
		if(2 == pStuTouchData->bEvent[iCount])
		{
			CClientDC dc(this);
			CPen pen(PS_SOLID,1,RGB(255, 0, 0));
			dc.SelectObject(&pen);
			CBrush Brush(RGB(255, 0, 0));
			dc.SelectObject(Brush);
			CPoint pc;
			CPoint& pt = pStuTouchData->Points[iCount];

			ScreenXYChange(pt.x, pt.y, m_rBackground, pc.x, pc.y);

			if(pc.x - 10 >= m_rBackground.left && pc.x + 10 <= m_rBackground.right\
				&& pc.y- 10 >= m_rBackground.top && pc.y + 10 <= m_rBackground.bottom)
			{
				dc.Ellipse(pc.x-10,pc.y-10,pc.x+10,pc.y+10);
				CString strPosition;
				strPosition.Format(_T("%d,%d"), pc.x, pc.y);
				SetTextColor(dc, RGB(255, 255, 255)); SetBkMode(dc, TRANSPARENT);
				::TextOut(dc, pc.x - 25, pc.y + 15, strPosition, sizeof(strPosition)+3);
			}
			
		}
	}

	ReleaseDC(pDC);

	//this->Invalidate(FALSE);

	return 0;
}

void CTestDlg::OnBnClickedButtonRead()
{
	// TODO: Add your control notification handler code here
	if(0 == m_Operator.GetDevicesNum())
	{
		MessageBox(_T("No Device!"));
		return;
	}

	CString strAddr;
	AddrEdit.GetWindowText(strAddr);

	if(strAddr.IsEmpty())
	{
		MessageBox(_T("Invalid Register Address!"));
		return;
	}
	
	USES_CONVERSION;
	char * pAddr = T2A(strAddr);   
	
	unsigned char RegData;

	int nAddr = atoi(pAddr);
	if(nAddr > 0xff || nAddr < 0x00)
	{
		MessageBox(_T("Invalid Register Address!"));
		return;
	}

	char ch = (char)nAddr;

	m_Operator.ReadReg(ch, &RegData);

	CString strData;
	strData.Format(_T("%d"), RegData);
	DataEdit.SetWindowText(strData);

}

void CTestDlg::OnBnClickedButtonWrite()
{
	// TODO: Add your control notification handler code here
	if(0 == m_Operator.GetDevicesNum())
	{
		MessageBox(_T("No device!"));
		return;
	}

	CString strAddr;
	CString strData;

	AddrEdit.GetWindowText(strAddr);
	DataEdit.GetWindowText(strData);

	if(strAddr.IsEmpty() || strData.IsEmpty())
	{
		MessageBox(_T("Invalid operator!"));
		return;
	}

	USES_CONVERSION;
	char *pAddr = T2A(strAddr);
	char *pData = T2A(strData);

	int nAddr = atoi(pAddr);
	int nData = atoi(pData);
	if(nAddr > 0xff || nAddr < 0x00 || nData > 0xff || nData < 0x00)
	{
		MessageBox(_T("Beyond the range of values!"));
		return;
	}

	char chAddr = (char)nAddr;
	char chData = (char)nData;
	m_Operator.WriteReg(chAddr, chData);
}

void CTestDlg::OnBnClickedBstart()
{
	// TODO: Add your control notification handler code here
	if(0 == m_Operator.GetDevicesNum())
	{
		MessageBox(_T("No device!"));
		return;
	}
	
	CButton *pRead = (CButton *)GetDlgItem(IDC_BUTTON1);
	CButton *pWrite = (CButton *)GetDlgItem(IDC_BUTTON2);

	if(pRead->IsWindowEnabled())
	{
		pRead->EnableWindow(FALSE);
	}

	if(pWrite->IsWindowEnabled())
	{
		pWrite->EnableWindow(FALSE);
	}

	if(!m_bRawData)
	{
		m_bRawData = true;
	}

// 	if(ERROR_CODE_OK != m_Operator.EnterFactory())
// 	{
// 		MessageBox(_T("111111"));
// 	}

	if(NULL != m_Operator.GetDlgHWnd())
	{
		//开启读取RawData值的线程
		m_Operator.TestItem_RawDataTest();
	}
}

void CTestDlg::OnBnClickedBstop()
{
	// TODO: Add your control notification handler code here
	if(0 == m_Operator.GetDevicesNum())
	{
		MessageBox(_T("No device!"));
		return;
	}

	CButton *pRead = (CButton *)GetDlgItem(IDC_BUTTON1);
	CButton *pWrite = (CButton *)GetDlgItem(IDC_BUTTON2);

	if(!pRead->IsWindowEnabled())
	{
		pRead->EnableWindow(TRUE);
	}

	if(!pWrite->IsWindowEnabled())
	{
		pWrite->EnableWindow(TRUE);
	}

	if(!m_bEndRawData)
	{
		m_bEndRawData = true;
	}


	if(m_bRawData)
	{
		m_bRawData = false;
	}
	
	//关闭读取RawData值的线程
	m_Operator.TestItem_StopRawDataTest();

// 	if(!pButtonStart->IsWindowEnabled())
// 	{
// 		pButtonStart->EnableWindow(TRUE);
// 	}
}


void CTestDlg::ScreenXYChange(long iIn_X,long iIn_Y,CRect rect,long& iOut_X,long& iOut_Y)
{
	if ((iIn_X == NO_TOUCH) || (iIn_Y == NO_TOUCH))
	{
		iOut_X = iIn_X;
		iOut_Y = iIn_Y;
		return;
	}

	int Width = rect.Width();
	int Height = rect.Height();

	unsigned int iTempX;
	unsigned int iTempY;

	//当前屏幕分辨率 
	int ixMaxPixel = 720; 
	int iyMaxPixel = 1280;


	iTempX = iIn_X;
	iTempY = iIn_Y;

	iTempX = iTempX*Width/ixMaxPixel  + rect.left;
	iTempY = iTempY*Height/iyMaxPixel + rect.top;


	iOut_X = /*rect.left+*/iTempX;
	iOut_Y = /*rect.top+*/iTempY;

	//根据参数计算坐标
	//根据参数计算坐标
}
