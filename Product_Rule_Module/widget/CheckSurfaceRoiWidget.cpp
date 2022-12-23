#include "CheckSurfaceRoiWidget.h"

CheckSurfaceRoiWidget::CheckSurfaceRoiWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.btn_last, &QPushButton::clicked, [this]() {change_surface(m_cur_index - 1); });
	connect(ui.btn_next, &QPushButton::clicked, [this]() {change_surface(m_cur_index + 1); });
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

void CheckSurfaceRoiWidget::set_product_surfaces(const std::vector<CheckSurface_Contain_ProductSurface::product_img_name_and_qimage>& data)
{
	m_product_surface_roi.clear();
	m_product_surface_data.clear();
	m_product_valid_surface.clear();
	m_exist_surface_name.clear();
	for (const auto& item : data)
	{
		product_path_img temp;
		temp.str_name = item.str_product_surface_name;
		temp.img = item.surface_img;
		m_exist_surface_name.insert(temp.str_name);
		m_product_surface_data.push_back(temp);
	}
	set_polygon({}, {}, {});
}

void CheckSurfaceRoiWidget::filter_set_polygon(std::map<QString, std::vector<QPolygon>>& src)
{
	auto iter = src.begin();
	while (iter != src.end())
	{
		auto exist = m_exist_surface_name.find(iter->first);
		if (exist != m_exist_surface_name.end())
		{
			iter++;
		}
		else
		{
			iter = src.erase(iter);
		}
	}
}

void CheckSurfaceRoiWidget::set_polygon(const std::map<QString, std::vector<QPolygon>>& roi,const std::set<QString>& valid_surface,const std::map<QString, std::vector<all_roi_color_polygon>>& other_roi)
{
	m_product_surface_roi = roi;
	m_product_valid_surface = valid_surface;
	m_other_rule_surface_roi = other_roi;
	filter_set_polygon(m_product_surface_roi);
	m_cur_index = -1;
	ui.roi_widget->set_polygon({}, {});
	change_surface(0);
}

void CheckSurfaceRoiWidget::set_roi_color(QColor color)
{
	ui.roi_widget->set_roi_color(color);
}

void CheckSurfaceRoiWidget::save_roi(int index)
{
	if (index >= 0 && index < m_product_surface_data.size())
	{
		auto polygon = ui.roi_widget->get_polygon();
		auto name = m_product_surface_data[index].str_name;
		if (polygon && polygon->size() != 0 )
		{
			m_product_surface_roi[name] = *polygon;
		}
		else
		{
			m_product_surface_roi.erase(name);				
		}

		bool is_checked = ui.check_product_surface->isChecked();
		if (is_checked)
		{
			m_product_valid_surface.insert(name);
		}
		else
		{
			m_product_valid_surface.erase(name);
		}
	}
}

void CheckSurfaceRoiWidget::change_surface(int index)
{
	if (index < 0 || index >= m_product_surface_data.size())
		return;

	save_roi(m_cur_index);
	m_cur_index = index;
	auto name = m_product_surface_data[index].str_name;
	ui.check_product_surface->setText(name);
	auto iter_valid = m_product_valid_surface.find(name);
	ui.check_product_surface->setChecked(iter_valid != m_product_valid_surface.end());
	ui.roi_widget->set_roi_img(m_product_surface_data[index].img);

	std::vector<QPolygon> cur_poly;
	std::vector<all_roi_color_polygon> other_polygon;

	auto iter = m_product_surface_roi.find(name);
	if (iter != m_product_surface_roi.end())
	{
		cur_poly = iter->second;
	}
	
	auto iter_other = m_other_rule_surface_roi.find(name);
	if (iter_other != m_other_rule_surface_roi.end())
	{
		other_polygon = iter_other->second;
	}
	if(cur_poly.size() == 0 )
		ui.roi_widget->set_polygon({}, other_polygon);
	else
		ui.roi_widget->set_polygon(cur_poly, other_polygon);
}
