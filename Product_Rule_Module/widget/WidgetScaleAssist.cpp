#include "WidgetScaleAssist.h"


double WidgetScaleAssist::calc_scaled_ratio(const QSizeF& src_size, const QSizeF& cur_size)
{

	double imgAspectRatio = (double)src_size.width() / src_size.height();
	double wgtAspectRatio = (double)cur_size.width() / cur_size.height();
	if (imgAspectRatio > wgtAspectRatio)
		return (double)cur_size.width() / src_size.width();
	else
		return (double)cur_size.height() / src_size.height();
}

QPointF WidgetScaleAssist::src_2_scaled(const QPointF& src, const QPointF& pt_lefttop, double ratio)
{
	return src * ratio + pt_lefttop;
}

QPointF WidgetScaleAssist::scaled_2_src(const QPointF& scaled, const QPointF& pt_lefttop, double ratio)
{
	return (scaled - pt_lefttop) / ratio;
}

void WidgetScaleAssist::init(bool calc_scale)
{
	if(calc_scale)
		m_scaled_ratio = calc_scaled_ratio(m_src_img_size, m_widget_size);

	m_scaled_size.setWidth(m_src_img_size.width() * m_scaled_ratio);
	m_scaled_size.setHeight(m_src_img_size.height() * m_scaled_ratio);
	auto size_widget = m_widget_size;
	auto size_delta = size_widget - m_scaled_size;
	m_pt_lefttop.setX(size_delta.width() / 2);
	m_pt_lefttop.setY(size_delta.height() / 2);
}

QRectF WidgetScaleAssist::calc_valid_rect() const
{
	auto rt_scaled = get_scaled_img_rect();
	QRectF rt_cur(0, 0, m_widget_size.width(), m_widget_size.height());
	return rt_scaled.intersected(rt_cur);
}

QPointF WidgetScaleAssist::src_2_scaled(const QPointF& src) const
{
	return src_2_scaled(src, m_pt_lefttop, m_scaled_ratio);
}

QPointF WidgetScaleAssist::scaled_2_src(const QPointF& scaled) const
{
	return scaled_2_src(scaled, m_pt_lefttop, m_scaled_ratio);
}

QPixmap WidgetScaleAssist::src_img_scaled(const QImage& img) const 
{
	auto pixmap = QPixmap::fromImage(img);
	return pixmap.scaledToWidth(m_scaled_size.width(), Qt::SmoothTransformation);
}

QImage WidgetScaleAssist::src_img_scaled_qimage(const QImage& img) const
{
	return img.scaledToWidth(m_scaled_size.width(), Qt::SmoothTransformation);
}

QRectF  WidgetScaleAssist::src_2_scaled(const QRectF& rt) const
{
	auto lt = src_2_scaled(rt.topLeft());
	auto rb = src_2_scaled(rt.bottomRight());
	return { lt,rb };
}

QRectF  WidgetScaleAssist::scaled_2_src(const QRectF& rt) const
{
	auto lt = scaled_2_src(rt.topLeft());
	auto rb = scaled_2_src(rt.bottomRight());
	return { lt,rb };
}

void   WidgetScaleAssist::scaled_by_point(const QPointF& cur_pt, bool bscale_up)
{
	double new_scale = m_scaled_ratio;
	double delta = m_scaled_ratio * m_scaed_ratio_delta;
	new_scale += bscale_up  ? delta : -delta;
	auto src_point = scaled_2_src(cur_pt);
	auto new_widget_point = src_point * new_scale;
	auto new_offset = cur_pt - new_widget_point;
	m_pt_lefttop = new_offset.toPoint();
	m_scaled_ratio = new_scale;
	m_scaled_size.setWidth(m_src_img_size.width() * m_scaled_ratio);
	m_scaled_size.setHeight(m_src_img_size.height() * m_scaled_ratio);	
}

QRectF WidgetScaleAssist::create_rect_by_2_point(const QPointF& pt1, const QPointF& pt2)
{
	QPointF lt = { std::min(pt1.x(),pt2.x()),std::min(pt1.y(),pt2.y()) };
	QPointF rb = { std::max(pt1.x(),pt2.x()),std::max(pt1.y(),pt2.y()) };
	return { lt,rb };
}

void WidgetScaleAssist::start_drag(QPointF click_point)
{
	m_opt_last_draw_left_top = m_pt_lefttop;
	m_opt_last_click_scaled_pt = click_point;
}

void WidgetScaleAssist::drag_to(QPointF pt)
{
	if(m_opt_last_draw_left_top && m_opt_last_click_scaled_pt)
		set_lefttop(*m_opt_last_draw_left_top + pt - *m_opt_last_click_scaled_pt);
}

void WidgetScaleAssist::reset_drag_point()
{
	m_opt_last_draw_left_top = {};
	m_opt_last_click_scaled_pt = {};
}