#ifndef AT_H
#define AT_H

#include "range.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef AT_INPUT_BUFFER_SIZE
#define AT_INPUT_BUFFER_SIZE 32
#endif

#ifndef AT_OUTPUT_BUFFER_SIZE
#define AT_OUTPUT_BUFFER_SIZE 32
#endif

enum AT_CMD_TYPE {
   AT_STANDALONE_COMMAND = 1,
   AT_ASSIGNMENT_COMMAND = 2,
   AT_STATUS_COMMAND = 3,
   AT_UNKOWN_COMMAND = 4
};

struct at_command_register_t;

struct at_function_result {
   bool result;
   int code;
   const char *detailed;
};

struct at_context_t;

struct at_function_context_t{
   struct at_context_t *context;
   struct range_t parameters;
};

void at_function_result_init(struct at_function_result *p);
void at_context_init(struct at_context_t **ctx, void (*flush)(struct range_t*));

void at_command_add(
      struct at_context_t *ctx,
      const char *tag,
      enum AT_CMD_TYPE cmd_type,
      void (*function)(struct at_function_result*, struct at_function_context_t*));

void at_process_input(
      struct at_context_t *ctx,
      struct range_t *data);

void at_context_free(struct at_context_t *ctx);

void at_flush_output(struct at_context_t *ctx);

iterator_t at_get_parameter(iterator_t begin, iterator_t end, struct range_t *result);

bool at_get_in_quota_value(struct range_t *range, struct range_t *result);

void at_invalid_chars_error(struct at_function_result *r);
void at_unknown_error(struct at_function_result *r);
void at_ok_result(struct at_function_result *r);
void at_text_string_too_long_error(struct at_function_result *r);
void at_return_not_found_error(struct at_function_result *r);
void at_return_invalid_index_error(struct at_function_result *r);
void at_return_operation_not_supported_error(struct at_function_result *r);
void at_return_operation_not_allowed_error(struct at_function_result *r);

void at_add_unsolicited(struct at_context_t *ctx, const char *prefix, const char *text);
void at_add_unsolicited_line(struct at_context_t *ctx, const char *text);

void at_append_line(struct at_context_t *ctx, const char *text);
void at_append_int(struct at_context_t *ctx, int value);
void at_append_text(struct at_context_t *ctx, const char *text);
void at_append_char(struct at_context_t *ctx, unsigned char c);

#endif
