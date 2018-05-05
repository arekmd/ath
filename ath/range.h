#ifndef RANGE_H
#define RANGE_H

#define iterator_t unsigned char *

#ifndef bool
#define bool int
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

struct range_t {
   iterator_t begin;
   iterator_t end;
};

void range_init(struct range_t *range);

struct range_t range_create_cnt(iterator_t begin, unsigned int size);
struct range_t range_empty(void);
struct range_t range_create_it(iterator_t begin, iterator_t end);
bool range_is_empty(struct range_t *range);

unsigned int range_size(struct range_t *range);

bool is_digit(unsigned char v);

bool range_convert_to_int(struct range_t *range, int *result);

bool range_all_digits(struct range_t *range);

void range_fill(struct range_t *range, unsigned char value);

bool range_equals(struct range_t *data, const char *text);

iterator_t range_search_character(struct range_t *r, char c);

struct range_t range_trim(struct range_t *data);

void range_lowercase(struct range_t *data);
void range_uppercase(struct range_t *data);

struct range_t get_range_by_iterators( iterator_t begin, iterator_t end);

struct range_t get_range( unsigned char *text);

bool range_is_numeric(struct range_t *data);

unsigned int range_strlen(unsigned char *p);

bool range_ranges_equals(struct range_t *first, struct range_t *second);

#endif
