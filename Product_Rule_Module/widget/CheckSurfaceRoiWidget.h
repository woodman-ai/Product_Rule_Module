#pragma once

#include <QWidget>
#include "ui_CheckSurfaceRoiWidget.h"
#include <set>
#include "Product_Rule_TypeDef.h"
class CheckSurfaceRoiWidget : public QWidget
{
	Q_OBJECT
	struct product_path_img
	{
		QString str_name;
		QImage  img;
 	};
	std::set<QString> m_exist_surface_name;
	std::vector<product_path_img> m_product_surface_data;
	std::map<QString, std::vector<QPolygon>> m_product_surface_roi;
	std::map<QString, std::vector<all_roi_color_polygon>> m_other_rule_surface_roi;
	std::set<QString> m_product_valid_surface; //ºÏ≤‚±Ì√Ê
	int m_cur_index = -1;
	void save_roi(int index);
	void change_surface(int index);
	void filter_set_polygon(std::map<QString, std::vector<QPolygon>>& src);
public:
	CheckSurfaceRoiWidget(QWidget *parent = nullptr);
	~CheckSurfaceRoiWidget();
	void start_draw_polygon();
	void stop_draw_polygon();
public:
	void set_product_surfaces(const std::vector<CheckSurface_Contain_ProductSurface::product_img_name_and_qimage>& data);
	void set_polygon(const std::map<QString, std::vector<QPolygon>>& roi,const std::set<QString>& valid_surface,const std::map<QString, std::vector<all_roi_color_polygon>>& other_roi);
	std::tuple<std::map<QString, std::vector<QPolygon>>, std::set<QString>> get_polygon() { save_roi(m_cur_index); return { m_product_surface_roi,m_product_valid_surface }; }
	std::set<QString> get_checksurface_contain_product_surface() const { return m_exist_surface_name; }
	void set_roi_color(QColor);
private:
	Ui::CheckSurfaceRoiWidgetClass ui;
};
