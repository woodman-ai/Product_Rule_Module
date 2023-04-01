#pragma once
#include "qjsonobject.h"
#include "rule_class.h"
void save_json_files(const QString& strPath, const QJsonObject& obj);
QJsonObject read_json_document(const QString& str_path);

QString rect_2_string(const QRect& rt);
QRect string_2_rect(const QString& str);

QString relation_2_string(relation re);
relation relation_from_string(const QString& string);

QString Defect_Tree_Node_Type_2_string(Defect_Node_Type);
Defect_Node_Type Defect_Tree_Node_Type_from_string(const QString& string);

QJsonObject Rule_Calc_Node_2_json(const Rule_Calc_Node& target);
Rule_Calc_Node Rule_Calc_Node_from_json(const QString& str_surface_name, const QJsonObject& obj);
QString Rule_Calc_Node_2_String(const std::wstring& str_surface_name,const Rule_Calc_Node& target);

QJsonObject Defect_Node_2_json(const Defect_Node& target);
Defect_Node Defect_Node_from_json(const QString& str_product_surface_name, const QJsonObject& obj);

QJsonObject Defect_Rule_2_json(const Defect_Rule& target);
Defect_Rule Defect_Rule_from_json(const QString& str_surface_name, const QJsonObject& obj);

QJsonObject Product_Level_2_json(const Product_Level& target);
Product_Level Product_Level_from_json(const QJsonObject& obj);

QJsonObject Product_Level_Defects_2_json(const Product_Level_Defects& target);
Product_Level_Defects Product_Level_Defects_from_json(const QString& str_surface_name, const QJsonObject& obj);

QJsonObject ProductSurface_One_Rule_2_json(const ProductSurface_One_Rule& target);
ProductSurface_One_Rule ProductSurface_One_Rule_from_json(const QString& str_surface_name,const QJsonObject& obj);

QJsonObject ProductSurface_Rule_2_json(const ProductSurface_Rule& target);
ProductSurface_Rule ProductSurface_Rule_from_json(const QJsonObject& obj);

QJsonObject Product_Rule_2_json(const Product_Rule& target);
Product_Rule Product_Rule_from_json(const QJsonObject& obj);

QJsonObject Defect_Feature_2_json(const Defect_Feature& data);
Defect_Feature Defect_Feature_from_json(const QJsonObject& obj);

QJsonObject product_surface_rule_attr_2_json(const Product_Surface_Defect_Info& data);
Product_Surface_Defect_Info product_surface_rule_attr_from_json(const QJsonObject& obj);

std::optional<std::vector<Defect_Feature>> get_surface_attribute(const QString& str_surface_name);
std::optional<std::vector<std::wstring>> get_surface_defect_attribute(const QString& str_surface_name, const QString& str_defect_name);
Product_Surface_Defect_Info get_product_surface_defect_features(const std::wstring& str_product_surface_name);
std::wstring attributes_literal_unit(const std::wstring& product_surface, const std::wstring& attribute_name);
std::function<bool(const std::map<check_attri_name, check_attri_value>& attri_dict, double)>
generate_defect_rule_node(const QString& str_product_surface_name, const Rule_Calc_Node& save_data);

QJsonObject Detect_Model_2_json(const Detect_Model& data);
Detect_Model Detect_Model_from_json(const QJsonObject& obj);

QJsonObject surface_train_backed_2_json(const surface_train_backed& data);
surface_train_backed surface_train_backed_from_json(const QJsonObject& obj);

QJsonObject product_surface_config_2_json(const product_surface_config& data);
product_surface_config product_surface_config_from_json(const QJsonObject& obj);

