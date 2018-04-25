// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    void Init();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void reset();
};

#endif // MAINWINDOW_H
