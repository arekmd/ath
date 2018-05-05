
#include <iostream>

extern "C" {
#include "at.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
}

#include <cstdint>

int masterfd;

void output_function(range_t *data){
   write(masterfd, data->begin, range_size(data));
}

bool exit_flag = false;

void  exit(struct at_function_result *r,  at_function_context_t *ctx){
   exit_flag = true;
   r->result = true;
}

int main (int argc, char **args) {

   at_context_t *context;
   at_context_init(&context, output_function);

   at_command_add(context, "+exit", AT_STANDALONE_COMMAND, exit);

   masterfd =  posix_openpt(O_RDWR);

   if (masterfd == -1) {
      perror("open ptmx");
      abort();
   }

   if (grantpt(masterfd) == -1) {
      perror("grantpt");
      abort();
   }

   if (unlockpt(masterfd) == -1) {
      perror("unlockpt");
      abort();
   }

   char slavename[256];

   if (ptsname_r(masterfd, slavename, 256) != 0) {
      perror("ptrsname_r");
      abort();
   }


   std::cout << "Terminal: "
             << slavename
             << std::endl;

   uint8_t buffer[128];

   while (true) {

      long r =  read (masterfd, buffer, 128);

      if (r <= 0) {
         break;
      }
      range_t range;
      range.begin = buffer;
      range.end = range.begin + r;

      at_process_input(context, &range);

      if (exit_flag)
         break;
   }

   return 1;
}
