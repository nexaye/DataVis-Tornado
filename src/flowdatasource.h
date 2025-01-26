#ifndef FLOWDATASOURCE_H
#define FLOWDATASOURCE_H

class FlowDataSource
{
public:
    FlowDataSource();
    FlowDataSource(int time);
    int createData(int time);
    float getDataValue(int iz, int iy, int ix, int ic);
    void printValuesOfHorizontalSlice(int iz);
    float getAbsoluteValue(int iz, int iy, int ix);
    int getTime();
    float maxMag;
    float minMag;

    float maxAll;
    float minAll;
private:
    static void generateTornado(int time, float *grid);
    void getMaxMinValues();
    int time;
    float cartesianDataGrid[98304];
};
#endif //FLOWDATASOURCE_H
