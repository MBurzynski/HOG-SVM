#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    video = new Video();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonAddFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Otworz plik YAML"), ".",tr("Video Files (*.yml *.yaml)"));
    if(!filename.isEmpty())
    {
        video->setFileNameYaml(filename);
        video->readFile();
        if(video->openVideo(video->getFileNameVideo()))
        {
            ui->listDetectors->addItem("LightsDetector.yml");
            ui->listDetectors->addItem("ColorDetector.yml");
            ui->listDetectors->addItem("LightsDetectorAuto.yml");
            ui->listDetectors->addItem("ColorDetectorAuto.yml");
            ui->listDetectors->addItem("RedLightsDetector.yml");
            ui->listDetectors->addItem("GreenLightsDetector.yml");
            ui->listDetectors->addItem("YellowLightsDetector.yml");
            ui->listDetectors->addItem("RedYellowLightsDetector.yml");

            ui->listImages->addItem("AllLights");
            ui->listImages->addItem("RedLights");
            ui->listImages->addItem("GreenLights");
            ui->listImages->addItem("YellowLights");
            ui->listImages->addItem("RedYellowLights");
            ui->listImages->addItem("Negatives");

            ui->buttonNegatives->setEnabled(true);
            ui->buttonTestSVM->setEnabled(true);
            ui->buttonLightsDetector->setEnabled(true);
            ui->buttonColorDetector->setEnabled(true);
            ui->buttonAutoColor->setEnabled(true);
            ui->buttonAutoLights->setEnabled(true);
        }
        else
        {
            QMessageBox msg;
            msg.setText("Nie udało się otworzyć pliku. Wybierz plik zawierający dane  dotyczące zbioru zaznaczeń na filmie");
            msg.exec();
            return;
        }

    }
}

void MainWindow::on_buttonNegatives_clicked()
{
    if(video->createNegatives())
    {
        QMessageBox msg;
        msg.setText("Negatywne próbki zostały wygenerowane");
        msg.exec();
        return;
    }

}

void MainWindow::on_buttonLightsDetector_clicked()
{
    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla wszystkich świateł
    video->calculateHOGfeatures(video->getSelectionVector(),video->lightsFeatures,video->lightsLabels,true,"LightsHOG");
    video->calculateHOGfeatures(video->getNegatives(),video->lightsFeatures,video->lightsLabels,false,"LightsFullHOG");
    video->trainSVM(video->lightsFeatures,video->lightsLabels,video->lightsTrainData,"LightsDetector");

}


void MainWindow::on_buttonTestSVM_clicked()
{
    QString detector = ui->listDetectors->currentItem()->text();
    QString images = ui->listImages->currentItem()->text();
    vector<Selection> selections;

    if(images=="AllLights")
        selections=video->selectionVector;
    else if(images=="RedLights")
        selections=video->redLights;
    else if(images=="GreenLights")
        selections=video->greenLights;
    else if(images=="YellowLights")
        selections=video->yellowLights;
    else if(images=="RedYellowLights")
        selections=video->redYellowLights;
    else if(images=="Negatives")
        selections=video->negatives;

    video->clasify(video->loadSVM(detector.toStdString()),selections,detector);
}

void MainWindow::on_buttonColorDetector_clicked()
{

    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla świateł czerwonych
    video->calculateHOGfeatures(video->redLights,video->redLightsFeatures,video->redLightsLabels,true,"RedLightsHOG");
//    video->calculateHOGfeatures(video->getNegatives(),video->redLightsFeatures,video->redLightsLabels,false,"RedLightsFullHOG");
//    video->trainSVM(video->redLightsFeatures,video->redLightsLabels,video->redLightsTrainData,"RedLightsDetector");

    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla świateł zielonych
    video->calculateHOGfeatures(video->greenLights,video->greenLightsFeatures,video->greenLightsLabels,true,"GreenLightsHOG");
//    video->calculateHOGfeatures(video->getNegatives(),video->greenLightsFeatures,video->greenLightsLabels,false,"GreenLightsFullHOG");
//    video->trainSVM(video->greenLightsFeatures,video->greenLightsLabels,video->greenLightsTrainData,"GreenLightsDetector");

    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla świateł żółtych
    video->calculateHOGfeatures(video->yellowLights,video->yellowLightsFeatures,video->yellowLightsLabels,true,"YellowLightsHOG");
//    video->calculateHOGfeatures(video->getNegatives(),video->yellowLightsFeatures,video->yellowLightsLabels,false,"YellowLightsFullHOG");
//    video->trainSVM(video->yellowLightsFeatures,video->yellowLightsLabels,video->yellowLightsTrainData,"YellowLightsDetector");

    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla świateł żółto-czerwonych
    video->calculateHOGfeatures(video->redYellowLights,video->redYellowLightsFeatures,video->redYellowLightsLabels,true,"RedYellowLightsHOG");
//    video->calculateHOGfeatures(video->getNegatives(),video->redYellowLightsFeatures,video->redYellowLightsLabels,false,"RedYellowLightsFullHOG");
//    video->trainSVM(video->redYellowLightsFeatures,video->redYellowLightsLabels,video->redYellowLightsTrainData,"RedYellowLightsDetector");

    video->trainSVM(video->allLightsFeatures,video->allLightsLabels,video->allLightsTrainData,"ColorDetector");
}

void MainWindow::on_buttonAutoLights_clicked()
{
    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla wszystkich świateł zaznaczonych automatycznie
    video->calculateHOGfeaturesFromFile("/home/marcin/Projekty/Python/lightsdetection/TP",video->lightsFeaturesAuto,video->lightsLabelsAuto,true,"LightsHOGAuto");
    video->calculateHOGfeaturesFromFile("/home/marcin/Projekty/Python/lightsdetection/FP",video->lightsFeaturesAuto,video->lightsLabelsAuto,false,"LightsFullHOGAuto");
    video->trainSVM(video->lightsFeaturesAuto,video->lightsLabelsAuto,video->lightsTrainDataAuto,"LightsDetectorAuto");
}

void MainWindow::on_buttonAutoColor_clicked()
{
    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla świateł czerwonych zaznaczonych automatycznie
    video->calculateHOGfeaturesFromFile("/home/marcin/Projekty/Python/lightsdetection/TP_R",video->redLightsFeaturesAuto,video->redLightsLabelsAuto,true,"RedLightsHOGAuto");

    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla świateł zielonych zaznaczonych automatycznie
    video->calculateHOGfeaturesFromFile("/home/marcin/Projekty/Python/lightsdetection/TP_G",video->greenLightsFeaturesAuto,video->greenLightsLabelsAuto,true,"GreenLightsHOGAuto");

    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla świateł żółtych zaznaczonych automatycznie
    video->calculateHOGfeaturesFromFile("/home/marcin/Projekty/Python/lightsdetection/TP_Y",video->yellowLightsFeaturesAuto,video->yellowLightsLabelsAuto,true,"YellowLightsHOGAuto");

    // Wyznaczenie cech HOG sygnalizacji i trenowanie SVM dla świateł żółto-czerwonych zaznaczonych automatycznie
    video->calculateHOGfeaturesFromFile("/home/marcin/Projekty/Python/lightsdetection/TP_RY",video->redYellowLightsFeaturesAuto,video->redYellowLightsLabelsAuto,true,"RedYellowLightsHOGAuto");

    video->trainSVM(video->allLightsFeaturesAuto,video->allLightsLabelsAuto,video->allLightsTrainDataAuto,"ColorDetectorAuto");
}
