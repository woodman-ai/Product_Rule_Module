#pragma once

#include <QWidget>
#include "ui_CheckSurfaceRoiWidget.h"
#include <set>
#include "Product_Rule_TypeDef.h"
class CheckSurfaceRoiWidget : public QWidget
{
	Q_OBJECT
	QImage  m_product_surface_image;
	std::vector<QPolygon> m_product_surface_roi;
	std::vector<all_roi_color_polygon> m_other_rule_surface_roi;
	void save_roi();
	void change_surface();
public:
	CheckSurfaceRoiWidget(QWidget *parent = nullptr);
	~CheckSurfaceRoiWidget();
	void start_draw_polygon();
	void stop_draw_polygon();
public:
	void set_product_surface_img(const QImage& img);
	void set_polygon(const std::vector<QPolygon>& roi,const std::vector<all_roi_color_polygon>& other_roi);
	std::vector<QPolygon> get_polygon() { save_roi(); return m_product_surface_roi; }
	void set_roi_color(QColor);
private:
	Ui::CheckSurfaceRoiWidgetClass ui;
};
