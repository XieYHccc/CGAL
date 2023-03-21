#pragma once

#include "shape.h"
#include "vector"

class Free : public Shape
{
public:
	Free();
	Free(std::vector<QPoint> point_list);
	~Free();
	void Draw(QPainter& painter);

	 
};