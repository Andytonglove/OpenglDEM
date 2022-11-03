
// MFCDemOpenGLDoc.cpp: CMFCDemOpenGLDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCDemOpenGL.h"
#endif

#include "MFCDemOpenGLDoc.h"
#include "MFCDemOpenGLView.h"  // 方便后续调用view类
#include <math.h>

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCDemOpenGLDoc

IMPLEMENT_DYNCREATE(CMFCDemOpenGLDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFCDemOpenGLDoc, CDocument)
END_MESSAGE_MAP()


// CMFCDemOpenGLDoc 构造/析构

CMFCDemOpenGLDoc::CMFCDemOpenGLDoc() noexcept
{
	// TODO: 在此添加一次性构造代码

	DataExist = FALSE;

	width = 0;
	height = 0;
	m_pTextureBits = NULL;

	demX = NULL;
	demY = NULL;
	demZ = NULL;
	nvs = NULL;
	column = 0;
	row = 0;
	noData = -9999.0;
}

CMFCDemOpenGLDoc::~CMFCDemOpenGLDoc()
{
	if (demX != NULL)	delete[]demX;
	if (demY != NULL)	delete[]demY;
	if (demY != NULL)	delete[]demZ;
	if (nvs != NULL)	delete[]nvs;
}

BOOL CMFCDemOpenGLDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMFCDemOpenGLDoc 序列化

void CMFCDemOpenGLDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

BOOL CMFCDemOpenGLDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CString strFilePath, strFileType;
	strFilePath.Format(_T("%s"), lpszPathName);

	int nIndex = strFilePath.ReverseFind('.');
	strFileType = strFilePath.Right(strFilePath.GetLength() - nIndex - 1);
	strFileType.MakeUpper();

	if (strFileType == "GRD")
	{
		return ReadGRDFile(lpszPathName);
	}
	else if (strFileType == "DAT")
	{
		return ReadDEMFile(lpszPathName);
	}
	else if (strFileType == "ASC")
	{
		return ReadDEMFile(lpszPathName);
	}
	else
	{
		MessageBox(NULL, _T("不支持的文件格式！目前仅支持GRD/DAT/ASC文件格式！"), _T("系统信息"), MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
	//return 0;
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMFCDemOpenGLDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMFCDemOpenGLDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMFCDemOpenGLDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMFCDemOpenGLDoc 诊断

#ifdef _DEBUG
void CMFCDemOpenGLDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCDemOpenGLDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFCDemOpenGLDoc 命令

BOOL CMFCDemOpenGLDoc::ReadDEMFile(CString fileName)
{
	DataExist = TRUE;

	if (demX != NULL)	delete[]demX;
	if (demY != NULL)	delete[]demY;
	if (demY != NULL)	delete[]demZ;
	if (nvs != NULL)	delete[]nvs;

	FILE* fp;

	//USES_CONVERSION;
	//char * pFileName = W2A(fileName); //在Unicode而非多字节情况下，需要调用函数T2A和W2A,支持ATL和MFC中的字符转换

	if ((fp = fopen(fileName, "r")) == NULL)
	{
		AfxMessageBox(_T("open DEM file error!"));
		return FALSE;
	}
	//fscanf(fp, "%f%f%d%d%f%f", &minX, &minY, &row, &column, &cellx, &celly);//&cell

	// 读取文件头
	CString strtmp;
	fscanf(fp, "%s%d\n%s%d\n%s%f\n%s%f\n%s%f\n%s%f\n", strtmp, &column, strtmp, &row, strtmp, &minX, strtmp, &minY, strtmp, &cellx, strtmp, &noData);
	celly = cellx; // 对于这个DEM是相同的
	
	demX = new GLfloat[column * row];
	demY = new GLfloat[column * row];
	demZ = new GLfloat[column * row];
	nvs = new Vector[(column - 1) * (row - 1) * 2];

	GLfloat z;
	for (int j = 0;j < row;j++)
		for (int i = 0;i < column;i++)
		{
			demX[j * column + i] = minX + i * cellx;
			demY[j * column + i] = minY + j * celly;

			fscanf(fp, "%f", &z);
			demZ[j * column + i] = z;

			if (i == 0 && j == 0)
			{
				minZ = demZ[j * column + i];
				maxZ = demZ[j * column + i];

				maxX = minX = demX[j * column + i];
				maxY = minY = demY[j * column + i];
			}
			else
			{
				if (demZ[j * column + i] > maxZ)	maxZ = demZ[j * column + i];
				if (demZ[j * column + i] < minZ)	minZ = demZ[j * column + i];
			}
		}

	fclose(fp);

	maxX = demX[column - 1];
	maxY = demY[(row - 1) * column];

	Xc = (maxX + minX) / 2;
	Yc = (maxY + minY) / 2;
	Zc = (maxZ + minZ) / 2;

	Centerlize();
	CalNormals();

	return TRUE;
}

BOOL CMFCDemOpenGLDoc::ReadGRDFile(CString fileName)
{
	DataExist = TRUE;

	if (demX != NULL)	delete[]demX;
	if (demY != NULL)	delete[]demY;
	if (demY != NULL)	delete[]demZ;
	if (nvs != NULL)	delete[]nvs;

	FILE* fp;
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		AfxMessageBox(_T("打开 DEM 文件失败!"));
		return FALSE;
	}

	char tmp[50];
	
	// 依次读入信息
	fscanf(fp, "%s", tmp);
	fscanf(fp, "%s", tmp);
	column = atoi(tmp);

	fscanf(fp, "%s", tmp);
	row = atoi(tmp);

	fscanf(fp, "%s", tmp);
	minX = (float)atof(tmp);

	fscanf(fp, "%s", tmp);
	maxX = (float)atof(tmp);

	fscanf(fp, "%s", tmp);
	minY = (float)atof(tmp);

	fscanf(fp, "%s", tmp);
	maxY = (float)atof(tmp);

	fscanf(fp, "%s", tmp);
	minZ = (float)atof(tmp);

	fscanf(fp, "%s", tmp);
	maxZ = (float)atof(tmp);

	cellx = (maxX - minX) / (column - 1);  // X向间距（横向）
	celly = (maxY - minY) / (row - 1);     // Y向间距（纵向）

	demX = new GLfloat[column * row];
	demY = new GLfloat[column * row];
	demZ = new GLfloat[column * row];
	nvs = new Vector[(column - 1) * (row - 1) * 2];

	GLfloat z;
	for (int j = 0;j < row;j++)
		for (int i = 0;i < column;i++)
		{
			demX[j * column + i] = minX + i * cellx;
			demY[j * column + i] = minY + j * celly;

			fscanf(fp, "%f", &z);
			demZ[j * column + i] = z;
		}

	fclose(fp);

	Xc = (maxX + minX) / 2;
	Yc = (maxY + minY) / 2;
	Zc = (maxZ + minZ) / 2;

	Centerlize();
	CalNormals();

	return TRUE;
}

void CMFCDemOpenGLDoc::Centerlize()
{
	for (int j = 0;j < row;j++)
		for (int i = 0;i < column;i++)
		{
			demX[j * column + i] = demX[j * column + i] - Xc;
			demY[j * column + i] = demY[j * column + i] - Yc;
			demZ[j * column + i] = demZ[j * column + i] - Zc;
		}

	maxZ -= Zc;	minZ -= Zc;
}

void CMFCDemOpenGLDoc::GetTriNormal(GLfloat* vertex1, GLfloat* vertex2, GLfloat* vertex3, GLfloat* normal)
{
	// 计算三角形的法向量
	GLfloat vector1[3], vector2[3];

	vector1[0] = vertex2[0] - vertex1[0];//X
	vector1[1] = vertex2[1] - vertex1[1];//Y
	vector1[2] = vertex2[2] - vertex1[2];//Z

	vector2[0] = vertex3[0] - vertex1[0];//X
	vector2[1] = vertex3[1] - vertex1[1];//Y
	vector2[2] = vertex3[2] - vertex1[2];//Z

	// 求向量vector1和vector2叉乘
	normal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
	normal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
	normal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];

	GLfloat normalLength;
	normalLength = (GLfloat)sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	if (normalLength != 0.0)
	{
		normal[0] /= normalLength;
		normal[1] /= normalLength;
		normal[2] /= normalLength;
	}
}

void CMFCDemOpenGLDoc::CalNormals()
{
	GLfloat vertex1[3], vertex2[3], vertex3[3], normal[3];
	GLint i, j;

	for (i = 0;i < row - 1;i++)
		for (j = 0;j < column - 1;j++)
		{
			// 将每个DEM网格分成左右两个三角形
			vertex1[0] = demX[(i + 1) * column + j];
			vertex1[1] = demY[(i + 1) * column + j];
			vertex1[2] = demZ[(i + 1) * column + j];

			vertex2[0] = demX[i * column + j];
			vertex2[1] = demY[i * column + j];
			vertex2[2] = demZ[i * column + j];

			vertex3[0] = demX[i * column + j + 1];
			vertex3[1] = demY[i * column + j + 1];
			vertex3[2] = demZ[i * column + j + 1];

			GetTriNormal(vertex1, vertex2, vertex3, normal);
			nvs[(i * (column - 1) + j) * 2 + 0].X = normal[0];
			nvs[(i * (column - 1) + j) * 2 + 0].Y = normal[1];
			nvs[(i * (column - 1) + j) * 2 + 0].Z = normal[2];

			vertex1[0] = demX[(i + 1) * column + j];
			vertex1[1] = demY[(i + 1) * column + j];
			vertex1[2] = demZ[(i + 1) * column + j];

			vertex2[0] = demX[i * column + j + 1];
			vertex2[1] = demY[i * column + j + 1];
			vertex2[2] = demZ[i * column + j + 1];

			vertex3[0] = demX[(i + 1) * column + j + 1];
			vertex3[1] = demY[(i + 1) * column + j + 1];
			vertex3[2] = demZ[(i + 1) * column + j + 1];

			GetTriNormal(vertex1, vertex2, vertex3, normal);
			nvs[(i * (column - 1) + j) * 2 + 1].X = normal[0];
			nvs[(i * (column - 1) + j) * 2 + 1].Y = normal[1];
			nvs[(i * (column - 1) + j) * 2 + 1].Z = normal[2];
		}
}

void CMFCDemOpenGLDoc::DrawTriangles()
{
	// 需要加入按比例画一定数量的三角形函数，如果视点远/距离远就画三角形稀疏一些，效率更好 TODO曲面细分
	GLint i, j;
	GLfloat normal[3];

	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	POSITION pos = GetFirstViewPosition();
	
	CMFCDemOpenGLView* pView = (CMFCDemOpenGLView*)GetNextView(pos);
	ASSERT(pView != NULL);

	if (pView->IsTextureDEM())
	{
		if (m_pTextureBits == NULL)
		{
			CString m_texFileName = pView->GetTextureFile();
			ReadTextureFile(m_texFileName);
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// 定义纹理
		glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_pTextureBits);//

		// 控制纹理
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// 说明贴图方式
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		// 启动纹理贴图
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);

		for (i = 0;i < row - 1;i++)
			for (j = 0;j < column - 1;j++)
			{
				glBegin(GL_TRIANGLES);

				normal[0] = nvs[(i * (column - 1) + j) * 2 + 0].X;
				normal[1] = nvs[(i * (column - 1) + j) * 2 + 0].Y;
				normal[2] = nvs[(i * (column - 1) + j) * 2 + 0].Z;
				glNormal3fv(normal);

				glTexCoord2f(j * 1.0f / column, (i + 1) * 1.0f / row);
				glVertex3f(demX[(i + 1) * column + j], demY[(i + 1) * column + j], demZ[(i + 1) * column + j]);

				glTexCoord2f(j * 1.0f / column, i * 1.0f / row);
				glVertex3f(demX[i * column + j], demY[i * column + j], demZ[i * column + j]);

				glTexCoord2f((j + 1) * 1.0f / column, i * 1.0f / row);
				glVertex3f(demX[i * column + j + 1], demY[i * column + j + 1], demZ[i * column + j + 1]);

				normal[0] = nvs[(i * (column - 1) + j) * 2 + 1].X;
				normal[1] = nvs[(i * (column - 1) + j) * 2 + 1].Y;
				normal[2] = nvs[(i * (column - 1) + j) * 2 + 1].Z;
				glNormal3fv(normal);

				glTexCoord2f(j * 1.0f / column, (i + 1) * 1.0f / row);
				glVertex3f(demX[(i + 1) * column + j], demY[(i + 1) * column + j], demZ[(i + 1) * column + j]);

				glTexCoord2f((j + 1) * 1.0f / column, i * 1.0f / row);
				glVertex3f(demX[i * column + j + 1], demY[i * column + j + 1], demZ[i * column + j + 1]);

				glTexCoord2f((j + 1) * 1.0f / column, (i + 1) * 1.0f / row);
				glVertex3f(demX[(i + 1) * column + j + 1], demY[(i + 1) * column + j + 1], demZ[(i + 1) * column + j + 1]);

				glEnd();
			}
		glFlush();
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		// 画三角形
		for (i = 0;i < row - 1;i++)
			for (j = 0;j < column - 1;j++)
			{
				glBegin(GL_TRIANGLES);
				glColor3f(0.2f, 1.0f, 0.2f);

				normal[0] = nvs[(i * (column - 1) + j) * 2 + 0].X;
				normal[1] = nvs[(i * (column - 1) + j) * 2 + 0].Y;
				normal[2] = nvs[(i * (column - 1) + j) * 2 + 0].Z;
				glNormal3fv(normal);

				glVertex3f(demX[(i + 1) * column + j], demY[(i + 1) * column + j], demZ[(i + 1) * column + j]);
				glVertex3f(demX[i * column + j], demY[i * column + j], demZ[i * column + j]);
				glVertex3f(demX[i * column + j + 1], demY[i * column + j + 1], demZ[i * column + j + 1]);

				normal[0] = nvs[(i * (column - 1) + j) * 2 + 1].X;
				normal[1] = nvs[(i * (column - 1) + j) * 2 + 1].Y;
				normal[2] = nvs[(i * (column - 1) + j) * 2 + 1].Z;
				glNormal3fv(normal);

				glVertex3f(demX[(i + 1) * column + j], demY[(i + 1) * column + j], demZ[(i + 1) * column + j]);
				glVertex3f(demX[i * column + j + 1], demY[i * column + j + 1], demZ[i * column + j + 1]);
				glVertex3f(demX[(i + 1) * column + j + 1], demY[(i + 1) * column + j + 1], demZ[(i + 1) * column + j + 1]);

				glEnd();
			}
	}

	// 画四边
	GLfloat baseheight = 0;//50.0f;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	j = column - 1;
	for (i = 0;i < row - 1;i++)
	{
		glBegin(GL_QUADS);
		glColor3f(0.9f, 0.9f, 0.9f);

		glVertex3f(demX[i * column + j], demY[i * column + j], demZ[i * column + j]);
		glVertex3f(demX[i * column + j], demY[i * column + j], minZ - baseheight);
		glVertex3f(demX[(i + 1) * column + j], demY[(i + 1) * column + j], minZ - baseheight);
		glVertex3f(demX[(i + 1) * column + j], demY[(i + 1) * column + j], demZ[(i + 1) * column + j]);

		glEnd();
	}
	j = 0;
	for (i = 0;i < row - 1;i++)
	{
		glBegin(GL_QUADS);
		glColor3f(0.9f, 0.9f, 0.9f);

		glVertex3f(demX[i * column + j], demY[i * column + j], demZ[i * column + j]);
		glVertex3f(demX[i * column + j], demY[i * column + j], minZ - baseheight);
		glVertex3f(demX[(i + 1) * column + j], demY[(i + 1) * column + j], minZ - baseheight);
		glVertex3f(demX[(i + 1) * column + j], demY[(i + 1) * column + j], demZ[(i + 1) * column + j]);

		glEnd();
	}
	i = row - 1;
	for (j = 0;j < column - 1;j++)
	{
		glBegin(GL_QUADS);
		glColor3f(0.9f, 0.9f, 0.9f);

		glVertex3f(demX[i * column + j], demY[i * column + j], demZ[i * column + j]);
		glVertex3f(demX[i * column + j], demY[i * column + j], minZ - baseheight);
		glVertex3f(demX[i * column + j + 1], demY[i * column + j + 1], minZ - baseheight);
		glVertex3f(demX[i * column + j + 1], demY[i * column + j + 1], demZ[i * column + j + 1]);

		glEnd();
	}
	i = 0;
	for (j = 0;j < column - 1;j++)
	{
		glBegin(GL_QUADS);
		glColor3f(0.9f, 0.9f, 0.9f);

		glVertex3f(demX[i * column + j], demY[i * column + j], demZ[i * column + j]);
		glVertex3f(demX[i * column + j], demY[i * column + j], minZ - baseheight);
		glVertex3f(demX[i * column + j + 1], demY[i * column + j + 1], minZ - baseheight);
		glVertex3f(demX[i * column + j + 1], demY[i * column + j + 1], demZ[i * column + j + 1]);

		glEnd();
	}

	glFlush();
}

void CMFCDemOpenGLDoc::ReadTextureFile(CString m_texFileName)
{
	// 读取纹理数据CDIB TODO

}
