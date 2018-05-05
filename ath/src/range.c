#include "range.h"
#include "range_internal.h"

void range_init(struct range_t *range) {
   range->begin = 0;
   range->end = 0;
}

struct range_t range_create_cnt(iterator_t begin, unsigned int size) {
   struct range_t r;
   r.begin = begin;
   r.end = begin + size;
   return r;
}

struct range_t range_empty(void) {
   struct range_t r;
   range_init(&r);
   return r;
}

struct range_t range_create_it(iterator_t begin, iterator_t end) {
   struct range_t r;
   r.begin = begin;
   r.end = end;
   return r;
}

bool range_is_empty(struct range_t *range) {
   return range->begin == range->end;
}

unsigned int range_size(struct range_t *range) {
   return range->end - range->begin;
}

int is_digit(unsigned char v) {
   return v >= '0' && v <= '9';
}

bool range_convert_to_int(struct range_t *range, int *result) {

   *result = 0;

   for (iterator_t i = range->begin; i != range->end; ++i) {

      if (false == is_digit(*i))
         return false;

      *result *= 10;
      *result += *i - '0';
   }

   return true;
}

bool range_all_digits(struct range_t *range) {

   for (iterator_t i = range->begin; i != range->end; ++i) {
      if (is_digit(*i) == false)
         return false;
   }

   return true;
}

void range_fill(struct range_t *range, unsigned char value){

   iterator_t it = range->begin;

   for (unsigned int i = 0; i <  range_size(range); ++i) {
      *it++ = value;
   }
}


iterator_t range_search_iterators(iterator_t first, iterator_t last, iterator_t s_first, iterator_t s_last) {
   for (; ; ++first) {
      iterator_t it = first;
      for (iterator_t s_it = s_first; ; ++it, ++s_it) {
         if (s_it == s_last) {
            return first;
         }
         if (it == last) {
            return last;
         }
         if (!(*it == *s_it)) {
            break;
         }
      }
   }
}

iterator_t range_search_character(struct range_t *r, char c) {
   for (iterator_t i = r->begin; i != r->end; ++i) {
      if (*i == c)
         return i;
   }

   return r->end;
}

iterator_t range_search_range(struct range_t *r1, struct range_t *r2) {
   return range_search_iterators(r1->begin, r1->end, r2->begin, r2->end);
}

struct range_t range_trim(struct range_t *data){

   iterator_t b = data->begin;
   iterator_t e = data->end;

   for (; b != data->end; ++b) {
      if (*b != ' ')
         break;
   }

   for (iterator_t i = b; i != data->end; ++i){
      if (*i != ' ')
         e = i + 1;
   }

   struct range_t result;
   result.begin = b;
   result.end = e;

   return result;
}

void range_lowercase(struct range_t *data){
   for (iterator_t it = data->begin; it != data->end; ++it) {
      if (*it >= 'A' && *it <= 'Z') {
         *it -= ( 'A' - 'a');
      }
   }
}

void range_uppercase(struct range_t *data){
   for (iterator_t it = data->begin; it != data->end; ++it) {
      if (*it >= 'a' && *it <= 'z') {
         *it += ( 'A' - 'a');
      }
   }
}

bool range_is_numeric(struct range_t *data){
   for (iterator_t it = data->begin; it != data->end; ++it) {
      if (*it < '0' || *it > '9')
         return false;
   }

   return true;
}

bool range_ranges_equals(struct range_t *first, struct range_t *second) {

   if (range_size(first) != range_size(second))
      return false;

   iterator_t it2 = second->begin;

   for (iterator_t it = first->begin; it != first->end; ++it) {
      if (*it != *it2++)
         return false;
   }

   return true;
}

unsigned int range_strlen(unsigned char *p){

   unsigned int len = 0;

   while (*p++ != 0)
      len++;

   return len;
}

bool range_equals(struct range_t *data, const char *text) {

   for (iterator_t it = data->begin; it != data->end; ++it) {

      if (*text == 0 || *text != *it)
         return false;

      text++;
   }

   return *text == 0;
}

struct range_t get_range( unsigned char *text) {
   unsigned int len = range_strlen(text);
   return range_create_cnt(text, len);
}

struct range_t get_range_by_iterators( iterator_t begin, iterator_t end) {
   struct range_t r;
   r.begin = begin;
   r.end = end;
   return r;
}


