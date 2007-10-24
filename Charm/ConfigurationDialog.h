#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>

#include "Application.h"
#include "Core/Configuration.h"

#include "ui_ConfigurationDialog.h"

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog( const Configuration&, QWidget* parent );
    ~ConfigurationDialog();

    Configuration configuration() const;

private slots:
    void on_comboDbType_currentIndexChanged( int );
    void on_dbTypeNextButton_clicked();
    void on_sqliteBackButton_clicked();
    void on_sqliteDoneButton_clicked();
    void on_sqliteLocationButton_clicked();
    void on_sqliteLocation_textChanged( const QString& text );
    void on_nameLineEdit_textChanged( const QString& text );

private:
    Configuration m_config;
    Ui::ConfigurationDialog m_ui;
};
#endif
