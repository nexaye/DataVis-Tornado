#include "flowdatasource.h"
#include "iostream"
#include "math.h"

FlowDataSource::FlowDataSource() : time(0), maxAll(0), minAll(0), maxMag(0),minMag(0)
{
    createData(0);
}
FlowDataSource::FlowDataSource(int time) : time(time), maxAll(0), minAll(0), maxMag(0),minMag(0)
{
    createData(time);
}
int FlowDataSource::createData(int time)
{
    generateTornado(time,cartesianDataGrid);
    this->time = time;
    getMaxMinValues();
    return time;
}
//TODO make adjustable grid size
void FlowDataSource::generateTornado(int time, float *grid)
{
    float x, y, z;
     int ix, iy, iz;
     int xs= 32, ys = 32, zs = 32;
     float r, xc, yc, scale, temp, z0;
     float r2 = 8;
     float SMALL = 0.00000000001;
     float xdelta = 1.0 / (xs-1.0);
     float ydelta = 1.0 / (ys-1.0);
     float zdelta = 1.0 / (zs-1.0);

     for( iz = 0; iz < zs; iz++ )
     {
        z = iz * zdelta;                        // map z to 0->1
        xc = 0.5 + 0.1*sin(0.04*time+10.0*z);   // For each z-slice, determine the spiral circle.
        yc = 0.5 + 0.1*cos(0.03*time+3.0*z);    //    (xc,yc) determine the center of the circle.
        r = 0.1 + 0.4 * z*z + 0.1 * z * sin(8.0*z); //  The radius also changes at each z-slice.
        r2 = 0.2 + 0.1*z;                           //    r is the center radius, r2 is for damping
        for( iy = 0; iy < ys; iy++ )
        {
           y = iy * ydelta;
           for( ix = 0; ix < xs; ix++ )
           {
               x = ix * xdelta;
               temp = sqrt( (y-yc)*(y-yc) + (x-xc)*(x-xc) );
               scale = fabs( r - temp );

              if ( scale > r2 )
                 scale = 0.8 - scale;
              else
                 scale = 1.0;
               z0 = 0.1 * (0.1 - temp*z );
              if ( z0 < 0.0 )  z0 = 0.0;
              temp = sqrt( temp*temp + z0*z0 );
               scale = (r + r2 - temp) * scale / (temp + SMALL);
               scale = scale / (1+z);
              *grid++ = scale * (y-yc) + 0.1*(x-xc);
              *grid++ = scale * -(x-xc) + 0.1*(y-yc);
              *grid++ = scale * z0;
           }
        }
     }
}

float FlowDataSource::getDataValue(int iz, int iy, int ix, int ic)
{
    if(ic == 3) return getAbsoluteValue(iz,iy,ix);
    return cartesianDataGrid[(iz*1024+iy*32+ix)*3 + ic];

}

void FlowDataSource::printValuesOfHorizontalSlice(int iz)
{
    std::cout << "Horizontal Slice for z = " << iz << "\n";
    std::cout << "----------------------------------------------------------------------" << "\n";
    std::cout << "----------------------------------------------------------------------" << "\n";
    for(int y = 0; y < 16;y++)
    {
        for(int x = 0; x< 16; x++){
            std::cout << "iZ = " << iz << "; iY = " << y << "; iX = " << x << ": ";
            std::cout << "(" << getDataValue(iz,y,x, 0) << ", " << getDataValue(iz, y, x, 1) << ", " << getDataValue(iz,y,x,2) << ")" << "\n";

        }
    }
}

float FlowDataSource::getAbsoluteValue(int iz, int iy, int ix)
{
    float t1 = getDataValue(iz,iy,ix,0), t2 = getDataValue(iz,iy,ix,1), t3 = getDataValue(iz,iy,ix,2);
    return sqrtf(powf(t1,2) + powf(t2,2) + powf(t3,2));
}

int FlowDataSource::getTime() {
    return this->time;
}

void FlowDataSource::getMaxMinValues() {
    minAll = 0;
    maxAll = 0;
    minMag = 0;
    maxMag = 0;
    for(int x = 0; x< 32; x++)
    {
        for(int y = 0; y< 32; y++)
        {
            for(int z = 0; z< 32; z++){
                float X = getDataValue(z,y,x,0);
                float Y = getDataValue(z,y,x,1);
                float Z = getDataValue(z,y,x,2);
                float MAG = sqrtf(powf(X,2) + powf(Y,2) + powf(Z,2));

                if(X > maxAll) maxAll = X;
                if(X < minAll) minAll = X;
                if(Y > maxAll) maxAll = Y;
                if(Y < minAll) minAll = Y;
                if(Z > maxAll) maxAll = Z;
                if(Z < minAll) minAll = Z;
                if(MAG > maxMag) maxMag = MAG;
                minMag = -0.3;

            }
        }
    }
}
