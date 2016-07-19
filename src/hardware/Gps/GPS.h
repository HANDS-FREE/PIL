#ifndef PIL_GPS_H
#define PIL_GPS_H
#include <vector>
#include <base/Types/Int.h>
#include <base/Types/Point.h>
#include <base/Utils/utils_str.h>
#include <iomanip>

namespace pi{

struct GPSData
{
    GPSData(const double& longitude=0,const double& latitude=0,const double& altitude=0,
            const ri64&   time=-1,const double& hdop=-1,ri32 nsat=0,ri32 fixed=0)
        :lng(longitude),lat(latitude),alt(altitude),timestamp(time),
          HDOP(hdop),nSat(nsat),fixQuality(fixed){}

    double              lng;        ///< longitude, degree
    double              lat;        ///< latitude, degree
    double              alt;        ///< altitude, meters above mean sea level
    ri64          timestamp;        ///< timestamp at utc

    double              HDOP;       ///< Horizontal Dilution of Precision (HDOP),
                                    ///< Relative accuracy of horizontal position
    ri32                nSat;       ///< Number of Satellites in view

    ri32                fixQuality; ///< Data is from a GPS fix
                                    ///< 0 = Invalid, 1 = GPS fix, 2 = DGPS fix

    Point3d     getXYZ();

    static void    setOrigin(const pi::Point3d& origin);

    static GPSData fromXYZ(const pi::Point3d& pt);

    static bool getXYfromLngLat(double lng2, double lat2,// input
                                double &dx, double &dy,  // output
                                double lng1=lng0, double lat1=lat0);//ref

    static bool getLngLatfromXY(double dx, double dy,       // input
                                double &lng2, double &lat2, // output
                                double lng1=lng0, double lat1=lat0); //ref

    friend inline std::ostream& operator <<(std::ostream& os,const GPSData& p)
    {
        os<<p.timestamp<<" "<<dtos(p.lat)<<" "<<dtos(p.lng)
         <<" "<<dtos(p.alt)<<" "<<dtos(p.HDOP)
        <<" "<<p.nSat<<" "<<p.fixQuality;
        return os;
    }

    friend inline std::istream& operator >>(std::istream& is,GPSData& p)
    {
        is>>p.timestamp>>p.lat>>p.lng>>p.alt
         >>p.HDOP>>p.nSat>>p.fixQuality;
        return is;
    }

    static double lng0,lat0,alt0;
    static bool   preciseConvert;
};

class GPS
{
public:
    GPS(const std::string& nameGPS="GPS"):name(nameGPS){}
    virtual ~GPS(){}

    virtual bool insert(const GPSData& gpsData){return false;}

    virtual size_t size(){return 0;}

    virtual bool save(const std::string& filename);

    virtual bool atTime(GPSData& gpsData,const ri64& time=-1,bool nearist=true){return false;}

    virtual bool getArray(std::vector<GPSData>& gpsArray){return false;} /// return all data

    virtual void getTimeRange(ri64& minTime,ri64& maxTime){minTime=-1;maxTime=-1;}

    inline bool  hasTime(const ri64& time){
        ri64 minTime,maxTime;
        getTimeRange(minTime,maxTime);
        return time>=minTime&&time<=maxTime;
    }

    std::string  name;
};

class GPSArray:public GPS
{
public:
    GPSArray(const std::string& nameGPS="GPS"):GPS(nameGPS){}

    virtual bool load(std::string filename);

    virtual bool insert(const GPSData& gpsData);

    virtual size_t size(){return data.size();}

    virtual bool atTime(GPSData& gpsData,const ri64& time=-1,bool nearist=true);

    virtual bool getArray(std::vector<GPSData>& gpsArray){gpsArray=data;return true;} /// return all data

    virtual void getTimeRange(ri64& minTime,ri64& maxTime);

    std::vector<GPSData> data;
};


}// namespace pi
#endif
