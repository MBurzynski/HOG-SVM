#include "selection.h"

Selection::Selection()
{

}

Selection::Selection(Point crnLeft, Point crnRight, string clr, int frmNumber)
{
    cornerLeft=crnLeft;
    cornerRight=crnRight;
    color=clr;
    status=2;
    selectionStable=(Rect(crnLeft,crnRight));
    frameNumber=frmNumber;

}

Rect Selection::getSelectionStable() const
{
    return selectionStable;
}

void Selection::setSelectionStable(const Rect &value)
{
    selectionStable = value;
}

int Selection::getStatus() const
{
    return status;
}

void Selection::setStatus(int value)
{
    status = value;
}

Point Selection::getCornerRight() const
{
    return cornerRight;
}

void Selection::setCornerRight(const Point &value)
{
    cornerRight = value;
}

Point Selection::getCornerLeft() const
{
    return cornerLeft;
}

void Selection::setCornerLeft(const Point &value)
{
    cornerLeft = value;
}

string Selection::getColor() const
{
    return color;
}

void Selection::setColor(const string &value)
{
    color = value;
}

int Selection::getFrameNumber() const
{
    return frameNumber;
}

void Selection::setFrameNumber(int value)
{
    frameNumber = value;
}

