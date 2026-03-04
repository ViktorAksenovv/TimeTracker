#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QDateTime>
#include <QMainWindow>
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
private slots:
    void on_addButton_clicked();
    void on_taskInput_returnPressed();
    void updateTimerDisplay();

private:
    QTimer *workTimer;
    QDateTime workStartTime;
    bool isTimerRunning;
    QString currentTimerTask;
    Ui::MainWindow *ui;
    void loadTasks(); // Загрузить задачи
    void saveTasks(); // Сохранить задачи
};
#endif // MAINWINDOW_H
