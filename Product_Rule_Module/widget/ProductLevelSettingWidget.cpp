#include "ProductLevelSettingWidget.h"
#include <qmessagebox.h>
#include <qmenu.h>
#include <AddRuleConditionWidget.h>
#include <QStyleFactory>
#include "SetMaxDefectCountDialog.h"
#include <qjsonobject.h>
#include "qinputdialog.h"
#include "Rule_Attri_Manager.h"
#include "qcolordialog.h"
ProductLevelSettingWidget::ProductLevelSettingWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.button_logic, &QPushButton::clicked, this, &ProductLevelSettingWidget::slot_add_logic);
	connect(ui.button_add_defect, &QPushButton::clicked, this, &ProductLevelSettingWidget::slot_add_defect);
	connect(ui.button_expression, &QPushButton::clicked, this, &ProductLevelSettingWidget::slot_add_expression);
	connect(ui.button_delete, &QPushButton::clicked, this, &ProductLevelSettingWidget::slot_delete);
	connect(ui.button_clear, &QPushButton::clicked, this, &ProductLevelSettingWidget::slot_clear);
	connect(ui.tree_rule, &QTreeWidget::itemDoubleClicked, this, &ProductLevelSettingWidget::slot_double_click_item);
	connect(ui.combo_roi, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ProductLevelSettingWidget::slot_combo_roi_change);
	connect(ui.btn_roi, &QPushButton::clicked, this, &ProductLevelSettingWidget::slot_roi_btn_clicked);
	connect(ui.btn_operator, &QPushButton::clicked, this, &ProductLevelSettingWidget::slot_btn_operator_clicked);
	connect(ui.btn_roi_color, &QPushButton::clicked, this, &ProductLevelSettingWidget::slot_btn_change_roi_color);
	
	ui.tree_rule->setStyle(QStyleFactory::create("windows"));
	ui.tree_rule->setColumnCount(3);
	ui.tree_rule->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ProductLevelSettingWidget::init_tree_by_one_surface(const CheckSurface_One_Rule& rule)
{
	auto get_product_level = [](const QString& str_product_level,const std::vector<Product_Level_Defects>& rule)->std::optional<std::reference_wrapper<const Product_Level_Defects>>
	{
		for (const auto& item : rule)
		{
			if (item.product_level.strLevelName == str_product_level)
				return item;
		}
		return {};
	};

	slot_clear();
	int count = ui.tree_rule->topLevelItemCount();
	for (int i = 0; i < count; i++)
	{
		auto item = ui.tree_rule->topLevelItem(i);
		auto node = (productTreeNode*)item;

		auto product_level = get_product_level(item->text(0),rule.rule_levels);
		if (product_level == std::nullopt)
			continue;
		for (const auto& defect : product_level->get().level_defects)
		{
			auto defect_node = new productTreeNode(node);
			defect_node->set_defect_name();
			defect_node->setText(0, defect.str_defect_name);
			defect_node->setText(1, QStringLiteral("�����������:") + QString::number(defect.max_count));
			defect_node->setWhatsThis(1, QString::number(defect.max_count));
			if (defect.max_area)
			{
				defect_node->setText(2, QStringLiteral("����������: ") + QString::number(*defect.max_area) + QStringLiteral(" ƽ������"));
				defect_node->setWhatsThis(2, QString::number(*defect.max_area));
			}
			else
			{
				defect_node->setText(2, QStringLiteral("δ��������������"));
				defect_node->setWhatsThis(2, "-1");
			}
			add_recursive_rule_node_2_tree(defect.calc_expression, defect_node);
		}	
	}
}

void ProductLevelSettingWidget::add_recursive_rule_node_2_tree(const Defect_Node& recursive_rule_node, productTreeNode* parent)
{
	auto tree_node = new productTreeNode(parent);
	if(recursive_rule_node.node_type == Defect_Node_Type::And)
	{
		tree_node->set_and();
		tree_node->setText(0, QStringLiteral("����"));
	}
	else if(recursive_rule_node.node_type == Defect_Node_Type::Or)
	{
		tree_node->set_or();
		tree_node->setText(0, QStringLiteral("����"));
	}
	else
	{
		productTreeNode::Rule_Calc_Node_2_productTreeNode(recursive_rule_node.expression, tree_node);
	}

	for (const auto& item : recursive_rule_node.subNodes)
	{
		add_recursive_rule_node_2_tree(item, tree_node);
	}
}

void ProductLevelSettingWidget::add_expression_to_node(productTreeNode* parent, int index, const Rule_Calc_Node& calc_node)
{
	auto tree_node = new productTreeNode(nullptr);
	productTreeNode::Rule_Calc_Node_2_productTreeNode(calc_node, tree_node);
	parent->insertChild(index, tree_node);
}

Product_Level_Defects ProductLevelSettingWidget::get_product_rule_by_tree_node(productTreeNode* node)
{
	int index = ui.tree_rule->indexOfTopLevelItem(node);
	Product_Level_Defects level_defects;
	level_defects.product_level = m_product_level[index];
	int child_count = node->childCount();
	for (int i = 0; i < child_count; i++)
	{
		auto child = (productTreeNode*)node->child(i);
		auto defect = get_defect_by_tree_node(child);
		level_defects.level_defects.push_back(defect);
	}
	return level_defects;
}

Defect_Rule ProductLevelSettingWidget::get_defect_by_tree_node(productTreeNode* node)
{
	Defect_Rule defect;
	defect.str_defect_name = node->text(0);
	defect.max_count = node->whatsThis(1).toInt();
	double max_area = node->whatsThis(2).toDouble();
	if (max_area > 0)
		defect.max_area = max_area;

	if (node->childCount() > 0)
	{
		auto child = (productTreeNode*)node->child(0);
		defect.calc_expression = get_recursive_node_by_tree_node(child);
	}
	return defect;
}

Defect_Node ProductLevelSettingWidget::get_recursive_node_by_tree_node(productTreeNode* tree_node)
{
	Defect_Node expr;
	if (tree_node->is_and())
	{
		expr.node_type = Defect_Node_Type::And;
	}
	else if (tree_node->is_or())
	{
		expr.node_type = Defect_Node_Type::Or;
	}
	else
	{
		expr.node_type = Defect_Node_Type::Expression;
		auto [str_attri, str_relation, compare_value, gray_index,a_index] = tree_node->get_exp_set();
		expr.expression.str_attri_name = str_attri;
		expr.expression.str_relation = str_relation;
		expr.expression.compare_value = compare_value;
		expr.expression.g_index = gray_index;
		expr.expression.a_index = a_index;
		expr.expression.func = Rule_Attri_Manager::generate_defect_rule_node(expr.expression);
	}

	int child_count = tree_node->childCount();
	for (int i = 0; i < child_count; i++)
	{
		auto sub_node = (productTreeNode*)tree_node->child(i);
		auto sub = get_recursive_node_by_tree_node(sub_node);
		expr.subNodes.push_back(sub);
	}
	return expr;
}

ProductLevelSettingWidget::~ProductLevelSettingWidget()
{
}

void ProductLevelSettingWidget::set_surface_rules(const Surface_Rule& rules)
{
	m_cur_surface_rule = rules;
	if (m_cur_surface_rule.surface_rules.size() == 0)
	{
		CheckSurface_One_Rule one_rule;
		one_rule.str_name = QStringLiteral("Ĭ��");
		one_rule.valid_product_surface = ui.roi_widget->get_checksurface_contain_product_surface();
		m_cur_surface_rule.surface_rules.push_back(one_rule);
	}
}

void ProductLevelSettingWidget::slot_trans_logic()
{
	auto current = ui.tree_rule->currentItem();
	if (current == nullptr)
		return;

	productTreeNode* now = (productTreeNode*)current;
	if (now->is_and())
	{
		now->set_or();
		now->setText(0, QStringLiteral("����"));
	}
	else if (now->is_or())
	{
		now->set_and();
		now->setText(0, QStringLiteral("����"));
	}
}

void ProductLevelSettingWidget::slot_add_and()
{
	auto current = ui.tree_rule->currentItem();
	if (current == nullptr)
		return;

	productTreeNode* parent = (productTreeNode*)current;
	if (!CanInsertLogicNode(parent))
		return;

	auto item = new productTreeNode(current);
	item->setText(0, QStringLiteral("����"));
	item->set_and();
	ui.tree_rule->expandItem(item);
}

void ProductLevelSettingWidget::slot_add_or()
{
	auto current = ui.tree_rule->currentItem();
	if (current == nullptr)
		return;

	productTreeNode* parent = (productTreeNode*)current;
	if (!CanInsertLogicNode(parent))
		return;

	auto item = new productTreeNode(current);
	item->setText(0, QStringLiteral("����"));
	item->set_or();
	ui.tree_rule->expandItem(item);
}

void ProductLevelSettingWidget::slot_add_logic()
{
	QMenu menu; //��Ӳ˵�
	QAction* action_and = new QAction(this); //��Ӷ���
	QAction* action_or = new QAction(this);
	QAction* action_trans = new QAction(this);
	//����������Ҫִ�еĲۺ�������
	QObject::connect(action_and, &QAction::triggered, this, &ProductLevelSettingWidget::slot_add_and);
	QObject::connect(action_or, &QAction::triggered, this, &ProductLevelSettingWidget::slot_add_or);
	QObject::connect(action_trans, &QAction::triggered, this, &ProductLevelSettingWidget::slot_trans_logic);
	action_and->setText(QStringLiteral("�߼���"));
	action_or->setText(QStringLiteral("�߼���"));
	action_trans->setText(QStringLiteral("�л�"));
	menu.addAction(action_and);
	menu.addSeparator();
	menu.addAction(action_or);
	menu.addSeparator();
	menu.addAction(action_trans);
	auto point = ui.button_logic->mapToGlobal(ui.button_logic->rect().bottomLeft());
	menu.exec(point);
}

void ProductLevelSettingWidget::slot_add_expression()
{
	auto item = ui.tree_rule->currentItem();
	if (item == nullptr)
		return;

	productTreeNode* node = (productTreeNode*)item;
	if (!CanInsertExpression(node))
		return;

	productTreeNode* parent = (productTreeNode*)node->parent();
	if (node->is_expression() && !parent->is_and() && !parent->is_or())
		return;

	AddRuleConditionWidget dlg(m_gray_photo_name);
	if (dlg.exec() == QDialog::Accepted)
	{
		auto expr = dlg.GetExpression();
		if (node->is_expression())
		{
			int index = parent->indexOfChild(node);		
			add_expression_to_node(parent, index, expr);
		}
		else
		{
			add_expression_to_node(node, node->childCount(), expr);
		}
	}
}

void ProductLevelSettingWidget::slot_delete()
{
	auto item = ui.tree_rule->currentItem();
	if (item == nullptr)
		return;

	productTreeNode* node = (productTreeNode*)item;
	if (node->is_product_level())
		return;

	delete item;
}

void ProductLevelSettingWidget::slot_add_defect()
{
	QMenu menu; //��Ӳ˵�
	for (const auto& item : defect_list)
	{
		QAction* action = new QAction(&menu); //��Ӷ���
		action->setText(item);
		QObject::connect(action, &QAction::triggered, this, [item, this]() {this->add_defect(item); });
		menu.addAction(action);
		menu.addSeparator();
	}

	auto point = ui.button_add_defect->mapToGlobal(ui.button_add_defect->rect().bottomLeft());
	menu.exec(point);
}

void ProductLevelSettingWidget::slot_btn_operator_clicked()
{
	QMenu menu; //��Ӳ˵�
	QAction* action1 = menu.addAction(QStringLiteral("���������"));
	connect(action1, &QAction::triggered, this, [this]() {this->add_roi(); });
	menu.addAction(action1);

	if (ui.combo_roi->count() > 1)
	{
		QAction* action1 = menu.addAction(QStringLiteral("ɾ������"));
		connect(action1, &QAction::triggered, this, [this]() {this->delete_roi(); });
		menu.addAction(action1);	

		QAction* action2 = menu.addAction(QStringLiteral("�޸���������"));
		connect(action2, &QAction::triggered, this, [this]() {this->modify_roi_name(); });
		menu.addAction(action2);
	}

	auto point = ui.btn_operator->mapToGlobal(ui.button_add_defect->rect().bottomLeft());
	menu.exec(point);
}

void ProductLevelSettingWidget::slot_btn_change_roi_color()
{
	auto color = ui.btn_roi_color->whatsThis();
	QColorDialog color_dialog;
	auto new_color = color_dialog.getColor(color, this, QStringLiteral("����roi��ɫ"));
	if (new_color.isValid())
	{
		auto str_color = new_color.name();
		QString str_style = "QPushButton{background-color:" + str_color + "}";
		ui.btn_roi_color->setStyleSheet(str_style);
		ui.btn_roi_color->setWhatsThis(str_color);
		ui.roi_widget->set_roi_color(new_color);
	}
}

void ProductLevelSettingWidget::modify_roi_name()
{
	auto index = ui.combo_roi->currentIndex();
	if (index == -1)
	{
		return;
	}

	auto surface_name = QInputDialog::getText(this, QStringLiteral("��������������"), QStringLiteral("��������������"));
	if (surface_name.isEmpty())
		return;

	auto& rules = m_cur_surface_rule.surface_rules[index];
	rules.str_name = surface_name;
	ui.combo_roi->setItemText(index, surface_name);
}

void ProductLevelSettingWidget::add_roi()
{
	auto index = ui.combo_roi->currentIndex();
	if (!save_rule(index))
		return;


	auto surface_name = QInputDialog::getText(this, QStringLiteral("��������������"), QStringLiteral("��������������"));
	if (surface_name.isEmpty())
		return;

	CheckSurface_One_Rule one_rule = m_cur_surface_rule.surface_rules[index];
	one_rule.str_name = surface_name;
	one_rule.rule_contours.clear();
	m_cur_surface_rule.surface_rules.push_back(one_rule);
	ui.combo_roi->addItem(surface_name);
	ui.combo_roi->setCurrentIndex(ui.combo_roi->count()-1);
}

void ProductLevelSettingWidget::delete_roi()
{
	auto index = ui.combo_roi->currentIndex();
	if (index == -1)
	{
		return;
	}

	auto& vect = m_cur_surface_rule.surface_rules;
	vect.erase(vect.begin() + index);
	init();
}

void ProductLevelSettingWidget::init()
{
	slot_clear();
	m_current_roi_index = -1;
	ui.combo_roi->clear();
	for (const auto& item : m_cur_surface_rule.surface_rules)
	{
		ui.combo_roi->addItem(item.str_name);
	}

	ui.combo_roi->setCurrentIndex(-1);
	if (ui.combo_roi->count() != 0)
	{
		ui.combo_roi->setCurrentIndex(0);
	}
}

void ProductLevelSettingWidget::init_product_level(const std::vector<Product_Level>& product_levels)
{
	m_product_level = product_levels;
	for (const auto& item : m_product_level)
	{
		productTreeNode* node = new productTreeNode(nullptr);
		node->setText(0, item.strLevelName);
		node->set_product_level();
		ui.tree_rule->insertTopLevelItem(ui.tree_rule->topLevelItemCount(), node);
	}
}

bool ProductLevelSettingWidget::TestCondition()
{
	int count = ui.tree_rule->topLevelItemCount();
	for (int i = 0; i < count; i++)
	{
		auto item = (productTreeNode*) ui.tree_rule->topLevelItem(i);
		if (!IsValidNode(item))
			return false;		
	}

	return true;
}

CheckSurface_One_Rule ProductLevelSettingWidget::get_surface_one_rule_from_widget()
{
	CheckSurface_One_Rule one_rule;
	auto [contours,valid_surface] = ui.roi_widget->get_polygon();
	one_rule.rule_contours = contours;
	one_rule.valid_product_surface = valid_surface;
	one_rule.str_roi_color = ui.btn_roi_color->whatsThis();
	int count = ui.tree_rule->topLevelItemCount();
	for (int i = 0; i < count; i++)
	{
		auto node = (productTreeNode*)ui.tree_rule->topLevelItem(i);
		one_rule.rule_levels.push_back(get_product_rule_by_tree_node(node));
	}
	return one_rule;
}

bool ProductLevelSettingWidget::save_rule(int index)
{
	if (!TestCondition())
	{
		ui.combo_roi->setCurrentIndex(index);
		return false;
	}

	auto roi_rule = get_surface_one_rule_from_widget();
	roi_rule.str_name = m_cur_surface_rule.surface_rules[index].str_name;
	m_cur_surface_rule.surface_rules[index] = roi_rule;
	return true;
}

std::tuple<bool, Surface_Rule> ProductLevelSettingWidget::get_surface_rules()
{
	if (!save_rule(ui.combo_roi->currentIndex()))
		return { false,{} };

	return { true,m_cur_surface_rule};
}

void ProductLevelSettingWidget::slot_clear()
{
	int count = ui.tree_rule->topLevelItemCount();
	for (int i = 0; i < count; i++)
	{
		auto item = ui.tree_rule->topLevelItem(i);
		while (item->childCount())
		{
			delete item->child(0);
		}
	}
}

void ProductLevelSettingWidget::slot_double_click_item(QTreeWidgetItem* item, int column)
{
	productTreeNode* node = (productTreeNode*)item;
	if (node->is_defect_name())
	{
		if (column == 1)
		{
			SetMaxDefectCountDialog dlg;
			if (dlg.exec() == QDialog::Accepted)
			{
				auto max_count = dlg.GetMaxCount();
				node->setText(1, QStringLiteral("�����������:") + QString::number(max_count));
				node->setWhatsThis(1, QString::number(max_count));
			}
		}
		else if (column == 2)
		{
			auto value = QInputDialog::getDouble(this, QStringLiteral("���������ϼ����(ƽ������)"), QStringLiteral("���������ϼ����(ƽ������)"), 3,-100.0,9999999.0,3);
			if (value < 0)
			{
				node->setText(2, QStringLiteral("δ��������������"));
				node->setWhatsThis(2, "-1");
			}
			else
			{
				node->setText(2, QStringLiteral("����������: ") + QString::number(value) + QStringLiteral(" ƽ������"));
				node->setWhatsThis(2, QString::number(value));
			}
		}
	}
	else if (node->is_expression() && column == 0)
	{
		AddRuleConditionWidget dlg(m_gray_photo_name);
		auto calc_node = productTreeNode::productTreeNode_2_Rule_Calc_Node(node);
		dlg.set_current_conditioin(calc_node);
		if (dlg.exec() == QDialog::Accepted)
		{
			auto expr = dlg.GetExpression();
			productTreeNode::Rule_Calc_Node_2_productTreeNode(expr, node);			
			node->setText(0, Rule_Calc_Node_2_String(expr));
		}
	}
}

void ProductLevelSettingWidget::slot_combo_roi_change(int index)
{
	if (index == -1)
		return;

	if (ui.combo_roi->count() == 0)
		return;

	if (m_current_roi_index == index)
		return;

	if (m_current_roi_index != -1)
	{
		if (!save_rule(m_current_roi_index))
			return;
	}

	m_current_roi_index = index;
	auto str_color = m_cur_surface_rule.surface_rules[index].str_roi_color;
	QString str_style = "QPushButton{background-color:" + str_color + "}";
	ui.btn_roi_color->setStyleSheet(str_style);
	ui.btn_roi_color->setWhatsThis(str_color);
	ui.roi_widget->set_roi_color(m_cur_surface_rule.surface_rules[index].str_roi_color);
	std::map<QString,std::vector<all_roi_color_polygon>> other_rule_polygon;
	for (int i = 0; i < m_cur_surface_rule.surface_rules.size(); i++)
	{
		if (i != index)
		{
			const auto& rule = m_cur_surface_rule.surface_rules[i];
			auto color = rule.str_roi_color;
			for (const auto& roi : rule.rule_contours)
			{
				all_roi_color_polygon temp;
				temp.str_color = color;
				temp.all_polygon = roi.second;
				other_rule_polygon[roi.first].push_back(temp);

			}
		}
	}
	ui.roi_widget->set_polygon(m_cur_surface_rule.surface_rules[index].rule_contours, m_cur_surface_rule.surface_rules[index].valid_product_surface, other_rule_polygon);
	init_tree_by_one_surface(m_cur_surface_rule.surface_rules[index]);
}

void ProductLevelSettingWidget::slot_roi_btn_clicked()
{
	bool bchecked = ui.btn_roi->isChecked();
	if (bchecked)
	{	
		ui.roi_widget->start_draw_polygon();
	}
	else
	{
		ui.roi_widget->stop_draw_polygon();
	}
}

void ProductLevelSettingWidget::slot_roi_widget_stop_draw()
{
	ui.btn_roi->setChecked(false);
}

bool ProductLevelSettingWidget::IsValidNode(const productTreeNode* node)
{
	int childCount = node->childCount();
	if (node->is_and() || node->is_or())
	{
		if (childCount < 2)
		{
			QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("�߼��ж�������������Ϊ2��"));
			ui.tree_rule->setCurrentItem(const_cast<productTreeNode*>(node));
			ui.tree_rule->expandItem(node);
			return false;
		}
	}

	for (int i = 0; i < childCount; i++)
	{
		auto item = node->child(i);
		const productTreeNode* pChild = (const productTreeNode*)item;
		if (!IsValidNode(pChild))
			return false;		
	}

	return true;
}

void ProductLevelSettingWidget::add_defect(QString strdefect)
{
	auto widget = ui.tree_rule->currentItem();
	if (widget == nullptr)
		return;

	productTreeNode* item = (productTreeNode*)widget;
	if (!item->is_product_level())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("���ڲ�Ʒ�ȼ������ȱ�ݷ���"));
		return;
	}

	productTreeNode* child = new productTreeNode(item);
	ui.tree_rule->expandItem(item);
	child->set_defect_name();
	child->setText(0, strdefect);
	child->setText(1, QStringLiteral("�����������:0"));
	child->setWhatsThis(1, "0");
}

bool ProductLevelSettingWidget::CanInsertLogicNode(productTreeNode* pNode)
{
	if (pNode->is_product_level())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("�����ڲ�Ʒ�ȼ�������߼��������"));
		return false;
	}

	if (pNode->is_defect_name())
	{
		int count = pNode->childCount();
		if (count > 0)
		{
			QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("ȱ����ֻ����һ���ж��������߼������"));
			return false;
		}
	}

	if (pNode->is_expression())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("�ж������²�������߼���������"));
		return false;
	}

	return true;
}

bool ProductLevelSettingWidget::CanInsertExpression(productTreeNode* pNode)
{
	if (pNode->is_product_level())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("�����ڲ�Ʒ�ȼ�������ж�����"));
		return false;
	}

	if (pNode->is_defect_name())
	{
		int count = pNode->childCount();
		if (count > 0)
		{
			QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("ȱ����ֻ����һ���ж��������߼������"));
			return false;
		}
	}
	return true;
}

void ProductLevelSettingWidget::set_checksurface_product_surface(const std::vector<CheckSurface_Contain_ProductSurface::product_img_name_and_qimage>& product_surface_data)
{
	ui.roi_widget->set_product_surfaces(product_surface_data);
}

