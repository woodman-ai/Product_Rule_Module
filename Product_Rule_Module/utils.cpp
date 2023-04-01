#include "utils.h"
#include "qjsondocument.h"
#include "qfile.h"
#include "qtextstream.h"
#include "Product_Rule_Export.h"
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
	return obj;
}

Rule_Calc_Node Rule_Calc_Node_from_json(const QString& str_surface_name, const QJsonObject& obj)
{
	Rule_Calc_Node data;
	data.str_attri_name = obj["compare_attri"].toString();
	data.str_relation = obj["relation"].toString();
	data.compare_value = obj["compare_value"].toDouble();
	data.func = generate_defect_rule_node(str_surface_name,data);
	return data;
}

QString Rule_Calc_Node_2_String(const std::wstring& str_surface_name, const Rule_Calc_Node& target)
{
	return target.str_attri_name + " " + target.str_relation + " " + QString::number(target.compare_value) + QString::fromStdWString(attributes_literal_unit(str_surface_name,target.str_attri_name.toStdWString()));
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

Defect_Node Defect_Node_from_json(const QString& str_product_surface_name, const QJsonObject& obj)
{
	Defect_Node temp_recursive;
	auto str = obj.value("node_type").toString();
	temp_recursive.node_type = Defect_Node_Type_from_string(str);
	if(obj.contains("node_exp"))
		temp_recursive.expression = Rule_Calc_Node_from_json(str_product_surface_name,obj.value("node_exp").toObject());
	auto sub_array = obj.value("sub_nodes").toArray();
	for (const auto& item : sub_array)
	{
		temp_recursive.subNodes.push_back(Defect_Node_from_json(str_product_surface_name,item.toObject()));
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


Defect_Rule Defect_Rule_from_json(const QString& str_surface_name, const QJsonObject& obj)
{
	Defect_Rule defect;
	defect.str_defect_name = obj["defect_name"].toString();
	defect.cur_count = 0;	
	defect.max_count = obj["max_count"].toInt();
	defect.cur_area = 0;
	if (obj.contains("max_area"))
		defect.max_area = obj["max_area"].toDouble();
	defect.calc_expression = Defect_Node_from_json(str_surface_name,obj["defect_exp"].toObject());
	
	return defect;
}

QJsonObject Product_Level_2_json(const Product_Level& target)
{
	QJsonObject obj;
	obj.insert("level_name", target.strLevelName);
	obj.insert("level", target.level);
	obj.insert("color", target.str_color);
	obj.insert("trivial_defect_color", target.str_trivial_defect_color);
	obj.insert("is_ok", target.is_ok);
	return obj;
}
Product_Level Product_Level_from_json(const QJsonObject& obj)
{
	Product_Level level;
	level.strLevelName = obj["level_name"].toString();
	level.level = obj["level"].toInt();
	level.str_color = obj["color"].toString();
	level.is_ok = obj.value("is_ok").toBool();
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
Product_Level_Defects Product_Level_Defects_from_json(const QString& str_surface_name, const QJsonObject& obj)
{
	Product_Level_Defects level_defects;
	level_defects.product_level = Product_Level_from_json(obj["product_level"].toObject());
	QJsonArray rule_array = obj["defects"].toArray();
	for (const auto& item : rule_array)
	{
		level_defects.level_defects.push_back(Defect_Rule_from_json(str_surface_name,item.toObject()));
	}
	return level_defects;
}

QJsonObject ProductSurface_One_Rule_2_json(const ProductSurface_One_Rule& target)
{
	QJsonObject obj;
	QJsonArray polygon_array;
	for (const auto& polygon : target.rule_contours)
	{
		QJsonArray one_poly;
		for (const auto& pt : polygon)
		{
			one_poly.append(pt.x());
			one_poly.append(pt.y());
		}
		polygon_array.append(one_poly);		
	}
	
	obj.insert("contours", polygon_array);
	
	QJsonArray rule_array;
	for (const auto& item : target.rule_levels)
	{
		rule_array.append(Product_Level_Defects_2_json(item));
	}

	obj.insert("rule", rule_array);
	obj.insert("rule_name", target.str_name);
	obj.insert("rule_roi_color", target.str_roi_color);
	return obj;
}

ProductSurface_One_Rule ProductSurface_One_Rule_from_json(const QString& str_surface_name, const QJsonObject& obj)
{
	ProductSurface_One_Rule one_rule;
	QJsonArray controus = obj["contours"].toArray();
	for (const auto& surface_polygon : controus)
	{
		auto poly_obj = surface_polygon.toArray();
		QPolygon polygon;
		for (int i = 0; i < poly_obj.size(); i += 2)
		{
			QPoint pt;
			pt.setX(poly_obj[i].toInt());
			pt.setY(poly_obj[i + 1].toInt());
			polygon.append(pt);
		}
		one_rule.rule_contours.push_back(polygon);
	}

	QJsonArray array = obj["rule"].toArray();
	for (const auto& item : array)
	{
		one_rule.rule_levels.push_back(Product_Level_Defects_from_json(str_surface_name,item.toObject()));
	}

	if(obj.contains("rule_name"))
		one_rule.str_name = obj["rule_name"].toString();

	if(obj.contains("rule_roi_color"))
		one_rule.str_roi_color = obj["rule_roi_color"].toString();

	return one_rule;
}

QJsonObject ProductSurface_Rule_2_json(const ProductSurface_Rule& target)
{
	QJsonObject obj;
	obj.insert("product_surface_name", target.str_product_surface_name);
	QJsonArray array;
	for (const auto& item : target.surface_rules)
	{
		array.append(ProductSurface_One_Rule_2_json(item));
	}
	obj.insert("surface_rule", array);
	return obj;
}

ProductSurface_Rule ProductSurface_Rule_from_json(const QJsonObject& obj)
{
	ProductSurface_Rule rule;
	rule.str_product_surface_name = obj["product_surface_name"].toString();
	auto array = obj["surface_rule"].toArray();
	for (const auto& item : array)
	{
		rule.surface_rules.push_back(ProductSurface_One_Rule_from_json(rule.str_product_surface_name,item.toObject()));
	}

	return rule;
}

QJsonObject Product_Rule_2_json(const Product_Rule& target)
{
	QJsonObject obj;
	QJsonArray array;
	for (const auto& rule : target)
	{
		array.append(ProductSurface_Rule_2_json(rule));
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
		auto temp = ProductSurface_Rule_from_json(obj);
		rule_vector.push_back(temp);
	}
	return rule_vector;
}


QJsonObject Defect_Feature_2_json(const Defect_Feature& data)
{
	QJsonObject obj;
	obj.insert("name", QString::fromStdWString(data.str_chn_name));
	obj.insert("unit", QString::fromStdWString(data.str_chn_unit_name));
	obj.insert("multi_time", data.multiple_times);
	return obj;
}

Defect_Feature Defect_Feature_from_json(const QJsonObject& obj)
{
	Defect_Feature temp;
	temp.multiple_times = obj.value("multi_time").toInt();
	temp.str_chn_name = obj.value("name").toString().toStdWString();
	temp.str_chn_unit_name = obj.value("unit").toString().toStdWString();
	return temp;
}

QJsonObject product_surface_rule_attr_2_json(const Product_Surface_Defect_Info& data)
{
	QJsonObject obj;
	obj.insert("surface_name", data.str_surface_name);
	QJsonArray attri_array;
	for (const auto& item : data.attribute_vector)
	{
		attri_array.append(Defect_Feature_2_json(item));
	}
	obj.insert("attri_array", attri_array);

	QJsonArray defect_array;
	for (const auto& item : data.defect_filter_features)
	{
		QJsonObject obj_defect;
		obj_defect.insert("name", QString::fromStdWString(item.first));
		QJsonArray defect_feature;
		for (const auto& feature : item.second)
		{
			defect_feature.append(QString::fromStdWString(feature));
		}
		obj_defect.insert("features", defect_feature);
		defect_array.append(obj_defect);
	}
	obj.insert("defect_array", defect_array);

	return obj;
}

Product_Surface_Defect_Info product_surface_rule_attr_from_json(const QJsonObject& obj)
{
	Product_Surface_Defect_Info data;
	data.str_surface_name = obj.value("surface_name").toString();
	QJsonArray attri_array = obj.value("attri_array").toArray();
	for (const auto& _ : attri_array)
	{
		data.attribute_vector.push_back(Defect_Feature_from_json(_.toObject()));
	}

	QJsonArray defect_array = obj.value("defect_array").toArray();
	for (const auto& _ : defect_array)
	{
		auto sub_obj = _.toObject();
		auto name = sub_obj.value("name").toString();
		auto defect_feature = sub_obj.value("features").toArray();
		std::vector<std::wstring> feature_vect;
		for (const auto& item : defect_feature)
		{
			feature_vect.push_back(item.toString().toStdWString());
		}
		data.defect_filter_features[name.toStdWString()] = feature_vect;
	}

	return data;
}


Product_Surface_Defect_Info get_product_surface_defect_features(const std::wstring& str_product_surface_name)
{
	auto data = Product_Surface_Defect_Feature_Manager::get_all_product_surface_info();
	auto iter = data.find(str_product_surface_name);
	if (iter != data.end())
		return iter->second;
	return {};
}

std::wstring attributes_literal_unit(const std::wstring& product_surface, const std::wstring& attribute_name)
{
	auto data = Product_Surface_Defect_Feature_Manager::get_all_product_surface_info();
	auto iter = data.find(product_surface);
	if (iter != data.end())
	{
		for (const auto& item : iter->second.attribute_vector)
		{
			if (item.str_chn_name == attribute_name)
				return item.str_chn_unit_name;
		}
	}
	return L"";
}


std::optional<std::vector<Defect_Feature>> get_surface_attribute(const QString& str_surface_name)
{
	auto data = Product_Surface_Defect_Feature_Manager::get_all_product_surface_info();
	auto iter = data.find(str_surface_name.toStdWString());
	if (iter == data.end())
		return {};
	return iter->second.attribute_vector;
}

std::optional<std::vector<std::wstring>> get_surface_defect_attribute(const QString& str_surface_name, const QString& str_defect_name)
{
	auto data = Product_Surface_Defect_Feature_Manager::get_all_product_surface_info();
	auto iter = data.find(str_surface_name.toStdWString());
		return {};
	auto iter2 = iter->second.defect_filter_features.find(str_defect_name.toStdWString());
	if (iter2 == iter->second.defect_filter_features.end())
		return {};
	return iter2->second;
}

std::function<bool(const std::map<check_attri_name, check_attri_value>& attri_dict, double)>
generate_defect_rule_node(const QString& str_product_surface_name, const Rule_Calc_Node& save_data)
{
	bool exist = true;
	auto opt_data = get_surface_attribute(str_product_surface_name);
	if (!opt_data)
		exist = false;
	auto attribute_name = save_data.str_attri_name.toStdWString();
	relation re = relation_from_string(save_data.str_relation);
	double calc_value = save_data.compare_value;
	int time = 0;
	if (opt_data)
	{
		for (const auto& item : *opt_data)
		{
			if (item.str_chn_name == save_data.str_attri_name)
			{
				time = item.multiple_times;
				break;
			}
		}
	}

	auto compare = [exist,attribute_name, calc_value, re, time](const std::map<check_attri_name, check_attri_value>& attri_dict, double scale)
	{
		if (!exist)
			return false;

		double compare_value = calc_value;
		for (int i = 0; i < time; i++)
		{
			compare_value *= scale;
		}

		double num = 0;
		auto iter = attri_dict.find(attribute_name);
		if (iter == attri_dict.end())
			return false;
		num = iter->second;

		if (re == relation::less_than)
			return num < compare_value;
		else
			return num > compare_value;
	};

	return compare;
}

QJsonObject Detect_Model_2_json(const Detect_Model& data)
{
	QJsonObject obj;
	obj.insert("model_path", QString::fromStdWString(data.str_model_path));
	QJsonArray img_array;
	for (const auto& num : data.use_img)
		img_array.append(num);

	obj.insert("use_img_array", img_array);
	return obj;
}

Detect_Model Detect_Model_from_json(const QJsonObject& obj)
{
	Detect_Model data;
	data.str_model_path = obj.value("model_path").toString().toStdWString();
	auto img_array = obj.value("use_img_array").toArray();
	for (const auto& item : img_array)
		data.use_img.push_back(item.toInt());
	return data;
}

QJsonObject surface_train_backed_2_json(const surface_train_backed& data)
{
	QJsonObject obj;
	obj.insert("program_id", data.train_backed_program_id);
	QJsonArray img_array;
	for (const auto& num : data.train_backed_use_img_vect)
		img_array.append(num);
	obj.insert("img_array", img_array);

	return obj;
}

surface_train_backed surface_train_backed_from_json(const QJsonObject& obj)
{
	surface_train_backed data;
	data.train_backed_program_id = obj.value("program_id").toInt();
	auto img_array = obj.value("img_array").toArray();
	for (const auto& item : img_array)
		data.train_backed_use_img_vect.push_back(item.toInt());
	return data;
}

/*
* struct product_surface_config
{
	std::wstring str_surface_name;
	std::wstring str_img_path;
	std::vector<Detect_Model> detect_model_vect;
	std::vector<surface_train_backed> backend_config;
};
*/

QJsonObject product_surface_config_2_json(const product_surface_config& data)
{
	QJsonObject obj;
	obj.insert("surface_name", QString::fromStdWString(data.str_surface_name));
	obj.insert("surface_img_path", QString::fromStdWString(data.str_img_path));
	obj.insert("detect_model", Detect_Model_2_json(data.detect_model));
	obj.insert("train_config", surface_train_backed_2_json(data.backend_config));	
	return obj;
}

product_surface_config product_surface_config_from_json(const QJsonObject& obj)
{
	product_surface_config config;
	config.str_surface_name = obj.value("surface_name").toString().toStdWString();
	config.str_img_path = obj.value("surface_img_path").toString().toStdWString();
	config.detect_model = Detect_Model_from_json(obj.value("detect_model").toObject());
	config.backend_config = surface_train_backed_from_json(obj.value("train_config").toObject());
	return config;
}
