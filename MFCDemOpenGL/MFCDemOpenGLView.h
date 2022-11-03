
// MFCDemOpenGLView.h: CMFCDemOpenGLView 类的接口
//

#pragma once


class CMFCDemOpenGLView : public CView
{
protected: // 仅从序列化创建
	CMFCDemOpenGLView() noexcept;
	DECLARE_DYNCREATE(CMFCDemOpenGLView)

// 特性
public:
	CMFCDemOpenGLDoc* GetDocument() const;

// 操作
public:
	BOOL IsTextureDEM() const { return IfTexture; }
	CString GetTextureFile() const { return m_texFileName; }

protected:
	CClientDC* m_pDC;
	CRect m_oldRect;
	// 规范化成员变量命名：m_iOldRect、m_bIsFilled，即m+_+类型首字母+名字

	GLfloat yaw_z, roll_x, pitch_y; // 旋转角
	GLfloat scale_z; // 高度比缩放参数
	GLfloat eye_z; // 视点高度
	GLfloat center_x; // 物体中心点X坐标

	BOOL m_play;
	BOOL IsFilled; // 是否填充三角形
	BOOL IfTexture; // 纹理
	BOOL IsPerspective; // 透视投影还是正射投影

	CString m_texFileName;

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMFCDemOpenGLView();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRotatez();
	afx_msg void OnRotateyu();
	afx_msg void OnRotateya();
	afx_msg void OnRotatexu();
	afx_msg void OnRotatexa();
	afx_msg void OnRight();
	afx_msg void OnLeft();
	afx_msg void OnHeighten();
	afx_msg void OnShorten();
	afx_msg void OnZoomin();
	afx_msg void OnZoomout();
	afx_msg void OnTexture();
	afx_msg void OnFill();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnProjection();
};

#ifndef _DEBUG  // MFCDemOpenGLView.cpp 中的调试版本
inline CMFCDemOpenGLDoc* CMFCDemOpenGLView::GetDocument() const
   { return reinterpret_cast<CMFCDemOpenGLDoc*>(m_pDocument); }
#endif

