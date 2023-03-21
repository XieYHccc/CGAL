#pragma once
#include <QWidget>
#include "shape.h"
#include "Rect.h"
#include "Free.h"
#include "LineScanner.h"
#include "Poisson_Fuse.h"

#include <vector>
#include <assert.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <Eigen\Sparse>
#include <Eigen\Dense>

using namespace Eigen;

class ChildWindow;
QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

enum DrawStatus
{
	kChoose,
	kPaste, 
	kNone
};

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(ChildWindow *relatewindow);
	~ImageWidget(void);

	int ImageWidth();											// Width of image
	int ImageHeight();											// Height of image
	void set_draw_status_to_Rectchoose();
	void set_draw_status_to_Freechoose();
	void set_draw_status_to_paste();
	bool set_copy();
	cv::Mat image();
	void set_source_window(ChildWindow* childwindow);			// set source image window
	MatrixXi get_mask();
	Matrix2i get_box();											// 2 x 2, two corner of the rectangle

private:
	MatrixXi get_relative_edge();								// size(vertices) x 2, position of boundry points

protected:
	void paintEvent(QPaintEvent *paintevent);
	void mousePressEvent(QMouseEvent *mouseevent);
	void mouseMoveEvent(QMouseEvent *mouseevent);
	void mouseReleaseEvent(QMouseEvent *mouseevent);

public slots:
	// File IO
	void Open(QString filename);								// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin

public:
	QPoint						point_start_;					// Left top point of rectangle region
	QPoint						point_end_;						// Right bottom point of rectangle region

private:
	cv::Mat						image_mat_;						// image
	cv::Mat                     image_mat_backup_;
	Shape						*choosing_shape;				// the shape you use to choose region

	// Pointer of child window
	ChildWindow					*source_window_;				// Source child window

	// Signs
	DrawStatus					draw_status_;					// Enum type of draw status
	bool						is_choosing_;
	bool						is_pasting_;
	bool						copyed_;						// if true, there is reigon copyed
	Shape::Type					Type_;							// Enum type of shape
};

