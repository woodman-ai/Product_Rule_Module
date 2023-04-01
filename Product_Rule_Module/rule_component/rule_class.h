#pragma once
#include "rule_typedef.h"
#include <vector>
#include <set>
#include <qpolygon.h>
#include <optional>
struct Defect_Node
{
	Defect_Node_Type node_type;
	Rule_Calc_Node expression;
	std::vector<Defect_Node> subNodes;
	bool get_value(const std::map<check_attri_name, check_attri_value>& attri_dict, double scale) const;
};

struct Defect_Rule
{
	QString str_defect_name;
	int max_count = 0;
	int cur_count = 0;
	std::optional<double> max_area;
	double cur_area;
	Defect_Node calc_expression;
	void reset();
	bool get_value(const std::map<check_attri_name, check_attri_value>& attri_dict, double scale) const;
};

struct Product_Level_Defects
{
	Product_Level product_level;
	std::vector<Defect_Rule> level_defects;
	bool bover_limit = false;

	bool is_valid() const;

	void defect_count_increase(int index);

	void defect_accumulate_area_increate(int index, double now_area);

	bool is_trivial_defect(int index); //如果最大数量不为0,就认为是

	bool operator < (const Product_Level_Defects& level) const;

	void reset();

	int is_out_of_rule(const std::map<check_attri_name, check_attri_value>& attri_dict, double scale);
};

struct ProductSurface_One_Rule
{
	QString str_name = QStringLiteral("默认规则");
	QString str_roi_color = "#ffffff";
	std::vector<QPolygon>  rule_contours; ;
	std::vector<Product_Level_Defects> rule_levels;
	void reset();

	bool is_valid() const;

	bool is_intersected(const QPolygon& poly) const;
};

struct ProductSurface_Rule
{
	QString str_product_surface_name;
	std::vector<ProductSurface_One_Rule> surface_rules;
	void reset();
};

using Product_Rule = std::vector<ProductSurface_Rule>;

struct Rule_Test_Result
{
	int now_level = -1;
	int rule_index = -1;
	int level_index = -1;
	int defect_index = -1;
};