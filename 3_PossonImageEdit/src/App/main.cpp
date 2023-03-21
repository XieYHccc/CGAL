#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include "LineScanner.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
	//vector<int> x{ 20,60,60,20 };
	//vector<int> y{ 40,40,80,80 };
	//LineScanner ls;
	//MatrixXi mask = ls.getMask(100, 100, x, y);
	//cout << mask << endl;
	//cin.get();
}
