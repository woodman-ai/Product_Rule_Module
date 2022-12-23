#include "Rule_Manager.h"
#include "rule_class.h"
#include "utils.h"
#include "RuleSettingDlg.h"
#include "qjsonarray.h"
#include "Rule_Attri_Manager.h"

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

std::map<check_attri_index, check_attri_value>  Rule_Manager::get_defect_attri(const Defect_Feature& feature)
{
	return Rule_Attri_Manager::get_check_attri_dict_from_defect(feature);
}

Rule_Manager::Rule_Manager(const Rule_Manager& left)
{
	*this = left;
}

Rule_Manager::~Rule_Manager()
{
	if (m_rule)
		delete m_rule;
	m_rule = nullptr;
}

Rule_Manager& Rule_Manager::operator = (const Rule_Manager& left)
{
	if (m_rule)
		delete m_rule;
	m_rule = nullptr;
	if (left.m_rule)
	{
		m_rule = new std::vector<Surface_Rule>();
		*m_rule = *left.m_rule;
	}
	return *this;
}

Rule_Manager::Rule_Manager(Rule_Manager&& left)
{
	if (m_rule)
		delete m_rule;
	m_rule = left.m_rule;
	left.m_rule = nullptr;
}

void Rule_Manager::load_from_json(const QString& str_path)
{
	auto obj = read_json_document(str_path);
	load_from_jsonobject(obj);
}

void Rule_Manager::save_to_json(const QString& str_path)
{
	if (m_rule)
	{
		auto obj = Product_Rule_2_json(*m_rule);
		save_json_files(str_path, obj);
	}
}

void Rule_Manager::load_from_jsonobject(const QJsonObject& obj)
{
	if (m_rule == nullptr)
		m_rule = new Product_Rule;
	*m_rule = Product_Rule_from_json(obj);
}

void Rule_Manager::set_product_rule(const std::vector<Surface_Rule>& rules)
{
	if (m_rule == nullptr)
		m_rule = new Product_Rule;
	*m_rule = rules;
}

void Rule_Manager::get_rules(std::vector<Surface_Rule>& vect) const
{
	if (m_rule == nullptr)
		return;
	vect = *m_rule;
}

int  Rule_Manager::get_check_surface_index(const QString& str_check_surface_name)
{
	auto& surface_rule = *((std::vector<Surface_Rule>*)m_rule);
	for (int i = 0; i < surface_rule.size(); i++)
	{
		if (surface_rule[i].str_check_surface_name == str_check_surface_name)
			return i;
	}

	return -1;	
}

std::set<int> Rule_Manager::get_surface_defect_inspect_gray(int rule_index, const QString& str_defect_name)
{
	auto& surface_rule = *((std::vector<Surface_Rule>*)m_rule);
	if (rule_index < 0 || rule_index >= surface_rule.size())
		return {};
	auto iter = surface_rule[rule_index]._defect_check_gray_index.find(str_defect_name);
	if (iter == surface_rule[rule_index]._defect_check_gray_index.end())
		return {};

	return iter->second;
}

std::set<int> Rule_Manager::get_surface_defect_inspect_area(int rule_index, const QString& str_defect_name)
{
	auto& surface_rule = *((std::vector<Surface_Rule>*)m_rule);
	if (rule_index < 0 || rule_index >= surface_rule.size())
		return {};
	auto iter = surface_rule[rule_index]._defect_check_area_index.find(str_defect_name);
	if (iter == surface_rule[rule_index]._defect_check_area_index.end())
		return {};

	return iter->second;
}

std::optional<Rule_Manager::rule_result> Rule_Manager::is_defect_out_of_rule(int index, const QString& str_product_surface_name,
	const QString& str_defect_name,
	const QPolygon& poly_countour,
	const inspect_defect_attri_map& defect_attri,
	const inspect_defect_gray_map& gray_attri,
	const inspect_defect_gray_map& a_attri,
	double scale)
{
	auto& surface_rule_vector = *((std::vector<Surface_Rule>*)m_rule);
	if (index < 0 || index >= surface_rule_vector.size())
		return { };

	auto& surface_rule_data = surface_rule_vector[index];   //具体找到某一个check_surface;
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

	const auto& rule_vector = surface_rule_data.surface_rules;

	Check_Result result;
	for (int rule_index = 0; rule_index < rule_vector.size(); rule_index++)
	{
		auto iter_surface_polygon = rule_vector[rule_index].rule_contours.find(str_product_surface_name);
		if (iter_surface_polygon != rule_vector[rule_index].rule_contours.end())
		{
			bool bintersected = false;
			const auto& poly_vector = iter_surface_polygon->second;
			if (!poly_vector.empty())
			{
				for (const auto& poly : poly_vector)
				{
					if (poly_countour.intersects(poly))
					{
						bintersected = true;
						break;
					}
				}
			}
			else
			{
				bintersected = true;
			}

			if (!bintersected)
			{
				continue;
			}
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
					bool brule = defect.get_value(defect_attri, gray_attri, a_attri,scale);
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
		auto& one_rule = surface_rule_vector[index].surface_rules[result._one_rule_index];
		auto& rule_level = one_rule.rule_levels[result._level_index];
		rule_level.defect_count_increase(result._defect_index);
		auto iter = defect_attri.find(3);
		if (iter != defect_attri.end())
		{
			double mm_value = iter->second / scale / scale;
			rule_level.defect_accumulate_area_increate(result._defect_index, mm_value);
		}

		bool is_ok = rule_level.product_level.is_ok;
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

bool Rule_Manager::is_check_surface_ok(int index)
{
	auto& surface_rule_vector = *((std::vector<Surface_Rule>*)m_rule);
	if (index < 0 || index >= surface_rule_vector.size())
		return true;

	auto& surface_rule_data = surface_rule_vector[index];   //具体找到某一个check_surface;
	for (const auto& one_surface : surface_rule_data.surface_rules)
	{
		for (const auto& one_level : one_surface.rule_levels)
		{
			if (one_level.bover_limit)
			{
				if (one_level.product_level.is_ok == false)
				{
					return false;
				}
			}
		}
	}

	return true;
	
}

void Rule_Manager::reset_check_surface(int index)
{
	auto& surface_rule_vector = *((std::vector<Surface_Rule>*)m_rule);
	if (index < 0 || index >= surface_rule_vector.size())
		return;

	auto& surface_rule_data = surface_rule_vector[index];   //具体找到某一个check_surface;
	surface_rule_data.reset();
}

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Product_Level> Product_Level_Manager::get_product_levels() const
{
	return m_product_level;
}

void Product_Level_Manager::load_from_json(const QString& str_path)
{
	auto obj = read_json_document(str_path);
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

bool Product_Level_Manager::is_product_level_ok(int level) const
{
	for (const auto& item : m_product_level)
	{
		if (item.level == level)
			return item.is_ok;
	}

	return false;
}

bool Product_Level_Manager::is_product_level_ok(const QString& str_level_name)
{
	for (const auto& item : m_product_level)
	{
		if (item.level == str_level_name)
			return item.is_ok;
	}

	return false;
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
		if (item.level == str_level_name)
			return item.str_trivial_defect_color;
	}

	return {};
}








//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace product_rule_module
{
	std::optional<Rule_Manager> PRODUCT_RULE_MODULE_EXPORT modify_product_rule(const Rule_Manager* src_rule, const product_info_manager* info_manager, const CheckSurfaceManager* surface_manager, const std::vector<Product_Level>& producb_levels)
	{
		RuleSettingDlg dlg;
		Product_Rule rule;
		src_rule->get_rules(rule);
		dlg.set_data(info_manager, surface_manager, rule,producb_levels);
		dlg.init();
		if (dlg.exec() == QDialog::Accepted)
		{
			auto rule  = dlg.get_product_rule();	
			Rule_Manager manager;
			manager.set_product_rule(rule);
			return manager;
		}

		return {};
	}
}

