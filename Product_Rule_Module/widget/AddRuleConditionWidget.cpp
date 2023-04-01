#include "AddRuleConditionWidget.h"
#include "qmessagebox.h"
#include "utils.h"

AddRuleConditionWidget::AddRuleConditionWidget(const std::wstring& product_surface, const std::wstring& defect_name, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.button_accept, &QPushButton::clicked, this, &AddRuleConditionWidget::slot_accept);
	connect(ui.button_cancel, &QPushButton::clicked, this, &QDialog::reject);
	connect(ui.combo_condition, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AddRuleConditionWidget::slot_combo_sel_change);
	surface_defect_feature = get_product_surface_defect_features(product_surface);
	for (const auto& attribute_data : surface_defect_feature.defect_filter_features)
	{
		if (attribute_data.first == defect_name)
		{			
			for (const auto& feature : attribute_data.second)
			{
				ui.combo_condition->addItem(QString::fromStdWString(feature));
			}
			break;
		}
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
	m_expression.compare_value = value;
	m_expression.str_attri_name = cond_text;
	m_expression.str_relation = relation_text;
	accept();
}

void AddRuleConditionWidget::slot_combo_sel_change(int index)
{
	auto current_text = ui.combo_condition->currentText().toStdWString();
	QString unit;
	for (const auto& item : surface_defect_feature.attribute_vector)
	{
		if (item.str_chn_name == current_text)
		{
			unit = QString::fromStdWString(item.str_chn_unit_name);
			break;
		}
	}

	ui.label->setText(unit);
}

void AddRuleConditionWidget::set_current_conditioin(const Rule_Calc_Node& rule)
{
	ui.combo_condition->setCurrentText(rule.str_attri_name);
	ui.combo_relation->setCurrentText(rule.str_relation);
	ui.lineEdit->setText(QString::number(rule.compare_value));
}