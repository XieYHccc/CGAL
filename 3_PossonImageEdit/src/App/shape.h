#pragma once

#include <QtGui>
#include <vector>

class Shape
{
public:
	Shape();
	Shape(std::vector<QPoint> p_list);
	virtual ~Shape();
	virtual void Draw(QPainter& paint) = 0;
	void set_start(QPoint s);
	QPoint get_start();
	QPoint get_end();
	void set_end(QPoint e);
	void add_vertice(QPoint p);
	std::vector<QPoint> get_vertices();

public:
	enum Type
	{
		kDefault = 0,
		kLine = 1,
		kRect = 2,
		kEllip = 3,
		KFreehand = 4,
		Kpoly = 5
	};

protected:
	QPoint start;
	QPoint end;
	std::vector<QPoint> v_list;						//vertices of the shape

};

