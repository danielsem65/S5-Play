#ifndef CONFIGURATION_ITEM_H
#define CONFIGURATION_ITEM_H

#include "common.h"

#include <QTreeWidgetItem>

#include <memory>

class QComboBox;
class QLabel;
class QLineEdit;
class QTreeWidget;
class QWidget;

class Configuration;
class ConfigurationItem: public QTreeWidgetItem {
public:
	explicit ConfigurationItem(std::unique_ptr<Configuration> info, QTreeWidget* parent);
	~ConfigurationItem() override;

	void Update();
	bool operator<(const QTreeWidgetItem& other) const override;

	KYTY_QT_CLASS_NO_COPY(ConfigurationItem);

	Configuration&                     GetInfo() { return *m_info; }
	[[nodiscard]] const Configuration& GetInfo() const { return *m_info; }
	void                               SetRunning(bool state);
	void                               SetCompatibilityEditable(bool editable);
	[[nodiscard]] bool                 IsRunning() const { return m_running; }
	QComboBox*                         GetStatusCombo();
	QLineEdit*                         GetCommentEdit();

private:
	void UpdateIcon();
	void UpdateStatusDot();

	std::unique_ptr<Configuration> m_info;
	bool                           m_running          = false;
	QWidget*                       m_card_widget      = nullptr;
	QLabel*                        m_icon_label       = nullptr;
	QLabel*                        m_name_label       = nullptr;
	QLabel*                        m_subtitle_label   = nullptr;
	QLabel*                        m_status_dot       = nullptr;
	QComboBox*                     m_status_combo     = nullptr;
	QLineEdit*                     m_comment_edit     = nullptr;
};

#endif // CONFIGURATION_ITEM_H