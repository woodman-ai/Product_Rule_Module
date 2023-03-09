#include "SetMaxDefectCountDialog.h"
#include "qmessagebox.h"

SetMaxDefectCountDialog::SetMaxDefectCountDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.button_accept, &QPushButton::clicked, this, &SetMaxDefectCountDialog::slot_btn_accept);
	setWindowTitle(QStringLiteral("����ȱ������"));
}

SetMaxDefectCountDialog::~SetMaxDefectCountDialog()
{
}

void SetMaxDefectCountDialog::slot_btn_accept()
{
	QString text = ui.edit_count->text();
	if (text.isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("�������������ȱ������"));
		return;
	}

	int count = text.toInt();
	if (count < 0)
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("���������ȱ���������ڵ���0"));
		return;
	}

	m_maxCount = count;
	accept();
}
