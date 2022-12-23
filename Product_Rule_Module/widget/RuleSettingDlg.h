#pragma once

#include <QDialog>
#include "ui_RuleSettingDlg.h"
#include "rule_class.h"
class RuleSettingDlg : public QDialog
{
	Q_OBJECT
public:
	RuleSettingDlg(QWidget *parent = Q_NULLPTR);
	void set_data(const product_info_manager* src_product_info_manager,
		const CheckSurfaceManager* src_check_surface_info_manager,
		const Product_Rule& product_rules,
		const std::vector<Product_Level>& product_level);
	void init() { ui.widget->init(); }
	~RuleSettingDlg();
	Product_Rule get_product_rule() const { return ui.widget->get_product_rule(); }
private:
	Ui::RuleSettingDlg ui;
private slots:
	void slot_accept();	
};
