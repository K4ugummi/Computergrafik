// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QSlider>

// Default Control values used to reset UI parameter.
const int MYCTRL_DEF_FOV = 45;
const int MYCTRL_DEF_ANGLE = 0;
const bool MYCTRL_DEF_IS_PROJECTION = true;
const double MYCTRL_DEF_NEARCLIPPING = 0.1;
const double MYCTRL_DEF_FARCLIPPING = 100.0;
const int MYCTRL_DEF_ROTATIONA = 0;
const int MYCTRL_DEF_ROTATIONB = 0;
const int MYCTRL_DEF_ROTATIONC = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Init();
}

void MainWindow::Init() {
    qDebug("MainWindow::Init()");

    // Connect Signals and Slots.
    connect(ui->pbReset, &QPushButton::clicked, this, &MainWindow::reset);

    // Lambda functions to print slider values.
    /*
    connect(ui->vsFOV, &QSlider::valueChanged,
            [](int value){ qDebug("MainWindow::ui->vsFOV(%i)", value); });
    connect(ui->vsAngle, &QSlider::valueChanged,
            [](int value){ qDebug("MainWindow::ui->vsAngle(%i)", value); });
    connect(ui->hsRotationA, &QSlider::valueChanged,
            [](int value){ qDebug("MainWindow::ui->hsRotationA(%i)", value); });
    connect(ui->hsRotationB, &QSlider::valueChanged,
            [](int value){ qDebug("MainWindow::ui->hsRotationB(%i)", value); });
    connect(ui->hsRotationC, &QSlider::valueChanged,
            [](int value){ qDebug("MainWindow::ui->hsRotationC(%i)", value); });
    */
}

MainWindow::~MainWindow() {
    delete ui;
}

// Reset UI-Control values.
void MainWindow::reset() {
    qDebug("MainWindow::reset()");

    ui->spFOV->setValue(MYCTRL_DEF_FOV);
    // vsFOV will be changed from spFOV signal.
    ui->spAngle->setValue(MYCTRL_DEF_ANGLE);
    // vsAngle will be changed from spAngle signal.

    ui->rbPerspective->setChecked(MYCTRL_DEF_IS_PROJECTION);
    // rbOrthogonal will be changed from rbPerspective signal.

    ui->hsRotationA->setValue(MYCTRL_DEF_ROTATIONA);
    ui->hsRotationB->setValue(MYCTRL_DEF_ROTATIONB);
    ui->hsRotationC->setValue(MYCTRL_DEF_ROTATIONC);

    ui->dsbFar->setValue(MYCTRL_DEF_FARCLIPPING);
    ui->dsbNear->setValue(MYCTRL_DEF_NEARCLIPPING);
}
