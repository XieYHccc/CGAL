#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"


ImageWidget::ImageWidget(ChildWindow* relatewindow)
{

	choosing_shape = NULL;
	source_window_ = NULL;
	draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;
	copyed_ = false;
	Type_ = Shape::Type::kDefault;
	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

}

ImageWidget::~ImageWidget(void)
{
	if (choosing_shape)
		delete choosing_shape;
}

int ImageWidget::ImageWidth()
{
	return image_mat_.cols;
}

int ImageWidget::ImageHeight()
{
	return image_mat_.rows;
}

void ImageWidget::set_draw_status_to_Rectchoose()
{
	draw_status_ = kChoose;	
	Type_ = Shape::Type::kRect;
}

void ImageWidget::set_draw_status_to_Freechoose()
{
	draw_status_ = kChoose;
	Type_ = Shape::Type::KFreehand;
}

void ImageWidget::set_draw_status_to_paste()
{
	draw_status_ = kPaste;
}

bool ImageWidget::set_copy()
{	
	if (choosing_shape == NULL)
	{
		return false;
	}

	copyed_ = true;
	Matrix2i box = get_box();
	int x1 = box(0, 0);
	int x2 = box(1, 0);
	int y1 = box(0, 1);
	int y2 = box(1, 1);

	std::cout << "boundry box size : " << y2 - y1 + 1 << "  x  " << x2 - x1 + 1 << std::endl;
	return true;

}

cv::Mat ImageWidget::image()
{
	return image_mat_;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
}

MatrixXi ImageWidget::get_mask()
{
	assert(choosing_shape != NULL);

	MatrixXi mask;

	// get boundry box
	Matrix2i box = get_box();
	int x1 = box(0, 0);
	int x2 = box(1, 0);
	int y1 = box(0, 1);
	int y2 = box(1, 1);

	// get mask 
	MatrixXi rela_edge = get_relative_edge();
	LineScanner line_scanner;
	vector<int>x_list;
	vector<int>y_list;
	for (int i = 0; i < rela_edge.rows(); i++)
	{
		x_list.push_back(rela_edge(i,0));
		y_list.push_back(rela_edge(i,1));
	}
	
	mask = line_scanner.getMask(y2 - y1 + 1, x2 - x1 + 1, x_list, y_list);
	return mask;
	
}


Matrix2i ImageWidget::get_box()
{
	if (choosing_shape != NULL)
	{
		std::vector<QPoint>vertices = choosing_shape->get_vertices();
		Matrix2i result;
		result(0, 0) = vertices[0].rx();
		result(1, 0) = vertices[0].rx();
		result(0, 1) = vertices[0].ry();
		result(1, 1) = vertices[0].ry();

		// get the rectangle boundary points of choosing_shape 
		for (int i = 0; i < vertices.size(); i++)
		{
			if (vertices[i].rx() < result(0, 0))result(0, 0) = vertices[i].rx();
			if (vertices[i].rx() > result(1, 0))result(1, 0) = vertices[i].rx();
			if (vertices[i].ry() < result(0, 1))result(0, 1) = vertices[i].ry();
			if (vertices[i].ry() > result(1, 1))result(1, 1) = vertices[i].ry();
		}

		return result;
	}

}
MatrixXi ImageWidget::get_relative_edge()
{
	std::vector<QPoint>vertices = choosing_shape->get_vertices();
	int size = (int)vertices.size();
	MatrixXi result = MatrixXi::Zero(size, 2);
	Matrix2i box = get_box();

	//get left top corner of rectangle
	int x = box(0, 0);
	int y = box(0, 1);

	for (int i = 0; i < vertices.size(); i++)
	{
		result(i, 0) = vertices[i].rx() - x;
		result(i, 1) = vertices[i].ry() - y;
	}

	return result;
}
void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QImage image_show = QImage((unsigned char*)(image_mat_.data), image_mat_.cols, image_mat_.rows, image_mat_.step, QImage::Format_RGB888);
	QRect rect = QRect(0, 0, image_show.width(), image_show.height());
	painter.drawImage(rect, image_show);

	// Draw choose region
	if (choosing_shape != NULL)
	{
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::red);
		choosing_shape->Draw(painter);
		if (Type_ == Shape::Type::KFreehand)
		{
			painter.drawLine(choosing_shape->get_end(), choosing_shape->get_start());
		}
	}

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (draw_status_)
		{

		case kChoose:
			is_choosing_ = true;
			point_start_ = point_end_ = mouseevent->pos();
			if(choosing_shape!=NULL)
				delete choosing_shape;
			if (Type_ == Shape::Type::KFreehand)
			{
				choosing_shape = new Free();
				choosing_shape->add_vertice(point_start_);
			}
			else if (Type_ == Shape::Type::kRect)
			{
				choosing_shape = new Rect();
			}
			
			choosing_shape->set_start(point_start_);
			choosing_shape->set_end(point_end_);
			break;

		case kPaste:
		{
			is_pasting_ = true;

			//get source image
			cv::Mat source_image = source_window_->imagewidget_->image().clone();
			
			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			Matrix2i box = source_window_->imagewidget_->get_box();
			int x_pos_source = box(0, 0);
			int y_pos_source = box(0, 1);

			// get the mask of source image including edge
			MatrixXi mask = source_window_->imagewidget_->get_mask();

			int w = mask.cols();
			int h = mask.rows();

			// Paste directly
			/*if ((xpos + w < ImageWidth()) && (ypos + h < ImageHeight()))
			{
				for (int i = 0; i < h; i++)
				{
					for (int j = 0; j < w; j++)
					{
						if (mask(i, j) == 1)
						{
							image_mat_.at<cv::Vec3b>(ypos + i, xpos + j) =
								source_image.at<cv::Vec3b>(y_pos_source+i, x_pos_source+j);
						}
					}
				}
			}*/

			// Paste with poisson fusion
			MatrixXd f_data(h, w);
			MatrixXd g_data(h, w);

			// three chanels do apply interpolation separetely
			Poisson_Fuse p_f(mask);
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < h; j++)
				{
					for (int k = 0; k < w; k++)
					{
						f_data(j, k) = source_image.at<cv::Vec3b>(y_pos_source + j, x_pos_source +k)[i];
						g_data(j, k) = image_mat_.at<cv::Vec3b>(ypos + j, xpos + k)[i];
					}
				}
				MatrixXd new_g = p_f.discrete_Poisson_Solve(f_data,g_data,true);
				for (int j = 0; j < h; j++)
				{
					for (int k = 0; k < w; k++)
					{
						if (mask(j, k) == 1)
						{
							image_mat_.at<cv::Vec3b>(ypos-(h/2) + j, xpos-(w/2) + k)[i] = new_g(j, k);
						}
					}
				}

			}


		}

		update();
		break;

		default:
			break;
		}
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();

			if (Type_ == Shape::Type::KFreehand)
			{
				// store the Freeshape boundry points
				choosing_shape->add_vertice(point_end_);
				
			}

			choosing_shape->set_end(point_end_);
		}
		break;

	case kPaste:
		// Paste rectangle region to object image
		if (is_pasting_)
		{
			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			// Paste
			//if ((xpos > 0) && (ypos > 0) && (xpos + w < image_->width()) && (ypos + h < image_->height()))
			//{
			//	// Restore image 
			//	*(image_) = *(image_backup_);

			//	// Paste
			//	for (int i = 0; i < w; i++)
			//	{
			//		for (int j = 0; j < h; j++)
			//		{
			//			image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
			//		}
			//	}
			//}
		}

	default:
		break;
	}

	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		if (is_choosing_)
		{
			is_choosing_ = false;
			draw_status_ = kNone;
			if (Type_ == Shape::Type::kRect)
			{
				QPoint p2(point_end_.rx(), point_start_.ry());
				QPoint p4(point_start_.rx(), point_end_.ry());
				choosing_shape->add_vertice(point_start_);
				choosing_shape->add_vertice(p2);
				choosing_shape->add_vertice(point_end_);
				choosing_shape->add_vertice(p4);
			}

		}

	case kPaste:
		if (is_pasting_)
		{
			is_pasting_ = false;
			draw_status_ = kNone;
		}

	default:
		break;
	}

	update();
}

void ImageWidget::Open(QString filename)
{
	// Load file
	if (!filename.isEmpty())
	{
		image_mat_ = cv::imread(filename.toLatin1().data());
		cv::cvtColor(image_mat_, image_mat_, cv::COLOR_BGR2RGB);
		image_mat_backup_ = image_mat_.clone();

		/*image_->load(filename);
		*(image_backup_) = *(image_);*/
	}

	std::cout << "image size: " << image_mat_.rows << ' ' << image_mat_.cols << std::endl;
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

	cv::Mat image_save;
	cv::cvtColor(image_mat_, image_save, cv::COLOR_BGR2RGB);
	cv::imwrite(filename.toLatin1().data(), image_save);
}

void ImageWidget::Invert()
{
	if (image_mat_.empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_mat_.begin<cv::Vec3b>(), iterend = image_mat_.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		(*iter)[0] = 255 - (*iter)[0];
		(*iter)[1] = 255 - (*iter)[1];
		(*iter)[2] = 255 - (*iter)[2];
	}

	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	if (image_mat_.empty())
		return;
	int width = image_mat_.cols;
	int height = image_mat_.rows;

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(height - 1 - j, width - 1 - i);
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(j, width - 1 - i);
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(height - 1 - j, i);
				}
			}
		}
	}

	update();
}

void ImageWidget::TurnGray()
{
	if (image_mat_.empty())
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter = image_mat_.begin<cv::Vec3b>(), iterend = image_mat_.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		int itmp = ((*iter)[0] + (*iter)[1] + (*iter)[2]) / 3;
		(*iter)[0] = itmp;
		(*iter)[1] = itmp;
		(*iter)[2] = itmp;
	}

	update();
}

void ImageWidget::Restore()
{
	image_mat_ = image_mat_backup_.clone();
	update();
}
