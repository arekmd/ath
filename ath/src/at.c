#include "at.h"
#include "at_internal.h"

struct at_context_t {
   void (*flush)(struct range_t*);
   unsigned char *output_buffer;
   iterator_t outputbuff_iterator;
   struct at_command_register_t *first;
   void *state;
   unsigned char *input_buffer;
   iterator_t inputbuff_iterator;
   unsigned char *last_input_buffer;
   iterator_t lastinbuff_iterator;
   int cmee_level;
   bool echo;
};


struct at_command_register_t {
   const char *tag;
   void (*function)(struct at_function_result*, struct at_function_context_t*);
   struct at_command_register_t *next;
   enum AT_CMD_TYPE cmd_type;
};

void at_function_result_init(struct at_function_result *p) {
   p->detailed = "OK";
   p->result = true;
   p->code = 0;
}

void at_flush_output(struct at_context_t *ctx){
   if (ctx->flush != 0){

      struct range_t range;

      range.begin = ctx->output_buffer;
      range.end = ctx->outputbuff_iterator;

      if ( range_is_empty (&range) == false) {
         ctx->flush(&range);
      }
   }

   ctx->outputbuff_iterator = ctx->output_buffer;
}

iterator_t at_get_output_buffer_end_iterator(struct at_context_t *ctx) {
   return ctx->output_buffer + AT_OUTPUT_BUFFER_SIZE;
}

void at_append_char(struct at_context_t *ctx, unsigned char c){

   if (ctx->outputbuff_iterator == at_get_output_buffer_end_iterator(ctx)) {
      at_flush_output(ctx);
   }

   *ctx->outputbuff_iterator++ = c;
}

void at_append_text(struct at_context_t *ctx, const char *text){
   while (*text != 0) {
      at_append_char(ctx, *text++);
   }
}

void at_append_int(struct at_context_t *ctx, int value){
   char buff[20];
   snprintf(buff, 20, "%i", value);
   at_append_text(ctx, buff);
}

void at_append_line(struct at_context_t *ctx, const char *text){
   at_append_text(ctx, text);
   at_append_text(ctx, "\r\n");
}


void at_return_operation_not_allowed_error(struct at_function_result *r) {
   r->code = 3;
   r->detailed = "Operation not allowed";
   r->result = false;
}

void at_return_operation_not_supported_error(struct at_function_result *r) {
   r->code = 4;
   r->detailed = "Operation not supported";
   r->result = false;
}


void at_return_invalid_index_error(struct at_function_result *r) {
   r->code = 21;
   r->detailed = "Invalid index";
   r->result = false;
}

void at_return_not_found_error(struct at_function_result *r) {
   r->code = 22;
   r->detailed = "Not found";
   r->result = false;
}

void at_text_string_too_long_error(struct at_function_result *r) {
   r->code = 24;
   r->detailed = "Text string too long";
   r->result = false;
}


void at_invalid_chars_error(struct at_function_result *r) {
   r->code = 25;
   r->detailed = "Invalid characters in text string";
   r->result = false;
}


void at_unknown_error(struct at_function_result *r) {
   r->code = 100;
   r->detailed = "Unknown error";
   r->result = false;
}

void at_ok_result(struct at_function_result *r) {
   r->code = 0;
   r->result = true;
   r->detailed = "OK";
}


void  at_cmee_buildin_status(struct at_function_result *r, struct at_function_context_t *ctx){
   at_append_line(ctx->context, "");
   at_append_text(ctx->context, "+CMEE: ");
   at_append_int(ctx->context, ctx->context->cmee_level);
   at_append_line(ctx->context, "");
   at_ok_result(r);
}

static void ate0_buildin_status(struct at_function_result  *r, struct at_function_context_t *ctx){
   ctx->context->echo = false;
   at_ok_result(r);
}


static void ate1_buildin_status(struct at_function_result  *r, struct at_function_context_t *ctx){
   ctx->context->echo = true;
   at_ok_result(r);
}

iterator_t at_get_parameter(iterator_t begin, iterator_t end, struct range_t *result){

   for (iterator_t i = begin; i != end; ++i){

      if (*i == ',') {
         result->begin = begin;
         result->end = i;
         return i + 1;
      }
   }

   result->begin = begin;
   result->end = end;

   return end;
}


static void at_standalone_buildin(struct at_function_result *r, struct at_function_context_t *ctx){
   at_ok_result(r);
}

static void at_cmee_buildin_assignment(
      struct at_function_result *r,
      struct at_function_context_t *ctx){


   if (range_is_empty(&ctx->parameters)) {

      at_return_operation_not_supported_error(r);
      return;
   }

   if (range_equals(&ctx->parameters, "?") ||
       range_equals(&ctx->parameters, "\"?\"")) {
      at_append_line(ctx->context, "");
      at_append_line(ctx->context, "+CMEE: (0-2)");
      at_ok_result(r);
      return ;
   }

   struct range_t result;

   iterator_t it = at_get_parameter(ctx->parameters.begin, ctx->parameters.end, &result);

   if (it == ctx->parameters.end &&
       (range_is_empty(&result) == false) &&
       range_all_digits(&result) &&
       (range_size(&result) == 1)) {

      int new_cmee;
      if (range_convert_to_int(&result, &new_cmee) && (new_cmee >= 0) && (new_cmee <= 2)) {
         ctx->context->cmee_level = new_cmee;
         at_ok_result(r);
         return ;
      }
   }

   at_return_operation_not_supported_error(r);
   return;
}

void at_command_free(struct at_command_register_t *c){

   if (c->next != 0) {
      at_command_free(c->next);
   }

   free(c);
}


void at_context_free(struct at_context_t *ctx){

   if (ctx->first != 0) {
      at_command_free(ctx->first);
   }

   if (ctx->input_buffer != 0) {
      free (ctx->input_buffer);
   }

   if (ctx->output_buffer != 0) {
      free (ctx->output_buffer);
   }

   if (ctx->last_input_buffer != 0) {
      free(ctx->last_input_buffer);
   }

   free (ctx);
}

void at_command_init(struct at_command_register_t *c){
   c->tag = 0;
   c->cmd_type = AT_STANDALONE_COMMAND;
   c->next = 0;
   c->function = 0;
}

void at_command_add(
      struct at_context_t *ctx,
      const char *tag,
      enum AT_CMD_TYPE cmd_type,
      void (*function)(struct at_function_result*, struct at_function_context_t*)){

   struct at_command_register_t *p = (struct at_command_register_t*)malloc(sizeof(struct at_command_register_t));
   at_command_init(p);

   p->cmd_type = cmd_type;
   p->function = function;
   p->tag = tag;
   p->next = ctx->first;
   ctx->first = p;
}

void at_context_init(struct at_context_t **ctx, void (*flush)(struct range_t*)) {

   *ctx = (struct at_context_t*)malloc(sizeof(struct at_context_t));

   if (ctx == 0)
      return;

   (*ctx)->cmee_level = 0;
   (*ctx)->flush = flush;
   (*ctx)->echo = true;
   (*ctx)->first = 0;
   (*ctx)->state = 0;
   (*ctx)->input_buffer = (unsigned char *)malloc(AT_INPUT_BUFFER_SIZE);
   (*ctx)->inputbuff_iterator = (*ctx)->input_buffer;
   (*ctx)->output_buffer = (unsigned char *)malloc(AT_OUTPUT_BUFFER_SIZE);
   (*ctx)->outputbuff_iterator = (*ctx)->output_buffer;

   (*ctx)->last_input_buffer = (unsigned char *)malloc(AT_INPUT_BUFFER_SIZE);
   (*ctx)->lastinbuff_iterator = (*ctx)->last_input_buffer;

   if ((*ctx)->input_buffer == 0  ||
       (*ctx)->output_buffer == 0 ||
       (*ctx)->last_input_buffer == 0) {

      at_context_free(*ctx);
      *ctx = 0;

      return;
   }

   at_command_add(*ctx, "+cmee", AT_ASSIGNMENT_COMMAND, at_cmee_buildin_assignment);
   at_command_add(*ctx, "+cmee", AT_STATUS_COMMAND, at_cmee_buildin_status);
   at_command_add(*ctx, "", AT_STANDALONE_COMMAND, at_standalone_buildin);

   at_command_add(*ctx, "e0", AT_STANDALONE_COMMAND, ate0_buildin_status);
   at_command_add(*ctx, "e1", AT_STANDALONE_COMMAND, ate1_buildin_status);

}

struct range_t get_line(struct range_t *data){

   iterator_t r = range_search_character(data, '\r');

   if (r != data->end) {
      return range_create_it(data->begin, r);
   } else {
      return range_empty();
   }
}

static void *at_get_state (struct at_context_t *context) {
   return context->state;
}

bool at_get_in_quota_value(struct range_t *range, struct range_t *result){

   if (range_is_empty(range) == false &&
       *(range->begin) == '\"' && *((range)->end -1) == '\"' ) {

      result->begin = range->begin + 1;
      result->end = range->end - 1;
      return true;
   }

   return false;
}


void split_at_commands(struct range_t *command, void(*ptr)(struct range_t* )){

   bool qt = false;

   iterator_t b_cmd = command->begin;

   for (iterator_t it = command->begin; it != command->end; ++it) {
      if (*it == '"') {
         qt = !qt;
      }

      if (qt == false && *it == ';'){

         struct range_t cmd_range;


         cmd_range.begin = b_cmd;
         cmd_range.end = it;

         cmd_range = range_trim(&cmd_range);

         ptr(&cmd_range);
         b_cmd = it + 1;
      }
   }

   struct range_t cmd_range = range_create_it(b_cmd, command->end);
   cmd_range = range_trim(&cmd_range);
   ptr(&cmd_range);
}

bool get_at_command(struct range_t *input, struct range_t *result){

   if ( range_size(input) < 2) {
      return  false;
   };

   unsigned char first = *input->begin;
   unsigned char second = *(input->begin + 1);

   if ((first == 'a' || first == 'A') && (second == 't' || second == 'T')) {
      result->begin = input->begin + 2;
      result->end = input->end;
      return true;
   }

   return false;
}

static void at_append_range(struct at_context_t *ctx, struct range_t *range){
   for (iterator_t it = range->begin; it != range->end; ++it) {
      at_append_char(ctx, *it);
   }
}


static bool is_character(unsigned char c){
   return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static enum AT_CMD_TYPE at_get_cmd_type(struct range_t *tag, struct range_t *command){

   if (tag->end == command->end) {
      return AT_STANDALONE_COMMAND;
   } else if ( *(tag->end) == '=' && ((range_size(tag) + 1) < range_size(command)))  {
      return AT_ASSIGNMENT_COMMAND;
   } else if ( *(tag->end) == '?' && ((range_size(tag) + 1) == range_size(command))) {
      return AT_STATUS_COMMAND;
   } else {
      return AT_UNKOWN_COMMAND;
   }
}

struct range_t at_get_tag(struct range_t *range) {

   for (iterator_t it = range->begin; it != range->end; ++it) {

      if (it == range->begin) {
         if (*it == '+' || *it == '&' || *it == '^'|| is_character(*it))
            continue;
      } else {
         if (is_character(*it) || is_digit(*it))
            continue;
      }

      struct range_t result;

      result.begin = range->begin;
      result.end = it;

      return result;
   }

   return *range;
}

static struct at_command_register_t *at_find_command_register(
      struct at_context_t *ctx,
      struct range_t tag,
      enum AT_CMD_TYPE cmd_type) {

   struct at_command_register_t *p = ctx->first;

   while (p != 0){
      if (range_equals(&tag, p->tag) && p->cmd_type == cmd_type)
         return p;

      p = p->next;
   }

   return 0;
}

static void at_append_ok(struct at_context_t *ctx) {
   at_append_line(ctx, "");
   at_append_line(ctx, "OK");
}

static void at_append_error(struct at_context_t *ctx) {
   at_append_line(ctx, "");
   at_append_line(ctx, "ERROR");
}


static struct at_function_result at_process_command(
      struct at_context_t *ctx,
      struct range_t *command) {

   struct range_t tag = at_get_tag(command);

   enum AT_CMD_TYPE cmd_type = at_get_cmd_type(&tag, command);


   if (cmd_type != AT_UNKOWN_COMMAND) {

      range_lowercase(&tag);

      struct at_command_register_t *reg_ptr = at_find_command_register(
               ctx,
               tag,
               cmd_type
      );

      if (reg_ptr != 0) {

         struct at_function_context_t fctx;
         fctx.context = ctx;
         range_init(&fctx.parameters);

         if (cmd_type == AT_ASSIGNMENT_COMMAND) {
            fctx.parameters.begin = tag.end + 1;
            fctx.parameters.end = command->end;
         }

         struct at_function_result result;

         at_function_result_init(&result);

         result.code = 100;
         result.detailed = "Unknown error";
         result.result = false;

         reg_ptr->function(&result, &fctx);

         return result;
      }
   }

   struct at_function_result r;
   at_return_operation_not_supported_error(&r);
   return r;
}


static struct at_function_result at_process_chunk(
      struct at_context_t *ctx,
      struct range_t *data,
      bool first_chunk){

   if (first_chunk) {
      struct range_t r;
      if (get_at_command(data, &r)){
         return at_process_command(ctx, &r);
      } else {
         struct at_function_result r;
         at_return_operation_not_supported_error(&r);
         return r;
      }
   } else {
      return at_process_command(ctx, data);
   }
}

static void at_process_commands(
      struct at_context_t *ctx,
      struct range_t *line) {

   bool qt = false;
   bool first_chunk = true;
   iterator_t b_cmd = line->begin;
   struct at_function_result result;
   at_function_result_init (&result);

   for (iterator_t it = line->begin; it != line->end; ++it) {

      if (*it == '"') {
         qt = !qt;
      }

      if (qt == false && *it == ';') {

         struct range_t cmd_range;

         cmd_range.begin = b_cmd;
         cmd_range.end = it;

         cmd_range = range_trim(&cmd_range);

         result = at_process_chunk(ctx, &cmd_range, first_chunk);

         if (result.result == false)
            break;

         first_chunk = false;
         b_cmd = it + 1;
      }
   }

   if (result.result == true) {
      struct range_t cmd_range = range_create_it(b_cmd, line->end);
      cmd_range = range_trim(&cmd_range);
      result = at_process_chunk(ctx, &cmd_range, first_chunk);
   }

   if (result.result == true) {
      at_append_ok(ctx);
   } else {

      switch (ctx->cmee_level){

      case 0:
         at_append_error(ctx);
         break;
      case 1:
         at_append_line(ctx, "");
         at_append_text(ctx, "+CME ERROR: ");
         at_append_int(ctx, result.code);
         at_append_line(ctx, "");
         break;
      case 2:
         at_append_line(ctx, "");
         at_append_text(ctx, "+CME ERROR: ");
         at_append_line(ctx, result.detailed);
      }
   }

   at_flush_output(ctx);
}


void at_process_line(
      struct at_context_t *ctx,
      struct range_t *line) {

   struct range_t trimmed_line =  range_trim(line);

   if ( range_is_empty(&trimmed_line) || range_size(&trimmed_line) < 2) {
      return;
   }

   at_process_commands(ctx, &trimmed_line);
}

static iterator_t at_get_input_buffer_end_iterator(
      struct at_context_t *ctx) {

   return ctx->input_buffer + AT_INPUT_BUFFER_SIZE;
}

void at_process_input(
      struct at_context_t *ctx,
      struct range_t *data){

   if ( range_is_empty(data))
      return;

   if (ctx->echo) {
      at_append_range(ctx, data);
      at_flush_output(ctx);
   }

   for (iterator_t i = data->begin; i != data->end; ++i) {

      if ( *i == '\r' && ctx->inputbuff_iterator != ctx->input_buffer) {
         struct range_t line = get_range_by_iterators(ctx->input_buffer, ctx->inputbuff_iterator);
         at_process_line( ctx, &line);

         ctx->lastinbuff_iterator = ctx->last_input_buffer;

         for (iterator_t i = ctx->input_buffer; i != ctx->inputbuff_iterator; ++i) {
            *(ctx->lastinbuff_iterator)++ = *i;
         }

         ctx->inputbuff_iterator = ctx->input_buffer;
         continue;
      }

      if (*i == '/') {

         struct range_t line = get_range_by_iterators(ctx->input_buffer, ctx->inputbuff_iterator);

         if ( (range_size(&line) == 1) && ( *ctx->input_buffer == 'A' || *ctx->input_buffer == 'a' )) {

            struct range_t lline = get_range_by_iterators(ctx->last_input_buffer, ctx->lastinbuff_iterator);

            if (range_is_empty(&lline) == false) {
               at_process_line(ctx, &lline);
            }

            ctx->inputbuff_iterator = ctx->input_buffer;
            continue;
         }
      }

      if ( ctx->inputbuff_iterator == at_get_input_buffer_end_iterator(ctx)) {
         // Silently discard input buffer, on buffer overflow
         ctx->inputbuff_iterator = ctx->input_buffer;
         continue;
      }

      *ctx->inputbuff_iterator++ = *i;
   }
}

void at_add_unsolicited(struct at_context_t *ctx, const char *prefix, const char *text){
   at_append_line(ctx, "");
   at_append_text(ctx, "+");
   at_append_text(ctx, prefix);
   at_append_text(ctx, ": ");
   at_append_line(ctx, text);
   at_flush_output(ctx);
}

void at_add_unsolicited_line(struct at_context_t *ctx, const char *text) {
   at_append_line(ctx, "");
   at_append_line(ctx, text);
   at_flush_output(ctx);
}
