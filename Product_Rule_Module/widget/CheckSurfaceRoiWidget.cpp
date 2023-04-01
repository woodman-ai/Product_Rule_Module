#include "CheckSurfaceRoiWidget.h"

CheckSurfaceRoiWidget::CheckSurfaceRoiWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

CheckSurfaceRoiWidget::~CheckSurfaceRoiWidget()
{

}

void CheckSurfaceRoiWidget::start_draw_polygon()
{
	ui.roi_widget->start_draw_polygon();
}

void CheckSurfaceRoiWidget::stop_draw_polygon()
{
	ui.roi_widget->stop_draw_polygon();
}

void CheckSurfaceRoiWidget::set_product_surface_img(const QImage& data)
{
	m_product_surface_roi.clear();
	m_product_surface_image = data;
	set_polygon({}, {});
}


void CheckSurfaceRoiWidget::set_polygon(const std::vector<QPolygon>& roi, const std::vector<all_roi_color_polygon>& other_roi)
{
	m_product_surface_roi = roi;
	m_other_rule_surface_roi = other_roi;
	ui.roi_widget->set_polygon(roi, other_roi);
	change_surface();
}

void CheckSurfaceRoiWidget::set_roi_color(QColor color)
{
	ui.roi_widget->set_roi_color(color);
}

void CheckSurfaceRoiWidget::save_roi()
{
	auto polygon = ui.roi_widget->get_polygon();
	if (polygon && polygon->size() != 0 )
	{
		m_product_surface_roi = *polygon;
	}
	else
	{
		m_product_surface_roi.clear();				
	}	
}

void CheckSurfaceRoiWidget::change_surface()
{
	ui.roi_widget->set_roi_img(m_product_surface_image);
	ui.roi_widget->set_polygon(m_product_surface_roi,m_other_rule_surface_roi);	
}
