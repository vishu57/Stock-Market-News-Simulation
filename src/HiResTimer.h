/** File "HiResTimer2.h", by KWR for CSE250, Fall 2009.  Still "mutable"?!?
    Wraps a class around basic timing functions, clarifying which times
    are clock-reads and which are *durations*, i.e. differences between reads.
    Assumes 64-bit longs (or higher), may break on (old) Windows setups
    that provide only 32-bit longs.

    IMPT: Any client that #includes this must use a trailing -lrt in g++1

    Not a "struct" because its fields are private; not quite "POD" either.
 */

#ifndef HI_RES_TIMER_H_
#define HI_RES_TIMER_H_

#include <vector>
#include <string>
//#include <sys/time.h>  
#include <ctime>
using std::string;


class HiResTimer {
    clockid_t clockID;
    double nanoDivider;
    struct timespec timestamp;            //standard calls require & in front
    struct timespec prevStamp;            //INV: == *timestamp upon update
    struct timespec newInterval;          //time *difference* not stamp
    struct timespec intervalSinceReset;

    void updateDurations() {
        clock_gettime(clockID,&timestamp);//deliberately first line of code
        const long nanoDiff = timestamp.tv_nsec - prevStamp.tv_nsec;
        const time_t secondsDiff = timestamp.tv_sec - prevStamp.tv_sec;
        prevStamp = timestamp;            //restore INV as soon as possible
        if (nanoDiff < 0) {  //nano-reader wrapped around signed long boundary
            newInterval.tv_nsec = 1000000000 + nanoDiff;
            newInterval.tv_sec = secondsDiff - 1;
        } else {
            newInterval.tv_nsec = nanoDiff;
            newInterval.tv_sec = secondsDiff;
        }
        intervalSinceReset.tv_nsec += newInterval.tv_nsec;
        intervalSinceReset.tv_sec += newInterval.tv_sec;
        // FYI:
        // The "Stopwatch" class maintained a timestamp of last reset
        // because these repeated adds can increase roundoff errors,
        // but with nanosecond precision now those matter much less.
    }

    long newIntervalInNanoseconds() const {
        return newInterval.tv_nsec + 1000000000*newInterval.tv_sec;
    }

    long sinceResetInNanoseconds() const {
        return intervalSinceReset.tv_nsec 
           + 1000000000*intervalSinceReset.tv_sec;
    }

  public:
    enum Units { SECONDS, MILLISECONDS, MICROSECONDS, NANOSECONDS };
    /** Default units are microseconds
     */
    HiResTimer()
      : clockID(CLOCK_PROCESS_CPUTIME_ID)
      , nanoDivider(1000.0)
      , timestamp(timespec())    //remember to eradicate "new" for value type!
      , prevStamp(timespec())    //overwritten
      , newInterval(timespec())  //meaningful
      , intervalSinceReset(timespec())    //meaningful
    {
        clock_gettime(clockID, &timestamp);
        prevStamp = timestamp;     //so INV holds
    }

    HiResTimer(clockid_t clockID, Units units)
      : clockID(clockID)
      , nanoDivider(units == SECONDS ? 1000000000.0
                        : (units == MILLISECONDS ? 1000000.0
                            : (units == NANOSECONDS ? 1.0 : 1000.0) ) ) 
      , timestamp(timespec())    //default constructor puts 0s
      , prevStamp(timestamp)     //order matters!  Legal and convenient
      , newInterval(timespec())  //meaningful
      , intervalSinceReset(timespec())    //meaningful
    {
        clock_gettime(clockID, &timestamp);
        prevStamp = timestamp;     //so INV holds
    }

    void reset() {
        newInterval = intervalSinceReset = timespec();
        clock_gettime(clockID, &timestamp);
        prevStamp = timestamp;
    }
        
    double elapsedTime() {  //NOT CONST! Returns time since last call or reset
        updateDurations();
        return newIntervalInNanoseconds() / nanoDivider;
    }

    double timeSinceReset() {
        updateDurations();
        return sinceResetInNanoseconds() / nanoDivider;
    }

    string getUnits() const {
        if (nanoDivider > 999999999.9) {
            return "seconds";
        } else if (nanoDivider > 999999.9) {
            return "milliseconds";
        } else if (nanoDivider > 999.9) {
            return "microseconds";
        } else {
            return "nanoseconds!";
        }
    }

      
};


#endif

