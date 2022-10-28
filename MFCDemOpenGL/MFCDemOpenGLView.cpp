
// MFCDemOpenGLView.cpp: CMFCDemOpenGLView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCDemOpenGL.h"
#endif

#include "MFCDemOpenGLDoc.h"
#include "MFCDemOpenGLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCDemOpenGLView

IMPLEMENT_DYNCREATE(CMFCDemOpenGLView, CView)

BEGIN_MESSAGE_MAP(CMFCDemOpenGLView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_ROTATEZ, OnRotatez)
	ON_COMMAND(ID_ROTATEYU, OnRotateyu)
	ON_COMMAND(ID_ROTATEYA, OnRotateya)
	ON_COMMAND(ID_ROTATEXU, OnRotatexu)
	ON_COMMAND(ID_ROTATEXA, OnRotatexa)
	ON_COMMAND(ID_RIGHT, OnRight)
	ON_COMMAND(ID_LEFT, OnLeft)
	ON_COMMAND(ID_HEIGHTEN, OnHeighten)
	ON_COMMAND(ID_SHORTEN, OnShorten)
	ON_COMMAND(ID_ZOOMIN, OnZoomin)
	ON_COMMAND(ID_ZOOMOUT, OnZoomout)
	ON_COMMAND(ID_TEXTURE, OnTexture)
	ON_COMMAND(ID_FILL, OnFill)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_PROJECTION, OnProjection)
END_MESSAGE_MAP()

// CMFCDemOpenGLView 构造/析构

CMFCDemOpenGLView::CMFCDemOpenGLView() noexcept
{
	// TODO: 在此处添加构造代码

	m_pDC = NULL;
	eyez = 4000.0f;
	pitch_y = 0.0f;
	roll_x = -60.0f;
	yaw_z = 0.0f;
	scale_z = 1.0f;
	centerx = 0.0f;

	m_play = FALSE;
	IfFill = false;
	IfTexture = FALSE;
	IsPerspective = true;
}

CMFCDemOpenGLView::~CMFCDemOpenGLView()
{
}

void CMFCDemOpenGLView::DrawScene()
{
	glClearColor(0.6f, 0.6f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0f, 1.0f, 1.0f, 10000.0f);

	// 透视投影gluPerspective or 正射投影glOrtho
	if (IsPerspective) {
		gluPerspective(45.0f, 1.0f, 1.0f, 10000.0f);
	}
	else if (!IsPerspective) {
		glOrtho(0.0f, 1000.0f, 0.0f, 1000.0f, 1.0f, 10000.0f); // 参数可能还需要再调一下
	}

	glMatrixMode(GL_MODELVIEW); // 设置模型视图矩阵
	glLoadIdentity();

	glDrawBuffer(GL_BACK);

	glPushMatrix();
	glLoadIdentity();
	gluLookAt(1.0f, 1.0f, eyez, centerx, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);	// This Determines Where The Camera's Position And View Is
	glRotatef(roll_x, 1.0f, 0.0f, 0.0f);//	
	glRotatef(yaw_z, 0.0f, 0.0f, 1.0f);//
	glRotatef(pitch_y, 0.0f, 1.0f, 0.0f);//

	glScalef(1.0f, 1.0f, scale_z);

	CMFCDemOpenGLDoc* pDoc = GetDocument();
	if (pDoc->DataExist)
	{
		if (IfFill)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		pDoc->DrawTriangles();
	}

	glPopMatrix();
	glFlush();
}

BOOL CMFCDemOpenGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	return CView::PreCreateWindow(cs);
}

// CMFCDemOpenGLView 绘图

void CMFCDemOpenGLView::OnDraw(CDC* pDC)
{
	CMFCDemOpenGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	DrawScene();
	// 交互缓冲区
	SwapBuffers(pDC->m_hDC);

}


// CMFCDemOpenGLView 打印

BOOL CMFCDemOpenGLView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCDemOpenGLView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCDemOpenGLView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMFCDemOpenGLView 诊断

#ifdef _DEBUG
BOOL CMFCDemOpenGLView::bSetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		PFD_DOUBLEBUFFER |
		PFD_SUPPORT_OPENGL,			    // support OpenGL
		PFD_TYPE_RGBA,                  // RGBA type
		24,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		32,                             // 32-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};
	int pixelformat;

	if ((pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)) == 0)
	{
		MessageBox(_T("ChoosePixelFormat failed"));
		return FALSE;
	}

	if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
	{
		MessageBox(_T("SetPixelFormat failed"));
		return FALSE;
	}

	return TRUE;
}

void CMFCDemOpenGLView::Init()
{
	PIXELFORMATDESCRIPTOR pfd;
    int n;
	HGLRC hrc;

    m_pDC = new CClientDC(this);

    ASSERT(m_pDC != NULL);

    if (!bSetupPixelFormat())
        return;

    n =::GetPixelFormat(m_pDC->GetSafeHdc());
    ::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

    hrc = wglCreateContext(m_pDC->GetSafeHdc());
    wglMakeCurrent(m_pDC->GetSafeHdc(), hrc);

//     GetClientRect(&m_oldRect);

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat m_light0Ambient[4]={0.6f,0.6f,0.6f,0.8f};
	GLfloat m_light0Diffuse[4]={0.6f,0.7f,0.6f,0.6f};
	GLfloat m_light0Pos[4]={1.0f,1.0f,1.0f,0.0f};

	glLightfv(GL_LIGHT0,GL_AMBIENT,m_light0Ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,m_light0Diffuse);
    glLightfv(GL_LIGHT0,GL_POSITION,m_light0Pos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void CMFCDemOpenGLView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCDemOpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCDemOpenGLDoc* CMFCDemOpenGLView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCDemOpenGLDoc)));
	return (CMFCDemOpenGLDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCDemOpenGLView 消息处理程序


void CMFCDemOpenGLView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	GetParent()->ShowWindow(SW_SHOWMAXIMIZED);
}


int CMFCDemOpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	Init();

	SetTimer(1, 50, 0);

	return 0;
}


void CMFCDemOpenGLView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	HGLRC hrc;

	hrc = wglGetCurrentContext();
	wglMakeCurrent(NULL, NULL);
	if (hrc)
		wglDeleteContext(hrc);
	if (m_pDC)
		delete m_pDC;

	KillTimer(1);
}


void CMFCDemOpenGLView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (cy > 0)
	{
		//		if((m_oldRect.right > cx) || (m_oldRect.bottom > cy))
		//            RedrawWindow();
		//
		//      m_oldRect.right  = cx ;
		//      m_oldRect.bottom = cy ;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glViewport(0, 0, cx, cy);
	}
}


void CMFCDemOpenGLView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nChar)
	{
	case VK_SPACE:
	{
		m_play = !m_play;
		Invalidate(FALSE);
		break;
	}
	case VK_DOWN:
	{
		eyez /= 1.2f;
		Invalidate(FALSE);		break;
	}

	case VK_UP:
	{
		eyez *= 1.2f;
		Invalidate(FALSE);		break;
	}

	case VK_LEFT:
	{
		centerx += 20.f;
		Invalidate(FALSE);      break;
	}
	case VK_RIGHT:
	{
		centerx -= 20.f;
		Invalidate(FALSE);      break;
	}
	case VK_HOME:
	{
		pitch_y += -5.0f;
		Invalidate(FALSE);		break;
	}
	case VK_END:
	{
		pitch_y -= -5.0f;
		Invalidate(FALSE);		break;
	};
	case VK_PRIOR:
	{
		scale_z *= 1.2f;
		Invalidate(FALSE);		break;
	}
	case VK_NEXT:
	{
		scale_z /= 1.2f;
		Invalidate(FALSE);		break;
	}
	case VK_INSERT:
	{
		roll_x -= -5.0f;
		Invalidate(FALSE);		break;
	}
	case VK_DELETE:
	{
		roll_x += -5.0f;
		Invalidate(FALSE);		break;
	}
	default:break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMFCDemOpenGLView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_play)
	{
		yaw_z += 1.0f;
		if (yaw_z >= 360.0f)
			yaw_z = 0.0f;
		Invalidate(FALSE);
	}

	CView::OnTimer(nIDEvent);
}

void CMFCDemOpenGLView::OnRotatez()
{
	// 绕Z轴旋转
	m_play = !m_play;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnRotateyu()
{
	// 绕Y轴逆时针旋转
	pitch_y -= -5.0f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnRotateya()
{
	// 绕Y轴顺时针旋转
	pitch_y += -5.0f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnRotatexu()
{
	// 绕X轴逆时针旋转
	roll_x -= -5.0f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnRotatexa()
{
	// 绕X轴顺时针旋转
	roll_x += -5.0f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnRight()
{
	//右移
	centerx -= 20.f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnLeft()
{
	// 左移
	centerx += 20.f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnHeighten()
{
	// 增加高度比
	scale_z *= 1.2f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnShorten()
{
	// 减小高度比
	scale_z /= 1.2f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnZoomin()
{
	// 放大
	eyez /= 1.2f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnZoomout()
{
	// 缩小
	eyez *= 1.2f;
	Invalidate(FALSE);
}

void CMFCDemOpenGLView::OnTexture()
{
	// 贴加纹理
	if (!IfTexture)
	{
		static char BASED_CODE szFilter[] = "BMP Files (*.bmp)|*.bmp||All Files (*.*)|*.*||";
		CFileDialog dlg(TRUE, "*.bmp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL); // 设置字符为双字节
		if (dlg.DoModal() == IDOK)
		{
			m_texFileName = dlg.GetPathName();
			IfTexture = !IfTexture;
			Invalidate();
		}
		else
		{
			AfxMessageBox(_T("请重新选择纹理文件！"));
		}
	}
}

void CMFCDemOpenGLView::OnFill()
{
	// 填充三角形
	IfFill = !IfFill;
	Invalidate(FALSE);
}


BOOL CMFCDemOpenGLView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 鼠标滚轮控制缩放，解决屏幕频繁重绘闪烁，用双缓冲+invalidate的false参数
	if (zDelta > 0)
	{
		eyez /= 1.2f;
		Invalidate(FALSE);
	}
	if (zDelta < 0)
	{
		eyez *= 1.2f;
		Invalidate(FALSE);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMFCDemOpenGLView::OnProjection()
{
	// 改变投影模式，实现正射投影和透射投影（默认）的切换
	IsPerspective = !IsPerspective;
	Invalidate(FALSE);
}
