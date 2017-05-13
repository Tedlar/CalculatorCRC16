
// CRCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CRC.h"
#include "CRCDlg.h"
#include "afxdialogex.h"
#include <string>
#include <climits>
#include <cmath>
#include <cstdint>
#include <vector>
#include <chrono>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

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


// CCRCDlg dialog



CCRCDlg::CCRCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CRC_DIALOG, pParent)
	, m_timeTotal(_T(""))
	, m_timeOnce(_T(""))
	, m_crc(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCRCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMETOTAL, m_timeTotal);
	DDX_Text(pDX, IDC_TIMEONCE, m_timeOnce);
	DDX_Text(pDX, IDC_CRC, m_crc);
	DDX_Control(pDX, IDC_EDIT1, m_editBytes);
	DDX_Control(pDX, IDC_EDIT2, m_editCycles);
}

BEGIN_MESSAGE_MAP(CCRCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCRCDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCRCDlg message handlers

BOOL CCRCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCRCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCRCDlg::OnPaint()
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
HCURSOR CCRCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCRCDlg::OnBnClickedOk()
{
	CString editBytes = _T("");
	CString editCycles = _T("");
	m_editBytes.GetWindowTextW(editBytes);
	m_editCycles.GetWindowTextW(editCycles);
	if (!checkEditBytes(editBytes)) return;
	if (!checkEditCycles(editCycles)) return;
	calculateCRC(editBytes, editCycles);
	UpdateData(FALSE);
}

bool CCRCDlg::checkEditBytes(CString _bytes) {
	if (_bytes.GetLength() > 256) {
		AfxMessageBox(_T("Zbyt du¿a iloœæ bajtów. Maksymalnie 256 znaków."));
		return false;
	}
	return true;
}

bool CCRCDlg::checkEditCycles(CString _cycles) {
	int cycles = _wtoi(_cycles);
	if (cycles == 0) {
		AfxMessageBox(_T("Liczba cyki nie mo¿e byæ równa 0."));
		return false;
	}
	if (cycles > pow(10, 9)) {
		AfxMessageBox(_T("Liczba cyki nie mo¿e byæ wiêksza od 10^9."));
		return false;
	}
	return true;
}

void CCRCDlg::calculateCRC(CString _bytes, CString _cycles) {
	_bytes.Remove(_T(' '));
	CT2CA psz(_bytes);
	std::string str(psz);

	if ((str.size() % 2) != 0)
		str = "0" + str;
	int bytesNumber = str.size()/2;

	std::vector<uint8_t> buffer(bytesNumber);
	int pos = 0;
	for (int i = 0; i < bytesNumber; ++i) {
		buffer[i] = std::strtoul(str.substr(pos,2).c_str(), 0, 16);
		pos += 2;
	}
	int cycles = _wtoi(_cycles);
	int passes = cycles;
	uint16_t crc;
	uint8_t* data = buffer.data();
	size_t lenght = buffer.size();
	auto start = std::chrono::system_clock::now();
	while (cycles--) {
		crc = CRC(data, lenght);
	}
	auto end = std::chrono::system_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	uint32_t timeTotal = elapsed_ms.count();
	uint32_t timeOnce  = elapsed_ns.count() / passes;

	CString tmp;
	tmp.Format(_T("%u"), timeTotal);
	m_timeTotal = tmp;
	tmp.Format(_T("%u"), timeOnce);
	m_timeOnce = tmp;
	tmp.Format(_T("%X"), crc);
	m_crc = tmp;
}

unsigned short CCRCDlg::CRC(unsigned char *pMessage, unsigned int NumberOfBytes)
{
	unsigned char HiByte = 0xFF;
	unsigned char LoByte = 0xFF;
	unsigned char Index;
	while (NumberOfBytes--)
	{
		Index = HiByte ^ *pMessage++;
		HiByte = LoByte ^ aCRCHi[Index];
		LoByte = aCRCLo[Index];
	};
	return (HiByte << 8 | LoByte);
}
