#include "mainDialog.h"

#include <QApplication>
#include <QArgument>
#include <QFont>
#include <QObject>
#include <QStyleFactory>

class QStyle;

static const char* PS5_STYLE = R"(
	QDialog, QWidget {
		background-color: #14151a;
		color: #ffffff;
		font-family: "Segoe UI", "SF Pro Display", "Helvetica Neue", Arial, sans-serif;
	}
	QToolTip {
		background-color: #1e1f26;
		color: #ffffff;
		border: 1px solid rgba(255,255,255,0.15);
		border-radius: 6px;
		padding: 6px 10px;
		font-size: 12px;
	}
	QLineEdit {
		background-color: #1e1f26;
		border: 1px solid rgba(255,255,255,0.12);
		border-radius: 8px;
		color: #ffffff;
		padding: 8px 14px;
		font-size: 13px;
		selection-background-color: #004fff;
	}
	QLineEdit:focus {
		border-color: #004fff;
	}
	QComboBox {
		background-color: #1e1f26;
		border: 1px solid rgba(255,255,255,0.12);
		border-radius: 8px;
		color: #ffffff;
		padding: 6px 12px;
		font-size: 12px;
	}
	QComboBox:focus {
		border-color: #004fff;
	}
	QComboBox::drop-down {
		border: none;
		width: 28px;
	}
	QComboBox::down-arrow {
		image: none;
		width: 0;
	}
	QComboBox QAbstractItemView {
		background-color: #1e1f26;
		color: #ffffff;
		border: 1px solid rgba(255,255,255,0.12);
		border-radius: 6px;
		selection-background-color: #004fff;
		padding: 4px;
		outline: none;
	}
	QScrollBar:vertical {
		background: transparent;
		width: 8px;
		margin: 0;
	}
	QScrollBar::handle:vertical {
		background: rgba(255,255,255,0.2);
		border-radius: 4px;
		min-height: 40px;
	}
	QScrollBar::handle:vertical:hover {
		background: rgba(255,255,255,0.35);
	}
	QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
		height: 0;
	}
	QScrollBar:horizontal {
		background: transparent;
		height: 8px;
		margin: 0;
	}
	QScrollBar::handle:horizontal {
		background: rgba(255,255,255,0.2);
		border-radius: 4px;
		min-width: 40px;
	}
	QScrollBar::handle:horizontal:hover {
		background: rgba(255,255,255,0.35);
	}
	QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
		width: 0;
	}
	QToolButton {
		background: rgba(255,255,255,0.06);
		border: 1px solid transparent;
		border-radius: 10px;
		padding: 8px;
	}
	QToolButton:hover {
		background: rgba(255,255,255,0.12);
		border-color: rgba(255,255,255,0.15);
	}
	QToolButton:pressed {
		background: rgba(255,255,255,0.04);
	}
	QPushButton {
		background-color: #004fff;
		border: none;
		border-radius: 10px;
		color: #ffffff;
		padding: 10px 24px;
		font-size: 13px;
		font-weight: 600;
	}
	QPushButton:hover {
		background-color: #0066ff;
	}
	QPushButton:pressed {
		background-color: #003dcc;
	}
	QPushButton:disabled {
		background-color: rgba(255,255,255,0.08);
		color: rgba(255,255,255,0.3);
	}
	QLabel {
		background: transparent;
		color: #ffffff;
	}
)";

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	a.setStyleSheet(PS5_STYLE);

	QFont defaultFont = a.font();
	defaultFont.setPointSize(10);
	defaultFont.setFamily(QStringLiteral("Segoe UI"));
	a.setFont(defaultFont);

	MainDialog w;

	QStyle* s = QStyleFactory::create("fusion");
	QApplication::setStyle(s);

	QObject::connect(&a, &QApplication::aboutToQuit, &w, &MainDialog::Quit);

	w.emit Start();

	w.show();

	return QApplication::exec();
}
