#pragma once

#include <QDialog>
#include <memory>

namespace Ui
{
	class EditorDialog;
} // namespace Ui

class EditorDialog : public QDialog
{
	Q_OBJECT
public:
	explicit EditorDialog(QWidget* parent = nullptr);
	~EditorDialog();

public slots:

	void updateJsonWidgets();
	void updateJsonWidgetValues();
	void updateJsonTextValues();

private slots:
	void onSchemaChanged();
	void onJsonTextChanged();

private:
	enum class EState : char;

	std::unique_ptr<Ui::EditorDialog> ui;
	EState state;
};
