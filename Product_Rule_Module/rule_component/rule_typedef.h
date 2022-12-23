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
	int     g_index = -1; // 当比较灰度时才有用
	int     a_index = -1; //当比较面积比时才有用
	std::function<bool(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict,const std::map<int,double> a_dict, double)> func = nullptr;
};

enum class Defect_Node_Type
{
	Expression,
	And,
	Or
};