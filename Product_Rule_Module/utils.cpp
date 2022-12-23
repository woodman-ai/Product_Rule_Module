#include "utils.h"
#include "qjsondocument.h"
#include "qfile.h"
#include "qtextstream.h"
#include "Rule_Attri_Manager.h"
#include "qjsonarray.h"

QJsonObject read_json_document(const QString& str_path)
{
	QFile file(str_path);
	file.open(QIODevice::ReadOnly);
	QByteArray data = file.readAll();
	file.close();
	auto jdoc = QJsonDocument::fromJson(data);
	return jdoc.object();
}

void save_json_files(const QString& strPath, const QJsonObject& obj)
{
	QJsonDocument jdoc;
	jdoc.setObject(obj);
	QFile FileSave(strPath);
	if (FileSave.open(QIODevice::WriteOnly | QFile::Truncate)) {
		QTextStream writeStream(&FileSave);
		writeStream.setCodec("UTF-8");
		writeStream << jdoc.toJson();
		FileSave.close();
	};
}

QString rect_2_string(const QRect& rt)
{
	QString str = QString::number(rt.x()) + "," +
		QString::number(rt.y()) + "," +
		QString::number(rt.width()) + "," +
		QString::number(rt.height());
	return str;
}

QRect string_2_rect(const QString& str)
{
	auto strs = str.split(',');
	if (strs.size() != 4)
		return {};

	QRect rt;
	rt.setX(strs[0].toInt());
	rt.setY(strs[1].toInt());
	rt.setWidth(strs[2].toInt());
	rt.setHeight(strs[3].toInt());
	return rt;
}

QString relation_2_string(relation re)
{
	switch (re)
	{
	case relation::less_than:
		return QStringLiteral("小于");
	case relation::bigger_than:
		return QStringLiteral("大于");
	default:
		break;
	}

	return "";
}

relation relation_from_string(const QString& string)
{
	if (string == QStringLiteral("小于"))
		return relation::less_than;
	else if (string == QStringLiteral("大于"))
		return relation::bigger_than;

	return relation::less_than;
}

QString Defect_Node_Type_2_string(Defect_Node_Type type)
{
	switch (type)
	{
	case Defect_Node_Type::Expression:
		return QStringLiteral("表达式");
	case Defect_Node_Type::And:
		return QStringLiteral("并且");
	case Defect_Node_Type::Or:
		return QStringLiteral("或者");
	default:
		break;
	}

	return "";
}

Defect_Node_Type Defect_Node_Type_from_string(const QString& string)
{
	if (string == QStringLiteral("表达式"))
		return Defect_Node_Type::Expression;
	else if (string == QStringLiteral("并且"))
		return Defect_Node_Type::And;
	else if (string == QStringLiteral("或者"))
		return Defect_Node_Type::Or;

	return Defect_Node_Type::Expression;
}

QJsonObject Rule_Calc_Node_2_json(const Rule_Calc_Node& target)
{
	QJsonObject obj;
	obj.insert("compare_attri", target.str_attri_name);
	obj.insert("compare_value", target.compare_value);
	obj.insert("relation", target.str_relation);
	if (target.str_attri_name == QStringLiteral("反射率"))
		obj.insert("gray_index", target.g_index);
	if (target.str_attri_name == QStringLiteral("面积比"))
		obj.insert("a_index", target.a_index);
	return obj;
}

Rule_Calc_Node Rule_Calc_Node_from_json(const QJsonObject& obj)
{
	Rule_Calc_Node data;
	data.str_attri_name = obj["compare_attri"].toString();
	data.str_relation = obj["relation"].toString();
	data.compare_value = obj["compare_value"].toDouble();
	if(data.str_attri_name == QStringLiteral("反射率"))
		data.g_index = obj["gray_index"].toInt();
	if (data.str_attri_name == QStringLiteral("面积比"))
		data.a_index = obj["a_index"].toInt();
	data.func = Rule_Attri_Manager::generate_defect_rule_node(data);
	return data;
}

QString Rule_Calc_Node_2_String(const Rule_Calc_Node& target)
{
	if (target.str_attri_name == QStringLiteral("反射率"))
	{
		return QStringLiteral("第") + QString::number(target.g_index + 1) + QStringLiteral("张图像上反射率 ") + target.str_relation + " " + QString::number(target.compare_value);
	}
	else if (target.str_attri_name == QStringLiteral("面积比"))
	{
		return QStringLiteral("第") + QString::number(target.a_index + 1) + QStringLiteral("张图像上面积比 ") + target.str_relation + " " + QString::number(target.compare_value);
	}
	else
	{
		return target.str_attri_name + " " + target.str_relation + " " + QString::number(target.compare_value) + Rule_Attri_Manager::attributes_literal_unit(target.str_attri_name);
	}
}

QJsonObject Defect_Node_2_json(const Defect_Node& target)
{
	QJsonObject obj;
	auto str = Defect_Node_Type_2_string(target.node_type);
	obj.insert("node_type", str);
	if(target.node_type == Defect_Node_Type::Expression)
		obj.insert("node_exp", Rule_Calc_Node_2_json(target.expression));
	if (!target.subNodes.empty())
	{
		QJsonArray array;
		for (const auto& item : target.subNodes)
		{
			array.append(Defect_Node_2_json(item));
		}
		obj.insert("sub_nodes", array);
	}
	
	return obj;
}

Defect_Node Defect_Node_from_json(const QJsonObject& obj)
{
	Defect_Node temp_recursive;
	auto str = obj.value("node_type").toString();
	temp_recursive.node_type = Defect_Node_Type_from_string(str);
	if(obj.contains("node_exp"))
		temp_recursive.expression = Rule_Calc_Node_from_json(obj.value("node_exp").toObject());
	auto sub_array = obj.value("sub_nodes").toArray();
	for (const auto& item : sub_array)
	{
		temp_recursive.subNodes.push_back(Defect_Node_from_json(item.toObject()));
	}
	return temp_recursive;
}

QJsonObject Defect_Rule_2_json(const Defect_Rule& target)
{
	QJsonObject obj;
	obj.insert("defect_name", target.str_defect_name);
	obj.insert("max_count", target.max_count);
	obj.insert("defect_exp", Defect_Node_2_json(target.calc_expression));
	if (target.max_area)
		obj.insert("max_area", *target.max_area);
	return obj;
}


Defect_Rule Defect_Rule_from_json(const QJsonObject& obj)
{
	Defect_Rule defect;
	defect.str_defect_name = obj["defect_name"].toString();
	defect.cur_count = 0;	
	defect.max_count = obj["max_count"].toInt();
	defect.cur_area = 0;
	if (obj.contains("max_area"))
		defect.max_area = obj["max_area"].toDouble();
	defect.calc_expression = Defect_Node_from_json(obj["defect_exp"].toObject());
	
	return defect;
}

QJsonObject Merge_Rule_2_json(const Merge_Rule& target)
{
	QJsonObject obj;
	obj.insert("src_defect_name", target.str_src_defect_name);
	obj.insert("merge_defect_name", target.str_merge_defect_name);
	obj.insert("radiums",target.radiums);
	obj.insert("min_num", target.min_num);
	obj.insert("defect_exp", Defect_Node_2_json(target.calc_expression));
	return obj;
}

Merge_Rule  Merge_Rule_from_json(const QJsonObject& obj)
{
	Merge_Rule rule;
	rule.str_src_defect_name = obj.value("src_defect_name").toString();
	rule.str_merge_defect_name = obj.value("merge_defect_name").toString();
	rule.radiums = obj.value("radiums").toDouble();
	rule.min_num = obj.value("min_num").toInt();
	rule.calc_expression = Defect_Node_from_json(obj["defect_exp"].toObject());
	return rule;
}

QJsonObject Product_Level_2_json(const Product_Level& target)
{
	QJsonObject obj;
	obj.insert("level_name", target.strLevelName);
	obj.insert("level", target.level);
	obj.insert("color", target.str_color);
	obj.insert("is_ok", target.is_ok);
	obj.insert("trivial_defect_color", target.str_trivial_defect_color);
	return obj;
}
Product_Level Product_Level_from_json(const QJsonObject& obj)
{
	Product_Level level;
	level.strLevelName = obj["level_name"].toString();
	level.level = obj["level"].toInt();
	level.is_ok = obj["is_ok"].toBool();
	level.str_color = obj["color"].toString();
	level.str_trivial_defect_color = level.str_color;
	if (obj.contains("trivial_defect_color"))
		level.str_trivial_defect_color = obj["trivial_defect_color"].toString();
	return level;
}

QJsonObject Product_Level_Defects_2_json(const Product_Level_Defects& target)
{
	QJsonObject obj;
	obj.insert("product_level", Product_Level_2_json(target.product_level));
	QJsonArray rule;
	for (const auto& defect : target.level_defects)
	{
		rule.append(Defect_Rule_2_json(defect));
	}
	obj.insert("defects", rule);
	return obj;
}
Product_Level_Defects Product_Level_Defects_from_json(const QJsonObject& obj)
{
	Product_Level_Defects level_defects;
	level_defects.product_level = Product_Level_from_json(obj["product_level"].toObject());
	QJsonArray rule_array = obj["defects"].toArray();
	for (const auto& item : rule_array)
	{
		level_defects.level_defects.push_back(Defect_Rule_from_json(item.toObject()));
	}
	return level_defects;
}

QJsonObject CheckSurface_One_Rule_2_json(const CheckSurface_One_Rule& target)
{
	QJsonObject obj;
	QJsonArray checksurface_polygon;
	for (const auto& polygon : target.rule_contours)
	{
		QJsonObject obj_temp;
		obj_temp.insert("product_surface_name", polygon.first);
		QJsonArray polygon_array;
		for (const auto& polygon : polygon.second)
		{
			QJsonArray one_poly;
			for (const auto& pt : polygon)
			{
				one_poly.append(pt.x());
				one_poly.append(pt.y());
			}
			polygon_array.append(one_poly);
		}
		obj_temp.insert("product_surface_roi", polygon_array);
		checksurface_polygon.append(obj_temp);
	}
	
	obj.insert("contours", checksurface_polygon);
	
	QJsonArray rule_array;
	for (const auto& item : target.rule_levels)
	{
		rule_array.append(Product_Level_Defects_2_json(item));
	}

	QJsonArray valid_surface_array;
	for (const auto& item : target.valid_product_surface)
	{
		valid_surface_array.append(item);
	}

	obj.insert("valid_product_surface", valid_surface_array);
	obj.insert("rule", rule_array);
	obj.insert("rule_name", target.str_name);
	obj.insert("rule_roi_color", target.str_roi_color);
	return obj;
}

CheckSurface_One_Rule CheckSurface_One_Rule_from_json(const QJsonObject& obj)
{
	CheckSurface_One_Rule one_rule;
	QJsonArray controus = obj["contours"].toArray();
	for (const auto& surface_polygon : controus)
	{
		auto obj_temp = surface_polygon.toObject();
		auto surface_name = obj_temp.value("product_surface_name").toString();
		std::vector<QPolygon> polygon_vector;
		QJsonArray polygon_array = obj_temp.value("product_surface_roi").toArray();
		for (const auto& item : polygon_array)
		{
			auto single_polygon_array = item.toArray();
			QPolygon polygon;
			for (int i = 0; i < single_polygon_array.size(); i += 2)
			{
				QPoint pt;
				pt.setX(single_polygon_array[i].toInt());
				pt.setY(single_polygon_array[i + 1].toInt());
				polygon.append(pt);
			}
			polygon_vector.push_back(polygon);
		}		
		one_rule.rule_contours[surface_name] = polygon_vector;
	}

	QJsonArray array = obj["rule"].toArray();
	for (const auto& item : array)
	{
		one_rule.rule_levels.push_back(Product_Level_Defects_from_json(item.toObject()));
	}

	QJsonArray valid_surface_array = obj["valid_product_surface"].toArray();
	for (const auto& item : valid_surface_array)
	{
		one_rule.valid_product_surface.insert(item.toString());
	}

	if(obj.contains("rule_name"))
		one_rule.str_name = obj["rule_name"].toString();

	if(obj.contains("rule_roi_color"))
		one_rule.str_roi_color = obj["rule_roi_color"].toString();

	return one_rule;
}

QJsonObject Surface_Rule_2_json(const Surface_Rule& target)
{
	QJsonObject obj;
	obj.insert("check_surface_name", target.str_check_surface_name);
	QJsonArray array;
	for (const auto& item : target.surface_rules)
	{
		array.append(CheckSurface_One_Rule_2_json(item));
	}
	obj.insert("surface_rule", array);
	return obj;
}

Surface_Rule Surface_Rule_from_json(const QJsonObject& obj)
{
	Surface_Rule rule;
	rule.str_check_surface_name = obj["check_surface_name"].toString();
	auto array = obj["surface_rule"].toArray();
	for (const auto& item : array)
	{
		rule.surface_rules.push_back(CheckSurface_One_Rule_from_json(item.toObject()));
	}
	std::map<QString, std::set<int>> defect_check_gray_index;
	for (const auto& item : rule.surface_rules)
	{
		auto temp = item.get_defect_gray_index();
		defect_check_gray_index.merge(temp);
	}
	rule._defect_check_gray_index = defect_check_gray_index;

	std::map<QString, std::set<int>> defect_check_area_index;
	for (const auto& item : rule.surface_rules)
	{
		auto temp = item.get_defect_area_index();
		defect_check_area_index.merge(temp);
	}
	rule._defect_check_area_index = defect_check_area_index;

	return rule;
}

QJsonObject Product_Rule_2_json(const Product_Rule& target)
{
	QJsonObject obj;
	QJsonArray array;
	for (const auto& rule : target)
	{
		array.append(Surface_Rule_2_json(rule));
	}
	obj.insert("product_rule", array);
	return obj;
}

Product_Rule Product_Rule_from_json(const QJsonObject& obj)
{
	Product_Rule rule_vector;
	auto array = obj.value("product_rule").toArray();
	for (const auto& item : array)
	{
		auto obj = item.toObject();
		auto temp = Surface_Rule_from_json(obj);
		rule_vector.push_back(temp);
	}
	return rule_vector;
}