#include "viewwidget.h"

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false;
	shape_ = NULL;
	type_ = Shape::kDefault;
}

ViewWidget::~ViewWidget()
{
	if (shape_)
		delete shape_;

	for (int i = 0; i < shape_list_.size(); i++)
	{
		if (shape_list_[i])
		{
			delete shape_list_[i];
		}
	}
}

 void ViewWidget::Undo()
{	
	 if(shape_list_.size()>0)
	 {
		 delete shape_list_[shape_list_.size() - 1];
		 shape_list_.pop_back();
		 repaint();
	 }
}

void ViewWidget::setLine()
{
	type_ = Shape::kLine;
}

void ViewWidget::setRect()
{
	type_ = Shape::kRect;
}

void ViewWidget::setEllip()
{
	type_ = Shape::kEllip;
}

void ViewWidget::setFreehand()
{
	type_ = Shape::KFreehand;
}

void ViewWidget::setPoly()
{
	type_ = Shape::Kpoly;
}

void ViewWidget::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button())
	{
		switch (type_)
		{
		case Shape::kLine:
			shape_ = new Line();
			break;
		case Shape::kDefault:
			break;

		case Shape::kRect:
			shape_ = new Rect();
			break;
		case Shape::kEllip:
			shape_ = new Ellip();
			break;
		case Shape::KFreehand:
			point_list_.push_back(event->pos());
			shape_ = new Free(point_list_);
			break;
		case Shape::Kpoly:
			point_list_.push_back(event->pos());
			if (draw_status_ == false) // false means you are creating a new Poly
				shape_ = new Poly(point_list_);
			else
			{
				delete shape_;
				shape_ = new Poly(point_list_);
			}
			break;
		}
		if (shape_ != NULL)
		{
			if (draw_status_ == false) // false means you are creating a new shape
			{
				draw_status_ = true;
				start_point_ = event->pos(); // Everytime creating a new shape will lose property start and end
				shape_->set_start(start_point_); //so we need a variable to record the start_point.

			}
			
			end_point_ = event->pos();
			shape_->set_end(end_point_);
			shape_->set_start(start_point_);
		}
	}
	else if(Qt::RightButton == event->button())
	{
		if (draw_status_)
		{
			draw_status_ = false;
			end_point_ = start_point_;
			shape_->set_end(end_point_);
		}
	}
	update();
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status_ && shape_ != NULL)
	{
		end_point_ = event->pos();
		shape_->set_end(end_point_);
	}
	if (draw_status_ && type_ == Shape::KFreehand)
	{
		point_list_.push_back(end_point_);
		delete shape_;
		shape_ = new Free(point_list_);
	}
	
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (shape_ != NULL)
	{
		if (type_ == Shape::Type::KFreehand)
		{
			std::vector<QPoint>().swap(point_list_); // not only could this clear the vector, but also release the memory
			draw_status_ = false;
			shape_list_.push_back(shape_);
			shape_ = NULL;
		}
		else if (type_ == Shape::Type::Kpoly)
		{
			if (draw_status_ == true)
				setMouseTracking(true); // track the mouse after release button
			else
			{
				setMouseTracking(false); // only track when user is drawing

				shape_list_.push_back(shape_);
				shape_ = NULL;
				std::vector<QPoint>().swap(point_list_);
			}
		}
		else
		{
			draw_status_ = false;
			shape_list_.push_back(shape_);
			shape_ = NULL;
		}
	}
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	for (int i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
	}

	if (shape_ != NULL) {
		shape_->Draw(painter);
	}
	if(draw_status_) //only while drawing, update window
		update();
}