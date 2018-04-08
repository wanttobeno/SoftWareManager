
// SoftWareManagerDlg.h : 头文件
//

#pragma once

#include "FastListCtrl.h"
#include <vector>
using namespace std;
//软件信息结构体
typedef struct _SOFTINFO
{
	TCHAR szSoftName[50];                //软件名称
	TCHAR szSoftVer[50];                //软件版本号
	TCHAR strSoftVenRel[50];            //软件发布厂商
	TCHAR szSoftData[20];                //软件安装日期
	TCHAR strSoftInsPath[MAX_PATH];        //软件安装路径
	TCHAR strSoftUniPath[MAX_PATH];        //软件卸载路径
}SOFTINFO, *PSOFTINFO;
#define Win32PATH _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
#define Win64PATH _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")


// CSoftWareManagerDlg 对话框
class CSoftWareManagerDlg : public CDialog
{
// 构造
public:
	CSoftWareManagerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SOFTWAREMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	CFastListCtrl m_ControlList;
	//所选则的软件
	int m_Select;
	CString strTemp;
	//保存软件相关信息
	vector<SOFTINFO> m_vecSoftInfo;

	BOOL CSoftWareManagerDlg::Is64();
	afx_msg void OnBnClickedBtnFolder();
	afx_msg void OnBnClickedBtnUninstall();
	afx_msg void OnBnClickedBtnRefresh();
	void CSoftWareManagerDlg::GetSoftwareInfo();
};
