#pragma once
#include <vector>
#include <qstring>
#include "rule_typedef.h"

class Rule_Attribute
{
	QString str_name;
	int index;
	int multiple_times; //数值需乘以scale几次(长宽一次，面积2次，得分不用)
	QString str_unit_name;
};

class Rule_Attri_Manager
{
public:
	static std::vector<QString> get_rule_attributes();
	static std::vector<QString> get_rule_attributes_literal();
	static QString attributes_literal_2_attributes(const QString& str_literal);
	static QString attributes_2_attributes_literal(const QString& str_attri);
	static QString attributes_literal_unit(const QString& str_literal);
	static check_attri_index  get_check_index_from_string(const QString& str_attri_name);
	static QString get_check_index_name(check_attri_index attri_index);

	static std::map<check_attri_index, check_attri_value> get_check_attri_dict_from_defect(const Defect_Feature& defect);
	static std::function<bool(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict,double)>
		generate_defect_rule_node(const Rule_Calc_Node& save_data);

	
};

