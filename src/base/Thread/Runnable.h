#ifndef RUNNABLE_H
#define RUNNABLE_H
#include <iostream>

namespace pi {

class Runnable
{
 public:
  //! Perform the function of this object.
   virtual void run(){std::cerr<<"pure virtual function Runnable::run called!\n";}
   virtual ~Runnable(){}
};

}//namespace pi
#endif // RUNNABLE_H
