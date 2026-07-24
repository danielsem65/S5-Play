#ifndef GAME_LIST_TREE_WIDGET_H
#define GAME_LIST_TREE_WIDGET_H

#include <QBasicTimer>
#include <QColor>
#include <QLinearGradient>
#include <QPaintEvent>
#include <QPainter>
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

		const QRect r = event->rect();

		if (m_scaled.isNull()) {
			QLinearGradient sky(r.topLeft(), r.bottomRight());
			int h = m_hue;
			sky.setColorAt(0.0, QColor::fromHsl(h % 360, 180, 12));
			sky.setColorAt(0.3, QColor::fromHsl((h + 60) % 360, 160, 16));
			sky.setColorAt(0.6, QColor::fromHsl((h + 120) % 360, 140, 14));
			sky.setColorAt(1.0, QColor::fromHsl((h + 240) % 360, 200, 8));
			painter.fillRect(r, sky);
		} else {
			painter.drawPixmap(m_scaled_pos, m_scaled);
			QLinearGradient overlay(r.topLeft(), r.bottomRight());
			overlay.setColorAt(0.0, QColor(0, 0, 0, 170));
			overlay.setColorAt(0.4, QColor(0, 0, 0, 100));
			overlay.setColorAt(1.0, QColor(20, 21, 26, 230));
			painter.fillRect(r, overlay);
		}

		for (const auto& p : m_particles) {
			qreal px = p.x * r.width();
			qreal py = p.y * r.height();
			qreal size = p.size * 1.5;
			int alpha = static_cast<int>(p.alpha * 255 * (0.5 + 0.5 * std::sin(m_hue * 0.02 + p.x * 10.0)));
			if (alpha < 20) continue;
			QRadialGradient dot(QPointF(px, py), size);
			dot.setColorAt(0.0, QColor(255, 255, 255, alpha));
			dot.setColorAt(0.4, QColor(180, 200, 255, alpha / 3));
			dot.setColorAt(1.0, QColor(255, 255, 255, 0));
			painter.fillRect(QRectF(px - size, py - size, size * 2, size * 2), dot);
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

	QBasicTimer   m_anim_timer;
	int           m_hue = 0;
	QString       m_path;
	QPixmap       m_source;
	QPixmap       m_scaled;
	QPoint        m_scaled_pos;
	QList<Particle> m_particles;
};

#endif // GAME_LIST_TREE_WIDGET_H