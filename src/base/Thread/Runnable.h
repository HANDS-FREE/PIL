#ifndef RUNNABLE_H
#define RUNNABLE_H

namespace pi {

class Runnable
{
 public:
  //! Perform the function of this object.
   virtual void run()=0;
   virtual ~Runnable(){}
};

}//namespace pi
#endif // RUNNABLE_H
