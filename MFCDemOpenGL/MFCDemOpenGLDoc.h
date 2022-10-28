
// MFCDemOpenGLDoc.h: CMFCDemOpenGLDoc 类的接口
//

#pragma once


struct Vector
{
	GLfloat X;
	GLfloat Y;
	GLfloat Z;
};

class CMFCDemOpenGLDoc : public CDocument
{
protected: // 仅从序列化创建
	CMFCDemOpenGLDoc() noexcept;
	DECLARE_DYNCREATE(CMFCDemOpenGLDoc)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
protected:
	GLfloat* dtmX, * dtmY, * dtmZ;
	int column, row;
	//GLfloat cell;
	GLfloat cellx, celly;
	GLfloat maxX, maxY, maxZ, minX, minY, minZ, Xc, Yc, Zc;
	GLfloat noData;
	Vector* nvs;//所有的法向量

	GLint width, height;
	GLubyte* m_pTextureBits;

public:
	BOOL DataExist;
	BOOL ReadDTMFile(CString fileName);
	BOOL ReadGRDFile(CString fileName);
	void Centerlize(); //坐标中心化
	void GetTriNormal(GLfloat* vertex1, GLfloat* vertex2, GLfloat* vertex3, GLfloat* normal); //计算三角形的法向量
	void CalNormals();
	void DrawTriangles();
	void ReadTextureFile(CString m_texFile);

public:
	virtual ~CMFCDemOpenGLDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
