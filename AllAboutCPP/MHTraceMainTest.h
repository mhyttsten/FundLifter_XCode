
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>



 #include <stdio.h>
 #include <iostream>

 void fooh1();
 void fooh2();
 void fooh3();
 void fooh1() {
char mhtracer_s[200];
char mhtracer_fn[40];
char* env_path = getenv("PATH");
if (!strstr(env_path, "MHTRACER_DISABLE")) {
   sprintf(mhtracer_s, "fooh1  [C-Style: 20 @ MHTraceMainTest.h\n");
   if (env_path && strstr(env_path, "MHTRACER_USEFILE")) {
      sprintf(mhtracer_fn, "/tmp/mhtracer_%d.log", getpid());
      FILE* mhtracer_fp = fopen(mhtracer_fn, "a");
      fprintf(mhtracer_fp, "%s", mhtracer_s);
      fclose(mhtracer_fp);
   } else {
      printf("%s", mhtracer_s);
   }
}


   fooh2();
 }
 void fooh2() {
char mhtracer_s[200];
char mhtracer_fn[40];
char* env_path = getenv("PATH");
if (!strstr(env_path, "MHTRACER_DISABLE")) {
   sprintf(mhtracer_s, "fooh2  [C-Style: 39 @ MHTraceMainTest.h\n");
   if (env_path && strstr(env_path, "MHTRACER_USEFILE")) {
      sprintf(mhtracer_fn, "/tmp/mhtracer_%d.log", getpid());
      FILE* mhtracer_fp = fopen(mhtracer_fn, "a");
      fprintf(mhtracer_fp, "%s", mhtracer_s);
      fclose(mhtracer_fp);
   } else {
      printf("%s", mhtracer_s);
   }
}


   fooh3();
 }
 void fooh3() {
char mhtracer_s[200];
char mhtracer_fn[40];
char* env_path = getenv("PATH");
if (!strstr(env_path, "MHTRACER_DISABLE")) {
   sprintf(mhtracer_s, "fooh3  [C-Style: 58 @ MHTraceMainTest.h\n");
   if (env_path && strstr(env_path, "MHTRACER_USEFILE")) {
      sprintf(mhtracer_fn, "/tmp/mhtracer_%d.log", getpid());
      FILE* mhtracer_fp = fopen(mhtracer_fn, "a");
      fprintf(mhtracer_fp, "%s", mhtracer_s);
      fclose(mhtracer_fp);
   } else {
      printf("%s", mhtracer_s);
   }
}


   std::cout << "Hello from fooh3\n";
 }



/*
 #include <stdio.h>
 #include <iostream>

 void fooh1();
 void fooh2();
 void fooh3();
 void fooh1() {
   fooh2();
 }
 void fooh2() {
   fooh3();
 }
 void fooh3() {
   std::cout << "Hello from fooh3\n";
 }
 */
