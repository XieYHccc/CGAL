#include "Free.h"

Free::Free()
{
}

Free::Free(std::vector<QPoint> point_list)
	:Shape(point_list)
{
}

Free::~Free()
{
}

void Free::Draw(QPainter& painter)
{
	if (v_list.size() > 1)
	{
		for (size_t i = 0; i < v_list.size() - 1; i++)
		{
			painter.drawLine(v_list[i], v_list[i + 1]);
		}
	}
	else if (v_list.size() == 1)
	{
		painter.drawLine(start, start);
	}	
}