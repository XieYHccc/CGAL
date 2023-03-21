#include "ImageWidget.h"
#include <QPainter>
#include <QtWidgets> 
#include <QImage>
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget()
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	Ifselect = false;
	current_line = NULL;
	Image_Warpper = NULL;
}

ImageWidget::~ImageWidget()
{
	delete current_line;
	delete Image_Warpper;
}

void ImageWidget::IDW()
{
	std::vector<QPoint> p_list;
	std::vector<QPoint> q_list;
	for (int i = 0; i < ControlPoints_list.size(); i++)
	{
		QPoint p1 = relative_pos(ControlPoints_list[i]->p1());
		QPoint p2 = relative_pos(ControlPoints_list[i]->p2());

		p_list.push_back(p1);
		q_list.push_back(p2);
	}
	Image_Warpper = new Idw(p_list,q_list);
	Image_Warpper->Warp_Image(ptr_image_);
	Image_Warpper->Fill_holl(ptr_image_);
	update();
}

void ImageWidget::RBF()
{
	std::vector<QPoint> p_list;
	std::vector<QPoint> q_list;
	for (int i = 0; i < ControlPoints_list.size(); i++)
	{
		QPoint p1 = relative_pos(ControlPoints_list[i]->p1());
		QPoint p2 = relative_pos(ControlPoints_list[i]->p2());

		p_list.push_back(p1);
		q_list.push_back(p2);
	}
	Image_Warpper = new Rbf(p_list, q_list);
	Image_Warpper->Warp_Image(ptr_image_);
	Image_Warpper->Fill_holl(ptr_image_);
	update();
}
QPoint ImageWidget::relative_pos(QPoint point) const
{
	int x = point.x() - (width() - ptr_image_->width()) / 2;
	int y = point.y() - (height() - ptr_image_->height()) / 2;
	
	return QPoint(x, y);
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	QPen pen;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect( (width()-ptr_image_->width())/2, (height()-ptr_image_->height())/2, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_); 

	// Draw Control points pairs
	if (ControlPoints_list.size() > 0)
	{
		//cout << ControlPoints_list.size() << endl;
		for (int i = 0; i < ControlPoints_list.size(); i++)  //Draw points pair in ControlPoints_list
		{
			QLine tmp_line = *(ControlPoints_list[i]);
			draw_InputLine(tmp_line);

		}
	}
	if (current_line != NULL)
	{
		draw_InputLine(*current_line);
	}

	painter.end();
}
void ImageWidget::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button())
	{
		if (Ifselect ==true && current_line == NULL)
		{	
			QPoint current_p = event->pos();
			current_line = new QLine(current_p, current_p);
			QPoint rela_pos = relative_pos(current_p);
			cout << "start (" << rela_pos.x() << "," << rela_pos.y() << ")" << endl;
			update();
		}
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (Ifselect && current_line != NULL)
	{
		current_line->setP2(event->pos());
		update();
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (Ifselect && current_line != NULL)
	{
		ControlPoints_list.push_back(current_line); // YOU SHOULD NOT DELETE CURRENT_LINE AFTER THIS.
		current_line = NULL;						//reset the pointer to null pointer
		QPoint current_p= event->pos();
		QPoint rela_pos = relative_pos(current_p);
		cout << "end (" << rela_pos.x() << "," << rela_pos.y() << ")" << endl;
		update();
	}
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), "../data", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else			//仅水平翻转			
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)		//仅垂直翻转
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	update();
}

void ImageWidget::IfAddControlPoint()
{
	if (Ifselect == false)
	{
		Ifselect = true;
		cout << "start selecting control points pairs " << endl;
		cursor.setShape(Qt::CrossCursor);
		QApplication::setOverrideCursor(cursor);
	}
	else
	{
		Ifselect = false;
		cout << "Finish selecting control points pairs " << endl;
		QApplication::restoreOverrideCursor();
	}
}

void ImageWidget::Clear()
{
	std::vector<QLine*>().swap(ControlPoints_list);
	update();
}
void ImageWidget::draw_InputLine(const QLine& line)
{
	QPainter painter(this);
	QPen pen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter.setPen(pen);
	painter.drawLine(line.p1() - QPoint(3, 3), line.p1() + QPoint(3, 3));
	painter.drawLine(line.p1() - QPoint(3, -3), line.p1() + QPoint(3, -3));
	painter.drawLine(line.p1(), line.p2());
	pen.setColor(Qt::blue);
	painter.setPen(pen);
	painter.drawLine(line.p2() - QPoint(3, 3), line.p2() + QPoint(3, 3));
	painter.drawLine(line.p2() - QPoint(3, -3), line.p2() + QPoint(3, -3));
}