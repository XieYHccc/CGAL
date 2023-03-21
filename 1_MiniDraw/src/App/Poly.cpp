#include "Poly.h"
Poly::Poly()
{
}

Poly::Poly(std::vector<QPoint> point_list)
	:mpoint_list(point_list)
{
}

Poly::~Poly()
{
}

void Poly::Draw(QPainter& painter)
{
	if (mpoint_list.size() > 1)
	{
		for (size_t i = 0; i < mpoint_list.size() - 1; i++)
		{
			painter.drawLine(mpoint_list[i], mpoint_list[i + 1]);
		}
	}
	painter.drawLine(mpoint_list[mpoint_list.size() - 1], end);

}