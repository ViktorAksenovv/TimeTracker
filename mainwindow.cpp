#include "mainwindow.h"
#include <QCloseEvent>
#include <QContextMenuEvent>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QTimer>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isTimerRunning(false)
{
    ui->setupUi(this);
    setWindowTitle("Список задач");

    loadTasks();

    connect(ui->taskInput, &QLineEdit::returnPressed, ui->addButton, &QPushButton::click);

    // Инициализация таймера
    workTimer = new QTimer(this);
    connect(workTimer, &QTimer::timeout, this, &MainWindow::updateTimerDisplay);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_addButton_clicked()
{
    QString task = ui->taskInput->text();

    if (task.isEmpty()) {
        return;
    }

    ui->taskList->addItem(task);
    ui->taskInput->clear();

    saveTasks(); // Сохраняем после добавления
}
void MainWindow::on_taskInput_returnPressed()
{
    on_addButton_clicked();
}
void MainWindow::loadTasks()
{
    QFile file("tasks.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString task = in.readLine();
        if (!task.isEmpty()) {
            ui->taskList->addItem(task);
        }
    }
    file.close();
}

void MainWindow::saveTasks()
{
    QFile file("tasks.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (int i = 0; i < ui->taskList->count(); i++) {
        out << ui->taskList->item(i)->text() << "\n";
    }
    file.close();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    saveTasks(); // Сохраняем при закрытии
    event->accept();
}
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    // Получаем элемент под курсором
    QListWidgetItem *item = ui->taskList->itemAt(ui->taskList->mapFromGlobal(event->globalPos()));

    if (!item)
        return;

    // Создаем меню
    QMenu menu(this);

    // Добавляем пункты меню
    QAction *timerAction = menu.addAction("⏱ Таймер");
    QAction *renameAction = menu.addAction("✏ Переименовать");
    QAction *descAction = menu.addAction("📝 Описание");
    menu.addSeparator();
    QAction *deleteAction = menu.addAction("🗑 Удалить");

    // ========== ТАЙМЕР ==========
    connect(timerAction, &QAction::triggered, [this, item]() {
        QString taskName = item->text();

        // Создаем диалог для таймера
        QDialog dialog(this);
        dialog.setWindowTitle("Таймер: " + taskName);
        dialog.resize(300, 200);

        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        QLabel *timeLabel = new QLabel("00:00:00", &dialog);
        timeLabel->setAlignment(Qt::AlignCenter);
        timeLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        layout->addWidget(timeLabel);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *startButton = new QPushButton("Старт", &dialog);
        QPushButton *stopButton = new QPushButton("Стоп", &dialog);
        stopButton->setEnabled(false);

        buttonLayout->addWidget(startButton);
        buttonLayout->addWidget(stopButton);
        layout->addLayout(buttonLayout);

        // Загружаем сохраненное время
        QString timeFile = taskName + "_time.txt";
        double totalHours = 0;
        QFile file(timeFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            totalHours = in.readLine().toDouble();
            file.close();
        }

        QLabel *totalLabel = new QLabel(QString("Всего: %1 ч").arg(totalHours, 0, 'f', 2), &dialog);
        totalLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(totalLabel);

        QTimer timer;
        QDateTime startTime;
        bool running = false;

        connect(startButton, &QPushButton::clicked, [&]() {
            running = true;
            startTime = QDateTime::currentDateTime();
            startButton->setEnabled(false);
            stopButton->setEnabled(true);
            timer.start(1000);
        });

        connect(stopButton, &QPushButton::clicked, [&]() {
            running = false;
            timer.stop();

            // Считаем время
            qint64 seconds = startTime.secsTo(QDateTime::currentDateTime());
            double hours = seconds / 3600.0;
            totalHours += hours;

            // Сохраняем
            QFile file(timeFile);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << totalHours;
                file.close();
            }

            totalLabel->setText(QString("Всего: %1 ч").arg(totalHours, 0, 'f', 2));
            timeLabel->setText("00:00:00");
            startButton->setEnabled(true);
            stopButton->setEnabled(false);
        });

        connect(&timer, &QTimer::timeout, [&]() {
            if (!running)
                return;
            qint64 seconds = startTime.secsTo(QDateTime::currentDateTime());
            int hours = seconds / 3600;
            int minutes = (seconds % 3600) / 60;
            int secs = seconds % 60;

            timeLabel->setText(QString("%1:%2:%3")
                                   .arg(hours, 2, 10, QChar('0'))
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(secs, 2, 10, QChar('0')));
        });

        dialog.exec();
    });

    // ========== ПЕРЕИМЕНОВАТЬ ==========
    connect(renameAction, &QAction::triggered, [this, item]() {
        QString oldName = item->text();

        bool ok;
        QString newName = QInputDialog::getText(this,
                                                "Переименовать задачу",
                                                "Введите новое название:",
                                                QLineEdit::Normal,
                                                oldName,
                                                &ok);

        if (ok && !newName.isEmpty() && newName != oldName) {
            // Переименовываем файлы
            QFile oldDesc(oldName + "_desc.txt");
            if (oldDesc.exists()) {
                oldDesc.rename(newName + "_desc.txt");
            }
            QFile oldTime(oldName + "_time.txt");
            if (oldTime.exists()) {
                oldTime.rename(newName + "_time.txt");
            }

            item->setText(newName);
            saveTasks();
        }
    });

    // ========== ОПИСАНИЕ ==========
    connect(descAction, &QAction::triggered, [this, item]() {
        QString taskName = item->text();

        QDialog dialog(this);
        dialog.setWindowTitle("Описание задачи: " + taskName);
        dialog.resize(400, 300);

        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QTextEdit *textEdit = new QTextEdit(&dialog);
        layout->addWidget(textEdit);

        QPushButton *saveButton = new QPushButton("Сохранить", &dialog);
        layout->addWidget(saveButton);

        QString filename = taskName + "_desc.txt";
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            textEdit->setText(in.readAll());
            file.close();
        }

        connect(saveButton, &QPushButton::clicked, [&]() {
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << textEdit->toPlainText();
                file.close();
                dialog.accept();
            }
        });

        dialog.exec();
    });

    // ========== УДАЛИТЬ ==========
    connect(deleteAction, &QAction::triggered, [this, item]() {
        int result = QMessageBox::question(this,
                                           "Удаление",
                                           "Удалить задачу \"" + item->text() + "\"?");

        if (result == QMessageBox::Yes) {
            // Удаляем все файлы задачи
            QFile(item->text() + "_desc.txt").remove();
            QFile(item->text() + "_time.txt").remove();

            delete item;
            saveTasks();
        }
    });

    // Показываем меню
    menu.exec(event->globalPos());
}
void MainWindow::updateTimerDisplay()
{
    // Пока пустая функция, чтобы компилировалось
    // Здесь потом будет код для обновления таймера
}
