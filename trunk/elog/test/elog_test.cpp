#include "elog.h"
#include <unistd.h>

bool g_run = true;

void* test_thread(void* p){

   while(g_run){
       elog::log& logs = *(elog::log*)p;
       char c = 'c';
       int i = 10;
       long l = 1234567890;
       long long ll = 1234567890123456;
       double d = 12345.6789;
       void* p = &d;
       std::string s = " haha,test s";

       logs("file_logger", elog::LOG_LEVEL_ALL) << "file test line:"
            << "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll
            << ",d" << d << ",p:" << p << ",s:" << s;
   }
   return 0; 
}

int main(){
   std::stringstream* os = new std::stringstream;
   *os << 1;
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
             << "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll 
             << ",d" << d << ",p:" << p << ",s:" << s;

   logs("file_logger", elog::LOG_LEVEL_ALL) << "file test line:"
            << "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll 
            << ",d" << d << ",p:" << p << ",s:" << s;

   const int pcnt = 20;
   pthread_t pids[pcnt];
   for(int i = 0; i < pcnt; ++i){
       pthread_create(&pids[i], NULL, test_thread, &logs); 
   }   
   
   sleep(5);
   logs.reload();
   sleep(5);
   g_run = false;
   for(int j = 0; j < pcnt; ++j){
       void* rt;
       pthread_join(pids[j], &rt);
   }   

   logs.uninit();
   return 0;   
}
