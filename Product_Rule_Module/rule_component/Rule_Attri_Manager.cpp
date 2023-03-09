#include "Rule_Attri_Manager.h"
#include "utils.h"

std::vector<QString> Rule_Attri_Manager::get_rule_attributes()
{
	return { "gray","width","height","area","skeleton_length","score","area_ratio"};
}

std::vector<QString> Rule_Attri_Manager::get_rule_attributes_literal()
{
	return { QStringLiteral("������"),QStringLiteral("���"),QStringLiteral("����") ,QStringLiteral("���") ,QStringLiteral("�Ǽܳ���") ,QStringLiteral("�÷�"),QStringLiteral("�����") };
}

QString Rule_Attri_Manager::attributes_literal_2_attributes(const QString& str_literal)
{
	if (str_literal == QStringLiteral("������"))
		return "gray";
	else if (str_literal == QStringLiteral("���"))
		return "width";
	else if (str_literal == QStringLiteral("����"))
		return "height";
	else if (str_literal == QStringLiteral("���"))
		return "area";
	else if (str_literal == QStringLiteral("�Ǽܳ���"))
		return "skeleton_length";
	else if (str_literal == QStringLiteral("�÷�"))
		return "score";
	else if (str_literal == QStringLiteral("�����"))
		return "area_ratio";

	return "";
}

QString Rule_Attri_Manager::attributes_2_attributes_literal(const QString& str_attri)
{
	if (str_attri == "gray")
		return  QStringLiteral("������");
	else if (str_attri == "width")
		return QStringLiteral("���");
	else if (str_attri == ("height"))
		return QStringLiteral("�߶�");
	else if (str_attri == ("area"))
		return QStringLiteral("���");
	else if (str_attri == ("skeleton_length"))
		return QStringLiteral("�Ǽܳ���");
	else if (str_attri == ("score"))
		return QStringLiteral("�÷�");
	else if (str_attri == ("area_ratio"))
		return QStringLiteral("�����");

	return "";
}

QString Rule_Attri_Manager::attributes_literal_unit(const QString& str_literal)
{
	if (str_literal == QStringLiteral("������"))
		return QStringLiteral("");
	else if (str_literal == QStringLiteral("���"))
		return QStringLiteral("����");
	else if (str_literal == QStringLiteral("����"))
		return QStringLiteral("����");
	else if (str_literal == QStringLiteral("���"))
		return QStringLiteral("ƽ������");
	else if (str_literal == QStringLiteral("�Ǽܳ���"))
		return QStringLiteral("����");
	else if (str_literal == QStringLiteral("�÷�"))
		return "";
	
	return "";
}

check_attri_index  Rule_Attri_Manager::get_check_index_from_string(const QString& str_attri_name)
{
	if (str_attri_name == "gray")
		return 0;
	else if (str_attri_name == "width")
		return 1;
	else if (str_attri_name == "height")
		return 2;
	else if (str_attri_name == "area")
		return 3;
	else if (str_attri_name == "skeleton_length")
		return 4;
	else if (str_attri_name == "score")
		return 5;
	else if (str_attri_name == "area_ratio")
		return 6;

	return -1;
}

QString Rule_Attri_Manager::get_check_index_name(check_attri_index attri_index)
{
	if (attri_index == 0)
		return "gray";
	else if (attri_index == 1)
		return "width";
	else if (attri_index == 2)
		return "height";
	else if (attri_index == 3)
		return "area";
	else if (attri_index == 4)
		return "skeleton_length";
	else if (attri_index == 5)
		return "score";
	else if (attri_index == 6)
		return "area_ratio";
	return "error_check_attri";
}

std::map<check_attri_index, check_attri_value> Rule_Attri_Manager::get_check_attri_dict_from_defect(const Defect_Feature& defect)
{
	std::map<check_attri_index, check_attri_value> rule_dict;
	rule_dict[get_check_index_from_string("width")] = defect.width;
	rule_dict[get_check_index_from_string("height")] = defect.height;
	rule_dict[get_check_index_from_string("area")] = defect.area;
	rule_dict[get_check_index_from_string("skeleton_length")] = defect.skeleton_length;
	rule_dict[get_check_index_from_string("score")] = defect.score;
	return rule_dict;
}

std::function<bool(const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict, const std::map<int, double>& a_dict, double)>
Rule_Attri_Manager::generate_defect_rule_node(const Rule_Calc_Node& save_data)
{
	auto use_str_attri = attributes_literal_2_attributes(save_data.str_attri_name);
	check_attri_index attri_index = get_check_index_from_string(use_str_attri);
	relation re = relation_from_string(save_data.str_relation);
	int gray_index = save_data.g_index;
	int a_index = save_data.a_index;
	double calc_value = save_data.compare_value;
	auto compare = [attri_index, calc_value, gray_index,a_index, re](const std::map<check_attri_index, check_attri_value>& attri_dict, const std::map<int, double>& gray_dict,const std::map<int,double> a_dict, double scale)
	{
		const std::map<int, double>* dict = nullptr;
		if (attri_index == 0)
			dict = &gray_dict;
		else if (attri_index == 6)
			dict = &a_dict;
		else
			dict = &attri_dict;

		if (dict == nullptr)
			return false;
				
		double compare_value = calc_value;
		if (attri_index != 0 && attri_index != 5 && attri_index != 6 )
		{
			compare_value *= scale;
			if (attri_index == 3)
				compare_value *= scale;
		}
		double num = 0;
		if (attri_index != 0 && attri_index != 6)
		{
			auto iter = dict->find(attri_index);
			if (iter == dict->end())
				return false;
			num = iter->second;
		}
		else if( attri_index == 0)
		{
			auto iter = dict->find(gray_index);
			if (iter == dict->end())
				return false;
			num = iter->second;
		}
		else if (attri_index == 6)
		{
			auto iter = dict->find(a_index);
			if (iter == dict->end())
				return false;
			num = iter->second;
		}

		if (re == relation::less_than)
			return num < compare_value;
		else
			return num > compare_value;
	};
	return compare;
}
