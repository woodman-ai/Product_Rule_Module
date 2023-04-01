#include "rule_class.h"

bool Defect_Node::get_value(const std::map<check_attri_name, check_attri_value>& attri_dict, double scale) const
{
	if (node_type == Defect_Node_Type::Expression)
	{
		return (expression.func)(attri_dict, scale);
	}
	else
	{
		bool value = subNodes[0].get_value(attri_dict,scale);
		for (int i = 1; i < subNodes.size(); i++)
		{
			if (node_type == Defect_Node_Type::And)
			{
				if (!value)
					return false;

				value &= subNodes[i].get_value(attri_dict, scale);
			}
			else
			{
				if (value)
					return true;
				value |= subNodes[i].get_value(attri_dict, scale);
			}
		}
		return value;
	}
}

/// /////////////////////////////////////////////////////////

void Defect_Rule::reset()
{
	cur_count = 0;
	cur_area = -1;
}

bool Defect_Rule::get_value(const std::map<check_attri_name, check_attri_value>& attri_dict, double scale) const
{
	return calc_expression.get_value(attri_dict, scale);
}


/////////////////////////////////////////////////////////////////////////////

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

int Product_Level_Defects::is_out_of_rule(const std::map<check_attri_name, check_attri_value>& attri_dict, double scale)
{
	for (int i = 0; i < level_defects.size(); i++)
	{
		const auto& rule = level_defects[i];
		if (rule.get_value(attri_dict,scale))
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSurface_One_Rule::reset()
{
	for (auto& rule : rule_levels)
	{
		rule.reset();
	}
}

bool ProductSurface_One_Rule::is_valid() const
{
	bool valid = false;
	for (const auto& item : rule_levels)
	{
		if (item.is_valid())
			return true;
	}
	return false;
}

bool ProductSurface_One_Rule::is_intersected(const QPolygon& poly) const
{
	if (rule_contours.empty())
		return true;

	for (const auto& item : rule_contours)
	{
		if (item.intersects(poly))
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////

void ProductSurface_Rule::reset()
{
	for (auto& rule : surface_rules)
		rule.reset();
}