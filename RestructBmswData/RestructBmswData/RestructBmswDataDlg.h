
// RestructBmswDataDlg.h : header file
//

#pragma once
#include <vector>
#include <string>

using namespace std;


//YYMMDD, HHMMSS.ms, SofDay.ms, kS, Vp, IFull, HV0nc, Polar, Azimuth, GSEPol, GSEAz, GSEPAb, GSEAzAb, RT, DSSPol, DSSAzim, Delta, GSEDPol, GSEDAzm
typedef struct
{
	SYSTEMTIME timeStamp;
	double  SecOfDay;
	double	kS;
	double	Vp;
	double	IFull;
	double	HV0nc;
	double	Polar;
	double	Azimuth;
	double	GSEPol;
	double	GSEAz;
	double	GSEPAb;
	double	GSEAzAb;
}BMSW_DATA;



// CRestructBmswDataDlg dialog
class CRestructBmswDataDlg : public CDialogEx
{
// Construction
public:
						CRestructBmswDataDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESTRUCTBMSWDATA_DIALOG };
#endif

protected:
	virtual void		DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON				m_hIcon;

	void				saveToRegistry();
	void				readFromRegistry();
	void				createFilteredDataArray();
	void				createFilteredDataArrayAllGood();
	void				createSelectedColumnsArray();
	void				createOutputFile();
	void				filterBmswData();
	bool				parseInputLineIntoStruct(string currentLine, BMSW_DATA &currentDataRow, bool readAll = false);
	int					readFirstLastLine(string &filename, string &firstLine, string &lastLine);
	void				initData(CString &inputFilename);
	CString				generateOutputName(CString &inputname);


	vector<int>			m_columnArray;
	vector<BMSW_DATA>	m_bmsvInputFilteredData;
	SYSTEMTIME			m_startTime;
	SYSTEMTIME			m_endTime;
	int					m_rowCount;

	// Generated message map functions
	virtual BOOL		OnInitDialog();
	afx_msg void		OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void		OnPaint();
	afx_msg HCURSOR		OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void		OnStartClicked();
	afx_msg void		OnBrowseInput();
	afx_msg void		OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void		OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void		OnBnClickedCancel();
	afx_msg void		OnBnClickedCheckAll();
	CString				m_inputFilename;
	CString				m_outputFilename;
	CString				m_csOutputsuffix;
	CString				m_statistics;
	CDateTimeCtrl		m_datePickerStart;
	CDateTimeCtrl		m_datePickerEnd;
	CSpinButtonCtrl		m_SpinCtrl;
	int					m_step;
	afx_msg void		OnBnClickedButton3();
};
