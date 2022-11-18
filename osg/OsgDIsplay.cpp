// OsgDIsplay.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <osg/Node>
#include <osgViewer/Viewer>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <osg/MatrixTransform>
#include <osg/StateSet>
#include <osgGA/CameraManipulator>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventHandler>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgFX/scribe>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>

#pragma comment(lib,"osgd.lib")
#pragma comment(lib,"osgviewerd.lib")
#pragma comment(lib,"osgFXd.lib")
#pragma comment(lib,"osggad.lib")
#pragma comment(lib,"osgUtild.lib")
#pragma comment(lib,"osgdbd.lib")

//平移，数值可修改
#define X 800
#define Y 800
#define Z 4200

using namespace std;
using namespace osg;
using namespace osgViewer;


//拾取类
class nodePick :public osgGA::GUIEventHandler
{
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*> (&aa);
		//Ctrl+鼠标左键
		if (ea.getEventType() != osgGA::GUIEventAdapter::RELEASE
			|| ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON
			|| !(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL)) return false;

		else {
			osgUtil::LineSegmentIntersector::Intersections intersections;
			osg::ref_ptr<osg::Node> node = new osg::Node();
			osg::ref_ptr<osg::Group> parent = new osg::Group();
			osg::ref_ptr<osg::Group> group0 = dynamic_cast<osg::Group*>
				(viewer->getSceneData()->asGroup()->getChild(0));
			osg::ref_ptr<osg::Group> group1 = dynamic_cast<osg::Group*>
				(viewer->getSceneData()->asGroup()->getChild(1));
			if (viewer->computeIntersections(ea.getX(), ea.getY(), intersections))
			{
				//得到选择的节点
				osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
				osg::NodePath& nodePath = intersection.nodePath;
				node = nodePath.back();

				osg::ref_ptr<osg::Group> group = dynamic_cast<osg::Group*>(nodePath[2]);
				if (group == group0 || group == group1)
				{
					//点击节点出白边
					//当前选择节点的父节点
					parent = dynamic_cast<osg::Group*> (nodePath[nodePath.size() - 2]);
					osgFX::Scribe* ot = dynamic_cast<osgFX::Scribe*>(parent.get());
					if (!ot) //若ot不存在（未白边） (node->parent)=>(node->scribe->parent)
					{
						osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe();
						scribe->addChild(node);
						parent->replaceChild(node, scribe);

					}
					//若ot存在（白边）找出当前scribe的父节点（node->scribe->*itr）=>(node->*itr)
					else
					{
						osg::Node::ParentList parentList = ot->getParents();
						osg::Node::ParentList::iterator itr = parentList.begin();
						(*itr)->replaceChild(ot, node);
					}

				}
			}
			return false;
		}
	}
};


//平移旋转类
class CTranHandler : public osgGA::GUIEventHandler {

public:
	CTranHandler();
	~CTranHandler();
public:
	osg::ref_ptr<osg::Node> node_1 = new osg::Node();
	osg::ref_ptr<osg::Node> node_2 = new osg::Node();
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);


private:

	//旋转矩阵
	osg::Vec3 Rotation1;
	osg::Vec3 Rotation2;

	bool lbuttonDown;
	bool m_rButtonDown;

	float m_leftX;
	float m_leftY;

	float m_rightX;
	float m_rightY;
};

CTranHandler::CTranHandler()
{
	Rotation1 = osg::Vec3(0, 0, 0);//对于node_1
	Rotation2 = osg::Vec3(0, 0, 0);//对于node_2

	m_rButtonDown = false;
	lbuttonDown = false;
	m_leftX = 0;
	m_leftY = 0;
	m_rightX = 0;
	m_rightY = 0;

}

CTranHandler::~CTranHandler()
{
}

//键盘鼠标响应函数，键盘操作需要切换到美式键盘才行
bool CTranHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* view = dynamic_cast<osgViewer::Viewer*> (&aa);//得到viewer
	if (!view) return false;

	node_1 = view->getSceneData()->asGroup()->getChild(0);
	node_2 = view->getSceneData()->asGroup()->getChild(1);


	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::KEYDOWN://平移操作
	{
		//6层移动
		//X轴
		if (ea.getKey() == 'q') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(-X, 0, 0));
			trans->addChild(node_1);
			group->addChild(trans);
			group->addChild(node_2);
			view->setSceneData(group.get());
		}

		if (ea.getKey() == 'e') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(X, 0, 0));
			trans->addChild(node_1);
			group->addChild(trans);
			group->addChild(node_2);
			view->setSceneData(group.get());
		}
		//Y轴
		if (ea.getKey() == 'a') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(0, -Y, 0));
			trans->addChild(node_1);
			group->addChild(trans);
			group->addChild(node_2);
			view->setSceneData(group.get());
		}
		if (ea.getKey() == 'd') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(0, Y, 0));
			trans->addChild(node_1);
			group->addChild(trans);
			group->addChild(node_2);
			view->setSceneData(group.get());
		}
		//Z轴
		if (ea.getKey() == 'z') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(0, 0, -Z));
			trans->addChild(node_1);
			group->addChild(trans);
			group->addChild(node_2);
			view->setSceneData(group.get());
		}
		if (ea.getKey() == 'c') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(0, 0, Z));
			trans->addChild(node_1);
			group->addChild(trans);
			group->addChild(node_2);
			view->setSceneData(group.get());
		}
		//7层移动
		//X轴
		if (ea.getKey() == 't') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(-X, 0, 0));
			group->addChild(node_1);
			trans->addChild(node_2);
			group->addChild(trans);
			view->setSceneData(group.get());
		}
		if (ea.getKey() == 'u') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(X, 0, 0));
			group->addChild(node_1);
			trans->addChild(node_2);
			group->addChild(trans);
			view->setSceneData(group.get());
		}
		//Y轴
		if (ea.getKey() == 'g') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(0, -Y, 0));
			group->addChild(node_1);
			trans->addChild(node_2);
			group->addChild(trans);
			view->setSceneData(group.get());
		}
		if (ea.getKey() == 'j') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(0, Y, 0));
			group->addChild(node_1);
			trans->addChild(node_2);
			group->addChild(trans);
			view->setSceneData(group.get());
		}
		//Z轴
		if (ea.getKey() == 'b') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(0, 0, -Z));
			group->addChild(node_1);
			trans->addChild(node_2);
			group->addChild(trans);
			view->setSceneData(group.get());
		}
		if (ea.getKey() == 'm') {
			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(0, 0, Z));
			group->addChild(node_1);
			trans->addChild(node_2);
			group->addChild(trans);
			view->setSceneData(group.get());
		}
		break;
	}

	//旋转
	case osgGA::GUIEventAdapter::PUSH:
	{
		//鼠标左键，/*控制node_1*/控制共同旋转
		if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{

			m_leftX = ea.getX();
			m_leftY = ea.getY();
			lbuttonDown = true;

		}

		// 鼠标右键，控制node_2
		if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
		{

			m_rightX = ea.getX();
			m_rightY = ea.getY();
			m_rButtonDown = true;
		}
		return false;
	}

	case osgGA::GUIEventAdapter::DRAG:
	{
		if (lbuttonDown)//
		{
			/*
			float ix = ea.getX() - m_leftX;
			float iy = ea.getY() - m_leftY;
			Rotation1[2] += osg::DegreesToRadians(0.05 * ix);//修改旋转矩阵
			Rotation1[0] -= osg::DegreesToRadians(0.05 * iy);

			osg::Vec3d center = node_1->getBound().center();//得到模型中心

			osg::ref_ptr<osg::Group> group = new osg::Group();

			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(-center) * osg::Matrix::rotate(Rotation1[0], osg::X_AXIS, Rotation1[1], osg::Y_AXIS, Rotation1[2], osg::Z_AXIS) *
				osg::Matrix::translate(center));//先移到原点旋转，再移回原位

			trans->addChild(node_1);
			trans->addChild(node_2);
			group->addChild(trans);
			//group->addChild(node_2);
			view->setSceneData(group.get());
			*/

			// 共同旋转
			float ix = ea.getX() - m_leftX;
			float iy = ea.getY() - m_leftY;
			Rotation1[2] += osg::DegreesToRadians(0.05 * ix);//修改旋转矩阵
			Rotation1[0] -= osg::DegreesToRadians(0.05 * iy);
			Rotation2[2] += osg::DegreesToRadians(0.05 * ix);
			Rotation2[0] -= osg::DegreesToRadians(0.05 * iy);

			osg::Vec3d center1 = node_1->getBound().center();//得到模型中心1
			osg::Vec3d center2 = node_2->getBound().center();//得到模型中心2

			osg::ref_ptr<osg::Group> group = new osg::Group();

			osg::ref_ptr <osg::MatrixTransform> trans1 = new osg::MatrixTransform;
			trans1->setMatrix(osg::Matrix::translate(-center1)* osg::Matrix::rotate(Rotation1[0], osg::X_AXIS, Rotation1[1], osg::Y_AXIS, Rotation1[2], osg::Z_AXIS)*
				osg::Matrix::translate(center1));//先移到原点旋转，再移回原位

			osg::ref_ptr <osg::MatrixTransform> trans2 = new osg::MatrixTransform;
			trans2->setMatrix(osg::Matrix::translate(-center1)* osg::Matrix::rotate(Rotation2[0], osg::X_AXIS, Rotation2[1], osg::Y_AXIS, Rotation2[2], osg::Z_AXIS)*
				osg::Matrix::translate(center1)); // 这里注意要用同样的旋转中心，不能用分别的！

			trans1->addChild(node_1);
			trans2->addChild(node_2);
			group->addChild(trans1);
			group->addChild(trans2);
			view->setSceneData(group.get());
		}

		if (m_rButtonDown)
		{
			float ix = ea.getX() - m_rightX;
			float iy = ea.getY() - m_rightY;
			Rotation2[2] += osg::DegreesToRadians(0.05 * ix);//修改旋转矩阵
			Rotation2[0] -= osg::DegreesToRadians(0.05 * iy);

			osg::Vec3d center = node_2->getBound().center();//得到模型中心

			osg::ref_ptr<osg::Group> group = new osg::Group();
			osg::ref_ptr <osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::translate(-center) * osg::Matrix::rotate(Rotation2[0], osg::X_AXIS, Rotation2[1], osg::Y_AXIS, Rotation2[2], osg::Z_AXIS) *
				osg::Matrix::translate(center));//先移到原点旋转，再移回原位
			trans->addChild(node_2);
			group->addChild(node_1);
			group->addChild(trans);
			view->setSceneData(group.get());
		}
	}

	case osgGA::GUIEventAdapter::RELEASE:
	{
		if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			lbuttonDown = false;
		}

		if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
		{
			m_rButtonDown = false;
		}
	}

	default:	break;
	}
	return false;
}

//实现函数——从高程图创建地形，推荐使用osgTerrain
osg::Node* createHeightField(std::string heightFile, std::string texFile)
{
	osg::Geode* geode = new osg::Geode();

	//定义并读取高度文件
	osg::Image* heightMap = osgDB::readImageFile(heightFile);

	osg::HeightField* heightField = new osg::HeightField();
	heightField->allocate(heightMap->s(), heightMap->t());

	heightField->setOrigin(osg::Vec3(-heightMap->s() / 2, -heightMap->t() / 2, 0));

	//设置间隔
	heightField->setXInterval(1.0f);
	heightField->setYInterval(1.0f);
	heightField->setSkirtHeight(1.0f);

	for (int r = 0; r < heightField->getNumRows(); r++)
	{
		for (int c = 0; c < heightField->getNumColumns(); c++)
		{
			heightField->setHeight(c, r, ((*heightMap->data(c, r)) / 255.0f) * 30.0f);  // 控制缩放比例夸张
		}
	}

	geode->addDrawable(new osg::ShapeDrawable(heightField));

	//定义并读取纹理图像
	osg::Texture2D* tex = new osg::Texture2D(osgDB::readImageFile(texFile));

	tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	
	return geode;
}

//主函数
int main() 
{
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();

	osg::ref_ptr<osg::Group> root = new osg::Group();

	//显示DEM
	osg::ref_ptr<osg::Node> node1 = createHeightField("./srtm_41_19.tif", "./srtm_41_19.jpg");
	root->addChild(node1);

	//读取3D模型
	osg::ref_ptr<osg::Node> node2 = osgDB::readNodeFile("3f\\model.obj");  // 展示3Dmax构建的贴图模型

	//拼接两层
	osg::MatrixTransform* tran = new osg::MatrixTransform;//创建平移矩阵
	// 进行平移旋转叠加等操作，注意Z轴相对坐标是原点
	osg::Vec3d center1 = node1->getBound().center();//得到模型中心
	tran->setMatrix(osg::Matrix::translate(-center1) * osg::Matrix::rotate(
		osg::DegreesToRadians(270.0), osg::X_AXIS,
		osg::DegreesToRadians(0.0), osg::Y_AXIS,
		osg::DegreesToRadians(0.0), osg::Z_AXIS) * osg::Matrix::translate(-250.0, 50.0, 10.0)); //设置平移旋转矩阵,Z轴存在问题一直向上
	tran->addChild(node2);
	root->addChild(tran);
	
	viewer->setSceneData(root.get());

	viewer->addEventHandler(new nodePick);//拾取
	viewer->addEventHandler(new CTranHandler());//响应

	//背景变成白色  white RGB和透明度
	viewer->getCamera()->setClearColor(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
	//添加帧率显示快捷键 s
	viewer->addEventHandler(new osgViewer::StatsHandler());
	//f键切换窗口全屏
	viewer->addEventHandler(new osgViewer::WindowSizeHandler());
	//w键切换绘制模式
	viewer->addEventHandler(new osgGA::StateSetManipulator
	(viewer->getCamera()->getOrCreateStateSet()));

	viewer->realize();
	viewer->run();
	return 0;
}
