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

	//�Զ�����Ϣ
	afx_msg LRESULT OnCANMsgIncoming(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnReadTouchData(WPARAM wparam, LPARAM lparam);

private:
	void ScreenXYChange(long iIn_X,long iIn_Y,CRect rect,long& iOut_X,long& iOut_Y);


private:
	// �ؼ�����
	CEdit AddrEdit;
	CEdit DataEdit;
	CEdit NumEdit;
	CFont m_StaticFont;
	CListCtrl m_RawDataList;

	CMyOperator m_Operator;
	CStatic m_Picture;

	//�Ƿ���ʾ�ؼ�
	bool m_bControl;

	CRect m_rBackground;
	TouchDataStruct* m_pStuTouchData;

	//�Ƿ���RawData���ܣ���ֹRawData���������󰴿ո��л���TouchData�Ĺ��ܣ����ǲ������
	bool m_bRawData;

	HIDD_ATTRIBUTES m_DeviceInfo;

	bool m_bEndRawData;
};
