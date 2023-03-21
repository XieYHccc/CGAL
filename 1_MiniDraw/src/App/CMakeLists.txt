FILE(GLOB UI_FILES "./*.ui")
#通过Ui文件生成对应的头文件，一定要添加
qt5_wrap_ui(WRAP_FILES ${UI_FILES})

Ubpa_AddTarget(
  MODE "EXE"
  QT ON
  LIBS "Qt5::Widgets"
)

