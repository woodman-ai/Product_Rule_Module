#pragma once

#include <QDialog>
#include "ui_AddRuleConditionWidget.h"
#include <functional>
#include "rule_typedef.h"

class AddRuleConditionWidget : public QDialog
{
	Q_OBJECT
	Rule_Calc_Node m_expression;
	Product_Surface_Defect_Info surface_defect_feature;
public:
	AddRuleConditionWidget(const std::wstring& product_surface, const std::wstring& defect_name, QWidget *parent = Q_NULLPTR);
	~AddRuleConditionWidget();
	void set_current_conditioin(const Rule_Calc_Node& rule);
	Rule_Calc_Node GetExpression() { return m_expression; }
private slots:
	void slot_accept();
	void slot_combo_sel_change(int index);
private:
	Ui::AddRuleConditionWidget ui;
};
