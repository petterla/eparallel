#include "elog.h"

int main(){
   elog::log logs;
   logs.init("log.conf");
   char c = 'c';
   int i = 10;
   long l = 1234567890;
   long long ll = 1234567890123456;
   double d = 12345.6789;
   void* p = &d;
   std::string s = " haha,test s";
   logs("console_logger", elog::LOG_LEVEL_ALL) << "console test line:"
            "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll <<
             ",d" << d << ",p:" << p << ",s:" << s;

   logs("file_logger", elog::LOG_LEVEL_ALL) << "console test line:"
            "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll <<
            ",d" << d << ",p:" << p << ",s:" << s;

   logs.uninit();
   return 0;   
}
