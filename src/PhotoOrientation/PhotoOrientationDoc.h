
// PhotoOrientationDoc.h : CPhotoOrientationDoc 类的接口
//


#pragma once
#include "IPhBaseDef.h"
#include <vector>
using namespace std;

class CPhotoOrientationDoc : public CDocument
{
protected: // 仅从序列化创建
	CPhotoOrientationDoc();
	DECLARE_DYNCREATE(CPhotoOrientationDoc)

// 属性
public:
	

private:
	vector <POSData_ALS50 > m_vPosData;
	iphCamera						m_camera;

	vector<string> m_ImgNameList;	
	vector<string> m_keyNameList;

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CPhotoOrientationDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnImportrawpos();
	afx_msg void OnExportiop();
	afx_msg void OnExportaop();
	afx_msg void OnUpdateExportiop(CCmdUI *pCmdUI);
	afx_msg void OnUpdateExportaop(CCmdUI *pCmdUI);
	afx_msg void OnTplane();
	afx_msg void OnRoughorientation();
};


