#include "RuleSettingDlg.h"
RuleSettingDlg::RuleSettingDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, &QPushButton::clicked, this, &RuleSettingDlg::slot_accept);
}

RuleSettingDlg::~RuleSettingDlg()
{
}

void RuleSettingDlg::set_data(const product_info_manager* src_product_info_manager,
	const CheckSurfaceManager* src_check_surface_info_manager,
	const Product_Rule& product_rules,
	const std::vector<Product_Level>& product_level)
{
	ui.widget->set_data(src_product_info_manager, src_check_surface_info_manager, product_rules, product_level);
}

void RuleSettingDlg::slot_accept()
{
	if (ui.widget->save())
	{
		QDialog::accept();
	}
}
