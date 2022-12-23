#include "RuleSettingWgt.h"
#include "ui_RuleSettingWgt.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include <algorithm>
#include "utils.h"

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
	auto surface = m_src_check_surface_info_manager->enum_check_surface();
	m_cur_select_surface_name = "";
	for (const auto& item : surface)
	{
		ui->listWidget->addItem(item);
	}
}

void RuleSettingWgt::set_data(const product_info_manager* src_product_info_manager, const CheckSurfaceManager* src_check_surface_info_manager,const Product_Rule&  product_rules, const std::vector<Product_Level>& product_level)
{
	m_src_product_info_manager = src_product_info_manager;
	m_src_check_surface_info_manager = src_check_surface_info_manager;
	m_product_rules = product_rules;
	ui->rule_widget->init_product_level(product_level);
}

Surface_Rule* RuleSettingWgt::get_surface_rule_by_name(const QString& str_name)
{
	for (int i = 0 ; i < m_product_rules.size(); i++)
	{
		if (m_product_rules[i].str_check_surface_name == str_name)
		{
			return &m_product_rules[i];
		}
	}

	return nullptr;
}

bool RuleSettingWgt::save_rule(const QString& strName)
{
	int index = m_src_check_surface_info_manager->get_check_surface_index(strName);
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
		new_rule.str_check_surface_name = strName;
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
	auto data = m_src_check_surface_info_manager->get_check_surface_data(m_cur_select_surface_name);
	if (data)
	{
		ui->rule_widget->set_surface_defects(data->surface_defect_name_vect,data->surface_gray_level_surface_name_vect);
		ui->rule_widget->set_checksurface_product_surface(data->contain_product_surface_vect);
	}
	
	
	auto p_suface_rule = get_surface_rule_by_name(str_surface_name);	
	ui->rule_widget->set_surface_rules({});
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

