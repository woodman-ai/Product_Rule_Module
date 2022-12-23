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
	bool get_value(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int,double>& a_dict, double scale) const;
	std::set<int> get_inspect_gray_index() const;
	std::set<int> get_inspect_area_ratio_index() const;
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
	bool get_value(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict, double scale) const;
};

struct Merge_Rule
{
	QString str_src_defect_name;
	QString str_merge_defect_name;
	Defect_Node calc_expression;
	double radiums;
	int min_num;
	bool get_value(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict, double scale) const;
	std::tuple<double, int> get_param() const { return { radiums,min_num }; }
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

	int is_out_of_rule(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict, double scale);
};

struct CheckSurface_One_Rule
{
	QString str_name = QStringLiteral("默认规则");
	QString str_roi_color = "#ffffff";
	std::set<QString> valid_product_surface;
	std::map<QString, std::vector<QPolygon>>  rule_contours; ;
	std::vector<Product_Level_Defects> rule_levels;
	void reset();

	bool is_valid() const;

	bool is_intersected(const QString& str_surface_name, const QPolygon& poly) const;

	std::map<QString, std::set<int>> get_defect_gray_index() const; 
	std::map<QString, std::set<int>> get_defect_area_index() const;
};

struct Surface_Rule
{
	std::map<QString, std::set<int>> _defect_check_gray_index;
	std::map<QString, std::set<int>> _defect_check_area_index;
	QString str_check_surface_name;
	std::vector<CheckSurface_One_Rule> surface_rules;
	void reset();
};

using Product_Rule = std::vector<Surface_Rule>;

struct Rule_Test_Result
{
	int now_level = -1;
	int rule_index = -1;
	int level_index = -1;
	int defect_index = -1;
};

struct Surface_Defect_Merge_Rule
{
	QString str_surface_name;
	std::map<QString, int>   defect_map;
	std::vector<Merge_Rule> defect_merge_rule_vector;
	int is_exist_defect(const QString& str_defect_name) const;
	std::tuple<double, int, QString > get_param(int index) const;
};

using Merge_Defect_Rules = std::vector<Surface_Defect_Merge_Rule>;