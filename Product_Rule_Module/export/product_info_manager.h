#pragma once
#include "product_rule_module_global.h"
#include "Product_Rule_TypeDef.h"
#include "qstring.h"
#include <map>
#include <vector>
#include <optional>
using product_surface = int;
using product_test_surface = int;
class PRODUCT_RULE_MODULE_EXPORT product_info_manager
{
	std::map<product_surface, QString> m_product_surface_map_id_2_name;
	std::map<QString, product_surface> m_product_surface_map_name_2_id;
	std::map<QString, surface_train_backed> m_product_surface_map_name_2_trainbackend;
	std::map<product_surface, surface_train_backed> m_product_surface_map_id_2_trainbackend;

	std::map<product_test_surface, QString> m_product_test_surface_map_id_2_name;
	std::map<QString, product_test_surface> m_product_test_surface_map_name_2_id;

	std::map<product_surface, product_test_surface> m_product_surface_2_test_surface;
	QSize m_draw_pix_size;
public:
	struct draw_related_surface
	{
		QString str_surface_name;
		QRect rt;
		std::vector<product_surface> sub_surface;
	};
private:
	std::vector<draw_related_surface> m_surface_ralated_sub_surface;
	void clear_data();
	
public:
	product_info_manager();
	void init_product_info(const QString& str_json_config_path);
	product_surface get_product_surface_id(const QString& name);
	QString  get_product_surface_name(product_surface id);
	product_test_surface get_product_test_surface_id(const QString& name);
	QString  get_product_test_surface_name(product_surface id);
	product_test_surface get_product_test_surface(product_surface surface_id);
	std::vector<product_surface> get_related_surface(const QString& surface_name);
	int get_surface_count() const;

	std::tuple<std::map<product_surface, QString>, std::map<QString, product_surface>>
	get_product_surface_name_maps();

	std::tuple<std::map<product_test_surface, QString>, std::map<QString, product_test_surface>>
	get_product_test_surface_name_maps();

	std::map<product_surface, product_test_surface> get_product_surface_2_test_surface_map();


	std::vector<draw_related_surface> get_product_related_surface();
	draw_related_surface get_product_draw_surface(const QString& str_surface_name);
	QSize get_draw_pixel_size() const { return m_draw_pix_size; }
	std::optional<surface_train_backed> get_product_surface_train_backend_info(product_surface surface_id);
	std::optional<surface_train_backed> get_product_surface_train_backend_info(QString surface_name);
	QStringList enum_all_product_surface() const;

};

class PRODUCT_RULE_MODULE_EXPORT CheckSurfaceManager
{
	std::vector<CheckSurface_Contain_ProductSurface> m_contain_surface;
public:
	void init_from_json_file(const QString& str_json_config);
	void save_to_json_file(const QString& str_json_config);
	std::vector<CheckSurface_Contain_ProductSurface::product_img_name_and_qimage>
		get_checksurface_contains_data(const QString& str_check_surface_name) const;
	std::optional<CheckSurface_Contain_ProductSurface> get_check_surface_data(const QString& str_check_surface_name) const;
	std::vector<QString> enum_check_surface() const;
	int get_check_surface_index(const QString& str_check_surface_name) const;
	void update_check_surface_defects(const QString& str_check_surface_name, const std::vector<QString>& defect_vector);
	void update_check_surface_defects_by_client_model(const QJsonObject& obj, const QString& str_config_path);
	void update_check_surface_image_name(const QString& str_check_surface_name, const std::vector<QString>& image_name_vector);
};

QJsonObject PRODUCT_RULE_MODULE_EXPORT load_check_surface_defect_from_model(const QString& str_merge_config_path);

