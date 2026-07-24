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
	NameColumn,
	SerialColumn,
	FirmwareVersionColumn,
	PathColumn,
	StatusColumn,
	CommentsColumn,
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

QString GetPathText(const Configuration& info) {
	return !info.game_path.isEmpty() ? info.game_path : info.basedir;
}

QString GetDisplayText(const Configuration& info) {
	QStringList lines({info.name});

	if (!info.title_id.isEmpty()) {
		lines.append(QStringLiteral("ID: %1").arg(info.title_id));
	}
	if (!info.firmwareVer.isEmpty()) {
		lines.append(QStringLiteral("FW: %1").arg(info.firmwareVer));
	}

	const auto path = GetPathText(info);
	if (!path.isEmpty()) {
		lines.append(QStringLiteral("Path: %1").arg(path));
	}
	if (!info.game_comment.isEmpty()) {
		lines.append(QStringLiteral("Note: %1").arg(info.game_comment));
	}

	return lines.join(QLatin1Char('\n'));
}

QString GetSortText(const Configuration& info) {
	return QStringLiteral("%1\n%2").arg(info.name.toCaseFolded(), info.game_path.toCaseFolded());
}

void MakeTransparent(QWidget* widget) {
	widget->setAutoFillBackground(false);
	widget->setAttribute(Qt::WA_NoSystemBackground);
	widget->setAttribute(Qt::WA_TranslucentBackground);
}

} // namespace

ConfigurationItem::ConfigurationItem(std::unique_ptr<Configuration> info, QTreeWidget* parent)
    : QTreeWidgetItem(parent), m_info(std::move(info)) {
	setSizeHint(NameColumn, QSize(0, 80));

	m_status_widget = new QWidget(parent);
	MakeTransparent(m_status_widget);
	auto* layout = new QHBoxLayout(m_status_widget);
	layout->setContentsMargins(4, 0, 4, 0);
	layout->setSpacing(8);

	m_status_indicator = new QLabel(m_status_widget);
	m_status_indicator->setFixedSize(QSize(10, 10));
	layout->addWidget(m_status_indicator);

	m_status_combo = new NoWheelComboBox(m_status_widget);
	MakeTransparent(m_status_combo);
	AddStatus(m_status_combo, Configuration::GameStatus::Unknown);
	AddStatus(m_status_combo, Configuration::GameStatus::MainMenu);
	AddStatus(m_status_combo, Configuration::GameStatus::InGame);
	AddStatus(m_status_combo, Configuration::GameStatus::Logo);
	AddStatus(m_status_combo, Configuration::GameStatus::DoesntBoot);
	m_status_combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_status_combo->setFixedWidth(120);
	m_status_combo->setStyleSheet(QStringLiteral(
	    "QComboBox { background: transparent; border: none; color: #b0b0c8; font-size: 12px; padding-left: 2px; }"
	    "QComboBox:focus { background: rgba(255,255,255,0.08); border-radius: 4px; }"
	    "QComboBox::drop-down { border: none; width: 20px; }"
	    "QComboBox QAbstractItemView { background: #1e1f26; color: #ffffff; "
	    "border: 1px solid rgba(255,255,255,0.1); border-radius: 6px; padding: 4px; "
	    "selection-background-color: #004fff; outline: none; }"));
	layout->addWidget(m_status_combo);
	layout->addStretch(1);

	parent->setItemWidget(this, StatusColumn, m_status_widget);

	m_comment_edit = new QLineEdit(parent);
	MakeTransparent(m_comment_edit);
	m_comment_edit->setClearButtonEnabled(true);
	m_comment_edit->setFrame(false);
	m_comment_edit->setStyleSheet(QStringLiteral(
	    "QLineEdit { background: transparent; border: none; color: #b0b0c8; font-size: 12px; padding-left: 2px; "
	    "selection-background-color: #004fff; }"
	    "QLineEdit:focus { background: rgba(255,255,255,0.06); "
	    "border: 1px solid rgba(255,255,255,0.15); border-radius: 4px; }"));
	parent->setItemWidget(this, CommentsColumn, m_comment_edit);

	Update();
	SetRunning(false);
}

ConfigurationItem::~ConfigurationItem() = default;

void ConfigurationItem::Update() {
	const auto display_text = GetDisplayText(*m_info);
	const auto path         = GetPathText(*m_info);

	setText(NameColumn, m_info->name);
	setText(SerialColumn, m_info->title_id);
	setText(FirmwareVersionColumn,
	        m_info->firmwareVer.isEmpty() ? QStringLiteral("\u2014") : m_info->firmwareVer);
	setText(PathColumn, path);
	setText(StatusColumn, {});
	setText(CommentsColumn, {});
	for (int column = NameColumn; column <= CommentsColumn; column++) {
		setToolTip(column, display_text);
	}
	SetStatus(m_status_combo, m_info->game_status);
	if (m_comment_edit->text() != m_info->game_comment) {
		m_comment_edit->setText(m_info->game_comment);
	}

	UpdateIcon();
	UpdateStatusIndicator();
}

bool ConfigurationItem::operator<(const QTreeWidgetItem& other) const {
	const auto* other_item = dynamic_cast<const ConfigurationItem*>(&other);
	if (other_item == nullptr) {
		return QTreeWidgetItem::operator<(other);
	}

	const int column = treeWidget() != nullptr ? treeWidget()->sortColumn() : NameColumn;
	switch (column) {
		case NameColumn: return GetSortText(*m_info) < GetSortText(*other_item->m_info);
		case StatusColumn:
			return GetStatusText(m_info->game_status) <
			       GetStatusText(other_item->m_info->game_status);
		case FirmwareVersionColumn: {
			const auto& version       = m_info->firmwareVer;
			const auto& other_version = other_item->m_info->firmwareVer;
			if (version.isEmpty() || other_version.isEmpty()) {
				return version.isEmpty() && !other_version.isEmpty();
			}
			return QVersionNumber::compare(QVersionNumber::fromString(version),
			                               QVersionNumber::fromString(other_version)) < 0;
		}
		case CommentsColumn:
			return m_info->game_comment.toCaseFolded() <
			       other_item->m_info->game_comment.toCaseFolded();
		default: return text(column).toCaseFolded() < other.text(column).toCaseFolded();
	}
}

void ConfigurationItem::SetRunning(bool state) {
	m_running = state;

	QFont f = font(NameColumn);
	f.setBold(state);
	for (int column = NameColumn; column <= CommentsColumn; column++) {
		setFont(column, f);
	}

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
			QPixmap scaled = pix.scaled(56, 56, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			QPixmap rounded(scaled.size());
			rounded.fill(Qt::transparent);
			QPainter p(&rounded);
			p.setRenderHint(QPainter::Antialiasing);
			QPainterPath path;
			path.addRoundedRect(QRectF(0, 0, 56, 56), 12, 12);
			p.setClipPath(path);
			p.drawPixmap(0, 0, scaled);
			p.setPen(QPen(QColor(255, 255, 255, 20), 1));
			p.setBrush(Qt::NoBrush);
			p.drawRoundedRect(QRectF(0.5, 0.5, 55, 55), 12, 12);
			p.end();
			setIcon(NameColumn, QIcon(rounded));
			return;
		}
	}

	if (m_running) {
		setIcon(NameColumn, StandardIcon(QStyle::SP_MediaPlay));
	} else if (m_info->custom_settings) {
		setIcon(NameColumn, StandardIcon(QStyle::SP_FileIcon));
	} else {
		setIcon(NameColumn, StandardIcon(QStyle::SP_ComputerIcon));
	}
}

void ConfigurationItem::UpdateStatusIndicator() {
	const auto color = GetStatusColor(m_info->game_status);
	m_status_indicator->setStyleSheet(
	    QStringLiteral(
	        "background-color: %1;"
	        "border: 1px solid rgba(255,255,255,0.06);"
	        "border-radius: 5px;"
	        "min-width: 10px; max-width: 10px;"
	        "min-height: 10px; max-height: 10px;")
	        .arg(color));
	m_status_indicator->setToolTip(GetStatusText(m_info->game_status));
}
