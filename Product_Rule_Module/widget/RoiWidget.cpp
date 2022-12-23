#include "RoiWidget.h"
#include "qpainter.h"
#include "qevent.h"
#include "qmenu.h"
#include "qapplication.h"
RoiWidget::RoiWidget(QWidget* parent)
	: QWidget(parent)
{
	setMouseTracking(true);
}

RoiWidget::~RoiWidget()
{}



void RoiWidget::init_roi_position()
{
	if (m_roi_img.isNull())
		return;
	m_scale_assist.set_src_size(m_roi_img.size());
	m_scale_assist.init();
	m_opt_scaled_img = {};
	update();
}

void RoiWidget::resizeEvent(QResizeEvent* event)
{
	m_scale_assist.set_widget_size(rect().size());
	if (!m_roi_img.isNull())
	{
		m_scale_assist.init();
		m_opt_scaled_img = {};
		update();
	}
}

void RoiWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	auto rect_pix = m_scale_assist.get_scaled_img_rect();
	painter.setBrush({});
	painter.setPen(QPen(QColor(255, 0, 0)));
	if (!m_roi_img.isNull())
	{
		if (m_opt_scaled_img == std::nullopt)
			m_opt_scaled_img = m_roi_img.scaled(rect_pix.size().toSize());
		painter.drawImage(rect_pix.topLeft(), *m_opt_scaled_img);
	}

	if (m_other_rule_polygon)
	{
		for (const auto& polygon : *m_other_rule_polygon)
		{
			QColor color = polygon.str_color;;
			color.setAlpha(128);
			painter.setBrush(QBrush(color));
			for (const auto& _poly : polygon.all_polygon)
			{
				QPolygon poly;
				for (const auto& pt : _poly)
				{
					auto target = m_scale_assist.src_2_scaled(pt).toPoint();
					poly.append(target);
				}
				painter.drawPolygon(poly);
			}
		}
	}

	QColor color = m_color_roi;;
	color.setAlpha(128);
	painter.setBrush(QBrush(color));

	if (m_all_polygon)
	{
		for (const auto& polygon : *m_all_polygon)
		{
			QPolygon poly;
			for (const auto& pt : polygon)
			{
				auto target = m_scale_assist.src_2_scaled(pt).toPoint();
				poly.append(target);
			}
			painter.drawPolygon(poly);

		}
	}

	if (m_draw_polygon)
	{
		QPolygon poly;
		for (const auto& pt : *m_draw_polygon)
		{
			auto target = m_scale_assist.src_2_scaled(pt).toPoint();
			poly.append(target);
		}

		if (m_opt_next_point)
		{
			auto target = m_scale_assist.src_2_scaled(*m_opt_next_point).toPoint();
			poly.append(target);
		}
		painter.drawPolygon(poly);
	}

	if (m_all_polygon == std::nullopt & m_draw_polygon == std::nullopt)
	{
		painter.setPen(QPen(Qt::red));
		painter.drawText(rect_pix, QStringLiteral("Î´Éè¶¨roi"));
	}
}

void RoiWidget::stop_draw_polygon()
{
	m_bdraw_polygon = false;
	if (m_draw_polygon)
	{
		if (m_draw_polygon->size() > 2)
		{
			if (m_all_polygon == std::nullopt)
				m_all_polygon = std::vector<QPolygon>();
			m_all_polygon->push_back(*m_draw_polygon);
		}
	}
	m_draw_polygon = {};
	m_opt_next_point = {};
	update();
}

void RoiWidget::delete_polygon(QPoint img_pos, QPoint gpos)
{
	if (m_all_polygon)
	{
		int i = 0;
		bool bin = false;
		for (; i < m_all_polygon->size(); i++)
		{
			const auto& poly = (*m_all_polygon)[i];

			auto point = m_scale_assist.scaled_2_src(img_pos).toPoint();
			if (poly.containsPoint(point, Qt::FillRule::OddEvenFill))
			{
				bin = true;
				break;
			}
		}

		if (bin)
		{
			QMenu menu;
			auto action = menu.addAction(QStringLiteral("É¾³ýÇøÓò"));
			connect(action, &QAction::triggered, this, [this, i]() {this->delete_polygon(i); });
			menu.exec(gpos);
		}
	}
}

void RoiWidget::mousePressEvent(QMouseEvent* event)
{
	bool bctrl = QApplication::keyboardModifiers() & Qt::ControlModifier;
	if (bctrl && event->button() == Qt::MouseButton::RightButton)
	{
		init_roi_position();
		return;
	}

	if (m_bdraw_polygon)
	{
		auto valid_rect = m_scale_assist.get_scaled_img_rect().toRect();
		if (event->button() == Qt::MouseButton::LeftButton && valid_rect.contains(event->pos()))
		{
			if (m_draw_polygon == std::nullopt)
				m_draw_polygon = QPolygon();
			auto target = m_scale_assist.scaled_2_src(event->pos()).toPoint();
			m_draw_polygon->push_back(target);
		}
		else if (event->button() == Qt::MouseButton::RightButton)
		{
			if (m_draw_polygon)
			{
				if (!m_draw_polygon->empty())
				{
					m_draw_polygon->erase(m_draw_polygon->end() - 1);
					if (m_draw_polygon->empty())
						m_draw_polygon = {};
				}
			}
			else
			{
				delete_polygon(event->pos(), event->globalPos());
			}


		}
		else if (event->button() == Qt::MouseButton::MidButton)
		{
			if (m_draw_polygon)
			{
				if (m_draw_polygon->size() > 2)
				{
					if (m_all_polygon == std::nullopt)
						m_all_polygon = std::vector<QPolygon>();
					m_all_polygon->push_back(*m_draw_polygon);
				}
			}
			m_draw_polygon = {};
			m_opt_next_point = {};
		}
		update();
	}
	else
	{
		if (event->button() == Qt::MouseButton::LeftButton)
		{
			m_opt_last_clicked = event->pos();
			m_opt_last_scaled_offset = m_scale_assist.get_lefttop().toPoint();

		}
		else if (event->button() == Qt::MouseButton::RightButton)
		{
			delete_polygon(event->pos(), event->globalPos());
		}
	}
}

void RoiWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (m_bdraw_polygon)
	{
		auto pt = m_scale_assist.scaled_2_src(event->pos());
		m_opt_next_point = pt.toPoint();
		update();
	}
	else
	{
		if (m_opt_last_scaled_offset && m_opt_last_clicked)
		{
			m_scale_assist.set_lefttop(*m_opt_last_scaled_offset + event->pos() - *m_opt_last_clicked);
			update();
		}
	}
}

void RoiWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		m_opt_last_clicked = {};
		m_opt_last_scaled_offset = {};
	}
}

void RoiWidget::delete_polygon(int index)
{
	if (m_all_polygon)
	{
		auto& vect = *m_all_polygon;
		if (index >= 0 && index < vect.size())
		{
			vect.erase(vect.begin() + index);
		}

		if (vect.empty())
			m_all_polygon = {};

		update();
	}
}

void RoiWidget::wheelEvent(QWheelEvent* event)
{
	auto now_widget_point = event->pos();
	auto valid_rect = m_scale_assist.get_scaled_img_rect();
	if (!valid_rect.contains(now_widget_point))
		return;

	m_scale_assist.scaled_by_point(event->pos(), event->delta() > 0);
	m_opt_scaled_img = {};
	update();
}
