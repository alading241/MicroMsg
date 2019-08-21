
// WxHookDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WxHook.h"
#include "WxHookDlg.h"
#include "afxdialogex.h"
#include"../Common/Config.h"
#include"../Common/Common.h"
#include"pipe.h"
#include"Lock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWxHookDlg 对话框



CWxHookDlg::CWxHookDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWxHookDlg::IDD, pParent)
	, strWxPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWxHookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, strWxPath);
	DDX_Control(pDX, IDC_LIST1, mViewList);
	DDX_Control(pDX, IDC_LIST2, mListView);
}

BEGIN_MESSAGE_MAP(CWxHookDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CWxHookDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CWxHookDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_WND_MSG, &CWxHookDlg::OnWndMsg)
END_MESSAGE_MAP()


// CWxHookDlg 消息处理程序

BOOL CWxHookDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 初始化程序路径
	strWxPath = "C:\\Program Files (x86)\\Tencent\\WeChat\\WeChat.exe";

	//listcontrol控件初始化
	char  column[][MAX_HEADLENGTH] = { "序号","消息类型","消息内容" };
	mListView.SetHeaders(column, sizeof(column) / sizeof(*column));
	mListView.MoveWindow(10, 75, 235, 265,TRUE);

	//hexview控件初始化
	pHexView = HexEditControl::ShowHexControl(this->m_hWnd, 250, 75, 460, 265);
	BYTE buffer[] = { 0x1,0x2 };
	HexEditControl::SetData(pHexView, buffer, sizeof(buffer));

	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWxHookDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWxHookDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWxHookDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//获取微信目录
CString CWxHookDlg::GetWxPath()
{
	//HKEY hKey;
	//TCHAR szProductType[MY_BUFSIZE];
	//memset(szProductType, 0, sizeof(szProductType));
	//DWORD dwBufLen = MY_BUFSIZE;
	//LONG lRet;

	//// 下面是打开注册表, 只有打开后才能做其他操作 
	//lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,  // 要打开的根键 
	//	TEXT("SOFTWARE\\TENCENT\\QQ2009"), // 要打开的子子键 
	//	0,        // 这个一定要为0 
	//	KEY_QUERY_VALUE,  //  指定打开方式,此为读 
	//	&hKey);    // 用来返回句柄 

	//if (lRet != ERROR_SUCCESS)   // 判断是否打开成功 
	//	return 1;
	UpdateData(TRUE);
	return strWxPath;
}
//启动微信
void CWxHookDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	CHAR szLauncherPath[MAX_PATH] = { NULL };
	GetModuleFileName(NULL, szLauncherPath, MAX_PATH);
	PathRemoveFileSpec(szLauncherPath);
	lstrcat(szLauncherPath, "\\");
	lstrcat(szLauncherPath, CONF_Launcher_FileName);

	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	StartupInfo.lpDesktop = NULL;
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_SHOWNORMAL;

	//
	//Command Line
	//
	//(Launcher.exe -exe [exe Name])
	CHAR szCommandLine[512] = { NULL };
	wsprintf(
		szCommandLine,
		"%s",
		GetWxPath()
		);
	//判断szCommandLine是否有空格
	CString szDest = szCommandLine;
	// 判断是否有空格
	if (szDest.Find(' ') != -1)
	{
		// 有空格,用转义符处理
		CString szSpaceFormat = " \"%s\"";
		szDest.Format(szSpaceFormat, szCommandLine);
	}

	BOOL bIsLauncherCalled = FALSE;
	if (CreateProcess(
		szLauncherPath,
		szDest.GetBuffer(),
		NULL,
		NULL,
		FALSE,
		NULL,
		NULL,
		NULL,
		&StartupInfo,
		&ProcessInfo
		))
	{
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);

		bIsLauncherCalled = TRUE;
		start_pipe();
	}
	else
	{
		bIsLauncherCalled = FALSE;
		AfxMessageBox("启动目标程序失败");
	}
}

//刷新列表
void CWxHookDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	unsigned char buffer[] = {
		0x08,0x02,0x12,0x27,0x08,0x01,0x10,0xCB,0xB1,0xB0,0xBD,0x03,0x18,0x04,0x22,0x1B,0x08,0x17,0x12,0x17,0x0A,0x13,0x77,0x78,0x69,0x64,0x5F,0x76,0x34,0x78,0x33,0x74,0x76,0x36,0x70,0x68,0x63,0x77,0x69,0x32,0x31,0x10,0x07,0x12,0x18,0x08,0x03,0x10,0xCB,0xB1,0xB0,0xBD,0x03,0x18,0x05,0x22,0x0C,0x08,0x08,0x12,0x08,0x08,0x04,0x12,0x04,0x05,0x00,0x00,0x00,0x18,0x06,
	};
	mParseProto.PrintDebugString(buffer, sizeof(buffer));
}

void CWxHookDlg::start_pipe()
{
	DWORD dwThreadId = 0;
	CreateThread(
		NULL,//default security attributes
		0,//use default stack size
		pipe_server,//thread function
		&m_hWnd,//argument to thread function
		0,//use default creation flags
		&dwThreadId);//returns the thread identifier
	
	//Check there turn value for success.
	if (dwThreadId == NULL)
	{
		return;
	}

}

afx_msg LRESULT CWxHookDlg::OnWndMsg(WPARAM wParam, LPARAM lParam)
{
	{
		AutoLock auto_lock;
		DWORD code = wParam;
		switch (code)
		{
		case 1:
		{
			unsigned char *data = (unsigned char *)lParam + 4;
			unsigned int len = htonlx(*(unsigned int*)lParam);
			std::string ret=mParseProto.PrintDebugString(data,len-4);

			//CString mProtoFile = "proto\\MicroMsg01.proto";
			//LoadProto(mProtoFile);
			//std::string msg_name = "";
			//std::string ret=ParseData(msg_name, data, len);
			mViewList.AddString(ret.c_str());
		}break;
		default:
			mViewList.AddString("11111111111111111111");
			break;
		}

	}
	Sleep(1);
	return 0;
}

void CWxHookDlg::LoadProto(CString &mProtoFile)
{
	//加载
	mMessageList.clear();
	unsigned int len1 = mProtoFile.GetLength();//proto文件路径长度
	if (len1)
	{
		std::string str = mProtoFile.GetString();
		mParseProto.LoadProtoFromFile(str);
		std::vector<std::string> Messages = mParseProto.GetMessageList();
		for (int i = 0; i < Messages.size(); i++)
		{
			mMessageList.push_back(Messages[i]);
		}
	}
}

std::string CWxHookDlg::ParseData(std::string &message_name,unsigned char *data, unsigned int len)
{
	std::string strResult = "";
	//动态解析
	if (len)
	{
		CString strMessage = "";
		strResult = mParseProto.PrintDataFile(message_name, data, len);
		if (strResult == "")
		{
			strResult = "解析出错...";
		}
		else
		{
		}
	}
	return strResult;
}