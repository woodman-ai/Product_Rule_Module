#include "Product_Rule_Export.h"
#include "utils.h"
#include "qjsonarray.h"

std::vector<product_surface_config> Product_Config::m_surface_config;
void Product_Config::clear()
{
	m_surface_config.clear();
}

void Product_Config::load_product_config(const std::wstring& str_config_dir)
{
	m_surface_config.clear();
	auto path = QString::fromStdWString(str_config_dir) + "\\product_config.json";
	auto obj = read_json_document(path);
	merge_client_config(obj);
}

void Product_Config::save_to_json(const std::wstring& str_config_dir)
{
	auto path = QString::fromStdWString(str_config_dir) + "\\product_config.json";
	auto obj = to_json_object();
	save_json_files(path, obj);
}

void Product_Config::merge_client_config(const QJsonObject& obj)
{
	auto array = obj.value("product_config").toArray();
	for (const auto& item : array)
	{
		m_surface_config.push_back(product_surface_config_from_json(item.toObject()));
	}
}

QJsonObject Product_Config::to_json_object()
{
	QJsonObject obj;
	QJsonArray array;
	for (const auto& item : m_surface_config)
	{
		array.append(product_surface_config_2_json(item));
	}
	obj.insert("product_config", array);
	return obj;
}

std::optional<product_surface_config> Product_Config::get_surface_config(const std::wstring& surface_name)
{
	for (const auto& item : m_surface_config)
	{
		if (item.str_surface_name == surface_name)
		{
			return item;
		}
	}

	return {};
}

void Product_Config::set_product_config(const std::vector<product_surface_config>& config_vector)
{
	m_surface_config = config_vector;
}

std::vector<product_surface_config> Product_Config::get_all_surface_config()
{
	return m_surface_config;
}

/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map<std::wstring, Product_Surface_Defect_Info> Product_Surface_Defect_Feature_Manager::product_surface_info;
void Product_Surface_Defect_Feature_Manager::load_product_surface_config(const std::wstring& str_product_surface_name, const std::wstring& str_product_config_path)
{
	auto surface_name = QString::fromStdWString(str_product_surface_name);
	auto& data = product_surface_info[surface_name.toStdWString()];
	data.str_surface_name = surface_name;
	auto path = QString::fromStdWString(str_product_config_path) + "\\feature.json";
	auto obj_feature = read_json_document(path);

	auto feature_array_default = obj_feature.value("default_feature").toArray();
	std::vector<std::wstring> defect_default_feature_vect;
	for (const auto& _ : feature_array_default)
	{
		auto feature_obj = _.toObject();
		Defect_Feature attri;
		attri.str_chn_name = feature_obj.value(QStringLiteral("名称")).toString().toStdWString();
		attri.str_chn_unit_name = feature_obj.value(QStringLiteral("单位")).toString().toStdWString();
		attri.multiple_times = feature_obj.value(QStringLiteral("计算系数")).toInt();
		data.attribute_vector.push_back(attri);
		defect_default_feature_vect.push_back(attri.str_chn_name);
	}

	auto feature_array = obj_feature.value("extra_feature").toArray();
	for (const auto& _ : feature_array)
	{
		auto feature_obj = _.toObject();
		Defect_Feature attri;
		attri.str_chn_name = feature_obj.value(QStringLiteral("名称")).toString().toStdWString();
		attri.str_chn_unit_name = feature_obj.value(QStringLiteral("单位")).toString().toStdWString();
		attri.multiple_times = feature_obj.value(QStringLiteral("计算系数")).toInt();
		data.attribute_vector.push_back(attri);
	}

	path = QString::fromStdWString(str_product_config_path) + "\\defect_info.json";
	auto obj_attri = read_json_document(path);
	auto defect_array = obj_attri.value("defect_info").toArray();
	for (const auto& _ : defect_array)
	{
		auto defect_obj = _.toObject();
		auto defect_name = defect_obj.value(QStringLiteral("名称")).toString().toStdWString();
		auto extra_feature_array = defect_obj.value(QStringLiteral("判断特征")).toArray();
		data.defect_filter_features[defect_name] = defect_default_feature_vect;
		for (const auto& feature : extra_feature_array)
		{
			data.defect_filter_features[defect_name].push_back(feature.toString().toStdWString());
		}
	}	
}

QJsonObject Product_Surface_Defect_Feature_Manager::cur_product_surface_info_to_json()
{
	QJsonObject obj;
	QJsonArray data_array;
	for (const auto& info : product_surface_info)
	{
		QJsonObject sub_obj;
		sub_obj.insert("name", QString::fromStdWString(info.first));
		sub_obj.insert("data", product_surface_rule_attr_2_json(info.second));
		data_array.append(sub_obj);
	}
	obj.insert("surface_info", data_array);
	return obj;
}

void Product_Surface_Defect_Feature_Manager::update_product_surface_info_by_QJsonObject(const QJsonObject& obj)
{
	auto array = obj.value("surface_info").toArray();
	for (const auto& _ : array)
	{
		auto sub_obj = _.toObject();
		auto name = sub_obj.value("name").toString();
		auto data = product_surface_rule_attr_from_json(sub_obj.value("data").toObject());
		product_surface_info[name.toStdWString()] = data;
	}
}

std::map<std::wstring, Product_Surface_Defect_Info> Product_Surface_Defect_Feature_Manager::get_all_product_surface_info()
{
	return product_surface_info;
}

void Product_Surface_Defect_Feature_Manager::clear()
{
	product_surface_info.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<product_surface_locate> Product_Surface_Locate_Manager::product_surface_locate_info;
int Product_Surface_Locate_Manager::draw_width;
int Product_Surface_Locate_Manager::draw_height;

void Product_Surface_Locate_Manager::load_product_surface_location(const std::wstring& product_path)
{
	product_surface_locate_info.clear();
	auto obj = read_json_document(QString::fromStdWString(product_path) + "/product_location.json");
	auto locate_array = obj.value("product_location").toArray();
	for (const auto& item : locate_array)
	{
		auto obj = item.toObject();
		product_surface_locate temp;
		temp.str_surface_name = obj.value(QStringLiteral("name")).toString().toStdWString();
		auto contain_array = obj.value(QStringLiteral("relate_surface")).toArray();
		for (const auto& item : contain_array)
		{
			temp.relate_surface.push_back(item.toString().toStdWString());
		}

		auto rect_array = obj.value("rect").toArray();
		temp.rt.setX(rect_array[0].toInt());
		temp.rt.setY(rect_array[1].toInt());
		temp.rt.setWidth(rect_array[2].toInt());
		temp.rt.setHeight(rect_array[3].toInt());
		product_surface_locate_info.push_back(temp);
	}
	draw_width = obj.value("draw_width").toInt();
	draw_height = obj.value("draw_height").toInt();
}

std::vector<std::wstring> Product_Surface_Locate_Manager::get_product_surface_related_item(const std::wstring& product_surface_name)
{
	for (const auto& item : product_surface_locate_info)
	{
		if (item.str_surface_name == product_surface_name)
			return item.relate_surface;
	}

	return {};
}

std::tuple<int, int> Product_Surface_Locate_Manager::get_draw_size()
{
	return { draw_width,draw_height };
}

std::vector<product_surface_locate> Product_Surface_Locate_Manager::get_all_draw_surface()
{
	return product_surface_locate_info;
}

void Product_Surface_Locate_Manager::set_draw_size(int width, int height)
{
	draw_width = width;
	draw_height = height;
}

void Product_Surface_Locate_Manager::set_product_surface_locate(const std::vector<product_surface_locate>& located)
{
	product_surface_locate_info = located;
}

void Product_Surface_Locate_Manager::save_to_json(const std::wstring& product_path)
{
	auto path = QString::fromStdWString(product_path) + "/product_location.json";
	QJsonObject obj;
	obj.insert("draw_width", draw_width);
	obj.insert("draw_height", draw_height);
	QJsonArray locate_array;
	for (const auto& item : product_surface_locate_info)
	{
		QJsonObject sub_obj;
		sub_obj.insert("name", QString::fromStdWString(item.str_surface_name));

		QJsonArray rect_array;
		rect_array.append(item.rt.x());
		rect_array.append(item.rt.y());
		rect_array.append(item.rt.width());
		rect_array.append(item.rt.height());
		sub_obj.insert("rect", rect_array);

		QJsonArray relate_surface;
		for (const auto& sub_surface : item.relate_surface)
		{
			relate_surface.append(QString::fromStdWString(sub_surface));
		}
		sub_obj.insert("relate_surface", relate_surface);
		locate_array.append(sub_obj);
	}

	obj.insert("product_location",locate_array);
	save_json_files(path,obj);
}

namespace Product_Rule_Module
{
	void PRODUCT_RULE_MODULE_EXPORT load_product_config_and_defect_attribute(const std::wstring& str_product_config_path)
	{
		Product_Config::clear();
		Product_Config::load_product_config(str_product_config_path);

		Product_Surface_Defect_Feature_Manager::clear();
		auto all_product_surface = Product_Config::get_all_surface_config();
		for (const auto& config : all_product_surface)
		{
			Product_Surface_Defect_Feature_Manager::load_product_surface_config(config.str_surface_name, config.detect_model.str_model_path);			
		}		
	}
}
