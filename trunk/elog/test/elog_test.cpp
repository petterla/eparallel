#include "elog.h"
#include "sleep.h"
#include "base64.h"
#include <unistd.h>

bool g_run = true;

void* test_thread(void* p){

   std::string s1 = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
       
   std::string s;
   for(int cnt = 0; cnt < 100; ++cnt){
       s += s1;
   }

   while(g_run){
       char c = 'c';
       int i = 10;
       long l = 1234567890;
       long long ll = 1234567890123456;
       double d = 12345.6789;
       void* p = &d;
    
       elog::elog_debug("file_logger") << "file test line:"
            << "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll
            << ",d" << d << ",p:" << p << ",s:" << s;

       elog::elog_debug("bin_logger") << s;
   }
   return 0; 
}

int main(){
   std::string src = "abcefgh";
   std::string enc = elog::base64_encode(src);
   std::string dec = elog::base64_decode(enc);
   std::cout << src << ",base64_encode ret:" << enc 
             << ", base64_decode ret:" << dec << std::endl;
   src = "a";
   enc = elog::base64_encode(src);
   dec = elog::base64_decode(enc);
   std::cout << src << ",base64_encode ret:" << enc
             << ", base64_decode ret:" << dec << std::endl;

   src = "abc";
   enc = elog::base64_encode(src);
   dec = elog::base64_decode(enc);
   std::cout << src << ",base64_encode ret:" << enc
             << ", base64_decode ret:" << dec << std::endl;
   elog::elog_init("log.conf");
   char c = 'c';
   int i = 10;
   long l = 1234567890;
   long long ll = 1234567890123456;
   double d = 12345.6789;
   void* p = &d;
               
   std::string s1 = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
       
   std::string s;
   for(int cnt = 0; cnt < 100; ++cnt){
       s += s1;
   }

   std::cout << "len(s):" << s.size() << std::endl;
   elog::elog_debug("console_logger") << "console test line:"
             << "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll 
             << ",d" << d << ",p:" << p << ",s:" << s;

   elog::elog_debug("file_logger") << "file test line:"
            << "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll 
            << ",d" << d << ",p:" << p << ",s:" << s;

   elog::elog_debug("bin_logger") << s;

   elog::elog_debug("use_default_logger") << "file test line:"
            << "c:" << c << ",i:" << i << ",l:" << l << ",ll:" << ll 
            << ",d" << d << ",p:" << p << ",s:" << s;


   elog::elog_debug("console_logger") << "sleep_ms(" << 3600 << ")";
   elog::sleep_ms(3600);
   elog::elog_debug("console_logger") << "sleep_ms(" << 3600 << ") finish";
   const int pcnt = 2;
   pthread_t pids[pcnt];
   for(int i = 0; i < pcnt; ++i){
       pthread_create(&pids[i], NULL, test_thread, NULL); 
   }   
   
   sleep(2);
   elog::elog_reload();
   sleep(2);
   g_run = false;
   for(int j = 0; j < pcnt; ++j){
       void* rt;
       pthread_join(pids[j], &rt);
   }   

   elog::elog_uninit();
   return 0;   
}
