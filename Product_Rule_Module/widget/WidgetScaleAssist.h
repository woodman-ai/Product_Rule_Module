#pragma once
#include <qsize.h>
#include <qrect.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <optional>
class WidgetScaleAssist
{
	QPointF m_pt_lefttop;
	QSizeF m_src_img_size = {};
	QSizeF m_widget_size = {};
	QSizeF m_scaled_size = {};
	double m_scaled_ratio = 1;
	double m_scaed_ratio_delta = 0.1;
	static double calc_scaled_ratio(const QSizeF& src_size, const QSizeF& cur_size);
	static QPointF src_2_scaled(const QPointF& src, const QPointF& pt_lefttop, double ratio);
	static QPointF scaled_2_src(const QPointF& scaled, const QPointF& pt_lefttop, double ratio);

	std::optional<QPointF> m_opt_last_draw_left_top;
	std::optional<QPointF> m_opt_last_click_scaled_pt;;
public:
	bool  is_src_size_valid() const { return m_src_img_size.isValid(); }
	void  set_src_size(const QSizeF& size) { m_src_img_size = size; }
	void set_widget_size(const QSizeF& size) { m_widget_size = size; }
	QRectF calc_valid_rect() const;
	QSizeF get_scaled_img_size() const { return m_scaled_size; }
	QRectF get_scaled_img_rect() const { return QRectF(m_pt_lefttop, m_pt_lefttop + QPointF(m_scaled_size.width(), m_scaled_size.height())); }
	void set_scaled_ratio_delta(double delta) { m_scaed_ratio_delta = delta; }
	void set_scaled_ratio(double scale) { m_scaled_ratio = scale; }
	void drag_img_delta(QPointF pt) { m_pt_lefttop = m_pt_lefttop + pt; }
	void init(bool calc_scale = true);
	void set_lefttop(const QPointF& pt) { m_pt_lefttop = pt; }
	QPointF get_lefttop() const { return m_pt_lefttop; }
	QPointF src_2_scaled(const QPointF& src) const;
	QPointF scaled_2_src(const QPointF& scaled) const;
	QPixmap src_img_scaled(const QImage& img) const;
	QImage src_img_scaled_qimage(const QImage& img) const;
	QRectF  src_2_scaled(const QRectF& rt) const;
	QRectF  scaled_2_src(const QRectF& rt) const;
	void   scaled_by_point(const QPointF& cur_pt, bool bscale_up);
	static QRectF create_rect_by_2_point(const QPointF& pt1, const QPointF& pt2);
	double get_scaled_ratio()const { return m_scaled_ratio; }
	void start_drag(QPointF click_point);
	void drag_to(QPointF pt);
	void reset_drag_point();
	QSizeF get_src_size() const { return m_src_img_size; }


};

