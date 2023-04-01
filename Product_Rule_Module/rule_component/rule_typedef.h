#pragma once
#include "Product_Rule_TypeDef.h"
#include "qstring.h"
#include <functional>
#include <map>
enum class relation
{
	less_than,
	bigger_than
};

struct Rule_Calc_Node
{
	QString str_attri_name;
	QString str_relation;
	double  compare_value = 0;
	std::function<bool(const std::map<check_attri_name, check_attri_value>& attri_dict, double)>	func = nullptr;
};

enum class Defect_Node_Type
{
	Expression,
	And,
	Or
};

