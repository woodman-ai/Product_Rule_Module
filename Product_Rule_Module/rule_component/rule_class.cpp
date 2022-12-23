#include "rule_class.h"

bool Defect_Node::get_value(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict, double scale) const
{
	if (node_type == Defect_Node_Type::Expression)
	{
		return (expression.func)(attri_dict, gray_dict, a_dict, scale);
	}
	else
	{
		bool value = subNodes[0].get_value(attri_dict, gray_dict, a_dict,scale);
		for (int i = 1; i < subNodes.size(); i++)
		{
			if (node_type == Defect_Node_Type::And)
			{
				if (value == 0)
					return false;

				value &= subNodes[i].get_value(attri_dict, gray_dict, a_dict, scale);
			}
			else
			{
				if (value == 1)
					return true;
				value |= subNodes[i].get_value(attri_dict, gray_dict, a_dict, scale);
			}
		}
		return value;
	}
}

std::set<int> Defect_Node::get_inspect_gray_index() const
{
	std::set<int> gray_set;
	if (node_type == Defect_Node_Type::Expression)
	{
		int index = expression.g_index;
		if (index != -1)
			gray_set.insert(index);
	}

	for (const auto& sub_node : subNodes)
	{
		auto temp = sub_node.get_inspect_gray_index();
		gray_set.merge(temp);
	}
	return gray_set;
}

std::set<int> Defect_Node::get_inspect_area_ratio_index() const
{
	std::set<int> a_set;
	if (node_type == Defect_Node_Type::Expression)
	{
		int index = expression.a_index;
		if (index != -1)
			a_set.insert(index);
	}

	for (const auto& sub_node : subNodes)
	{
		auto temp = sub_node.get_inspect_area_ratio_index();
		a_set.merge(temp);
	}
	return a_set;
}


/// /////////////////////////////////////////////////////////

void Defect_Rule::reset()
{
	cur_count = 0;
	cur_area = -1;
}

bool Defect_Rule::get_value(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict, double scale) const
{
	return calc_expression.get_value(attri_dict, gray_dict, a_dict, scale);
}


/////////////////////////////////////////////////////////////////////////////

bool Merge_Rule::get_value(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict, double scale) const
{
	return calc_expression.get_value(attri_dict, gray_dict, a_dict, scale);
}
/// /////////////////////////////////////////////////////////

bool Product_Level_Defects::is_valid() const
{
	return !level_defects.empty();
}


void Product_Level_Defects::defect_count_increase(int index)
{
	if (index < 0 || index > level_defects.size())
		return;
	++level_defects[index].cur_count;
	if (level_defects[index].cur_count > level_defects[index].max_count)
		bover_limit = true;
}


void Product_Level_Defects::defect_accumulate_area_increate(int index, double now_area)
{
	if (index < 0 || index >= level_defects.size() || level_defects[index].max_area == std::nullopt)
		return;
	level_defects[index].cur_area += now_area;
	if (level_defects[index].cur_area > level_defects[index].max_area)
		bover_limit = true;
}

bool Product_Level_Defects::is_trivial_defect(int index)
{
	if (index < 0 || index > level_defects.size())
		return false;
	return level_defects[index].max_count > 0;
}

bool Product_Level_Defects::operator < (const Product_Level_Defects& level) const
{
	return this->product_level.level < level.product_level.level;
}

void Product_Level_Defects::reset()
{
	bover_limit = false;
	for (auto& item : level_defects)
	{
		item.reset();
	}
}

int Product_Level_Defects::is_out_of_rule(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict, double scale)
{
	for (int i = 0; i < level_defects.size(); i++)
	{
		const auto& rule = level_defects[i];
		if (rule.get_value(attri_dict, gray_dict, a_dict, scale))
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CheckSurface_One_Rule::reset()
{
	for (auto& rule : rule_levels)
	{
		rule.reset();
	}
}

bool CheckSurface_One_Rule::is_valid() const
{
	bool valid = false;
	for (const auto& item : rule_levels)
	{
		if (item.is_valid())
			return true;
	}
	return false;
}

bool CheckSurface_One_Rule::is_intersected(const QString& str_surface_name, const QPolygon& poly) const
{
	if (rule_contours.empty())
		return true;

	auto iter = rule_contours.find(str_surface_name);
	if (iter == rule_contours.end())
	{
		return true;
	}else
	{
		for (const auto& item : iter->second)
		{
			if (item.intersects(poly))
				return true;
		}
	}

	return false;
}

std::map<QString, std::set<int>> CheckSurface_One_Rule::get_defect_gray_index() const
{
	std::map<QString, std::set<int>> data;
	for (const auto& levels : rule_levels)
	{
		for (const auto& defects : levels.level_defects)
		{
			auto gray_set = defects.calc_expression.get_inspect_gray_index();
			data[defects.str_defect_name].merge(gray_set);
		}
	}
	return data;
}

std::map<QString, std::set<int>> CheckSurface_One_Rule::get_defect_area_index() const
{
	std::map<QString, std::set<int>> data;
	for (const auto& levels : rule_levels)
	{
		for (const auto& defects : levels.level_defects)
		{
			auto gray_set = defects.calc_expression.get_inspect_area_ratio_index();
			data[defects.str_defect_name].merge(gray_set);
		}
	}
	return data;
}

////////////////////////////////////////////////////////////////////////////////

void Surface_Rule::reset()
{
	for (auto& rule : surface_rules)
		rule.reset();
}

////////////////////////////////////////////////////////////////////////////////////

int Surface_Defect_Merge_Rule::is_exist_defect(const QString& str_defect_name) const
{
	auto iter = defect_map.find(str_defect_name);
	if (iter == defect_map.end())
		return -1;
	return iter->second;
}

std::tuple<double, int, QString > Surface_Defect_Merge_Rule::get_param(int index) const
{
	if (index < 0 || index >= defect_merge_rule_vector.size())
		return {};

	const auto& data = defect_merge_rule_vector[index];
	return { data.radiums,data.min_num,data.str_merge_defect_name };
}

