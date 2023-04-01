#pragma once
#include "qstring.h"
#include "qjsonobject.h"
#include "Product_Rule_TypeDef.h"
#include "product_rule_module_global.h"
#include <set>
#include <optional>
struct ProductSurface_Rule;
class PRODUCT_RULE_MODULE_EXPORT Rule_Manager
{
	std::map<std::wstring, ProductSurface_Rule>* m_product_surface_rule;
public:
	Rule_Manager();
	Rule_Manager(const Rule_Manager&) = delete;
	Rule_Manager& operator = (const Rule_Manager&) = delete;
	~Rule_Manager();
public:

	enum class Out_Of_Rule_Defect_Type
	{
		criticial_defect, //严重缺陷
		trivial_defect    //max_count 不为0的缺陷
	};

	struct rule_result
	{
		Product_Level level;
		int rule_index;
		int level_index;
		int defect_index;
		Out_Of_Rule_Defect_Type out_of_rule_type;
	};

	static QString defect_rule_result_ok();
	static QString defect_rule_result_ng();
	static QString defect_rule_result_trivial_defect();
public:
	using inspect_defect_attri_map = std::map<check_attri_name, check_attri_value>;

	void load_from_json(const QString& str_path);
	void save_to_json(const QString& str_path);
	void load_from_jsonobject(const QJsonObject& obj);
	void set_product_rule(const std::vector<ProductSurface_Rule>& rules);
	void get_rules(std::vector<ProductSurface_Rule>& vect) const;
	std::optional<rule_result> is_defect_out_of_rule(const QString& str_product_surface_name,
		const QString& str_defect_name,
		const QPolygon& poly_countour,
		const inspect_defect_attri_map& defect_attri,
		double scale); //返回值 [规则index,等级index,缺陷index，is_out_of_rule] ,没被规则拦住 返回[-1,-1,-1,false];
	int get_surface_level(const QString& str_product_surface_name);
	void reset_check_surface(const QString& str_product_surface_name);
};

class PRODUCT_RULE_MODULE_EXPORT Product_Level_Manager
{
	std::vector<Product_Level> m_product_level;
public:
	std::vector<Product_Level> get_product_levels() const;
	void load_from_json(const QString& str_path);
	void load_from_json(const QJsonObject& obj);
	QColor get_product_level_color(int level) const;
	QColor get_product_level_color(const QString& str_level_name) const;
	QColor get_product_level_trivial_color(int level) const;
	QColor get_product_level_trivial_color(const QString& str_level_name) const;
	bool is_level_ok(int level);
};

class product_info_manager;
class CheckSurfaceManager;
namespace product_rule_module
{
	void PRODUCT_RULE_MODULE_EXPORT modify_product_rule(Rule_Manager& src_rule, const std::vector<Product_Level>& product_levels);
}

