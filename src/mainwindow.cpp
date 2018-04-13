#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QSlider>

// Default Control values.
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
    QObject::connect(ui->rbReset, SIGNAL(released()), this, SLOT(reset()));
}

MainWindow::~MainWindow() {
    delete ui;
}

// Reset UI-Control values.
void MainWindow::reset() {
    qDebug("MainWindow::reset()");

    ui->spFOV->setValue(MYCTRL_DEF_FOV);
    ui->spAngle->setValue(MYCTRL_DEF_ANGLE);

    ui->rbPerspective->setChecked(true);

    ui->hsRotationA->setValue(MYCTRL_DEF_ROTATIONA);
    ui->hsRotationB->setValue(MYCTRL_DEF_ROTATIONB);
    ui->hsRotationC->setValue(MYCTRL_DEF_ROTATIONC);

    ui->dsbFar->setValue(MYCTRL_DEF_FARCLIPPING);
    ui->dsbNear->setValue(MYCTRL_DEF_NEARCLIPPING);
}
