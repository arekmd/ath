#ifndef AT_INTERNAL_H
#define AT_INTERNAL_H

#include "range.h"
#include "at.h"

void split_at_commands(struct range_t *command, void(*ptr)(struct range_t* ));
struct range_t at_get_tag(struct range_t *range);
struct range_t get_line(struct range_t *data);
bool get_at_command(struct range_t *input, struct range_t *result);


#endif // AT_INTERNAL_H
