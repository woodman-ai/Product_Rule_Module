#pragma once
#include "qstring.h"
#include "qjsonobject.h"
#include "Product_Rule_TypeDef.h"
#include "product_rule_module_global.h"
#include <set>
#include <optional>
struct Surface_Rule;
class PRODUCT_RULE_MODULE_EXPORT Rule_Manager
{
	std::vector<Surface_Rule>* m_rule = nullptr;
public:
	Rule_Manager() {}
	Rule_Manager(const Rule_Manager& left);
	Rule_Manager(Rule_Manager&& left);
	Rule_Manager& operator = (const Rule_Manager& left);
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
	using inspect_defect_attri_map = std::map<check_attri_index, check_attri_value>;
	using inspect_defect_gray_map = std::map<int, double>;

	void load_from_json(const QString& str_path);
	void save_to_json(const QString& str_path);
	void load_from_jsonobject(const QJsonObject& obj);
	void set_product_rule(const std::vector<Surface_Rule>& rules);
	void get_rules(std::vector<Surface_Rule>& vect) const;
	static std::map<check_attri_index, check_attri_value>  get_defect_attri(const Defect_Feature& feature);
	/*
	* 调用时，先调用 get_check_surface_index 得到待测规则的index;
	* 再调用 get_surface_defect_inspect_gray ，得到这个缺陷要在那些图上得到灰度值。存到map<int,double>中
	* 最后再调用 is_out_of_rule 判断缺陷是否在规则以内。
	*/
	int  get_check_surface_index(const QString& str_check_surface_name);
	std::set<int> get_surface_defect_inspect_gray(int rule_index, const QString& str_defect_name);
	std::set<int> get_surface_defect_inspect_area(int rule_index, const QString& str_defect_name);
	std::optional<rule_result> is_defect_out_of_rule(int index,const QString& str_product_surface_name,
		const QString& str_defect_name,
		const QPolygon& poly_countour,
		const inspect_defect_attri_map& defect_attri,
		const inspect_defect_gray_map& gray_attri,
		const inspect_defect_gray_map& a_attri,
		double scale); //返回值 [规则index,等级index,缺陷index，is_out_of_rule] ,没被规则拦住 返回[-1,-1,-1,false];
	bool is_check_surface_ok(int index);
	void reset_check_surface(int index);
};

class PRODUCT_RULE_MODULE_EXPORT Product_Level_Manager
{
	std::vector<Product_Level> m_product_level;
public:
	std::vector<Product_Level> get_product_levels() const;
	void load_from_json(const QString& str_path);
	void load_from_json(const QJsonObject& obj);
	bool is_product_level_ok(int level) const;
	bool is_product_level_ok(const QString& str_level_name);
	QColor get_product_level_color(int level) const;
	QColor get_product_level_color(const QString& str_level_name) const;
	QColor get_product_level_trivial_color(int level) const;
	QColor get_product_level_trivial_color(const QString& str_level_name) const;
};

class product_info_manager;
class CheckSurfaceManager;
namespace product_rule_module
{
	using std::optional;
	std::optional<Rule_Manager> PRODUCT_RULE_MODULE_EXPORT modify_product_rule(const Rule_Manager* src_rule, const product_info_manager* info_manager, const CheckSurfaceManager* surface_manager,const std::vector<Product_Level>& producb_levels);
}

