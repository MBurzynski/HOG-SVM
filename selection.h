#ifndef SELECTION_H
#define SELECTION_H

#include "opencv/cv.h"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

class Selection
{
public:
    Selection();
    Selection(Point crnLeft, Point crnRight, string clr, int frmNumber);

    Point getLpm() const;
    void setLpm(const Point &value);

    Rect getSelectionTemporary() const;
    void setSelectionTemporary(const Rect &value);

    Rect getSelectionStable() const;
    void setSelectionStable(const Rect &value);

    int getStatus() const;
    void setStatus(int value);

    Point getCornerRight() const;
    void setCornerRight(const Point &value);

    Point getCornerLeft() const;
    void setCornerLeft(const Point &value);

    string getColor() const;
    void setColor(const string &value);

    int getFrameNumber() const;
    void setFrameNumber(int value);

private:

    Rect selectionStable;
    // Status zaznaczenia (0 - brak zaznaczenia, 1 - zaznaczenie tymczasowe, 2 - zaznaczenie końcowe)
    int status = 0;
    // Kolor światła
    string color;
    // Numer klatki
    int frameNumber;

    Point cornerLeft;
    Point cornerRight;

};

#endif // SELECTION_H
