// ValuePropagationDlg.h : header file
//

#pragma once


// CValuePropagationDlg dialog
class CValuePropagationDlg : public CDialog
{
// Construction
public:
	CValuePropagationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VALUEPROPAGATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	void OnSize_Setting();
	bool IsInitFile();
	void Threshold_Setting();

private:
	int m_iLineHigh;
	CString m_strFile;
	afx_msg void OnBnClickedButton1();
};
