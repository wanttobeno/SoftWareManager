
// SoftWareManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SoftWareManager.h"
#include "SoftWareManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL OpenFolderAndSelectFile(TCHAR* szPath);

// CSoftWareManagerDlg 对话框




CSoftWareManagerDlg::CSoftWareManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoftWareManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	
}

void CSoftWareManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ControlList);
}

BEGIN_MESSAGE_MAP(CSoftWareManagerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_Folder, &CSoftWareManagerDlg::OnBnClickedBtnFolder)
	ON_BN_CLICKED(IDC_BTN_Uninstall, &CSoftWareManagerDlg::OnBnClickedBtnUninstall)
	ON_BN_CLICKED(IDC_BTN_Refresh, &CSoftWareManagerDlg::OnBnClickedBtnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSoftWareManagerDlg 消息处理程序

BOOL CSoftWareManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_Select = -1;
	m_ControlList.AddColumn(5,
		_T("ID"),
		_T("名称"),
		_T("发布者"),
		_T("安装时间"),
		_T("版本"));
	m_ControlList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	GetSoftwareInfo();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSoftWareManagerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSoftWareManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSoftWareManagerDlg::OnBnClickedBtnFolder()
{
    // TODO: 在此添加控件通知处理程序代码
    CListCtrl *ListCtrl = NULL;
    if (m_ControlList.IsWindowVisible())
        ListCtrl = &m_ControlList;
    POSITION Pos  = ListCtrl->GetFirstSelectedItemPosition();
    while(Pos)
    {
        m_Select = ListCtrl->GetNextSelectedItem(Pos);
        if (m_vecSoftInfo[m_Select].strSoftInsPath[0])
        {
            OpenFolderAndSelectFile(m_vecSoftInfo[m_Select].strSoftInsPath);
        }
		else if (m_vecSoftInfo[m_Select].strSoftUniPath[0])
		{
			OpenFolderAndSelectFile(m_vecSoftInfo[m_Select].strSoftUniPath);
		}
    }
}


void CSoftWareManagerDlg::OnBnClickedBtnUninstall()
{
    // TODO: 在此添加控件通知处理程序代码
 
    CListCtrl    *ListCtrl = NULL;
    if (m_ControlList.IsWindowVisible())
        ListCtrl = &m_ControlList;
    POSITION Pos  = ListCtrl->GetFirstSelectedItemPosition();
    while(Pos)
    {
        m_Select = ListCtrl->GetNextSelectedItem(Pos);
        if (m_vecSoftInfo[m_Select].strSoftUniPath[0])
        {
             ShellExecute(NULL, _T("open"),m_vecSoftInfo[m_Select].strSoftUniPath, NULL, NULL, SW_SHOWNORMAL);
        }
    }
    
}


void CSoftWareManagerDlg::OnBnClickedBtnRefresh()
{
    // TODO: 在此添加控件通知处理程序代码
    GetSoftwareInfo();
}


//************************************************************
// 函数名称:    GetSoftwareInfo
// 函数说明:    获取软件信息
// 作    者:    cyxvc
// 时    间:    2015/11/10
// 返 回    值:    void
//************************************************************
void CSoftWareManagerDlg::GetSoftwareInfo()
{
    m_ControlList.DeleteAllItems();
    m_vecSoftInfo.clear();
    SOFTINFO stcSoftInfo = {0};
    HKEY RootKey;            // 主键
    LPCTSTR lpSubKey;        // 子键名称
    HKEY hkResult;           // 将要打开键的句柄 
    HKEY hkRKey;
    LONG lReturn;            // 记录读取注册表是否成功
    CString strBuffer;
    CString strMidReg;

    DWORD index = 0;
    TCHAR szKeyName[255] = { 0 };        // 注册表项名称
    DWORD dwKeyLen = 255;
    DWORD dwNameLen = 255;
    DWORD dwType = 0;

    RootKey = HKEY_LOCAL_MACHINE;
    lpSubKey = Is64() ? Win64PATH : Win32PATH;

    lReturn = RegOpenKeyEx(RootKey, lpSubKey, 0, 
        KEY_ALL_ACCESS, &hkResult);
    if (lReturn == ERROR_SUCCESS)
    {
        DWORD index = 0;
        DWORD ListIndex = 0;
		TCHAR szIDbuf[32]={0};
        while (ERROR_NO_MORE_ITEMS != 
            RegEnumKeyEx(hkResult, index, szKeyName, &dwKeyLen, 
            0, NULL, NULL, NULL))
        {
            strBuffer.Format(_T("%s"), szKeyName);
            if (!strBuffer.IsEmpty())
            {
                strMidReg = (CString)lpSubKey + _T("\\") + strBuffer;

                if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strMidReg, 
                    0, KEY_ALL_ACCESS, &hkRKey) == ERROR_SUCCESS)
                {
                    RegQueryValueEx(hkRKey, _T("DisplayName"), 
                        0, &dwType, (LPBYTE)stcSoftInfo.szSoftName, &dwNameLen);
                    dwNameLen = 255;

                    RegQueryValueEx(hkRKey, _T("DisplayVersion"), 
                        0, &dwType, (LPBYTE)stcSoftInfo.szSoftVer, &dwNameLen);
                    dwNameLen = 255;

                    RegQueryValueEx(hkRKey, _T("InstallLocation"), 
                        0, &dwType, (LPBYTE)stcSoftInfo.strSoftInsPath, &dwNameLen);
                    dwNameLen = 255;

                    RegQueryValueEx(hkRKey, _T("Publisher"), 
                        0, &dwType, (LPBYTE)stcSoftInfo.strSoftVenRel, &dwNameLen);
                    dwNameLen = 255;

                    RegQueryValueEx(hkRKey, _T("UninstallString"), 
                        0, &dwType, (LPBYTE)stcSoftInfo.strSoftUniPath, &dwNameLen);
                    dwNameLen = 255;

                    RegQueryValueEx(hkRKey, _T("InstallDate"),
                        0, &dwType, (LPBYTE)stcSoftInfo.szSoftData, &dwNameLen);
                    if (stcSoftInfo.szSoftData[0])
                    {
                        stcSoftInfo.szSoftData[9] = stcSoftInfo.szSoftData[7];
                        stcSoftInfo.szSoftData[8] = stcSoftInfo.szSoftData[6];
                        stcSoftInfo.szSoftData[7] = '-';
                        stcSoftInfo.szSoftData[6] = stcSoftInfo.szSoftData[5];
                        stcSoftInfo.szSoftData[5] = stcSoftInfo.szSoftData[4];
                        stcSoftInfo.szSoftData[4] = '-';
                    }
                    dwNameLen = 255;

                    //保存数据
                    if (stcSoftInfo.szSoftName[0]!='\0')
                    {
                        m_vecSoftInfo.push_back(stcSoftInfo);

						_itot(ListIndex+1,szIDbuf,10);
                        m_ControlList.AddItem(ListIndex++, 5,
							szIDbuf,
                            stcSoftInfo.szSoftName,
                            stcSoftInfo.strSoftVenRel,
                            stcSoftInfo.szSoftData,
                            stcSoftInfo.szSoftVer);
                    }

                    index++;
                }
                dwKeyLen = 255;
                memset(szKeyName, 0, 255);
                memset(&stcSoftInfo, 0, sizeof(SOFTINFO));
            }
        }
        RegCloseKey(hkResult);
    }
}


//************************************************************
// 函数名称:    Is64
// 函数说明:    判断系统是否是64位
// 作    者:    cyxvc
// 时    间:    2015/11/10
// 返 回    值:    BOOL
//************************************************************
BOOL CSoftWareManagerDlg::Is64()
{
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
        si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_IA64)
    {
        //64 位操作系统
        return TRUE;
    }
    else
    {
        // 32 位操作系统
        return FALSE;
    }
    return FALSE;
}

//************************************************************
// 函数名称:    On32776
// 函数说明:    打开文件所在位置
// 作    者:    cyxvc
// 时    间:    2015/11/10
// 返 回    值:    void
//************************************************************
BOOL OpenFolderAndSelectFile(TCHAR* szPath)
{
	BOOL bRet = FALSE;
	::CoInitialize(NULL);
	do
	{
		LPSHELLFOLDER pDesktopFolder;
		if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
		{
			LPITEMIDLIST     pidl;
			ULONG             chEaten;
			ULONG             dwAttributes;
			HRESULT             hr;

			hr = pDesktopFolder->ParseDisplayName(
				NULL, NULL, szPath, &chEaten, &pidl, &dwAttributes);
			if (FAILED(hr))
			{
				pDesktopFolder->Release();
				break;;
			}
			LPCITEMIDLIST pidlFolder = pidl;

			hr = SHOpenFolderAndSelectItems(pidl, 0, NULL, 0);
			pDesktopFolder->Release();
			if (hr == S_OK)
			{
				bRet = TRUE;
			}
		}
	}
	while(0);
	::CoUninitialize();
    return bRet;
}