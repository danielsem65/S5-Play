#include "configurationItem.h"

#include "configuration.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QSize>
#include <QStringList>
#include <QStyle>
#include <QTreeWidget>
#include <QVersionNumber>
#include <QWheelEvent>
#include <QWidget>

namespace {

enum Column {
	CardColumn,
};

class NoWheelComboBox: public QComboBox {
public:
	explicit NoWheelComboBox(QWidget* parent = nullptr): QComboBox(parent) {}

protected:
	void wheelEvent(QWheelEvent* event) override { event->ignore(); }
};

QString GetStatusText(Configuration::GameStatus status) {
	switch (status) {
		case Configuration::GameStatus::Unknown: return QStringLiteral("Unknown");
		case Configuration::GameStatus::MainMenu: return QStringLiteral("Main Menu");
		case Configuration::GameStatus::InGame: return QStringLiteral("Playable");
		case Configuration::GameStatus::Logo: return QStringLiteral("Boots to Logo");
		case Configuration::GameStatus::DoesntBoot: return QStringLiteral("Doesn't Boot");
	}

	return QStringLiteral("Unknown");
}

QString GetStatusColor(Configuration::GameStatus status) {
	switch (status) {
		case Configuration::GameStatus::InGame: return QStringLiteral("#2fb344");
		case Configuration::GameStatus::MainMenu: return QStringLiteral("#2f80ed");
		case Configuration::GameStatus::Logo: return QStringLiteral("#f2c94c");
		case Configuration::GameStatus::DoesntBoot: return QStringLiteral("#e55353");
		case Configuration::GameStatus::Unknown: return QStringLiteral("#6b6b80");
	}

	return QStringLiteral("#6b6b80");
}

void AddStatus(QComboBox* combo, Configuration::GameStatus status) {
	combo->addItem(GetStatusText(status), static_cast<int>(status));
}

void SetStatus(QComboBox* combo, Configuration::GameStatus status) {
	const int index = combo->findData(static_cast<int>(status));
	combo->setCurrentIndex(index >= 0 ? index : 0);
}

QIcon StandardIcon(QStyle::StandardPixmap icon) {
	return QApplication::style()->standardIcon(icon);
}

void MakeTransparent(QWidget* widget) {
	widget->setAutoFillBackground(false);
	widget->setAttribute(Qt::WA_NoSystemBackground);
	widget->setAttribute(Qt::WA_TranslucentBackground);
}

} // namespace

ConfigurationItem::ConfigurationItem(std::unique_ptr<Configuration> info, QTreeWidget* parent)
    : QTreeWidgetItem(parent), m_info(std::move(info)) {
	setSizeHint(CardColumn, QSize(0, 64));

	m_card_widget = new QWidget(parent);
	MakeTransparent(m_card_widget);
	auto* layout = new QHBoxLayout(m_card_widget);
	layout->setContentsMargins(16, 8, 16, 8);
	layout->setSpacing(12);

	m_icon_label = new QLabel(m_card_widget);
	m_icon_label->setFixedSize(QSize(48, 48));
	layout->addWidget(m_icon_label);

	auto* text_layout = new QVBoxLayout;
	text_layout->setSpacing(2);
	text_layout->setContentsMargins(0, 0, 0, 0);

	m_name_label = new QLabel(m_card_widget);
	m_name_label->setStyleSheet(QStringLiteral(
	    "font-size: 14px; font-weight: 600; color: #ffffff; background: transparent;"));
	text_layout->addWidget(m_name_label);

	m_subtitle_label = new QLabel(m_card_widget);
	m_subtitle_label->setStyleSheet(QStringLiteral(
	    "font-size: 11px; color: rgba(255,255,255,0.3); background: transparent;"));
	text_layout->addWidget(m_subtitle_label);

	layout->addLayout(text_layout, 1);

	m_status_dot = new QLabel(m_card_widget);
	m_status_dot->setFixedSize(QSize(8, 8));
	layout->addWidget(m_status_dot);

	m_status_combo = new NoWheelComboBox(m_card_widget);
	MakeTransparent(m_status_combo);
	AddStatus(m_status_combo, Configuration::GameStatus::Unknown);
	AddStatus(m_status_combo, Configuration::GameStatus::MainMenu);
	AddStatus(m_status_combo, Configuration::GameStatus::InGame);
	AddStatus(m_status_combo, Configuration::GameStatus::Logo);
	AddStatus(m_status_combo, Configuration::GameStatus::DoesntBoot);
	m_status_combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_status_combo->setFixedWidth(100);
	m_status_combo->setStyleSheet(QStringLiteral(
	    "QComboBox { background: rgba(255,255,255,0.04); border: 1px solid rgba(255,255,255,0.06); "
	    "border-radius: 12px; color: rgba(255,255,255,0.5); font-size: 11px; padding: 4px 10px; }"
	    "QComboBox:hover { border-color: rgba(0,79,255,0.3); }"
	    "QComboBox::drop-down { border: none; width: 16px; }"
	    "QComboBox QAbstractItemView { background: #1a1b22; color: #ffffff; "
	    "border: 1px solid rgba(0,79,255,0.2); border-radius: 8px; padding: 4px; "
	    "selection-background-color: rgba(0,79,255,0.3); outline: none; }"));
	layout->addWidget(m_status_combo);

	m_comment_edit = new QLineEdit(m_card_widget);
	MakeTransparent(m_comment_edit);
	m_comment_edit->setClearButtonEnabled(true);
	m_comment_edit->setFrame(false);
	m_comment_edit->setPlaceholderText(QStringLiteral("Add note..."));
	m_comment_edit->setStyleSheet(QStringLiteral(
	    "QLineEdit { background: rgba(255,255,255,0.03); border: 1px solid transparent; "
	    "border-radius: 12px; color: rgba(255,255,255,0.3); font-size: 11px; padding: 4px 10px; "
	    "max-width: 120px; selection-background-color: #004fff; }"
	    "QLineEdit:focus { border-color: rgba(0,79,255,0.3); "
	    "background: rgba(0,79,255,0.06); color: #ffffff; }"));
	layout->addWidget(m_comment_edit);

	parent->setItemWidget(this, CardColumn, m_card_widget);

	Update();
	SetRunning(false);
}

ConfigurationItem::~ConfigurationItem() = default;

void ConfigurationItem::Update() {
	m_name_label->setText(m_info->name);

	QString subtitle;
	if (!m_info->title_id.isEmpty()) {
		subtitle = m_info->title_id;
	}
	if (!m_info->firmwareVer.isEmpty()) {
		if (!subtitle.isEmpty()) subtitle += QStringLiteral("  \u00B7  ");
		subtitle += QStringLiteral("FW ") + m_info->firmwareVer;
	}
	m_subtitle_label->setText(subtitle);

	SetStatus(m_status_combo, m_info->game_status);
	if (m_comment_edit->text() != m_info->game_comment) {
		m_comment_edit->setText(m_info->game_comment);
	}

	UpdateIcon();
	UpdateStatusDot();
}

bool ConfigurationItem::operator<(const QTreeWidgetItem& other) const {
	const auto* other_item = dynamic_cast<const ConfigurationItem*>(&other);
	if (other_item == nullptr) {
		return QTreeWidgetItem::operator<(other);
	}
	return m_info->name.toCaseFolded() < other_item->m_info->name.toCaseFolded();
}

void ConfigurationItem::SetRunning(bool state) {
	m_running = state;
	UpdateIcon();
}

void ConfigurationItem::SetCompatibilityEditable(bool editable) {
	m_status_combo->setEnabled(editable);
	m_comment_edit->setReadOnly(!editable);
	m_comment_edit->setClearButtonEnabled(editable);
}

void ConfigurationItem::UpdateIcon() {
	const QString icon_file = QDir(m_info->basedir).filePath(QStringLiteral("sce_sys/icon0.png"));
	if (QFileInfo::exists(icon_file)) {
		QPixmap pix(icon_file);
		if (!pix.isNull()) {
			QPixmap scaled = pix.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			QPixmap rounded(scaled.size());
			rounded.fill(Qt::transparent);
			QPainter p(&rounded);
			p.setRenderHint(QPainter::Antialiasing);
			QPainterPath path;
			path.addRoundedRect(QRectF(0, 0, 48, 48), 12, 12);
			p.setClipPath(path);
			p.drawPixmap(0, 0, scaled);
			p.setPen(QPen(QColor(255, 255, 255, 16), 1));
			p.setBrush(Qt::NoBrush);
			p.drawRoundedRect(QRectF(0.5, 0.5, 47, 47), 12, 12);
			p.end();
			m_icon_label->setPixmap(rounded);
			return;
		}
	}

	QPixmap fallback(48, 48);
	fallback.fill(Qt::transparent);
	QPainter p(&fallback);
	p.setRenderHint(QPainter::Antialiasing);
	QPainterPath path;
	path.addRoundedRect(QRectF(0, 0, 48, 48), 12, 12);
	p.fillPath(path, QColor(255, 255, 255, 8));
	p.setPen(QPen(QColor(255, 255, 255, 12), 1));
	p.drawRoundedRect(QRectF(0.5, 0.5, 47, 47), 12, 12);
	p.end();
	m_icon_label->setPixmap(fallback);
}

void ConfigurationItem::UpdateStatusDot() {
	const auto color = GetStatusColor(m_info->game_status);
	m_status_dot->setStyleSheet(
	    QStringLiteral(
	        "background-color: %1; border-radius: 4px;"
	        "min-width: 8px; max-width: 8px; min-height: 8px; max-height: 8px;")
	        .arg(color));
	m_status_dot->setToolTip(GetStatusText(m_info->game_status));
}

QComboBox* ConfigurationItem::GetStatusCombo() { return m_status_combo; }
QLineEdit* ConfigurationItem::GetCommentEdit() { return m_comment_edit; }