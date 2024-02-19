#pragma once

#include <QMainWindow>
#include <QTranslator>
#include <memory>

namespace Ui
{
	class EditorDialog;
} // namespace Ui

class EditorDialog : public QMainWindow
{
	Q_OBJECT
public:
	EditorDialog(QTranslator& qt, QTranslator& app, QWidget* parent = nullptr);
	~EditorDialog();

	void changeEvent(QEvent*) override;

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
	QTranslator& qtTranslator;
	QTranslator& translator;
	EState state;
};
