#pragma once

#include <QWidget>
#include <QFile>
#include <QDir>
#include <QTime>
#include "Rule_Manager.h"
#include "rule_class.h"
#include <map>


namespace Ui { class RuleSettingWgt; };
class RuleSettingWgt : public QWidget
{
	Q_OBJECT
private:
	Product_Rule  m_product_rules;
private:
	void slot_combo_area_sel(const QString& str);
	ProductSurface_Rule* get_surface_rule_by_name(const QString& str_name);
public:
	RuleSettingWgt(QWidget *parent = Q_NULLPTR);
	~RuleSettingWgt();
	void set_data(const Product_Rule&  product_rules,const std::vector<Product_Level>& product_level);
	void init();
	Ui::RuleSettingWgt *ui;
	QString m_cur_select_surface_name;	
	bool save_rule(const QString& strName);
private slots:
	void slot_current_row_changed();
public:
	bool save();
	Product_Rule get_product_rule() const { return m_product_rules; }
};
