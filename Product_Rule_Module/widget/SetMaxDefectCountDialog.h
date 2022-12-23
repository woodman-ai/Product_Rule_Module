#pragma once

#include <QDialog>
#include "ui_SetMaxDefectCountDialog.h"

class SetMaxDefectCountDialog : public QDialog
{
	Q_OBJECT
		
	int m_maxCount = 0;
public:
	int GetMaxCount() { return m_maxCount; }
	SetMaxDefectCountDialog(QWidget *parent = Q_NULLPTR);
	~SetMaxDefectCountDialog();
private slots:
	void slot_btn_accept();
private:
	Ui::SetMaxDefectCountDialog ui;
};
