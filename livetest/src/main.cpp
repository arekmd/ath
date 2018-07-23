
#include <iostream>

extern "C" {
    #include "at.h"
}


void output_function(range_t *data){
   for (iterator_t it = data->begin; it != data->end; ++it) {
      std::cout << *it;
   }
}

bool exit_flag = false;

void  exit(struct at_function_result *r,  at_function_context_t *ctx){
   r->result = true;
   exit_flag = true;
}

int main (int argc, char **args) {

   at_context_t *context;
   at_context_init(&context, output_function);

   at_command_add(context, "+exit", AT_STANDALONE_COMMAND, exit);

   at_add_unsolicited_line(context, "+POWERON");
   at_add_unsolicited(context, "STATUS", "BOOTING");

   while (true) {

      uint8_t c = getchar();

      if (c == 10)
         c = 13;

      range_t r;
      r.begin = &c;
      r.end = r.begin + 1;

      at_process_input(context, &r);

      if (exit_flag)
         break;
   }

   return 1;
}
