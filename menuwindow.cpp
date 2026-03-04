#include "menuwindow.h"
#include "ui_menuwindow.h"
#include "mainwindow.h"
#include <QMessageBox>

MenuWindow::MenuWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuWindow)
{
    ui->setupUi(this);
    setWindowTitle("Time Tracker - Главное меню");
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::on_tasksButton_clicked()
{
    // Открываем окно с задачами
    MainWindow *taskWindow = new MainWindow(this);
    taskWindow->show();

    // Скрываем меню (можно и не скрывать)
    // this->hide();
}

void MenuWindow::on_analyticsButton_clicked()
{
    QMessageBox::information(this, "Аналитика",
                             "Функция аналитика будет добавлена позже!");
}

void MenuWindow::on_exitButton_clicked()
{
    close();
}
