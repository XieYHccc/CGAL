#include "minidraw.h"

#include <QToolBar>

MiniDraw::MiniDraw(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	view_widget_ = new ViewWidget(); // only Qobject who has a parent will be auto released
	Creat_Action();
	Creat_ToolBar();
	Creat_Menu();
	resize(1200, 800);
	setCentralWidget(view_widget_);
}

void MiniDraw::Creat_Action() {
	Action_About = new QAction(tr("&About"), this);
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);

	Action_Line = new QAction(tr("&Line"), this);
	connect(Action_Line, SIGNAL(triggered()), view_widget_, SLOT(setLine()));

	Action_Rect = new QAction(tr("&Rect"), this);
	connect(Action_Rect, &QAction::triggered, view_widget_, &ViewWidget::setRect);

	Action_Ellip = new QAction(tr("&Ellip"), this);
	connect(Action_Ellip, &QAction::triggered, view_widget_, &ViewWidget::setEllip);

	Action_Freehand = new QAction(tr("&Freehand"), this);
	connect(Action_Freehand, &QAction::triggered, view_widget_, &ViewWidget::setFreehand);

	Action_Poly = new QAction(tr("&Poly"), this);
	connect(Action_Poly, &QAction::triggered, view_widget_, &ViewWidget::setPoly);

	Action_Undo = new QAction(tr("&Undo"), this);
	connect(Action_Undo, &QAction::triggered, view_widget_, &ViewWidget::Undo);
}

void MiniDraw::Creat_ToolBar() {
	pToolBar = addToolBar(tr("&Main"));
	pToolBar->addAction(Action_About);
	pToolBar->addAction(Action_Line);
	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellip);
	pToolBar->addAction(Action_Freehand);
	pToolBar->addAction(Action_Poly);
	pToolBar->addAction(Action_Undo);
}

void MiniDraw::Creat_Menu() {
	pMenu = menuBar()->addMenu(tr("&Figure Tool"));
	pMenu->addAction(Action_About);
	pMenu->addAction(Action_Line);
	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellip);
	pMenu->addAction(Action_Freehand);
	pMenu->addAction(Action_Poly);
	pMenu->addAction(Action_Undo);
}

void MiniDraw::AboutBox() {
	QMessageBox::about(this, tr("About"), tr("MiniDraw"));
}

MiniDraw::~MiniDraw()
{
}
