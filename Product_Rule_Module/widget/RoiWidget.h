#pragma once

#include <QWidget>
#include <optional>
#include "WidgetScaleAssist.h"

struct all_roi_color_polygon
{
	QString str_color;
	std::vector<QPolygon> all_polygon;
};
class RoiWidget  : public QWidget
{
	Q_OBJECT
	QImage m_roi_img;
	std::optional<std::vector<QPolygon>> m_all_polygon;
	std::optional<std::vector<all_roi_color_polygon>> m_other_rule_polygon;
	std::optional<QPolygon> m_draw_polygon;
	std::optional<QPoint>   m_opt_next_point;
	bool m_bdraw_polygon = false;
	void init_roi_position();
	void delete_polygon(QPoint img_pos, QPoint gpos);
	void delete_polygon(int index);
	WidgetScaleAssist m_scale_assist;
	std::optional<QImage> m_opt_scaled_img;
	QColor m_color_roi ="#ff0000";
private:
	std::optional<QPoint> m_opt_last_clicked;
	std::optional<QPoint> m_opt_last_scaled_offset;
signals:
	void sig_stop_draw();
public:
	void set_roi_img(const QImage& img) { m_roi_img = img; init_roi_position(); }
	void set_polygon(std::optional<std::vector<QPolygon>> opt_polygon , std::optional<std::vector<all_roi_color_polygon>> opt_other_rule)
	{
		m_all_polygon = opt_polygon;
		m_other_rule_polygon = opt_other_rule;
		update();
	}

	std::optional<std::vector<QPolygon>> get_polygon() const { return m_all_polygon; }
	void clear_select_roi() { m_all_polygon = {}; update(); }
	void start_draw_polygon() { m_bdraw_polygon = true; }
	void stop_draw_polygon();
	void set_roi_color(QColor color) { m_color_roi = color; update(); }
	RoiWidget(QWidget *parent);
	~RoiWidget();
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
};
