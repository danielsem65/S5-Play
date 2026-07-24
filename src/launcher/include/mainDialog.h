#ifndef MAIN_DIALOG_H
#define MAIN_DIALOG_H

#include "common.h"

#include <QDialog>
#include <QMouseEvent>
#include <QPoint>
#include <QString>

class QWidget;
class QProcess;
class MainDialogPrivate;
class QSettings;
class QResizeEvent;

class Configuration;
class MainDialog: public QDialog {
	Q_OBJECT
	KYTY_QT_CLASS_NO_COPY(MainDialog);

signals:
	void Start();
	void Quit();
	void Resize();

public:
	explicit MainDialog(QWidget* parent = nullptr);
	~MainDialog() override = default;

	void RunInterpreter(QProcess* process, const Configuration& info);

	static void WriteSettings(QSettings& s);
	static void ReadSettings(QSettings& s);

	void resizeEvent(QResizeEvent* event) override;

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	MainDialogPrivate* m_p = nullptr;
	bool               m_dragging = false;
	QPoint             m_drag_start;
};

#endif // MAIN_DIALOG_H
