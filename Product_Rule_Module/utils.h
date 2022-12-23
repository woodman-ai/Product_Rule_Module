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
Rule_Calc_Node Rule_Calc_Node_from_json(const QJsonObject& obj);
QString Rule_Calc_Node_2_String(const Rule_Calc_Node& target);

QJsonObject Defect_Node_2_json(const Defect_Node& target);
Defect_Node Defect_Node_from_json(const QJsonObject& obj);

QJsonObject Defect_Rule_2_json(const Defect_Rule& target);
Defect_Rule Defect_Rule_from_json(const QJsonObject& obj);

QJsonObject Merge_Rule_2_json(const Merge_Rule& target);
Merge_Rule  Merge_Rule_from_json(const QJsonObject& obj);

QJsonObject Product_Level_2_json(const Product_Level& target);
Product_Level Product_Level_from_json(const QJsonObject& obj);

QJsonObject Product_Level_Defects_2_json(const Product_Level_Defects& target);
Product_Level_Defects Product_Level_Defects_from_json(const QJsonObject& obj);

QJsonObject CheckSurface_One_Rule_2_json(const CheckSurface_One_Rule& target);
CheckSurface_One_Rule CheckSurface_One_Rule_from_json(const QJsonObject& obj);

QJsonObject Surface_Rule_2_json(const Surface_Rule& target);
Surface_Rule Surface_Rule_from_json(const QJsonObject& obj);

QJsonObject Product_Rule_2_json(const Product_Rule& target);
Product_Rule Product_Rule_from_json(const QJsonObject& obj);