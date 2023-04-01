#pragma once
#include "Product_Rule_TypeDef.h"
#include "product_rule_module_global.h"
#include "qjsonobject.h"
#include <optional>

class PRODUCT_RULE_MODULE_EXPORT Product_Config
{
	static std::vector<product_surface_config> m_surface_config;
public:
	static void clear();
	static void load_product_config(const std::wstring& str_config_dir);
	static void save_to_json(const std::wstring& str_config_dir);
	static void merge_client_config(const QJsonObject& obj);
	static QJsonObject to_json_object();
	static std::optional<product_surface_config> get_surface_config(const std::wstring& surface_name);
	static void set_product_config(const std::vector<product_surface_config>& config_vector);
	static std::vector<product_surface_config> get_all_surface_config();
};
////////////////////////////////////////////////////////////////////////////////////

class PRODUCT_RULE_MODULE_EXPORT Product_Surface_Defect_Feature_Manager
{
	static std::map<std::wstring, Product_Surface_Defect_Info> product_surface_info;
public:
	static void clear();
	static void load_product_surface_config(const std::wstring& str_product_surface_name, const std::wstring& str_product_config_path);
	static void update_product_surface_info_by_QJsonObject(const QJsonObject& obj);
	static QJsonObject cur_product_surface_info_to_json();
	static std::map<std::wstring, Product_Surface_Defect_Info> get_all_product_surface_info();
};

class PRODUCT_RULE_MODULE_EXPORT Product_Surface_Locate_Manager
{
	static std::vector<product_surface_locate> product_surface_locate_info;
	static int draw_width;
	static int draw_height;
public:
	static void load_product_surface_location(const std::wstring& product_path);	
	static std::vector<std::wstring> get_product_surface_related_item(const std::wstring& product_surface_name);
	static std::tuple<int, int> get_draw_size();
	static std::vector<product_surface_locate> get_all_draw_surface();
	static void set_draw_size(int width, int height);
	static void set_product_surface_locate(const std::vector<product_surface_locate>& located);
	static void save_to_json(const std::wstring& product_path);
};

namespace Product_Rule_Module
{
	void PRODUCT_RULE_MODULE_EXPORT load_product_config_and_defect_attribute(const std::wstring& str_product_config_path);
}

