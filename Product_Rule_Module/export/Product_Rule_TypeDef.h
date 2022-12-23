#pragma once
#include <qstring.h>
#include <qimage.h>
using check_attri_index = int;
using check_attri_value = double;
struct Defect_Feature {
	double width;
	double height;
	double area;
	double score;
	int level;
	double skeleton_length;
	QString strFeatures = "";
	QString strFilteredResult = "NORULE";
};

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
	std::vector<product_img_name_and_qimage> contain_product_surface_vect;
	std::vector<QString> surface_defect_name_vect;
	std::vector<QString> surface_gray_level_surface_name_vect;
};

struct surface_train_backed
{
	int train_backed_program_id = -1;
	std::vector<int> train_backed_use_img_vect;
};