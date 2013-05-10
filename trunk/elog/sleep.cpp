#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "sleep.h"

namespace elog{

int sleep_ms(int ms){
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;   
    
    return select(0, NULL, NULL, NULL, &tv);
}

}
