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
		criticial_defect, //����ȱ��
		trivial_defect    //max_count ��Ϊ0��ȱ��
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
	* ����ʱ���ȵ��� get_check_surface_index �õ���������index;
	* �ٵ��� get_surface_defect_inspect_gray ���õ����ȱ��Ҫ����Щͼ�ϵõ��Ҷ�ֵ���浽map<int,double>��
	* ����ٵ��� is_out_of_rule �ж�ȱ���Ƿ��ڹ������ڡ�
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
		double scale); //����ֵ [����index,�ȼ�index,ȱ��index��is_out_of_rule] ,û��������ס ����[-1,-1,-1,false];
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

