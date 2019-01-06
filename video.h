#ifndef VIDEO_H
#define VIDEO_H

#include "opencv/cv.h"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv/ml.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <QString>
#include <QStringList>
#include "selection.h"
#include <QObject>

using namespace std;
using namespace cv;
using namespace cv::ml;

class Video : public QObject
{
    friend class MainWindow;
    Q_OBJECT
public:
    Video();
    void readFile();
    bool openVideo(QString filename);
    void addSelectionToVector(Selection selectionToAdd);
    QString getShortFileName(QString name);
    bool calculateHOGfeatures(vector<Selection> selections, vector<vector<float> > &features, vector<int> &labels, bool positive, QString name);
    bool calculateHOGfeaturesFromFile(QString path, vector<vector<float> > &features, vector<int> &labels, bool positive, QString name);
    bool createNegatives();
    void saveNegativesToFile();
    void saveHogToFile(vector<vector<float> > hogvector, QString name);
    bool readHogFromFile(vector<vector<float> > &hogvector, vector<int> &labels, bool positive, QString name);
    bool readHogFromFileAuto(vector<vector<float> > &hogvector, vector<int> &labels, bool positive, QString name);
    void createAllLightsVector();
    bool readNegativesFromFile();
    void convertVecToMat(const vector<Mat> &samples, Mat &trainData);
    void convertVecFloatsToMat(vector<vector<float> > &samples, Mat &trainData);
    void trainSVM(vector<vector<float>> samples, vector<int> labels, Mat trainData, string name);
    Ptr<SVM> loadSVM(const String name);
    void clasify(Ptr<SVM> svm, vector<Selection> selections, QString name);
    void smallerVector(vector<Selection> &vec, int divide);
    void biggerVector(vector<Selection> &vec, int multiply);
    void computeHistograms(Mat image, vector<float> &features);
    void sel2Mat(Selection selection);
    vector<Mat> imageToPatches(Mat image, int patchSize = 5);
    void setCurrentFrame(int value);
    QString getFileNameVideo() const;
    void setFileNameVideo(const QString &value);
    QString getFileNameYaml() const;
    void setFileNameYaml(const QString &value);
    vector<Selection> getSelectionVector() const;
    int getNumberOfFrames();
    int getCurrentFrame();
    vector<Selection> getNegatives() const;

signals:
    void negativesProgress(int);
    void negativesMax(int);

private:

    VideoCapture *capture;
    Mat frame;
    QString fileNameVideo;
    QString fileNameYaml;

    vector<Selection> selectionVector;
    vector<Selection> redLights;
    vector<Selection> yellowLights;
    vector<Selection> greenLights;
    vector<Selection> redYellowLights;
    vector<Selection> negatives;

    vector<vector<float>> lightsFeatures;
    vector<vector<float>> negLightsFeatures;
    vector<vector<float>> redLightsFeatures;
    vector<vector<float>> greenLightsFeatures;
    vector<vector<float>> yellowLightsFeatures;
    vector<vector<float>> redYellowLightsFeatures;
    vector<vector<float>> allLightsFeatures;

    vector<vector<float>> lightsFeaturesAuto;
    vector<vector<float>> negLightsFeaturesAuto;
    vector<vector<float>> redLightsFeaturesAuto;
    vector<vector<float>> greenLightsFeaturesAuto;
    vector<vector<float>> yellowLightsFeaturesAuto;
    vector<vector<float>> redYellowLightsFeaturesAuto;
    vector<vector<float>> allLightsFeaturesAuto;

    vector<Mat> posLightsMat;
    vector<Mat> negLightsMat;
    vector<Mat> posRedLightsMat;
    vector<Mat> posGreenLightsMat;
    vector<Mat> posYellowLightsMat;
    vector<Mat> posRedYellowLightsMat;
    vector<Mat> allLightsMat;

    Mat lightsTrainData;
    Mat redLightsTrainData;
    Mat yellowLightsTrainData;
    Mat greenLightsTrainData;
    Mat redYellowLightsTrainData;
    Mat allLightsTrainData;

    Mat lightsTrainDataAuto;
    Mat redLightsTrainDataAuto;
    Mat yellowLightsTrainDataAuto;
    Mat greenLightsTrainDataAuto;
    Mat redYellowLightsTrainDataAuto;
    Mat allLightsTrainDataAuto;

    vector<int> lightsLabels;
    vector<int> redLightsLabels;
    vector<int> greenLightsLabels;
    vector<int> yellowLightsLabels;
    vector<int> redYellowLightsLabels;
    vector<int> allLightsLabels;

    vector<int> lightsLabelsAuto;
    vector<int> redLightsLabelsAuto;
    vector<int> greenLightsLabelsAuto;
    vector<int> yellowLightsLabelsAuto;
    vector<int> redYellowLightsLabelsAuto;
    vector<int> allLightsLabelsAuto;

};

#endif // VIDEO_H
