#ifndef ENUM_H
#define ENUM_H

#include <QRgb>

enum Color { X, Y, Z, MAG};
enum streamLineMethod {EULER, KUTTA2, KUTTA4};
enum editModes{IS, SL};
struct Point
{
    float x;
    float y;
    float z;
    float value;
    Point(float x, float y, float z, float value)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->value = value;
    }

};

struct PointWind
{
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
    PointWind(float x, float y, float z, float vx, float vy, float vz)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->vx = vx;
        this->vy = vy;
        this->vz = vz;
    }
};

struct stateControl
{
    int streamLineAmount;
    float stepSize;
    int method;
    bool editMode;
    int editModes;
    bool sLine;
    int currentlySelectedIsoLine;
    int currentlySelectedSLOption;

    QString comp[4] ={
            "X",
            "Y",
            "Z",
            "MAG"
    };
    QString meth[3] ={
            "EULER",
            "KUTTA2",
            "KUTTA4"
    };
    QVector<float> isoLines;
    stateControl(): streamLineAmount(0), stepSize(0.5), method(EULER), editMode(false), sLine(false),currentlySelectedIsoLine(-1),editModes(IS),currentlySelectedSLOption(-1){
    }

    void addIsoline(){
        if(isoLines.size() < 5)return
        isoLines.append(0);
    }
    void removeIsoLine(){
        if(isoLines.empty() || currentlySelectedIsoLine == -1){
            return;
        }
        isoLines.remove(currentlySelectedIsoLine);
        currentlySelectedIsoLine--;
    }
    void addStreamLine(int i){
        if(streamLineAmount + i < 0)return;
        streamLineAmount += i;
    }
    void editStepSize(float i)
    {
        if(stepSize + i < 0.1)return;
        stepSize += i;
    }
    void changeOption(int i)
    {
        if(currentlySelectedSLOption == -1) {
            currentlySelectedSLOption = 0;
            return;
        }
        if(currentlySelectedSLOption + i < 0) currentlySelectedSLOption = 2;
        else currentlySelectedSLOption = (currentlySelectedSLOption + i) % 3;
    }
    void changeIntegration(int i)
    {
        if(method + i < 0) method = 2;
        else method = (method + i) % 3;
    }
    void doThing(int i, float step, int integration)
    {
        switch(currentlySelectedSLOption)
        {
            case 0:
                addStreamLine(i);
                break;
            case 1:
                editStepSize(step);
                break;
            case 2:
                changeIntegration(integration);
                break;
            default:
                break;
        }
    }

    void toggleEditMode(){
        if(editMode){
            currentlySelectedIsoLine = -1;
            currentlySelectedSLOption = -1;
            editModes = IS;
            editMode = false;
        }
        else{
            editMode = true;
        }

    }
    void selectNextIsoLine(int i){
        if(isoLines.empty())return;
        if(currentlySelectedIsoLine == -1 && !isoLines.empty()) currentlySelectedIsoLine = 0;
        else if(currentlySelectedIsoLine + i < 0) currentlySelectedIsoLine = isoLines.size()-1;
        else currentlySelectedIsoLine = (currentlySelectedIsoLine + i) % isoLines.size();
    }
    void editIsoLineValue(float amount){
        if(currentlySelectedIsoLine == -1 || isoLines.empty())return;
        isoLines.replace(currentlySelectedIsoLine, isoLines.at(currentlySelectedIsoLine) + amount);
    }
    void toggleEditModes()
    {
        editModes = (editModes + 1) % 2;
        currentlySelectedIsoLine = -1;
        currentlySelectedSLOption = -1;
    }
};



#endif //ENUM_H