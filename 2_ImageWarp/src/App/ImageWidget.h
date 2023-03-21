#pragma once
#include <QWidget>
#include<vector>
#include"Warp.h"
#include"Idw.h"
#include"RBF.h"

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget();
	~ImageWidget();
private:
	QPoint relative_pos(QPoint point) const;

protected:
	void paintEvent(QPaintEvent *paintevent);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin
	void IDW();													// IDW method
	void RBF();													// Rbf method
	void IfAddControlPoint();									// Change the mode to select pairs of control points or change back
	void Clear();												// clear the control pairs
	void draw_InputLine(const QLine& line);						// draw line
	//void RBF();
private:
	QImage		*ptr_image_;				// image 
	QImage		*ptr_image_backup_;
	QCursor		cursor;
	QLine		*current_line;
	std::vector<QLine*> ControlPoints_list;
	bool Ifselect;
	Warp		*Image_Warpper;
};

