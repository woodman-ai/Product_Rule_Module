#pragma once
#include <qstring.h>
#include <qimage.h>
#include <qrect.h>
using check_attri_name = std::wstring;
using check_attri_value = float;

struct Product_Level
{
	QString strLevelName;
	int level;
	QString str_color;
	QString str_trivial_defect_color;
	bool is_ok = false;
};

struct CheckSurface_Contain_ProductSurface
{
	struct product_img_name_and_qimage
	{
		QString str_product_surface_name;
		QString str_surface_img_path;
		QImage  surface_img;
	};
	QString str_check_surface_name;
	product_img_name_and_qimage contain_product_surface_vect;
	std::vector<QString> surface_defect_name_vect;
};

struct surface_train_backed
{
	int train_backed_program_id = -1;
	std::vector<int> train_backed_use_img_vect;
};

struct checksurface_update_info
{
	QString  checksurface_name;
	std::vector<QString> model_path_vect;
	std::vector<QString> model_defect_vect;
	std::vector<QString> img_name_vect;
};

struct Defect_Feature
{
	std::wstring str_chn_name;
	std::wstring str_chn_unit_name;
	int multiple_times = 0; //数值需乘以scale几次(长宽一次，面积2次，得分不用)
};

using Defect_Name = std::wstring;
using Defect_Extra_Features = std::vector<std::wstring>;

struct Product_Surface_Defect_Info
{
	QString str_surface_name;
	std::vector<Defect_Feature> attribute_vector;
	std::map<Defect_Name, Defect_Extra_Features> defect_filter_features;
};

struct Detect_Model
{
	std::wstring str_model_path;
	std::vector<int> use_img;
};

struct product_surface_config
{
	std::wstring str_surface_name;
	std::wstring str_img_path;
	Detect_Model detect_model;
	surface_train_backed backend_config;
};

struct product_surface_locate
{
	std::wstring str_surface_name;
	QRect rt;
	std::vector<std::wstring> relate_surface;
};