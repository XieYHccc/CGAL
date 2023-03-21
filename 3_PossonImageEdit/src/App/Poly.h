#pragma once

#include "shape.h"
#include "vector"

class Poly : public Shape
{
public:
	Poly();
	Poly(std::vector<QPoint> point_list);
	~Poly();
	void Draw(QPainter& painter);

private:
	std::vector<QPoint> mpoint_list;

};