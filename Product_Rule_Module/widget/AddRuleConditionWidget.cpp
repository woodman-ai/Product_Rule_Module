#include "AddRuleConditionWidget.h"
#include "qmessagebox.h"
#include "utils.h"
#include "Rule_Attri_Manager.h"

AddRuleConditionWidget::AddRuleConditionWidget(const std::vector<QString>& gray_surface,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.button_accept, &QPushButton::clicked, this, &AddRuleConditionWidget::slot_accept);
	connect(ui.button_cancel, &QPushButton::clicked, this, &QDialog::reject);
	connect(ui.combo_condition, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AddRuleConditionWidget::slot_combo_sel_change);
	ui.combo_gray->hide();
	for (const auto& item : gray_surface)
		ui.combo_gray->addItem(item);
	ui.combo_gray->setCurrentIndex(0);

	auto literal_attributes = Rule_Attri_Manager::get_rule_attributes_literal();
	for (const auto& str_attri : literal_attributes)
	{
		ui.combo_condition->addItem(str_attri);
	}

	ui.combo_condition->setCurrentIndex(0);
}

AddRuleConditionWidget::~AddRuleConditionWidget()
{
}

void AddRuleConditionWidget::slot_accept()
{
	auto text = ui.lineEdit->text();
	if (text.isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请输入比较参数"));
		return;
	}
	
	double value = text.toDouble();
	auto cond_text = ui.combo_condition->currentText();
	auto relation_text = ui.combo_relation->currentText();
	int gray_index = ui.combo_gray->currentIndex();
	m_expression.compare_value = value;
	m_expression.g_index = gray_index;
	m_expression.str_attri_name = cond_text;
	m_expression.str_relation = relation_text;
	m_expression.a_index = gray_index;
	accept();
}

void AddRuleConditionWidget::slot_combo_sel_change(int index)
{
	auto current_text = ui.combo_condition->currentText();
	if(current_text == QStringLiteral("反射率") || current_text == QStringLiteral("面积比"))
		ui.combo_gray->show();
	else
		ui.combo_gray->hide();
	auto unit = Rule_Attri_Manager::attributes_literal_unit(current_text);
	ui.label->setText(unit);
}

void AddRuleConditionWidget::set_current_conditioin(const Rule_Calc_Node& rule)
{
	auto name = rule.str_attri_name;
	if (name == QStringLiteral("高度"))
		name = QStringLiteral("长度");
	ui.combo_condition->setCurrentText(name);
	ui.combo_relation->setCurrentText(rule.str_relation);
	if(name == QStringLiteral("反射率"))
		ui.combo_gray->setCurrentIndex(rule.g_index);
	if(name == QStringLiteral("面积比"))
		ui.combo_gray->setCurrentIndex(rule.a_index);
	ui.lineEdit->setText(QString::number(rule.compare_value));
}