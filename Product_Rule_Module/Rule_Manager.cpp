#include "Rule_Manager.h"
#include "rule_class.h"
#include "utils.h"
#include "RuleSettingDlg.h"
#include "qjsonarray.h"

QString Rule_Manager::defect_rule_result_ok()
{
	return "rule_filter_ok";
}

QString Rule_Manager::defect_rule_result_ng()
{
	return "rule_filter_ng";
}

QString Rule_Manager::defect_rule_result_trivial_defect()
{
	return "rule_filter_trivial_defect";
}

Rule_Manager::Rule_Manager()
{
	m_product_surface_rule = new std::map<std::wstring, ProductSurface_Rule>();
}

Rule_Manager::~Rule_Manager()
{
	delete m_product_surface_rule;
}

void Rule_Manager::load_from_json(const QString& str_path)
{
	auto obj = read_json_document(str_path + "/rule.json");
	load_from_jsonobject(obj);
}

void Rule_Manager::save_to_json(const QString& str_path)
{
	Product_Rule all_rule;
	for (const auto& item : *m_product_surface_rule)
	{
		all_rule.push_back(item.second);
	}
	auto obj = Product_Rule_2_json(all_rule);
	save_json_files(str_path, obj);

	
}

void Rule_Manager::load_from_jsonobject(const QJsonObject& obj)
{
	auto all_rule = Product_Rule_from_json(obj);
	m_product_surface_rule->clear();
	for (const auto& item : all_rule)
	{
		(*m_product_surface_rule)[item.str_product_surface_name.toStdWString()] = item;
	}
}

void Rule_Manager::set_product_rule(const std::vector<ProductSurface_Rule>& rules)
{
	m_product_surface_rule->clear();
	for (const auto& item : rules)
	{
		(*m_product_surface_rule)[item.str_product_surface_name.toStdWString()] = item;
	}
}

void Rule_Manager::get_rules(std::vector<ProductSurface_Rule>& vect) const
{
	for (const auto& item : *m_product_surface_rule)
	{
		vect.push_back(item.second);
	}
}

std::optional<Rule_Manager::rule_result> Rule_Manager::is_defect_out_of_rule(const QString& str_product_surface_name,
	const QString& str_defect_name,
	const QPolygon& poly_countour,
	const inspect_defect_attri_map& defect_attri,
	double scale)
{
	auto iter = (*m_product_surface_rule).find(str_product_surface_name.toStdWString());
	if (iter == (*m_product_surface_rule).end())
		return {};

	auto& rule_vector = iter->second.surface_rules;
	struct Check_Result
	{
		int _current_level = -1;
		int _one_rule_index = -1; 
		int _level_index = -1;
		int _defect_index = -1;

		void update(int level, int rule_index, int level_index, int defect_index)
		{
			if (_current_level == -1)
			{
				_current_level = level;
				_one_rule_index = rule_index;
				_level_index = level_index;
				_defect_index = defect_index;
			}
			else
			{
				if (_current_level > level)
				{
					_current_level = level;
					_one_rule_index = rule_index;
					_level_index = level_index;
					_defect_index = defect_index;
				}
			}
		}
	};


	Check_Result result;
	for (int rule_index = 0; rule_index < rule_vector.size(); rule_index++)
	{
		if (!rule_vector[rule_index].is_intersected(poly_countour))
		{
			continue;
		}
				
		const auto& levels = rule_vector[rule_index].rule_levels;
		for (int level_index = 0; level_index < levels.size(); level_index++)
		{
			const auto& defects = levels[level_index].level_defects;
			for (int defect_index = 0; defect_index < defects.size(); defect_index++)
			{
				const auto& defect = defects[defect_index];
				if (defect.str_defect_name == str_defect_name)
				{
					bool brule = defect.get_value(defect_attri, scale);
					if (brule)
					{
						result.update(levels[level_index].product_level.level, rule_index, level_index, defect_index);
					}
				}
			}
		}
	}

	if (result._current_level != -1)
	{
		auto& one_rule = rule_vector[result._one_rule_index];
		auto& rule_level = one_rule.rule_levels[result._level_index];
		rule_level.defect_count_increase(result._defect_index);
		auto iter = defect_attri.find(L"面积");
		if (iter != defect_attri.end())
		{
			double mm_value = iter->second / scale / scale;
			rule_level.defect_accumulate_area_increate(result._defect_index, mm_value);
		}

		rule_result ret;
		ret.level = rule_level.product_level;
		ret.rule_index = result._one_rule_index;
		ret.level_index = result._level_index;
		ret.defect_index = result._defect_index;
		bool is_trivial = rule_level.is_trivial_defect(result._defect_index);
		if (is_trivial)
			ret.out_of_rule_type = Rule_Manager::Out_Of_Rule_Defect_Type::trivial_defect;
		else
			ret.out_of_rule_type = Rule_Manager::Out_Of_Rule_Defect_Type::criticial_defect;
		return ret;
	}

	return { };
}

int Rule_Manager::get_surface_level(const QString& str_product_surface_name)
{
	const auto& iter = (*m_product_surface_rule).find(str_product_surface_name.toStdWString());
	if (iter == (*m_product_surface_rule).end())
		return -1;

	int lowest = -1;
	auto& surface_rule_data = iter->second.surface_rules;   //具体找到某一个check_surface;
	for (const auto& one_rule : surface_rule_data)
	{
		for (const auto& one_level : one_rule.rule_levels)
		{
			if (one_level.bover_limit)
			{
				if (lowest == -1)
				{
					lowest = one_level.product_level.level;
				}
				else
				{
					if(one_level.product_level.level < lowest)
						lowest = one_level.product_level.level;
				}
			}
		}
	}

	return lowest;	
}

void Rule_Manager::reset_check_surface(const QString& str_product_surface_name)
{
	const auto& iter = (*m_product_surface_rule).find(str_product_surface_name.toStdWString());
	if (iter == (*m_product_surface_rule).end())
		return;

	iter->second.reset();
}

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Product_Level> Product_Level_Manager::get_product_levels() const
{
	return m_product_level;
}

void Product_Level_Manager::load_from_json(const QString& str_path)
{
	auto obj = read_json_document(str_path + "/product_level.json");
	load_from_json(obj);
}

void Product_Level_Manager::load_from_json(const QJsonObject& obj)
{
	m_product_level.clear();
	auto array = obj.value("product_levels").toArray();
	for (const auto& level_obj : array)
	{
		m_product_level.push_back(Product_Level_from_json(level_obj.toObject()));
	}	
}

QColor Product_Level_Manager::get_product_level_color(int level) const
{
	for (const auto& item : m_product_level)
	{
		if (item.level == level)
			return item.str_color;
	}

	return {};
}

QColor Product_Level_Manager::get_product_level_color(const QString& str_level_name) const
{
	for (const auto& item : m_product_level)
	{
		if (item.level == str_level_name)
			return item.str_color;
	}

	return {};
}

QColor Product_Level_Manager::get_product_level_trivial_color(int level) const
{
	for (const auto& item : m_product_level)
	{
		if (item.level == level)
			return item.str_trivial_defect_color;
	}

	return {};
}

QColor Product_Level_Manager::get_product_level_trivial_color(const QString& str_level_name) const
{
	for (const auto& item : m_product_level)
	{
		if (item.strLevelName == str_level_name)
			return item.str_trivial_defect_color;
	}

	return {};
}

bool Product_Level_Manager::is_level_ok(int level)
{
	for (const auto& item : m_product_level)
	{
		if (item.level == level)
			return item.is_ok;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace product_rule_module
{
	void PRODUCT_RULE_MODULE_EXPORT modify_product_rule(Rule_Manager& src_rule,const std::vector<Product_Level>& product_levels)
	{
		RuleSettingDlg dlg;
		Product_Rule rule;
		src_rule.get_rules(rule);
		dlg.set_data(rule, product_levels);
		dlg.init();
		if (dlg.exec() == QDialog::Accepted)
		{
			auto rule  = dlg.get_product_rule();			
			src_rule.set_product_rule(rule);
		}	
	}
}

