#include "Warp.h"
#include<iostream>

Warp::Warp()
{
}

Warp::Warp(std::vector<QPoint> p_list, std::vector<QPoint> q_list)
	:mp_points(p_list), mq_points(q_list)
{
}

Warp::~Warp()
{
}
void Warp::Warp_Image(QImage* image) const		
{
	QImage tmp_image(*(image));
	image->fill(Qt::white);
	int width = image->width();
	int height = image->height();

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			Vector2d trans_point = warp_func(QPoint(i, j));
			image->setPixel(trans_point[0], trans_point[1], tmp_image.pixel(i, j));
		}
	}
}


double Warp::Eu_Dis(QPoint p1, QPoint p2) const
{
	return sqrt(pow((p1.x() - p2.x()), 2) + pow((p1.y() - p2.y()), 2));
}

void Warp::Fill_holl(QImage* image)
{
	int width = image->width();
	int height = image->height();
	
	QImage tmp_image(*image);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			QRgb pix1 = image->pixel(i, j);
			QColor c1(pix1);
			if (c1.red() == 255 && c1.green() == 255 && c1.blue() == 255)
			{	
				bool filled = false;

				for (int m = 1; m <= 1; m++)			// set the number of boundry
				{	
					for (int n = 0; n <= 2; n++)		//traverse according to the importance level
					{
						for (int k = 0; k <= 2; k++)
						{
							QColor c2 = tmp_image.pixel(i - m + n * m, j - m + k * m);
							if (c2.red() != 255 || c2.green() != 255 || c2.blue() != 255)
							{
								image->setPixel(i, j, qRgb(c2.red(), c2.green(), c2.blue()));
								//std::cout << "fill successfully" << std::endl;
								filled = true;
								break;
							}
						}

						if (filled)
							break;
					}
					if (filled)
						break;
				}
			}
		}
	}
}
