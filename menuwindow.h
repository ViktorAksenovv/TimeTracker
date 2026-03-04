#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MenuWindow; }
QT_END_NAMESPACE

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

private slots:
    void on_tasksButton_clicked();
    void on_analyticsButton_clicked();
    void on_exitButton_clicked();

private:
    Ui::MenuWindow *ui;
};

#endif // MENUWINDOW_H
