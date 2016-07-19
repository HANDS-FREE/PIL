#ifndef SYSTEM_H
#define SYSTEM_H
#include "VideoRef.h"
#include <base/Thread/Thread.h>

class System : public pi::Thread
{
public:
    System();
    ~System();
    void run();
    bool trainVideoRef(VideoRef& refVideo,const std::string& videoPath);
    bool findVideoDiff(VideoRef& refVideo,const std::string& videoPath);

    SPtr<VideoRef> videoRef;
};

#endif // SYSTEM_H
