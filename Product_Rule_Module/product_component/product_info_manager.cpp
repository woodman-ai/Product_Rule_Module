#include "product_info_manager.h"
#include "utils.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include <iostream>
template <typename T,typename K,typename V>
auto get_from_map(const T& dict, const K& key, const V& value)
{
	auto iter = dict.find(key);
	return iter == dict.end() ? value : iter->second;
}

product_info_manager::product_info_manager()
{
	
}

void product_info_manager::clear_data()
{
	m_product_surface_map_id_2_name.clear();
	m_product_surface_map_name_2_id.clear();
	m_product_surface_map_name_2_trainbackend.clear();
	m_product_surface_map_id_2_trainbackend.clear();
	m_product_test_surface_map_id_2_name.clear();
	m_product_test_surface_map_name_2_id.clear();
	m_product_surface_2_test_surface.clear();
	m_surface_ralated_sub_surface.clear();
}

void product_info_manager::init_product_info(const QString& str_json_config_path)
{
	clear_data();
	auto obj = read_json_document(str_json_config_path);
	if (obj.contains("check_surface"))
	{
		auto check_array = obj.value("check_surface").toArray();
		int index = 0;
		for (const auto& check_ : check_array)
		{	
			QString str_name = check_.toString();
			m_product_test_surface_map_id_2_name[index] = str_name;
			m_product_test_surface_map_name_2_id[str_name] = index;
			index++;
		}
	}
	if (obj.contains("surface"))
	{
		auto surface_array = obj.value("surface").toArray();
		int index = 0;
		for (const auto& check_ : surface_array)
		{
			auto surface_obj = check_.toObject();			
			QString str_name = surface_obj.value("name").toString();
			m_product_surface_map_id_2_name[index] = str_name;
			m_product_surface_map_name_2_id[str_name] = index;

			QString str_check_surface = surface_obj.value("check_surface").toString();
			int check_surface = m_product_test_surface_map_name_2_id[str_check_surface];
			m_product_surface_2_test_surface[index] = check_surface;	

			if (surface_obj.contains("train_backend"))
			{
				auto obj_train_backend = surface_obj.value("train_backend").toObject();
				surface_train_backed backend;
				backend.train_backed_program_id = obj_train_backend.value("program_id").toInt();
				QJsonArray img_array = obj_train_backend.value("use_img").toArray();
				for (const auto& item : img_array)
				{
					backend.train_backed_use_img_vect.push_back(item.toInt());
				}

				m_product_surface_map_name_2_trainbackend[str_name] = backend;
				m_product_surface_map_id_2_trainbackend[index] = backend;
			}

			index++;
		}
	}

	if (obj.contains("related_surface"))
	{
		auto related = obj.value("related_surface").toArray();
		for (const auto& related_ : related)
		{
			auto related_obj = related_.toObject();
			auto str_name = related_obj.value("name").toString();
			auto sub_array = related_obj.value("sub_surface").toArray();
			draw_related_surface temp;
			temp.rt = string_2_rect(related_obj.value("rt_location").toString());
			temp.str_surface_name = str_name;
			for (const auto& item : sub_array)
			{
				auto name = item.toString();
				temp.sub_surface.push_back(m_product_surface_map_name_2_id[name]);
			}
			m_surface_ralated_sub_surface.push_back(temp);
		}
	}

	int width = obj.value("draw_pixel_width").toInt();
	int height = obj.value("draw_pixel_height").toInt();
	m_draw_pix_size = { width,height };
}

product_surface product_info_manager::get_product_surface_id(const QString& name)
{
	return get_from_map(m_product_surface_map_name_2_id, name, -1);	
}

QString  product_info_manager::get_product_surface_name(product_surface id)
{
	return get_from_map(m_product_surface_map_id_2_name, id, "");
}

product_test_surface product_info_manager::get_product_test_surface_id(const QString& name)
{
	return get_from_map(m_product_test_surface_map_name_2_id, name, -1);
}

QString  product_info_manager::get_product_test_surface_name(product_surface id)
{
	return get_from_map(m_product_test_surface_map_id_2_name, id, "");
}

product_test_surface product_info_manager::get_product_test_surface(product_surface surface_id)
{
	return get_from_map(m_product_surface_2_test_surface, surface_id, -1);
}

std::vector<product_surface> product_info_manager::get_related_surface(const QString& surface_name)
{
	for (const auto& item : m_surface_ralated_sub_surface)
	{
		if (item.str_surface_name == surface_name)
			return item.sub_surface;
	}

	return {};
}

int product_info_manager::get_surface_count() const 
{
	return m_product_surface_map_id_2_name.size();
}

std::tuple<std::map<product_surface, QString>, std::map<QString, product_surface>>
product_info_manager::get_product_surface_name_maps()
{
	return { m_product_surface_map_id_2_name,m_product_surface_map_name_2_id };
}

std::tuple<std::map<product_test_surface, QString>, std::map<QString, product_test_surface>>
product_info_manager::get_product_test_surface_name_maps()
{
	return { m_product_test_surface_map_id_2_name,m_product_test_surface_map_name_2_id };
}

std::map<product_surface, product_test_surface> product_info_manager::get_product_surface_2_test_surface_map()
{
	return m_product_surface_2_test_surface;
}


std::vector<product_info_manager::draw_related_surface> product_info_manager::get_product_related_surface()
{
	return m_surface_ralated_sub_surface;
}

product_info_manager::draw_related_surface product_info_manager::get_product_draw_surface(const QString& str_surface_name)
{
	for (const auto& item : m_surface_ralated_sub_surface)
	{
		if (item.str_surface_name == str_surface_name)
			return item;
	}
	return {};
}

std::optional<surface_train_backed> product_info_manager::get_product_surface_train_backend_info(product_surface surface_id)
{
	auto iter = m_product_surface_map_id_2_trainbackend.find(surface_id);
	if (iter != m_product_surface_map_id_2_trainbackend.end())
		return iter->second;
	return {};
}

std::optional<surface_train_backed>  product_info_manager::get_product_surface_train_backend_info(QString surface_name)
{
	auto iter = m_product_surface_map_name_2_trainbackend.find(surface_name);
	if (iter != m_product_surface_map_name_2_trainbackend.end())
		return iter->second;
	return {};
}

QStringList product_info_manager::enum_all_product_surface() const
{
	QStringList list;
	for (const auto& item : m_product_surface_map_name_2_id)
	{
		list << item.first;
	}
	return list;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CheckSurfaceManager::init_from_json_file(const QString& str_json_config)
{
	auto obj = read_json_document(str_json_config);
	auto surface_size = obj.value("check_surface_data").toArray();
	m_contain_surface.clear();
	for (const auto& surface : surface_size)
	{
		auto single_surface = surface.toObject();
		CheckSurface_Contain_ProductSurface temp;
		temp.str_check_surface_name = single_surface.value("check_name").toString();
		auto contains_product_surface = single_surface.value("contain_surface").toArray();
		for (const auto& _ : contains_product_surface)
		{
			auto obj = _.toObject();
			CheckSurface_Contain_ProductSurface::product_img_name_and_qimage surface_and_img;
			surface_and_img.str_product_surface_name = obj.value("surface_name").toString();
			surface_and_img.str_surface_img_path = obj.value("surface_img_path").toString();
			surface_and_img.surface_img = QImage(surface_and_img.str_surface_img_path);
			temp.contain_product_surface_vect.push_back(surface_and_img);
		}

		auto contain_defects = single_surface.value("contain_defects").toArray();
		for (const auto& item : contain_defects)
		{
			temp.surface_defect_name_vect.push_back(item.toString());
		}

		auto contain_gray_level_img_name = single_surface.value("gray_level_img_name").toArray();
		for (const auto& item : contain_gray_level_img_name)
		{
			temp.surface_gray_level_surface_name_vect.push_back(item.toString());
		}

		m_contain_surface.push_back(temp);
	}
}

void CheckSurfaceManager::save_to_json_file(const QString& str_json_config)
{
	QJsonArray check_surface_array;
	for (const auto& item : m_contain_surface)
	{
		QJsonObject obj_surface;
		obj_surface.insert("check_name", item.str_check_surface_name);
		QJsonArray contain_surface_array;
		for (const auto& contain_surface : item.contain_product_surface_vect)
		{
			QJsonObject obj_temp;
			obj_temp.insert("surface_name", contain_surface.str_product_surface_name);
			obj_temp.insert("surface_img_path", contain_surface.str_surface_img_path);
			contain_surface_array.append(obj_temp);
		}
		obj_surface.insert("contain_surface", contain_surface_array);

		QJsonArray contain_defects;
		for (const auto& item : item.surface_defect_name_vect)
		{
			contain_defects.append(item);
		}
		obj_surface.insert("contain_defects", contain_defects);

		QJsonArray contain_gray;
		for (const auto& item : item.surface_gray_level_surface_name_vect)
		{
			contain_gray.append(item);
		}
		obj_surface.insert("gray_level_img_name", contain_gray);


		check_surface_array.append(obj_surface);
 	}

	QJsonObject _obj;
	_obj.insert("check_surface_data", check_surface_array);
	save_json_files(str_json_config, _obj);
}

std::vector<CheckSurface_Contain_ProductSurface::product_img_name_and_qimage>
CheckSurfaceManager::get_checksurface_contains_data(const QString& str_check_surface_name) const
{
	for (const auto& item : m_contain_surface)
	{
		if (item.str_check_surface_name == str_check_surface_name)
		{
			return item.contain_product_surface_vect;
		}
	}
	return {};
}

std::optional<CheckSurface_Contain_ProductSurface> CheckSurfaceManager::get_check_surface_data(const QString& str_check_surface_name) const
{
	for (const auto& item : m_contain_surface)
	{
		if (item.str_check_surface_name == str_check_surface_name)
		{
			return item;
		}
	}
	return {};
}

std::vector<QString> CheckSurfaceManager::enum_check_surface() const
{
	std::vector<QString> result;
	for (const auto& item : m_contain_surface)
	{
		result.push_back(item.str_check_surface_name);
	}
	return result;
}

int CheckSurfaceManager::get_check_surface_index(const QString& str_check_surface_name) const
{
	for (int i = 0; i < m_contain_surface.size(); i++)
	{
		if (m_contain_surface[i].str_check_surface_name == str_check_surface_name)
		{
			return i;
		}
	}

	return -1;
}

void CheckSurfaceManager::update_check_surface_image_name(const QString & str_check_surface_name, const std::vector<QString>&  gray_image_name)
{
	if (gray_image_name.empty())
		return;

	for (int i = 0; i < m_contain_surface.size(); i++)
	{
		if (m_contain_surface[i].str_check_surface_name == str_check_surface_name)
		{
			m_contain_surface[i].surface_gray_level_surface_name_vect = gray_image_name;
			return;
		}
	}
}

void CheckSurfaceManager::update_check_surface_defects(const QString& str_check_surface_name, const std::vector<QString>& defect_vector)
{
	if (defect_vector.empty())
		return;

	for (int i = 0; i < m_contain_surface.size(); i++)
	{
		if (m_contain_surface[i].str_check_surface_name == str_check_surface_name)
		{
			m_contain_surface[i].surface_defect_name_vect = defect_vector;
			return;
		}
	}
}

void CheckSurfaceManager::update_check_surface_defects_by_client_model(const QJsonObject& obj, const QString& str_config_path)
{
	auto array = obj.value("check_surface_defect_data").toArray();
	for (const auto& _ : array)
	{
		auto single_data = _.toObject();
		auto name = single_data.value("check_surface_name").toString();

		std::vector<QString> defect_vect;
		auto defect_array = single_data.value("defect_arrays").toArray();
		for (const auto& name : defect_array)
			defect_vect.push_back(name.toString());
		update_check_surface_defects(name, defect_vect);

		std::vector<QString> image_names;
		auto image_array = single_data.value("image_names").toArray();
		for (const auto& name : image_array)
			image_names.push_back(name.toString());
		update_check_surface_image_name(name, image_names);
	}

	save_to_json_file(str_config_path);
}

static std::vector<QString> get_defects_from_model_config(const QString& str_model_path)
{
	std::vector<QString> defect_vector;
	auto obj = read_json_document(str_model_path + "/config.json");
	auto defect_array = obj.value("defects").toArray();
	for (const auto& _ : defect_array)
	{
		auto obj = _.toObject();
		defect_vector.push_back(obj.value("name").toString());
	}

	return defect_vector;
}

QJsonObject PRODUCT_RULE_MODULE_EXPORT load_check_surface_defect_from_model(const QString& str_merge_config_path)
{
	QJsonArray check_surface_data_array;
	auto obj_config = read_json_document(str_merge_config_path);
	auto merge_config = obj_config.value("merge_config").toArray();
	for (const auto& _ : merge_config)
	{
		auto obj = _.toObject();
		auto check_surface_name = obj.value("check_surface_name").toString();

		QJsonArray defect_array;
		QJsonArray image_names_array;
		std::set<QString> exist_defect;
		auto model_array = obj.value("use_model_array").toArray();
		for (const auto& _ : model_array)
		{
			auto _vector = get_defects_from_model_config(_.toString());
			for (const auto& defect_name : _vector)
			{
				auto iter = exist_defect.find(defect_name);
				if (iter == exist_defect.end())
				{
					exist_defect.insert(defect_name);
					defect_array.append(defect_name);
				}
			}
		}

		auto append_defect_array = obj.value("append_defect_array").toArray();
		for (const auto& _ : append_defect_array)
		{
			auto defect_name = _.toString();
			auto iter = exist_defect.find(defect_name);
			if (iter == exist_defect.end())
			{
				exist_defect.insert(defect_name);
				defect_array.push_back(defect_name);
			}
		}

		auto gray_level_image_name_array = obj.value("image_names").toArray();
		for (const auto& _ : gray_level_image_name_array)
		{
			auto gray_names = _.toString();	
			image_names_array.append(gray_names);
		}


		QJsonObject obj_surface;
		obj_surface.insert("check_surface_name", check_surface_name);
		obj_surface.insert("defect_arrays", defect_array);
		obj_surface.insert("image_names", image_names_array);
		check_surface_data_array.append(obj_surface);
	}

	QJsonObject obj;
	obj.insert("check_surface_defect_data", check_surface_data_array);
	return obj;
}