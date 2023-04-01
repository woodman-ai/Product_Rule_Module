#include "RuleSettingWgt.h"
#include "ui_RuleSettingWgt.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include <algorithm>
#include "utils.h"
#include "Product_Rule_Export.h"

RuleSettingWgt::RuleSettingWgt(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::RuleSettingWgt();
	ui->setupUi(this);
	connect(ui->listWidget, &QListWidget::itemSelectionChanged,this, &RuleSettingWgt::slot_current_row_changed);
}

RuleSettingWgt::~RuleSettingWgt()
{
	delete ui;
}

void RuleSettingWgt::init()
{
	auto surface = Product_Surface_Defect_Feature_Manager::get_all_product_surface_info();
	m_cur_select_surface_name = "";
	for (const auto& item : surface)
	{
		ui->listWidget->addItem(QString::fromStdWString(item.first));
	}
}

void RuleSettingWgt::set_data(const Product_Rule&  product_rules, const std::vector<Product_Level>& product_level)
{
	m_product_rules = product_rules;
	ui->rule_widget->init_product_level(product_level);
}

ProductSurface_Rule* RuleSettingWgt::get_surface_rule_by_name(const QString& str_name)
{
	for (int i = 0 ; i < m_product_rules.size(); i++)
	{
		if (m_product_rules[i].str_product_surface_name == str_name)
		{
			return &m_product_rules[i];
		}
	}

	return nullptr;
}

bool RuleSettingWgt::save_rule(const QString& strName)
{
	int index = -1;
	for (int i = 0; i < ui->listWidget->count(); i++)
	{
		if (ui->listWidget->item(i)->text() == strName)
		{
			index = i;
			break;
		}
	}
	
	if (index == -1)
		return false;


	if (!ui->rule_widget->TestCondition())
	{
		ui->listWidget->setCurrentRow(index);
		return false;
	}

	auto [suc,new_rule] = ui->rule_widget->get_surface_rules();
	if (!suc)
		return false;
	else
	{
		new_rule.str_product_surface_name = strName;
		auto p_rule = get_surface_rule_by_name(strName);
		if(p_rule)
			*p_rule = new_rule;
		else
		{
			m_product_rules.push_back(new_rule);
		}
	}
		
	return true;
}

void RuleSettingWgt::slot_current_row_changed()
{
	auto index = ui->listWidget->currentRow();
	if (index == -1)
		return;

	auto text = ui->listWidget->currentItem()->text();
	slot_combo_area_sel(text);
}

void RuleSettingWgt::slot_combo_area_sel(const QString& str_surface_name)
{
	if (m_cur_select_surface_name == str_surface_name)
		return;

	if (m_cur_select_surface_name != "")
	{
		if (!save_rule(m_cur_select_surface_name))
			return;
	}
	
	m_cur_select_surface_name = str_surface_name;

	ui->rule_widget->clear();
	auto data = get_product_surface_defect_features(str_surface_name.toStdWString());
	std::vector<QString> defect_name_vect;
	for (const auto& item : data.defect_filter_features)
	{
		defect_name_vect.push_back(QString::fromStdWString(item.first));
	}

	ui->rule_widget->set_surface_defects(defect_name_vect);
	auto surface_config = Product_Config::get_surface_config(str_surface_name.toStdWString());
	if(surface_config)
		ui->rule_widget->set_product_surface_img(QImage(QString::fromStdWString(surface_config->str_img_path)));	

	auto p_suface_rule = get_surface_rule_by_name(str_surface_name);	
	ProductSurface_Rule temp;
	temp.str_product_surface_name = str_surface_name;
	ui->rule_widget->set_surface_rules(temp);
	if (p_suface_rule)
	{
		ui->rule_widget->set_surface_rules(*p_suface_rule);
	}

	ui->rule_widget->init();

}
bool RuleSettingWgt::save()
{
	if (m_cur_select_surface_name.isEmpty())
		return false;

	return save_rule(m_cur_select_surface_name);
}

