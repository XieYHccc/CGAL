#include "Shape.h"

Shape::Shape()
{
}

Shape::Shape(std::vector<QPoint> p_list)
	:v_list(p_list)
{
}

Shape::~Shape()
{
}

void Shape::set_start(QPoint s)
{
	start = s;
}

void Shape::set_end(QPoint e)
{
	end = e;
}
QPoint Shape::get_start()
{
	return start;
}

QPoint Shape::get_end()
{
	return end;
}

void Shape::add_vertice(QPoint p)
{
	v_list.push_back(p);
}

std::vector<QPoint> Shape::get_vertices()
{
	return v_list;
}
