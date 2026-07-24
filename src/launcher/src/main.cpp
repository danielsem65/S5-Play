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
		background-color: rgba(20,21,26,230);
		color: #ffffff;
		border: 1px solid rgba(0,79,255,0.3);
		border-radius: 8px;
		padding: 8px 12px;
		font-size: 12px;
	}
	QLineEdit {
		background-color: rgba(255,255,255,0.06);
		border: 1px solid rgba(255,255,255,0.08);
		border-radius: 10px;
		color: #ffffff;
		padding: 8px 14px;
		font-size: 13px;
		selection-background-color: #004fff;
	}
	QLineEdit:focus {
		border-color: #004fff;
		background-color: rgba(0,79,255,0.06);
	}
	QComboBox {
		background-color: rgba(255,255,255,0.06);
		border: 1px solid rgba(255,255,255,0.08);
		border-radius: 10px;
		color: #ffffff;
		padding: 7px 14px;
		font-size: 12px;
		min-height: 20px;
	}
	QComboBox:focus, QComboBox:hover {
		border-color: rgba(0,79,255,0.4);
		background-color: rgba(0,79,255,0.08);
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
		border: 1px solid rgba(0,79,255,0.2);
		border-radius: 10px;
		padding: 6px;
		outline: none;
		selection-background-color: rgba(0,79,255,0.35);
	}
	QComboBox QAbstractItemView::item {
		padding: 8px 12px;
		border-radius: 6px;
	}
	QComboBox QAbstractItemView::item:hover {
		background-color: rgba(0,79,255,0.15);
	}
	QCheckBox {
		spacing: 10px;
		font-size: 13px;
		color: #e0e0f0;
	}
	QCheckBox::indicator {
		width: 20px;
		height: 20px;
		border-radius: 5px;
		border: 2px solid rgba(255,255,255,0.2);
		background-color: rgba(255,255,255,0.04);
	}
	QCheckBox::indicator:checked {
		border-color: #004fff;
		background-color: #004fff;
	}
	QCheckBox::indicator:hover {
		border-color: rgba(0,79,255,0.5);
	}
	QSpinBox {
		background-color: rgba(255,255,255,0.06);
		border: 1px solid rgba(255,255,255,0.08);
		border-radius: 10px;
		color: #ffffff;
		padding: 7px 14px;
		font-size: 12px;
		min-height: 20px;
		min-width: 80px;
	}
	QSpinBox:focus {
		border-color: #004fff;
	}
	QSpinBox::up-button, QSpinBox::down-button {
		border: none;
		background: transparent;
		width: 24px;
	}
	QSpinBox::up-arrow { image: none; }
	QSpinBox::down-arrow { image: none; }
	QScrollBar:vertical {
		background: transparent;
		width: 6px;
		margin: 0;
	}
	QScrollBar::handle:vertical {
		background: rgba(255,255,255,0.15);
		border-radius: 3px;
		min-height: 40px;
	}
	QScrollBar::handle:vertical:hover {
		background: rgba(0,79,255,0.4);
	}
	QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
		height: 0;
	}
	QScrollBar:horizontal {
		background: transparent;
		height: 6px;
		margin: 0;
	}
	QScrollBar::handle:horizontal {
		background: rgba(255,255,255,0.15);
		border-radius: 3px;
		min-width: 40px;
	}
	QScrollBar::handle:horizontal:hover {
		background: rgba(0,79,255,0.4);
	}
	QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
		width: 0;
	}
	QToolButton {
		background: rgba(255,255,255,0.04);
		border: 1px solid transparent;
		border-radius: 10px;
		padding: 8px;
	}
	QToolButton:hover {
		background: rgba(0,79,255,0.12);
		border-color: rgba(0,79,255,0.2);
	}
	QToolButton:pressed {
		background: rgba(0,79,255,0.18);
	}
	QPushButton {
		background-color: #004fff;
		border: none;
		border-radius: 10px;
		color: #ffffff;
		padding: 10px 24px;
		font-size: 13px;
		font-weight: 700;
	}
	QPushButton:hover {
		background-color: #0066ff;
	}
	QPushButton:pressed {
		background-color: #003dcc;
	}
	QPushButton:disabled {
		background-color: rgba(255,255,255,0.05);
		color: rgba(255,255,255,0.2);
		border: 1px solid rgba(255,255,255,0.06);
	}
	QScrollArea {
		background: transparent;
		border: none;
	}
	QScrollArea > QWidget > QWidget {
		background: transparent;
	}
	QLabel {
		background: transparent;
		color: #ffffff;
	}
	QFrame {
		background: transparent;
	}
	QGroupBox {
		background-color: rgba(255,255,255,0.03);
		border: 1px solid rgba(255,255,255,0.06);
		border-radius: 12px;
		margin-top: 20px;
		padding: 16px 12px 12px 12px;
		font-size: 13px;
		font-weight: 600;
		color: #b0b0c8;
	}
	QGroupBox::title {
		subcontrol-origin: margin;
		subcontrol-position: top left;
		padding: 4px 12px;
		background-color: #1e1f26;
		border: 1px solid rgba(255,255,255,0.06);
		border-radius: 6px;
		color: #b0b0c8;
		font-size: 11px;
		letter-spacing: 1px;
	}
	QListWidget {
		background-color: rgba(255,255,255,0.03);
		border: 1px solid rgba(255,255,255,0.06);
		border-radius: 10px;
		color: #ffffff;
		outline: none;
		padding: 4px;
	}
	QListWidget::item {
		padding: 8px 12px;
		border-radius: 6px;
	}
	QListWidget::item:selected {
		background-color: rgba(0,79,255,0.2);
	}
	QListWidget::item:hover {
		background-color: rgba(255,255,255,0.04);
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
