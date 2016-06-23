#include "GPS.h"
#include <stdio.h>
#include <fstream>
#include <math.h>

#define EARTH_RADIUS        6378137.0               ///< Earth radius (unit: m)
#define SQR(x) ((x)*(x))

#define DEG2RAD              0.017453292519943      ///< degree to radian
#define RAD2DEG             57.295779513082323      ///< radian to degree

#define _PI                  3.14159265
using namespace std;

namespace pi {

double GPSData::lat0=0;
double GPSData::lng0=0;
double GPSData::alt0=0;
bool   GPSData::preciseConvert=false;

/// \param lng1     - longitude of point 1
/// \param lat1     - latitude of point 1
/// \param lng2     - longitude of point 2
/// \param lat2     - latitude of point 2
/// \param dx       - distance in x-axis
/// \param dy       - distance in y-axis
/// \param method   - calculation method (0, 1)
///
/// \ref http://en.wikipedia.org/wiki/Latitude
///
bool GPSData::getXYfromLngLat(double lng2, double lat2,// input
                                double &dx, double &dy,  // output
                                double lng1, double lat1)//ref
{
    double        lng_unit, lat_unit, phi_rad;
    const double  a = DEG2RAD *EARTH_RADIUS;

    phi_rad = (lat1+lat2)*0.5 * DEG2RAD;

    // longitude & latitude unit
    if(preciseConvert)
    {
        const double f = 1.0/298.257223563;
        double      e_2 = (2 - f)*f;
        lng_unit =  a * cos(phi_rad) / sqrt(1 - e_2*SQR(sin(phi_rad)));
        lat_unit =  a * (1-e_2) / pow(1-e_2*SQR(sin(phi_rad)), 1.5);
    }
    else
    {
        lng_unit =  a * cos(phi_rad);
        lat_unit =  a;
    }

    // calculate dx, dy
    dx = (lng2 - lng1) * lng_unit;
    dy = (lat2 - lat1) * lat_unit;
    return true;
}

bool GPSData::getLngLatfromXY(double dx, double dy,       // input
                                double &lng2, double &lat2, // output
                                double lng1, double lat1) //ref

{
    double        lng_unit, lat_unit, phi_rad;
    const double  a = DEG2RAD *EARTH_RADIUS;

    phi_rad = (lat1+lat2)*0.5 * DEG2RAD;

    // longitude & latitude unit
    if(preciseConvert)
    {
        const double f = 1.0/298.257223563;
        double      e_2 = (2 - f)*f;
        lng_unit =  a * cos(phi_rad) / sqrt(1 - e_2*SQR(sin(phi_rad)));
        lat_unit =  a * (1-e_2) / pow(1-e_2*SQR(sin(phi_rad)), 1.5);
    }
    else
    {
        lng_unit =  a * cos(phi_rad);
        lat_unit =  a;
    }

    // calculate lng2, lat2
    lng2=dx/lng_unit+lng1;
    lat2=dy/lat_unit+lat1;
    return 0;
}


void  GPSData::setOrigin(const pi::Point3d& origin)
{
    lng0=origin.x;
    lat0=origin.y;
    alt0=origin.z;
}

Point3d GPSData::getXYZ()
{
    pi::Point3d xyz;
    getXYfromLngLat(lng,lat,xyz.x,xyz.y);
    xyz.z=alt-alt0;
    return xyz;
}


GPSData GPSData::fromXYZ(const pi::Point3d& pt)
{
    GPSData result;
    getLngLatfromXY(pt.x,pt.y,result.lng,result.lat);
    result.alt=pt.z+alt0;
    return result;
}

bool GPS::save(const std::string& filename)
{
    ofstream ofs(filename.c_str());
    if(!ofs.is_open())
    {
        std::cerr<<"Can't open file "<<filename<<endl;
        return false;
    }
    std::vector<GPSData> data;
    if(getArray(data))
    {
        size_t size=data.size();
        ofs.write((char*)&size,sizeof(size_t));
        ofs.write((char*)data.data(),data.size()*sizeof(GPSData));
        return true;
    }
    return  false;
}

bool GPSArray::insert(const GPSData& gpsData)
{
    if(gpsData.timestamp>data.back().timestamp);
    data.push_back(gpsData);
    return true;
}

void GPSArray::getTimeRange(ri64& minTime,ri64& maxTime)
{
    if(!data.size())
    {
        minTime=-1;
        maxTime=-1;
    }
    else
    {
        minTime=data.front().timestamp;
        maxTime=data.back().timestamp;
    }
}

bool GPSArray::atTime(GPSData& gpsData,const ri64& time,bool nearist)
{
    if(!data.size()) return false;

    if(time<data.front().timestamp||time>=data.back().timestamp||data.size()<2)
    {
        gpsData=data.back();
        return true;
    }

    int idxMin=0,idxMax=data.size()-2;


    while(idxMax-idxMin>16)// fast up with idx approciate
    {
        int idx=idxMin+(idxMax-idxMin)*(time-data[idxMin].timestamp)/(data[idxMax].timestamp-data[idxMin].timestamp);
        if(data[idx].timestamp>=time) idxMax=idx;
        else idxMin=idx;
    }

    while(idxMax-idxMin>1)
    {
        int idx=(idxMax+idxMin)>>1;
        if(data[idx].timestamp>time) idxMax=idx;
        else idxMin=idx;
    }


    if(nearist)
    {
        ri64 timeDiffLow =time-data[idxMin].timestamp;
        ri64 timeDiffHigh=data[idxMax].timestamp-time;
        if(timeDiffLow<timeDiffHigh)
        {
            if(timeDiffLow>1e6) return false;
            //less than one second
            gpsData=data[idxMin];
            return true;
        }
        else
        {
            if(timeDiffHigh>1e6) return false;
            gpsData=data[idxMax];
        }
    }
    else
    {
        GPSData& low=data[idxMin];
        GPSData& up =data[idxMax];
        ri64 timeDiffAll=up.timestamp-low.timestamp;
        if(timeDiffAll>1e6) return false;
        double kLow=((double)(time-low.timestamp))/timeDiffAll;
        double kUp =1.-kLow;
        gpsData=GPSData(low.lat*kLow+up.lat*kUp,low.lng*kLow+up.lng*kUp,
                        low.alt*kLow+up.alt*kUp,time,(low.HDOP+up.HDOP)*0.5,
                        (low.nSat+up.nSat)>>1,low.fixQuality);
        return true;
    }

    return true;
}

bool GPSArray::load(std::string filename)
{
    ifstream ifs(filename.c_str());
    if(!ifs.is_open())
    {
        std::cerr<<"Can't open file "<<filename<<endl;
        return false;
    }
    size_t size=0;
    ifs.read((char*)&size,sizeof(size_t));
    if(size>1e5)
    {
        cerr<<"Size too large!\n";
        return false;
    }
    data.resize(size);
    ifs.read((char*)data.data(),data.size()*sizeof(GPSData));
    return true;
}

}
