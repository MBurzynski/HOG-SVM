#include "video.h"


Video::Video()
{

}

void Video::readFile()
{

    QString fileName = getFileNameYaml();
    fileName = getShortFileName(fileName);
    fileName.append(".yml");
    FileStorage fs2(fileName.toStdString(), FileStorage::READ);
    if(!fs2.isOpened())
    {
        cout<<"Nie udalo sie otworzyc pliku";
    }
    else
    {
    fileName.chop(4);
    FileNode videoData=fs2[fileName.toStdString()];
    FileNodeIterator it = videoData.begin();
    FileNodeIterator it_end=videoData.end();
    for(;it!=it_end;++it)
    {
        Point cL,cP;
        cL.x=(int)(*it)["x"];
        cL.y=(int)(*it)["y"];
        int width=(int)(*it)["width"];
        cP.x=cL.x+width;
        cP.y=cL.y+2*width;
        string colour=(string)(*it)["Color"];
        int frameNumber = (int)(*it)["Frame number"];
        Selection selectionFromFile(cL,cP,colour,frameNumber);
        addSelectionToVector(selectionFromFile);
    }
    cout<<"Film wczytany"<<endl;
    fileName.append(".mp4");
    setFileNameVideo(fileName);
    }

}

bool Video::openVideo(QString filename)
{
    string filename1 = filename.toStdString();
    capture = new VideoCapture(filename1);
    if(capture->isOpened())
    {
        readNegativesFromFile();
        cout<<"Negatywne probki wczytane"<<endl;

        //Zmniejszenie wektorów zawierających wszystkie światła, światła czerwone i próbki negatywne,...
        //w celu uniknięcia błędu spowodowanego niewystarczającą ilością pamięci

        smallerVector(redLights,2);
        smallerVector(selectionVector,3);
        smallerVector(negatives,6);
        cout<<"Wektory zmniejszone"<<endl;

        //Powiększenie wektorów zawierających światła żółte i czerwono-żółte
        biggerVector(yellowLights,2);
        biggerVector(redYellowLights,2);
        cout<<"Wektory zwiekszone"<<endl;

        return true;
    }
    else
        return false;

}

QString Video::getShortFileName(QString name)
{
    QString filenameStorage = name;
    QStringList StringList = filenameStorage.split('/');
    filenameStorage=StringList.last();
    filenameStorage.chop(4);
    return filenameStorage;
}

bool Video::calculateHOGfeatures(vector<Selection> selections, vector<vector<float> > &features, vector<int> &labels, bool positive, QString name)
{
    if(readHogFromFile(features,labels,positive,name))
    {
        return true;
    }

    int size = selections.size();
    if(positive==false)
    {
        if(negLightsFeatures.size()!=0)
        {
            for(int i=0;i<negLightsFeatures.size();++i)
            {
                features.push_back(negLightsFeatures[i]);
                labels.push_back(0);
                cout<<"HOG zapisany: "<<i<<endl;
            }
            saveHogToFile(features,name);
            return true;
        }
    }
    for(int i=0;i<size;++i)
    {
        Selection sel=selections[i];
        Mat img, imgGray;
        setCurrentFrame(sel.getFrameNumber());
        if(!capture->read(frame))
        {
            return false;
        }
        else
        {
            img=frame(sel.getSelectionStable()).clone();
//            imshow("caly",img);
//            waitKey(0);
            cvtColor(img,imgGray,CV_BGR2GRAY);
            resize(imgGray,imgGray,Size(20,40));

            //HOG: window size=(20,40), block size=10,block stride=5,cell size=5
            HOGDescriptor hog(Size(20,40),Size(10,10),Size(5,5),Size(5,5),9);
            vector<float> featureValues;
            vector<Point> locations;
            hog.compute(imgGray,featureValues,Size(5,5),Size(0,0),locations);

            //Color descriptor
            vector<Mat> patches = imageToPatches(img,5);
            for(int i = 0; i<patches.size();++i)
            {
                computeHistograms(patches.at(i),featureValues);
            }


            features.push_back(featureValues);

            if(positive)
            {
                labels.push_back(1);

                if(name!="LightsHOG")
                    allLightsFeatures.push_back(featureValues);

                if(name=="RedLightsHOG")
                    allLightsLabels.push_back(1);
                else if(name=="GreenLightsHOG")
                    allLightsLabels.push_back(2);
                else if(name=="YellowLightsHOG")
                    allLightsLabels.push_back(3);
                else if(name=="RedYellowLightsHOG")
                    allLightsLabels.push_back(4);
            }
            else
            {
                labels.push_back(0);
                negLightsFeatures.push_back(featureValues);
            }

            cout<<"HOG "<<name.toStdString()<<": "<<i<<endl;

        }
        img.release();
        imgGray.release();
        frame.release();

    }
    cout<<"Wyszedlem z glownej petli"<<endl;

    saveHogToFile(features,name);
    cout<<"Rozmiar AllLightsFeatures: "<<allLightsFeatures.size()<<endl;
    cout<<"Rozmiar AllLightsLabels: "<<allLightsLabels.size()<<endl;

}

bool Video::calculateHOGfeaturesFromFile(QString path, vector<vector<float> > &features, vector<int> &labels, bool positive, QString name)
{
    if(readHogFromFileAuto(features,labels,positive,name))
    {
        return true;
    }

    if(positive==false)
    {
        if(negLightsFeaturesAuto.size()!=0)
        {
            for(int i=0;i<negLightsFeaturesAuto.size();++i)
            {
                features.push_back(negLightsFeaturesAuto[i]);
                labels.push_back(0);
                cout<<"HOG zapisany: "<<i<<endl;
            }
            saveHogToFile(features,name);
            return true;
        }
    }


    int index = 1;
    if(positive)
    {
        path = path + "/tp_";
    }
    else
    {
        path = path + "/fp_";
    }
    while(true)
    {
        if(index>25000)
        {
            break;
        }
        Mat img, imgGray;
        QString img_name = path + QString::number(index) + ".png";
        img = imread(img_name.toStdString());
        if(! img.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            break;
        }
        cvtColor(img,imgGray,CV_BGR2GRAY);
//        imshow("caly",img);
//        waitKey(0);
        resize(imgGray,imgGray,Size(20,40));

        //HOG: window size=(20,40), block size=10,block stride=5,cell size=5
        HOGDescriptor hog(Size(20,40),Size(10,10),Size(5,5),Size(5,5),9);
        vector<float> featureValues;
        vector<Point> locations;
        hog.compute(imgGray,featureValues,Size(5,5),Size(0,0),locations);

        //Color descriptor
        vector<Mat> patches = imageToPatches(img,5);
        for(int i = 0; i<patches.size();++i)
        {
            computeHistograms(patches.at(i),featureValues);
        }


        features.push_back(featureValues);

        if(positive)
        {
            labels.push_back(1);

            if(name!="LightsHOGAuto")
                allLightsFeaturesAuto.push_back(featureValues);

            if(name=="RedLightsHOGAuto")
                allLightsLabelsAuto.push_back(1);
            else if(name=="GreenLightsHOGAuto")
                allLightsLabelsAuto.push_back(2);
            else if(name=="YellowLightsHOGAuto")
                allLightsLabelsAuto.push_back(3);
            else if(name=="RedYellowLightsHOGAuto")
                allLightsLabelsAuto.push_back(4);
        }
        else
        {
            labels.push_back(0);
            negLightsFeaturesAuto.push_back(featureValues);
        }

        cout<<"HOG Auto"<<name.toStdString()<<": "<<index<<endl;


        img.release();
        imgGray.release();
        index +=1;

    }
    cout<<"Wyszedlem z glownej petli"<<endl;

    saveHogToFile(features,name);
    cout<<"Rozmiar AllLightsFeatures: "<<allLightsFeaturesAuto.size()<<endl;
    cout<<"Rozmiar AllLightsLabels: "<<allLightsLabelsAuto.size()<<endl;
}

bool Video::createNegatives()
{

    if(readNegativesFromFile())
        return true;
    else
    {
    vector<int> framesWithoutLights;

    for(int i=0;i<getNumberOfFrames();i++)
    {
        bool jest=false;
        for(int j=0;j<selectionVector.size();++j)
        {
            if(i==selectionVector[j].getFrameNumber())
                jest=true;
        }
        if(jest==false)
            framesWithoutLights.push_back(i);
    }
    emit negativesMax(framesWithoutLights.size());
    negatives.reserve(framesWithoutLights.size());


    for(int i=0;i<framesWithoutLights.size();++i)
    {
        setCurrentFrame(framesWithoutLights[i]);
        if(!capture->read(frame))
        {
            return false;
        }
        else
        {
        int randx = qrand()*(frame.cols-50)/RAND_MAX;
        int randy = qrand()*(frame.rows-100)/RAND_MAX;
        int randw = qrand()*40/RAND_MAX+10;
        int h = 2*randw;
        Selection randSelection(Point(randx,randy),Point(randx+randw,randy+h),"None",getCurrentFrame());
        negatives.push_back(randSelection);
        cout<<getCurrentFrame()<<endl;

        emit negativesProgress(i);
        }
    }
    saveNegativesToFile();
    }
    return true;

}

void Video::saveNegativesToFile()
{
    QString filenameStorage = "Negatives";
    filenameStorage.append(".yml");
    FileStorage fs(filenameStorage.toStdString(),FileStorage::WRITE);
    Selection slc;
    filenameStorage.chop(4);
    fs<<filenameStorage.toStdString()<<"[";
    for(int i=0;i<negatives.size();++i)
    {
        slc=negatives.at(i);
        cout<<slc.getFrameNumber()<<": "<<slc.getColor()<<' '<<slc.getSelectionStable().x<<endl;
        fs<<"{:"<<"Frame number"<<slc.getFrameNumber()<<"Color"<<slc.getColor()<<"x"<<slc.getSelectionStable().x<<"y"<<slc.getSelectionStable().y<<"width"<<slc.getSelectionStable().width<<"}";
    }
    fs<<"]";
    fs.release();
    cout<<"Zapisywanie udane"<<endl;
}

void Video::saveHogToFile(vector<vector<float>> hogvector, QString name)
{
    int i= 0;
    QString filenameStorage = name;
    filenameStorage.append(".yml");
    FileStorage fs(filenameStorage.toStdString(),FileStorage::WRITE);
    filenameStorage.chop(4);
    vector<vector<float>>::iterator itr;
    int size = hogvector.size();
    fs<<"Size"<<size;
    fs<<filenameStorage.toStdString()<<"[";
    for (itr=hogvector.begin();itr!=hogvector.end();++itr)
    {
        string title = "Mat";
        string x = to_string(i);
        title+=x;
        fs<<"{:"<<title<<*itr<<"}";
        ++i;
    }
    fs<<"]";
    fs.release();
    cout<<"Zapisywanie udane"<<endl;

}

bool Video::readHogFromFile(vector<vector<float>> &hogvector, vector<int> &labels,bool positive, QString name)
{
    int size=hogvector.size();
    QString fileName = name;
    fileName.append(".yml");
    FileStorage fs2(fileName.toStdString(), FileStorage::READ);
    if(!fs2.isOpened())
    {
        cout<<"Nie udalo sie otworzyc pliku"<<endl;
        return false;
    }
    else
    {
    fileName.chop(4);
    FileNode videoData=fs2[fileName.toStdString()];

    FileNodeIterator it = videoData.begin();
    FileNodeIterator it_end=videoData.end();

    int i=0;
    int lowBorder=0; //Wyłuskanie jedynie cech próbek negatywnych z pliku
    if(positive==false)
    {
        lowBorder=hogvector.size();
        negLightsFeatures.clear();
    }


    for(;it!=it_end;++it)
    {
        vector<float> tmpVec;
        string title = "Mat";
        string x = to_string(i);
        title+=x;
        if(i>=lowBorder)
        {
        (*it)[title] >> tmpVec;
        hogvector.push_back(tmpVec);

        if(positive==true && name!= "LightsHOG")
            allLightsFeatures.push_back(tmpVec);

        if(name=="RedLightsHOG")
            allLightsLabels.push_back(1);
        else if(name=="GreenLightsHOG")
            allLightsLabels.push_back(2);
        else if(name=="YellowLightsHOG")
            allLightsLabels.push_back(3);
        else if(name=="RedYellowLightsHOG")
            allLightsLabels.push_back(4);

        if(positive==false)
            negLightsFeatures.push_back(tmpVec);
        cout<<"Wczytano: "<<i<<" cech"<<endl;
        }
        ++i;

    }

    if(positive)
        for(int i=0;i<hogvector.size();++i)
            labels.push_back(1);
    else
        for(int i=0;i<hogvector.size()-size;++i)
            labels.push_back(0);
    }
    cout<<"Wczytywanie zakonczone powodzeniem"<<endl;
    cout<<"Rozmiar AllLightsFeatures: "<<allLightsFeatures.size()<<endl;
    cout<<"Rozmiar AllLightsLabels: "<<allLightsLabels.size()<<endl;
    return true;
}

bool Video::readHogFromFileAuto(vector<vector<float> > &hogvector, vector<int> &labels, bool positive, QString name)
{
    int size=hogvector.size();
    QString fileName = name;
    fileName.append(".yml");
    FileStorage fs2(fileName.toStdString(), FileStorage::READ);
    if(!fs2.isOpened())
    {
        cout<<"Nie udalo sie otworzyc pliku"<<endl;
        return false;
    }
    else
    {
    fileName.chop(4);
    FileNode videoData=fs2[fileName.toStdString()];

    FileNodeIterator it = videoData.begin();
    FileNodeIterator it_end=videoData.end();

    int i=0;
    int lowBorder=0; //Wyłuskanie jedynie cech próbek negatywnych z pliku
    if(positive==false)
    {
        lowBorder=hogvector.size();
        negLightsFeaturesAuto.clear();
    }


    for(;it!=it_end;++it)
    {
        vector<float> tmpVec;
        string title = "Mat";
        string x = to_string(i);
        title+=x;
        if(i>=lowBorder)
        {
        (*it)[title] >> tmpVec;
        hogvector.push_back(tmpVec);

        if(positive==true && name!= "LightsHOGAuto")
            allLightsFeaturesAuto.push_back(tmpVec);

        if(name=="RedLightsHOGAuto")
            allLightsLabelsAuto.push_back(1);
        else if(name=="GreenLightsHOGAuto")
            allLightsLabelsAuto.push_back(2);
        else if(name=="YellowLightsHOGAuto")
            allLightsLabelsAuto.push_back(3);
        else if(name=="RedYellowLightsHOGAuto")
            allLightsLabelsAuto.push_back(4);

        if(positive==false)
            negLightsFeaturesAuto.push_back(tmpVec);
        cout<<"Wczytano: "<<i<<" cech"<<endl;
        }
        ++i;

    }

    if(positive)
        for(int i=0;i<hogvector.size();++i)
            labels.push_back(1);
    else
        for(int i=0;i<hogvector.size()-size;++i)
            labels.push_back(0);
    }
    cout<<"Wczytywanie zakonczone powodzeniem"<<endl;
    cout<<"Rozmiar AllLightsFeaturesAuto: "<<allLightsFeaturesAuto.size()<<endl;
    cout<<"Rozmiar AllLightsLabelsAuto: "<<allLightsLabelsAuto.size()<<endl;
    return true;
}

void Video::createAllLightsVector()
{
    if(allLightsFeatures.size()==0)
    {
        auto it = allLightsFeatures.end();
        allLightsFeatures.insert(it,redLightsFeatures.begin(),redLightsFeatures.end());
        it = allLightsFeatures.end();
        allLightsFeatures.insert(it,greenLightsFeatures.begin(),redLightsFeatures.end());
        it = allLightsFeatures.end();
        allLightsFeatures.insert(it,redLightsFeatures.begin(),redLightsFeatures.end());
        it = allLightsFeatures.end();
        allLightsFeatures.insert(it,redLightsFeatures.begin(),redLightsFeatures.end());
        it = allLightsFeatures.end();
        allLightsFeatures.insert(it,redLightsFeatures.begin(),redLightsFeatures.end());
    }
}

bool Video::readNegativesFromFile()
{
    QString fileName = "Negatives";
    fileName.append(".yml");
    FileStorage fs2(fileName.toStdString(), FileStorage::READ);
    if(!fs2.isOpened())
    {
        cout<<"Nie udalo sie otworzyc pliku";
        return false;
    }
    else
    {
    fileName.chop(4);
    FileNode videoData=fs2[fileName.toStdString()];
    FileNodeIterator it = videoData.begin();
    FileNodeIterator it_end=videoData.end();
    for(;it!=it_end;++it)
    {
        Point cL,cP;
        cL.x=(int)(*it)["x"];
        cL.y=(int)(*it)["y"];
        int width=(int)(*it)["width"];
        cP.x=cL.x+width;
        cP.y=cL.y+2*width;
        string colour=(string)(*it)["Color"];
        int frameNumber = (int)(*it)["Frame number"];
        Selection selectionFromFile(cL,cP,colour,frameNumber);
        negatives.push_back(selectionFromFile);
    }
    }
    if(negatives.size()==0)
    {
        return false;
    }
    return true;
}

void Video::convertVecToMat(const vector<Mat> & samples,Mat& trainData )
{
    int rows = (int) samples.size();
    int cols = (int)std::max(samples[0].cols,samples[0].rows);
    Mat tmp(1, cols, CV_32FC1); //< used for transposition if needed
    trainData = Mat(rows, cols, CV_32FC1 );
    vector< Mat >::const_iterator it = samples.begin();
    vector< Mat >::const_iterator itEnd = samples.end();
    for(int i=0; it != itEnd ; ++it, ++i )
    {
        cout<<i<<endl;

        CV_Assert( it->cols == 1 ||
            it->rows == 1 );
        if( it->cols == 1 )
        {
            transpose( *(it), tmp );
            tmp.copyTo( trainData.row( i ) );
        }
        else if( it->rows == 1 )
        {
            it->copyTo( trainData.row( i ) );
        }
    }


}

void Video::convertVecFloatsToMat(vector<vector<float>> &samples, Mat &trainData)
{
    int row=samples.size();
    int col=samples[0].size();
    Mat temporary(row,col,CV_32FC1);

    for (int i = 0; i<row; i++)
    {
        for (int j = 0; j<col; j++)
        {
            temporary.at<float>(i,j) = samples[i][j];
        }
    }
    temporary.copyTo(trainData);
}

void Video::trainSVM(vector<vector<float> > samples, vector<int> labels, Mat trainData , string name)
{
    convertVecFloatsToMat(samples,trainData);


    cout<<"Trening rozpoczety"<<endl;
    Ptr<SVM> svm = SVM::create();
    /* Default values to train SVM */
    svm->setDegree(3);
    svm->setTermCriteria(TermCriteria( CV_TERMCRIT_ITER, 100000, 0.00001 )); //Zmiana tych parametrów ma bardzo istotny wpływ!
    svm->setGamma(1);
    svm->setP(0.1);
    svm->setKernel(SVM::LINEAR);
    svm->setC(320);
    svm->setType(SVM::C_SVC); // Najlepiej stosować SVM:C_SVC
    Ptr<ml::TrainData> tData = ml::TrainData::create(trainData, ml::SampleTypes::ROW_SAMPLE, Mat(labels));
    svm->train(tData);
    cout<<"Trening ukonczony" << endl;

    name.append(".yml");
    svm->save( name );


}

Ptr<SVM> Video::loadSVM(const String name)
{
     Ptr<SVM> svm;
     svm = StatModel::load<SVM>( name );
     return svm;
}

void Video::clasify(Ptr<SVM> svm, vector<Selection> selections, QString name)
{
    int size = selections.size();
    name.chop(4);

    if(size>1500)
        size/=5;

    int positive = 0;
    for(int i=0;i<size;++i)
    {
//        cout<<"Var count "<<svm->getVarCount()<<endl;
        Selection selection = selections[i];
        Mat img, imgGray;
        setCurrentFrame(selection.getFrameNumber());
        if(!capture->read(frame))
        {
            return;
        }
        else
        {
            img=frame(selection.getSelectionStable()).clone();
            cvtColor(img,imgGray,CV_BGR2GRAY);
            resize(imgGray,imgGray,Size(20,40));

            //HOG: window size=(30,60), block size=10,block stride=5,cell size=5
            HOGDescriptor hog(Size(20,40),Size(10,10),Size(5,5),Size(5,5),9);
            vector<float> featureValues;
            vector<Point> locations;
            hog.compute(imgGray,featureValues,Size(5,5),Size(0,0),locations);

            //Color descriptor
            vector<Mat> patches = imageToPatches(img,5);
            for(int i = 0; i<patches.size();++i)
            {
                computeHistograms(patches.at(i),featureValues);
            }

            int row=1;
            int col=featureValues.size();
            Mat featureValuesMat(row,col,CV_32FC1);
            for(int i=0;i<col;++i)
            {
                featureValuesMat.at<float>(0,i)=featureValues[i];
            }

//            cout<<"Mat cols: "<<featureValuesMat.cols<<endl;
            int result = svm->predict(featureValuesMat);

            if(name=="LightsDetector" || name=="LightsDetectorAuto" || name=="RedLightsDetector" ||name=="GreenLightsDetector" ||name=="YellowLightsDetector" ||name=="RedYellowLightsDetector")
            {
                if(result==1)
                {
                    positive++;
                    cout<<"Result "<<i<<": Wykryto swiatlo"<<endl;
                }
                else
                    cout<<"Result "<<i<<": Nie wykryto swiatla"<<endl;

            }
            else if(name=="ColorDetector" || name=="ColorDetectorAuto")
            {
                cout<<"Result "<<i<<": Wykryto swiatlo ";

                if(result==1)
                {
                    cout<<"czerwone"<<endl;
                    if(selection.getColor()=="Red")
                        positive++;
                }
                else if(result==2)
                {
                    cout<<"zielone"<<endl;
                    if(selection.getColor()=="Green")
                    positive++;
                }
                else if(result==3)
                {
                    cout<<"zolte"<<endl;
                    if(selection.getColor()=="Yellow")
                    positive++;
                }
                else if(result==4)
                {
                    cout<<"czerwono-zolte"<<endl;
                    if(selection.getColor()=="RedYellow")
                    positive++;
                }
            }

        }
    }
    double percent = double(positive*100/size);
    cout<<"Skutecznosc: "<<percent<<" %"<<endl;

}

void Video::smallerVector(vector<Selection> &vec, int divide)
{
    vector<Selection> tmp;
    for(int i=0;i<vec.size();i+=divide)
    {
        tmp.push_back(vec[i]);
    }
    vec=tmp;
}

void Video::biggerVector(vector<Selection> &vec, int multiply)
{
    int size= vec.size();
    for(int i=0;i<size;++i)
    {
        for(int j=2;j<multiply+2;++j)
        {
            Selection tmp = vec[i];
            Rect toAdd = tmp.getSelectionStable();
            if(toAdd.x>multiply+3)
            {
                toAdd -= Point(j,0);
                tmp.setSelectionStable(toAdd);
                vec.push_back(tmp);
            }

            Selection tmp1 = vec[i];
            Rect toAdd1 = tmp1.getSelectionStable();
            if(toAdd1.x<(1024-(multiply+2)))
            {
                toAdd1 += Point(j,0);
                tmp.setSelectionStable(toAdd1);
                vec.push_back(tmp1);
            }
        }
    }
    cout<<vec.size()<<endl;

}

void Video::computeHistograms(Mat image, vector<float> &features)
{
    vector<Mat> bgrPlanes;
    split(image,bgrPlanes);
    int bins = 10;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true;
    bool accumulate = false;
    Mat bHist, gHist, rHist;

    calcHist( &bgrPlanes[0], 1, 0, Mat(), bHist, 1, &bins, &histRange, uniform, accumulate );
    calcHist( &bgrPlanes[1], 1, 0, Mat(), gHist, 1, &bins, &histRange, uniform, accumulate );
    calcHist( &bgrPlanes[2], 1, 0, Mat(), rHist, 1, &bins, &histRange, uniform, accumulate );

    normalize(bHist, bHist, 0, 1, NORM_MINMAX, -1, Mat() );
    normalize(gHist, gHist, 0, 1, NORM_MINMAX, -1, Mat() );
    normalize(rHist, rHist, 0, 1, NORM_MINMAX, -1, Mat() );

    vector<float> bHistFloats;
    bHistFloats.assign((float*)bHist.datastart,(float*)bHist.dataend);
    vector<float> gHistFloats;
    gHistFloats.assign((float*)gHist.datastart,(float*)gHist.dataend);
    vector<float> rHistFloats;
    rHistFloats.assign((float*)rHist.datastart,(float*)rHist.dataend);

    features.insert(features.end(),bHistFloats.begin(),bHistFloats.end());
    features.insert(features.end(),gHistFloats.begin(),gHistFloats.end());
    features.insert(features.end(),rHistFloats.begin(),rHistFloats.end());

//    for(int i=0;i<gHistFloats.size();++i)
//    {
//        cout<< gHistFloats.at(i)<< " ";
//    }
//    cout<<endl;
//    imshow("patch",image);
//    waitKey(0);


    bHist.release();
    gHist.release();
    rHist.release();
    bgrPlanes.clear();
    bgrPlanes.shrink_to_fit();
    bHistFloats.clear();
    bHistFloats.shrink_to_fit();
    gHistFloats.clear();
    gHistFloats.shrink_to_fit();
    rHistFloats.clear();
    rHistFloats.shrink_to_fit();

}

void Video::sel2Mat(Selection selection)
{
    Mat img;

    setCurrentFrame(selection.getFrameNumber());

    if(!capture->read(frame))
    {
        ;
    }
    else
    {
        img = frame(selection.getSelectionStable());
    }
//    computeHistograms(img);


}

vector<Mat> Video::imageToPatches(Mat image, int patchSize)
{
    resize(image,image,Size(20,40));
    vector<Mat> patches;
    int width = image.cols;
    int height= image.rows;

    for(int i = 0; i<height/patchSize; ++i)
    {
        for(int j=0;j<width/patchSize;++j)
        {
            Mat patch = image(Rect(j*patchSize,i*patchSize,patchSize,patchSize)).clone();
            patches.push_back(patch);
            patch.release();
        }
    }
    image.release();

    return patches;

}


void Video::setCurrentFrame(int value)
{
    capture->set(CV_CAP_PROP_POS_FRAMES,value);
}

void Video::addSelectionToVector(Selection selectionToAdd)
{
    Selection slc=selectionToAdd;
    selectionVector.push_back(selectionToAdd);

    if(selectionToAdd.getColor()=="Red")
        redLights.push_back(slc);
    else if(selectionToAdd.getColor()=="Green")
        greenLights.push_back(slc);
    else if(selectionToAdd.getColor()=="Yellow")
        yellowLights.push_back(slc);
    else if(selectionToAdd.getColor()=="RedYellow")
        redYellowLights.push_back(slc);
}

QString Video::getFileNameVideo() const
{
    return fileNameVideo;
}

void Video::setFileNameVideo(const QString &value)
{
    fileNameVideo = value;
}

QString Video::getFileNameYaml() const
{
    return fileNameYaml;
}

void Video::setFileNameYaml(const QString &value)
{
    fileNameYaml = value;
}

vector<Selection> Video::getSelectionVector() const
{
    return selectionVector;
}

int Video::getNumberOfFrames()
{
    return capture->get(CV_CAP_PROP_FRAME_COUNT);
}

int Video::getCurrentFrame()
{
    return capture->get(CV_CAP_PROP_POS_FRAMES);
}

vector<Selection> Video::getNegatives() const
{
    return negatives;
}

