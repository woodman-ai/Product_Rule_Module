#include "SetMaxDefectCountDialog.h"
#include "qmessagebox.h"

SetMaxDefectCountDialog::SetMaxDefectCountDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.button_accept, &QPushButton::clicked, this, &SetMaxDefectCountDialog::slot_btn_accept);
	setWindowTitle(QStringLiteral("设置缺陷上限"));
}

SetMaxDefectCountDialog::~SetMaxDefectCountDialog()
{
}

void SetMaxDefectCountDialog::slot_btn_accept()
{
	QString text = ui.edit_count->text();
	if (text.isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请输入最大允许缺陷数量"));
		return;
	}

	int count = text.toInt();
	if (count < 0)
	{
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请最大允许缺陷数量大于等于0"));
		return;
	}

	m_maxCount = count;
	accept();
}
