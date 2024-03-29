#pragma once

#include <qwidget>
#include "ui_ProductLevelSettingWidget.h"
#include <optional>
#include "qjsonobject.h"
#include "rule_typedef.h"
#include "utils.h"
#include "rule_class.h"
class productTreeNode : public QTreeWidgetItem
{
    enum class tree_node_type
    {
        product_level,
        defect_name,
        defect_calc_expr,
        defect_logic_and,
        defect_logic_or
    };

    tree_node_type node_type;
    QString str_attri;
    QString str_relation;
    double calc_num;
public:
    productTreeNode(QTreeWidgetItem* parent) : QTreeWidgetItem(parent)
    {

    }

    std::tuple<QString, QString, double > get_exp_set() const
    {
        return { str_attri,str_relation,calc_num };
    }

    void set_product_level() { node_type = tree_node_type::product_level; }
    void set_defect_name() { node_type = tree_node_type::defect_name; }
    void set_and() { node_type = tree_node_type::defect_logic_and; }
    void set_or() { node_type = tree_node_type::defect_logic_or; }
    void set_expression() { node_type = tree_node_type::defect_calc_expr; }
   
    bool is_product_level() const { return node_type == tree_node_type::product_level; }
    bool is_defect_name()const { return node_type == tree_node_type::defect_name; }
    bool is_and() const{ return node_type == tree_node_type::defect_logic_and; }
    bool is_or() const{ return node_type == tree_node_type::defect_logic_or; }
    bool is_expression() const { return node_type == tree_node_type::defect_calc_expr; }       
   
    static void Rule_Calc_Node_2_productTreeNode(const std::wstring& surface_name, const Rule_Calc_Node& rule_node, productTreeNode* node)
    {
        node->str_attri = rule_node.str_attri_name;
        node->str_relation = rule_node.str_relation;
        node->calc_num = rule_node.compare_value;
        node->node_type = tree_node_type::defect_calc_expr;
        node->setText(0, Rule_Calc_Node_2_String(surface_name,rule_node));
    }
    static Rule_Calc_Node  productTreeNode_2_Rule_Calc_Node(const productTreeNode* product_node)
    {
        Rule_Calc_Node node;
        node.str_attri_name = product_node->str_attri;
        node.str_relation = product_node->str_relation;
        node.compare_value = product_node->calc_num;
        return node;
    }
};

class ProductLevelSettingWidget : public QWidget
{
    Q_OBJECT

    void add_defect(QString strdefect);    
    bool CanInsertLogicNode(productTreeNode*);
    bool CanInsertExpression(productTreeNode*);
    bool IsValidNode(const productTreeNode* node);
    std::vector<QString>  defect_list;
    bool save_rule(int index);     
    ProductSurface_Rule m_cur_surface_rule;
    std::vector<Product_Level> m_product_level;

    int m_current_roi_index = -1;
    QString get_node_defect_name(productTreeNode* node);
private:
    void init_tree_by_one_surface(const ProductSurface_One_Rule& rule);
    void add_recursive_rule_node_2_tree(const Defect_Node& recursive_rule_node,productTreeNode* parent);
    void add_expression_to_node(productTreeNode* node, int index, const Rule_Calc_Node& calc_node);
private:
    ProductSurface_One_Rule get_surface_one_rule_from_widget();
    Product_Level_Defects get_product_rule_by_tree_node(productTreeNode* node);
    Defect_Rule get_defect_by_tree_node(productTreeNode* node);
    Defect_Node get_recursive_node_by_tree_node(productTreeNode* tree_node);
public:
	ProductLevelSettingWidget(QWidget *parent = Q_NULLPTR);
	~ProductLevelSettingWidget();
    void set_surface_rules(const ProductSurface_Rule& rules);
    std::tuple<bool, ProductSurface_Rule>  get_surface_rules();
    bool TestCondition();
    void clear() { slot_clear(); }
    void set_surface_defects(const std::vector<QString>& list) { defect_list = list; }
    void set_product_surface_img(const QImage& img);
    void add_roi();
    void delete_roi();
    void modify_roi_name();
    void init();
    void init_product_level(const std::vector<Product_Level>& product_levels);
 private slots:
    void slot_trans_logic();
    void slot_add_and();
    void slot_add_or();
    void slot_add_logic();
    void slot_add_expression();
    void slot_delete();
    void slot_add_defect();
    void slot_clear();
    void slot_double_click_item(QTreeWidgetItem* item, int column);   
    void slot_combo_roi_change(int);
    void slot_roi_btn_clicked();
    void slot_roi_widget_stop_draw();
    void slot_btn_operator_clicked();
    void slot_btn_change_roi_color();
private:
	Ui::ProductLevelSettingWidget ui;
};
