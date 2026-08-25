#pragma once

#include <QDialog>
#include <QObject>

class HelpDialog : public QDialog {
    Q_OBJECT
  public:
    explicit HelpDialog(QWidget* parent = nullptr);
    // ~HelpDialog();
    void onHelpRequested();

  private:
    QString getHelpHtmlContent();
};
