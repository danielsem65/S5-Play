#ifndef GAME_LIST_TREE_WIDGET_H
#define GAME_LIST_TREE_WIDGET_H

#include <QBasicTimer>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QPixmap>
#include <QPoint>
#include <QRadialGradient>
#include <QResizeEvent>
#include <QString>
#include <QTreeWidget>
#include <QtMath>
#include <cstdlib>

class GameListTreeWidget: public QTreeWidget {
public:
	explicit GameListTreeWidget(QWidget* parent = nullptr): QTreeWidget(parent) {
		setAutoFillBackground(false);
		viewport()->setAutoFillBackground(false);
		viewport()->setAttribute(Qt::WA_OpaquePaintEvent, false);
		m_particles.reserve(60);
		for (int i = 0; i < 60; i++) {
			m_particles.append({
			    static_cast<qreal>(std::rand() % 1000) / 1000.0,
			    static_cast<qreal>(std::rand() % 1000) / 1000.0,
			    static_cast<qreal>(std::rand() % 40 + 10) / 100.0,
			    static_cast<qreal>(std::rand() % 100) / 100.0,
			});
		}
		m_anim_timer.start(33, this);
	}

	void SetBackgroundImage(const QString& path) {
		if (m_path == path) {
			return;
		}
		m_path   = path;
		m_source = QPixmap();
		if (!path.isEmpty()) {
			m_source.load(path);
		}
		UpdateScaledBackground();
		viewport()->update();
	}

	void SetHeroText(const QString& title, const QString& subtitle) {
		m_hero_title    = title;
		m_hero_subtitle = subtitle;
		viewport()->update();
	}

	int GetHeroHeight() const { return m_hero_height; }

protected:
	void resizeEvent(QResizeEvent* event) override {
		QTreeWidget::resizeEvent(event);
		UpdateScaledBackground();
	}

	void timerEvent(QTimerEvent* event) override {
		if (event->timerId() == m_anim_timer.timerId()) {
			m_hue = (m_hue + 1) % 360;
			if (m_hue % 2 == 0) {
				for (auto& p : m_particles) {
					p.x += (std::sin(p.y * 12.0 + m_hue * 0.008) * 0.0015);
					p.y += 0.0015;
					if (p.y > 1.0) p.y = 0.0;
					if (p.x > 1.0) p.x = 0.0;
					if (p.x < 0.0) p.x = 1.0;
				}
			}
			viewport()->update();
		}
		QTreeWidget::timerEvent(event);
	}

	void paintEvent(QPaintEvent* event) override {
		QPainter painter(viewport());
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);

		const QRect vr = viewport()->rect();
		m_hero_height   = vr.height() * 55 / 100;

		if (!m_scaled.isNull()) {
			painter.drawPixmap(m_scaled_pos, m_scaled);
		}

		{
			QLinearGradient sky(vr.topLeft(), vr.bottomRight());
			if (!m_scaled.isNull()) {
				sky.setColorAt(0.0, QColor(0, 0, 0, 40));
				sky.setColorAt(0.45, QColor(0, 0, 0, 80));
				sky.setColorAt(0.55, QColor(0, 0, 0, 180));
				sky.setColorAt(1.0, QColor(0, 0, 0, 240));
			} else {
				int h = m_hue;
				sky.setColorAt(0.0, QColor::fromHsl(h % 360, 200, 10));
				sky.setColorAt(0.4, QColor::fromHsl((h + 60) % 360, 160, 13));
				sky.setColorAt(0.7, QColor::fromHsl((h + 140) % 360, 120, 11));
				sky.setColorAt(1.0, QColor::fromHsl((h + 240) % 360, 180, 7));
			}
			painter.fillRect(vr, sky);
		}

		{
			QLinearGradient fade_out(0, m_hero_height - 80, 0, m_hero_height);
			fade_out.setColorAt(0.0, QColor(0, 0, 0, 0));
			fade_out.setColorAt(1.0, QColor(0, 0, 0, 200));
			painter.fillRect(0, m_hero_height - 80, vr.width(), 80, fade_out);
		}

		QRect hero_rect(0, 0, vr.width(), m_hero_height);

		if (m_scaled.isNull() && m_hero_title.isEmpty()) {
			QFont f = painter.font();
			f.setPointSize(20);
			f.setWeight(QFont::ExtraLight);
			painter.setFont(f);
			painter.setPen(QColor(255, 255, 255, 30));
			painter.drawText(hero_rect, Qt::AlignCenter, QStringLiteral("S5 PLAY"));

			f.setPointSize(13);
			f.setWeight(QFont::Light);
			painter.setFont(f);
			painter.setPen(QColor(255, 255, 255, 15));
			painter.drawText(hero_rect.adjusted(0, 40, 0, 0), Qt::AlignCenter,
			                 QStringLiteral("Add a game folder to get started"));
		}

		if (!m_hero_title.isEmpty()) {
			QFont f;
			f.setPointSize(36);
			f.setWeight(QFont::Bold);
			painter.setFont(f);
			painter.setPen(QColor(255, 255, 255, 220));
			QRect title_r = hero_rect.adjusted(48, 0, -48, 0);
			painter.drawText(title_r, Qt::AlignLeft | Qt::AlignBottom, m_hero_title);

			if (!m_hero_subtitle.isEmpty()) {
				title_r.adjust(0, -48, 0, 0);
				f.setPointSize(15);
				f.setWeight(QFont::Normal);
				painter.setFont(f);
				painter.setPen(QColor(255, 255, 255, 120));
				painter.drawText(title_r.adjusted(2, 0, 0, 0), Qt::AlignLeft | Qt::AlignBottom,
				                 m_hero_subtitle);
			}
		}

		for (const auto& p : m_particles) {
			if (p.y * vr.height() > m_hero_height) continue;
			qreal px    = p.x * vr.width();
			qreal py    = p.y * vr.height();
			qreal size  = p.size * 1.5;
			int   alpha = static_cast<int>(p.alpha * 255 *
			                               (0.5 + 0.5 * std::sin(m_hue * 0.02 + p.x * 10.0)));
			if (alpha < 20) continue;
			QRadialGradient dot(QPointF(px, py), size);
			dot.setColorAt(0.0, QColor(255, 255, 255, alpha));
			dot.setColorAt(0.4, QColor(180, 200, 255, alpha / 3));
			dot.setColorAt(1.0, QColor(255, 255, 255, 0));
			painter.fillRect(QRectF(px - size, py - size, size * 2, size * 2), dot);
		}

		{
			QPainterPath clip_path;
			clip_path.addRect(0, m_hero_height, vr.width(), vr.height() - m_hero_height);
			painter.setClipPath(clip_path);
		}

		QTreeWidget::paintEvent(event);
	}

private:
	void UpdateScaledBackground() {
		m_scaled = QPixmap();
		if (m_source.isNull() || viewport()->size().isEmpty()) {
			return;
		}
		m_scaled = m_source.scaled(viewport()->size(), Qt::KeepAspectRatioByExpanding,
		                           Qt::SmoothTransformation);
		m_scaled_pos = QPoint((viewport()->width() - m_scaled.width()) / 2,
		                      (viewport()->height() - m_scaled.height()) / 2);
	}

	struct Particle {
		qreal x, y, size, alpha;
	};

	int m_hero_height = 0;
	QString m_hero_title;
	QString m_hero_subtitle;
	QBasicTimer m_anim_timer;
	int         m_hue = 0;
	QString     m_path;
	QPixmap     m_source;
	QPixmap     m_scaled;
	QPoint      m_scaled_pos;
	QList<Particle> m_particles;
};

#endif // GAME_LIST_TREE_WIDGET_H