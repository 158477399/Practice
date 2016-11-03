// TestDlg.h : header file
//

#pragma once

#include "MyOperator.h"
#include "afxwin.h"

// CTestDlg dialog
class CTestDlg : public CDialog
{
// Construction
public:
	CTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedBstart();
	afx_msg void OnBnClickedBstop();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	//自定义消息
	afx_msg LRESULT OnCANMsgIncoming(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnReadTouchData(WPARAM wparam, LPARAM lparam);

private:
	void ScreenXYChange(long iIn_X,long iIn_Y,CRect rect,long& iOut_X,long& iOut_Y);


private:
	// 控件对象
	CEdit AddrEdit;
	CEdit DataEdit;
	CEdit NumEdit;
	CFont m_StaticFont;
	CListCtrl m_RawDataList;

	CMyOperator m_Operator;
	CStatic m_Picture;

	//是否显示控件
	bool m_bControl;

	CRect m_rBackground;
	TouchDataStruct* m_pStuTouchData;

	//是否是RawData功能，防止RawData功能启动后按空格切换到TouchData的功能，这是不允许的
	bool m_bRawData;

	HIDD_ATTRIBUTES m_DeviceInfo;

	bool m_bEndRawData;
};
