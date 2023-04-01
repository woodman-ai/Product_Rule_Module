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

void RuleSettingDlg::set_data(const Product_Rule& product_rules, const std::vector<Product_Level>& product_level)
{
	ui.widget->set_data(product_rules, product_level);
}

void RuleSettingDlg::slot_accept()
{
	if (ui.widget->save())
	{
		QDialog::accept();
	}
}
