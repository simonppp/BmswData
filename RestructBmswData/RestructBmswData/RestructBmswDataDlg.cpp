
// RestructBmswDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RestructBmswData.h"
#include "RestructBmswDataDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <regex>

std::string myLtrim(const std::string& s) {
	return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
}

std::string myRtrim(const std::string& s) {
	return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
}

std::string myTrim(const std::string& s) {
	return myLtrim(myRtrim(s));
}

//=======================================================================
int calcFileRowCount(CString& csFileName)
{
	string fileName((CStringA(csFileName)));

	ifstream file(fileName);
	__int64 count = 0;
	if (file.is_open())
	{
		count = std::count(std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>(), '\n');
	}
	file.close();

	return (int)count;
}
//=======================================================================
int operator - (SYSTEMTIME &st1, SYSTEMTIME &st2)
{
	FILETIME ft1;
	HRESULT hrto1 = SystemTimeToFileTime(&st1, &ft1);
	ULARGE_INTEGER ul1;
	ul1.LowPart = ft1.dwLowDateTime;
	ul1.HighPart = ft1.dwHighDateTime;

	FILETIME ft2;
	HRESULT hrto2 = SystemTimeToFileTime(&st2, &ft2);
	ULARGE_INTEGER ul2;
	ul2.LowPart = ft2.dwLowDateTime;
	ul2.HighPart = ft2.dwHighDateTime;

	ul1.QuadPart = ul1.QuadPart - ul2.QuadPart;

	int nn = (int)(ul1.QuadPart);
	return nn/10000;

}
//=======================================================================
SYSTEMTIME convertStringToSystemTime(string &date, string &time)
{
	//date = YYMMDD
	string syy = date.substr(0, 2);//YY
	string smm = date.substr(2, 2);//MM
	string sdd = date.substr(4, 2);//DD

								   //time = HHMMSS.msc
	string shour = time.substr(0, 2);//HH
	string smin = time.substr(2, 2);//MM
	string ssec = time.substr(4, 2);//SS
	string msec = time.substr(7, 3);//msec

	SYSTEMTIME st;
	st.wYear = atoi(syy.c_str()) + 2000;
	st.wMonth = atoi(smm.c_str());
	st.wDay = atoi(sdd.c_str());
	st.wHour = atoi(shour.c_str());
	st.wMinute = atoi(smin.c_str());
	st.wSecond = atoi(ssec.c_str());
	st.wMilliseconds = atoi(msec.c_str());

	//calc day of the week
	FILETIME ft;
	HRESULT hrto = SystemTimeToFileTime(&st, &ft);
	HRESULT hrback = FileTimeToSystemTime(&ft, &st);

	return st;
}
//=======================================================================
//=======================================================================
//=======================================================================
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
//=======================================================================
//=======================================================================
//=======================================================================
CRestructBmswDataDlg::CRestructBmswDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESTRUCTBMSWDATA_DIALOG, pParent)
	, m_step(50)
	, m_outputFilename(_T(""))
	, m_csOutputsuffix(_T(""))
	, m_rowCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_columnArray.resize(10);// 10 = number of elements in structure BMSW (number of columns)
	readFromRegistry();
}
//=======================================================================
void CRestructBmswDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_datePickerStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_datePickerEnd);
	DDX_Text(pDX, IDC_EDIT1, m_inputFilename);
	DDX_Text(pDX, IDC_STATISTICS, m_statistics);
	DDX_Control(pDX, IDC_SPIN1, m_SpinCtrl);
	DDX_Text(pDX, IDC_EDIT2, m_step);
	DDX_Text(pDX, IDC_EDIT4, m_outputFilename);
	DDX_Text(pDX, IDC_EDIT5, m_csOutputsuffix);
}
//=======================================================================
BEGIN_MESSAGE_MAP(CRestructBmswDataDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRestructBmswDataDlg::OnStartClicked)
	ON_BN_CLICKED(IDC_BUTTON2, &CRestructBmswDataDlg::OnBrowseInput)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CRestructBmswDataDlg::OnDtnDatetimechangeDatetimepicker1)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, &CRestructBmswDataDlg::OnDtnDatetimechangeDatetimepicker2)
	ON_BN_CLICKED(IDCANCEL, &CRestructBmswDataDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_ALL, &CRestructBmswDataDlg::OnBnClickedCheckAll)
	ON_BN_CLICKED(IDC_BUTTON3, &CRestructBmswDataDlg::OnBnClickedButton3)
END_MESSAGE_MAP()
//=======================================================================
// CRestructBmswDataDlg message handlers
BOOL CRestructBmswDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// DATA INITIALIZATION=====================================================================
	m_datePickerStart.SetFormat(L"yyyy-MMM-dd HH:mm:ss");
	m_datePickerEnd.SetFormat(L"yyyy-MMM-dd HH:mm:ss");

	if (!m_inputFilename.IsEmpty())
	{
		initData(m_inputFilename);
	}

	m_SpinCtrl.SetRange(0, 1000);
	m_SpinCtrl.SetBuddy((CEdit *)GetDlgItem(IDC_EDIT2));
	m_SpinCtrl.SetPos(m_step);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
//=======================================================================
void CRestructBmswDataDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRestructBmswDataDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRestructBmswDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//============================================================================================
//YYMMDD, HHMMSS.ms, SofDay.ms, kS, Vp, IFull, HV0nc, Polar, Azimuth, GSEPol, GSEAz, GSEPAb, GSEAzAb, RT, DSSPol, DSSAzim, Delta, GSEDPol, GSEDAzm
//============================================================================================

//============================================================================================
void CRestructBmswDataDlg::OnStartClicked()
{
	filterBmswData();
}
//============================================================================================
void CRestructBmswDataDlg::filterBmswData()
{
	createSelectedColumnsArray();//selected columns to include in output file
	createFilteredDataArray();//read input file and create data arrray from selected rows
	createOutputFile();

	m_statistics.Format(L"Filtered number of rows: %d out of %d", m_bmsvInputFilteredData.size(), m_rowCount -1);//-1 without header
	UpdateData(false);
}
//============================================================================================
void CRestructBmswDataDlg::createSelectedColumnsArray()
{
	int startID = IDC_CHECK4;
	for (int i = 0; i < (int)m_columnArray.size(); i++)
	{
		m_columnArray[i] = ((CButton*)GetDlgItem(startID+i))->GetCheck();
	}
}
//============================================================================================
bool CRestructBmswDataDlg::parseInputLineIntoStruct(string currentLine, BMSW_DATA &currentDataRow, bool readAll)
{
	if (readAll == false && currentLine.find_first_of("NaN") != string::npos)
		return false;

	// Vector of string to save tokens 
	vector <string> tokens;

	// stringstream class check1 
	stringstream check1(currentLine);

	string intermediate;

	// Tokenizing w.r.t. ',' 
	while (getline(check1, intermediate, ','))
	{
		tokens.push_back(intermediate);
	}

	tokens[0] = myTrim(tokens[0]);
	tokens[1] = myTrim(tokens[1]);
	currentDataRow.timeStamp = convertStringToSystemTime(tokens[0], tokens[1]);
	tokens.erase(tokens.begin(), tokens.begin()+2);

	int count = 0;
	double curNumber;
	for (auto token : tokens)
	{
		token = myTrim(token);
		curNumber = atof(token.c_str());
		switch (count)
		{
		case 0:
			currentDataRow.SecOfDay = curNumber;
			break;
		case 1:
			currentDataRow.kS = curNumber;
			break;
		case 2:
			currentDataRow.Vp = curNumber;
			break;
		case 3:
			currentDataRow.IFull = curNumber;
			break;
		case 4:
			currentDataRow.HV0nc = curNumber;
			break;
		case 5:
			currentDataRow.Polar = curNumber;
			break;
		case 6:
			currentDataRow.Azimuth = curNumber;
			break;
		case 7:
			currentDataRow.GSEPol = curNumber;
			break;
		case 8:
			currentDataRow.GSEAz = curNumber;
			break;
		case 9:
			currentDataRow.GSEPAb = curNumber;
			break;
		case 10:
			currentDataRow.GSEAzAb = curNumber;
			break;
		}
		count++;
	}
	return true;
}
//============================================================================================
void CRestructBmswDataDlg::createFilteredDataArray()
{
	UpdateData(true);

	if (m_inputFilename.IsEmpty())
	{
		m_statistics.Format(L"File doesn't exist : %s", m_inputFilename);
		UpdateData(false);
		return;
	}

	m_bmsvInputFilteredData.clear();

	std::string inputFilename((CStringA(m_inputFilename)));
	ifstream fin(inputFilename);
	if (!fin.is_open())
	{
		m_statistics.Format(L"Error opening file: %s", m_inputFilename);
		UpdateData(false);
		return;
	}

	BMSW_DATA currentDataRow;
	std::string varDate;
	std::string varTime;
	string header;

	getline(fin, header);//skip header line

	int nRowCount = 0;
	string currentLine;
	while (getline(fin, currentLine))
	{
		if (!parseInputLineIntoStruct(currentLine, currentDataRow)) //"NaN" found
			continue;

		if ((currentDataRow.timeStamp - m_startTime) < 0)
			continue;

		if ((currentDataRow.timeStamp - m_endTime) > 0)
			break;

		if (m_bmsvInputFilteredData.size() > 0)
		{
			int diff = currentDataRow.timeStamp - m_bmsvInputFilteredData[nRowCount - 1].timeStamp;
			if (diff < m_step)
			{
				continue;
			}
		}

		m_bmsvInputFilteredData.push_back(currentDataRow);
		nRowCount++;
	}
	fin.close();
}
//============================================================================================
void CRestructBmswDataDlg::createFilteredDataArrayAllGood()
{
	UpdateData(true);

	if (m_inputFilename.IsEmpty())
	{
		m_statistics.Format(L"File doesn't exist : %s", m_inputFilename);
		UpdateData(false);
		return;
	}

	m_bmsvInputFilteredData.clear();

	std::string inputFilename((CStringA(m_inputFilename)));
	ifstream fin(inputFilename);
	if (!fin.is_open())
	{
		m_statistics.Format(L"Error opening file: %s", m_inputFilename);
		UpdateData(false);
		return;
	}

	BMSW_DATA currentDataRow;
	char dummy;
	std::string varDate;
	std::string varTime;
	string header;

	getline (fin,header);//skip header line

	int nRowCount = 0;
	while (fin					>>
		varDate					>> 
		varTime					>> 

		currentDataRow.SecOfDay >> dummy >>
		currentDataRow.kS		>> dummy >>
		currentDataRow.Vp		>> dummy >>
		currentDataRow.IFull	>> dummy >>
		currentDataRow.HV0nc	>> dummy >>
		currentDataRow.Polar	>> dummy >>
		currentDataRow.Azimuth	>> dummy >>
		currentDataRow.GSEPol	>> dummy >>
		currentDataRow.GSEAz	>> dummy >>
		currentDataRow.GSEPAb	>> dummy >>
		currentDataRow.GSEAzAb	>> dummy)
	{
		currentDataRow.timeStamp = convertStringToSystemTime(varDate, varTime);

		if ((currentDataRow.timeStamp - m_startTime) < 0)
			continue;

		if ((currentDataRow.timeStamp - m_endTime) > 0)
			break;

		if (m_bmsvInputFilteredData.size() > 0)
		{
			int diff = currentDataRow.timeStamp - m_bmsvInputFilteredData[nRowCount - 1].timeStamp;
			if (diff < m_step)
			{
				continue;
			}
		}

		m_bmsvInputFilteredData.push_back(currentDataRow);
		nRowCount++;
	}
	fin.close();
}
//============================================================================================
void CRestructBmswDataDlg::createOutputFile()
{
	if (m_inputFilename.IsEmpty())
	{
		m_statistics.Format(L"File doesn't exist : %s", m_inputFilename);
		UpdateData(false);
		return;
	}


	ofstream fout(m_outputFilename);
	if (! fout.is_open())
	{
		m_statistics.Format(L"Error opening file: %s", m_outputFilename);
		UpdateData(false);
		return;
	}

	char buffer[256];
	CStringArray columnNames;
	columnNames.Add(L"kS ");
	columnNames.Add(L"  Vp ");
	columnNames.Add(L"IFull ");
	columnNames.Add(L" HV0nc ");
	columnNames.Add(L"Polar");
	columnNames.Add(L"Azimuth");
	columnNames.Add(L"GSEPol");
	columnNames.Add(L"GSEAz");
	columnNames.Add(L"GSEPAb");
	columnNames.Add(L"GSEAzAb");

	string header = "TimeStamp               SofDay.ms";
	fout << header << "     ";

	CString str;
	for (int i = 0; i < (int)m_columnArray.size(); i++)
	{
		if (m_columnArray[i])
		{
			str.Format(L"%s", columnNames[i]);
			std::string tempString((CStringA(str)));
			fout << tempString << " ";
		}
	}
	fout << "\n";

	//WRITE SPECIFIED COLUMNS FROM FILTERED ROWS TO COMMA SEPARATED FILE
	for (auto outstruct : m_bmsvInputFilteredData)
	{
		CString cstrTimeStamp;
		SYSTEMTIME st = outstruct.timeStamp;
		cstrTimeStamp.Format(L"%d-%02d-%02d %02d:%02d:%02d.%03d",
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds);

		std::string timeStamp((CStringA(cstrTimeStamp)));
		fout << timeStamp << ", ";

		sprintf_s(buffer, "%.3f", outstruct.SecOfDay);
		fout << buffer << ", ";

		if (m_columnArray[0])
		{
			sprintf_s(buffer, "%.1f", outstruct.kS);
			fout << buffer << ", ";
		}
		if (m_columnArray[1])
		{
			sprintf_s(buffer, "%.1f", outstruct.Vp);
			fout << buffer << ", ";
		}
		if (m_columnArray[2])
		{
			sprintf_s(buffer, "%.4f", outstruct.IFull);
			fout << buffer << ", ";
		}
		if (m_columnArray[3])
		{
			sprintf_s(buffer, "%6.1f", outstruct.HV0nc);
			fout << buffer << ", ";
		}
		if (m_columnArray[4])
		{
			sprintf_s(buffer, "%3.1f", outstruct.Polar);
			fout << buffer << ", ";
		}
		if (m_columnArray[5])
		{
			sprintf_s(buffer, "%5.1f", outstruct.Azimuth);
			fout << buffer << ", ";
		}
		if (m_columnArray[6])
		{
			sprintf_s(buffer, "%3.1f", outstruct.GSEPol);
			fout << buffer << ", ";
		}
		if (m_columnArray[7])
		{
			sprintf_s(buffer, "%5.1f", outstruct.GSEAz);
			fout << buffer << ", ";
		}
		if (m_columnArray[8])
		{
			sprintf_s(buffer, "%3.1f", outstruct.GSEPAb);
			fout << buffer << ", ";
		}
		if (m_columnArray[9])
		{
			sprintf_s(buffer, "%5.1f", outstruct.GSEAzAb);
			fout << buffer << ", ";
		}

		fout << "\n";
	}

	fout.close();
}

void CRestructBmswDataDlg::OnBrowseInput()
{
	UpdateData();

	CString szFilter("Input Data Files(*.*)|*.*|");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter, NULL);
	if (dlg.DoModal() != IDOK)
		return;
	UpdateData(true);

	m_inputFilename = dlg.GetPathName();

	initData(m_inputFilename);

	UpdateData(false);

}

//============================================================================================
CString	CRestructBmswDataDlg::generateOutputName(CString &inputname)
{
	UpdateData(true);

	if (m_csOutputsuffix.IsEmpty())
	{
		m_csOutputsuffix = L"postfix.txt";
	}
	string outputFilename((CStringA(inputname)));
	size_t lastindex = outputFilename.find_last_of(".");
	string temp((CStringA(m_csOutputsuffix)));
	outputFilename = outputFilename.insert(lastindex, temp);

	CString csOutputFilename (outputFilename.c_str());
	return csOutputFilename;
}

//============================================================================================
void CRestructBmswDataDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
//============================================================================================
void CRestructBmswDataDlg::OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CRestructBmswDataDlg::saveToRegistry()
{
	UpdateData(true);

	CRestructBmswDataApp *app = (CRestructBmswDataApp*)AfxGetApp();
	app->WriteProfileString(L"Folders", L"Input", m_inputFilename);
	app->WriteProfileString(L"Folders", L"Output", m_outputFilename);
	app->WriteProfileString(L"Folders", L"Outputsuffix", m_csOutputsuffix);
}

void CRestructBmswDataDlg::readFromRegistry()
{
	CRestructBmswDataApp *app = (CRestructBmswDataApp*)AfxGetApp();
	m_inputFilename = app->GetProfileString(L"Folders", L"Input");
	m_outputFilename = app->GetProfileString(L"Folders", L"Output");
	m_csOutputsuffix = app->GetProfileString(L"Folders", L"Outputsuffix");
}

void CRestructBmswDataDlg::OnBnClickedCancel()
{
	saveToRegistry();
	CDialogEx::OnCancel();
}

void CRestructBmswDataDlg::OnBnClickedCheckAll()
{
	int check = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
	int startID = IDC_CHECK4;
	for (int i = 0; i < (int)m_columnArray.size(); i++)
	{
		((CButton*)GetDlgItem(startID + i))->SetCheck(check);
		m_columnArray[i] = check;
	}
}


void CRestructBmswDataDlg::OnBnClickedButton3()
{
	UpdateData(true);

	if ( !m_inputFilename.IsEmpty())
		m_outputFilename = generateOutputName(m_inputFilename);

	UpdateData(false);
}

int CRestructBmswDataDlg::readFirstLastLine(string &filename, string &firstLine, string &lastLine)
{
	ifstream fin;

	fin.open(filename);
	if (fin.is_open()) {

		getline(fin, firstLine);                      // skip the header
		getline(fin, firstLine);                      // Read the first line

		fin.seekg(-3, ios_base::end);                // go to one spot before the EOF

		bool keepLooping = true;
		while (keepLooping) {
			char ch;
			fin.get(ch);                            // Get current byte's data

			if ((int)fin.tellg() <= 1) {             // If the data was at or before the 0th byte
				fin.seekg(0);                       // The first line is the last line
				keepLooping = false;                // So stop there
			}
			else if (ch == '\n') {                   // If the data was a newline
				keepLooping = false;                // Stop at the current position.
			}
			else {                                  // If the data was neither a newline nor at the 0 byte
				fin.seekg(-2, ios_base::cur);        // Move to the front of that data, then to the front of the data before it
			}
		}

		getline(fin, lastLine);                      // Read the current line

		fin.close();
	}
	return 0;
}

void CRestructBmswDataDlg::initData(CString &inputFilename)
{
	m_rowCount = calcFileRowCount(m_inputFilename);

	//read First and Last line of input file
	std::string fname((CStringA(m_inputFilename)));
	string firsтLine, lastLine;
	readFirstLastLine(fname, firsтLine, lastLine);

	//init date picker comtrols
	BMSW_DATA tempData;
	parseInputLineIntoStruct(firsтLine, tempData, true);
	m_startTime = tempData.timeStamp;
	parseInputLineIntoStruct(lastLine, tempData, true);
	m_endTime = tempData.timeStamp;

	m_datePickerStart.SetTime(m_startTime);
	m_datePickerEnd.SetTime(m_endTime);

	m_outputFilename = generateOutputName(m_inputFilename);
}
