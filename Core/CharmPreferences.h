#ifndef CHARMPREFERENCES_H
#define CHARMPREFERENCES_H

#include <QDialog>

#include "Configuration.h"
#include "ui_CharmPreferences.h"

class CharmPreferences : public QDialog
{
    Q_OBJECT

public:
    explicit CharmPreferences( const Configuration& config,
                               QWidget* parent = 0 );
    ~CharmPreferences();

    bool oneEventAtATime() const;
    bool eventsInLeafsOnly() const;
    bool always24hEditing() const;

    Configuration::TaskTrackerFontSize taskTrackerFontSize() const;

private:
    Ui::CharmPreferences m_ui;
};

#endif
