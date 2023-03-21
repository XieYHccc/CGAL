#include "Free.h"

Free::Free()
{
}

Free::Free(std::vector<QPoint> point_list)
	:mpoint_list(point_list)
{
}

Free::~Free()
{
}

void Free::Draw(QPainter& painter)
{
	if (mpoint_list.size() > 1)
	{
		for (size_t i = 0; i < mpoint_list.size() - 1; i++)
		{
			painter.drawLine(mpoint_list[i], mpoint_list[i + 1]);
		}
	}
	else if (mpoint_list.size() == 1)
	{
		painter.drawLine(start, start);
	}	
}