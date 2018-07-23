#include <gtest/gtest.h>

#include <vector>

extern "C" {
   #include "at.h"
   #include "at_internal.h"
}

TEST(at_test, ptr_test) {

   std::vector<char> v;
   v.resize(10);

   char *begin = &*v.begin();
   char *end = begin + v.size();


   std::fill(begin, end, 1);

   ASSERT_TRUE(begin != nullptr);

}

int test_24_out_calls = 0;
int test_24_at_calls = 0;
void test_24_output_function(range_t *data){
   test_24_out_calls++;
}

void  test_24_at_command(struct at_function_result *r,  at_function_context_t *ctx){
   test_24_at_calls++;
   r->result = true;
}

TEST(at_test, test_24) {

   at_context_t *context;
   at_context_init(&context, test_24_output_function);
    at_command_add(context, "", AT_STANDALONE_COMMAND, test_24_at_command);

    unsigned char cmd_buffer[] = "abc\r";
    range_t cmd_range = get_range(cmd_buffer);

    at_process_input(context, &cmd_range);

    ASSERT_EQ(test_24_at_calls, 0);
}

int test_23_at_calls = 0;


void  test_23_at_command(at_function_result *r,  at_function_context_t *){
   test_23_at_calls++;
   r->result = true;
}

int test_23_out_calls = 0;

void test_23_output_function(range_t *data){
   test_23_out_calls++;
}

TEST(at_test, test_23) {

   at_context_t *context;
   at_context_init(&context, test_23_output_function);

   at_command_add(context, "", AT_STANDALONE_COMMAND, test_23_at_command);

   unsigned char cmd_buffer[] = "AT\r";
   range_t cmd_range = get_range(cmd_buffer);

   at_process_input(context, &cmd_range);

   ASSERT_EQ(test_23_at_calls, 1);

   unsigned char cmd_buffer2[] = "A/";
   range_t cmd_range2 = get_range(cmd_buffer2);

   at_process_input(context, &cmd_range2);

   ASSERT_EQ(test_23_at_calls, 2);

   at_context_free(context);
}


bool test_22_at_called = false;


void test_22_at_command(at_function_result *r,  at_function_context_t *){
   test_22_at_called = true;
   r->result = true;
}

bool test_22_cpin_assignment_called = false;

void test_22_cpin_assignment_command(at_function_result *r,  at_function_context_t *){
   test_22_cpin_assignment_called = true;
   r->result = true;
}

bool test_22_cpin_status_called = false;

void test_22_cpin_status_command(at_function_result *r,  at_function_context_t *){
   test_22_cpin_status_called = true;
   r->result = true;
}

int test_22_out_calls = 0;

void test_22_output_function(range_t *data){
   test_22_out_calls++;
}

TEST(at_test, test_22) {

   at_context_t *context;
   at_context_init(&context, test_22_output_function);

   at_command_add(context, "", AT_STANDALONE_COMMAND, test_22_at_command);
   at_command_add(context, "+cpin", AT_ASSIGNMENT_COMMAND, test_22_cpin_assignment_command);
   at_command_add(context, "+cpin", AT_STATUS_COMMAND, test_22_cpin_status_command);

   unsigned char cmd_buffer[] = "AT;+CPIN=?; +CPIN?\r";
   range_t cmd_range = get_range(cmd_buffer);

   at_process_input(context, &cmd_range);

   at_flush_output(context);

   at_context_free(context);

   ASSERT_EQ(test_22_out_calls, 2);
   ASSERT_TRUE(test_22_at_called);
   ASSERT_TRUE(test_22_cpin_assignment_called);
   ASSERT_TRUE(test_22_cpin_status_called);
}

bool test_21_at_called = false;


void test_21_at_command(at_function_result  *r, at_function_context_t *){
   test_21_at_called = true;
   r->result = true;
}

bool test_21_cpin_assignment_called = false;


void test_21_cpin_assignment_command(at_function_result  *r, at_function_context_t *){
   test_21_cpin_assignment_called = true;
   r->result = true;
}

bool test_21_cpin_status_called = false;

void test_21_cpin_status_command(at_function_result  *r, at_function_context_t *){
   test_21_cpin_status_called = true;
   r->result = true;
}

void test_21_output_function(range_t *data){

}

TEST(at_test, test_21) {

   at_context_t *context;
   at_context_init(&context, test_21_output_function);

   at_command_add(context, "", AT_STANDALONE_COMMAND, test_21_at_command);
   at_command_add(context, "+cpin", AT_ASSIGNMENT_COMMAND, test_21_cpin_assignment_command);
   at_command_add(context, "+cpin", AT_STATUS_COMMAND, test_21_cpin_status_command);

   unsigned char cmd_buffer[] = "AT;+CPIN=?; +CPIN?\r";
   range_t cmd_range = get_range(cmd_buffer);

   at_process_input(context, &cmd_range);

   at_context_free(context);

   ASSERT_TRUE(test_21_at_called);
   ASSERT_TRUE(test_21_cpin_assignment_called);
   ASSERT_TRUE(test_21_cpin_status_called);
}

bool test_20_at_called = false;

void test_20_at_command(at_function_result *r, at_function_context_t *ctx){
   test_20_at_called = true;
   r->result = true;
}

void test_20_output_function(range_t *data){
}

TEST(at_test, test_20) {

   at_context_t *context;
   at_context_init(&context, test_20_output_function);

   at_command_add(context, "", AT_STANDALONE_COMMAND, test_20_at_command);

   unsigned char cmd_buffer[] = "AT\r";
   range_t cmd_range = get_range(cmd_buffer);

   range_t r1;

   r1.begin = cmd_buffer;
   r1.end = r1.begin + 1;

   range_t r2;
   r2.begin = r1.end;
   r2.end = r2.begin + 2;

   at_process_input(context, &r1);
   ASSERT_FALSE(test_20_at_called);

   at_process_input(context, &r2);
   ASSERT_TRUE(test_20_at_called);

   at_context_free(context);
}

static unsigned char global_output_buffer[2048];
static unsigned char *g_it = global_output_buffer;

int pin = 0;

void at_set_cpin_command(at_function_result  *r, at_function_context_t *ctx){


   range_t value;

   if (at_get_in_quota_value(&ctx->parameters, &value) && range_size(&value) == 4 && range_all_digits(&value)) {
      range_convert_to_int(&value, &pin);
      r->result = true;
   } else {
      r->detailed = "Invalid PIN value";
      r->result  = false;
   }

}


void output_function(range_t *data){
   for (iterator_t it = data->begin; it != data->end; ++it) {
      *g_it++ = *it;
   }
}

static int dbl_val_1 = 0;
static int dbl_val_2 = 0;

void dbl_parameters(at_function_result  *r, at_function_context_t *ctx){


   range_t first_parameter;
   range_init(&first_parameter);

   range_t second_parameter;
   range_init(&second_parameter);

   iterator_t it = at_get_parameter(ctx->parameters.begin, ctx->parameters.end, &first_parameter);

   range_t value;

   if (false == range_is_empty(&first_parameter) &&  at_get_in_quota_value(&first_parameter, &value) && range_all_digits(&value)) {

      range_convert_to_int(&value, &dbl_val_1);

      it =  at_get_parameter(it, ctx->parameters.end, &second_parameter);

      range_t second_value;
      range_init(&second_value);

      if (false == range_is_empty(&second_parameter) && at_get_in_quota_value(&second_parameter, &second_value) && range_all_digits(&second_value)) {
         range_convert_to_int(&second_value, &dbl_val_2);
      } else {
         r->detailed = "Invalid test value";
         r->result  = false;
      }
   } else {
      r->detailed = "Invalid test value";
      r->result  = false;
   }

}


TEST(at_test, test_19) {

   at_context_t *context;
   at_context_init(&context, output_function);

   at_command_add(context, "+tst", AT_ASSIGNMENT_COMMAND, dbl_parameters);

   unsigned char cmd_buffer[] = "AT+TST=\"1111\",\"33\"\r";
   range_t cmd_range = get_range(cmd_buffer);

   at_process_input(context, &cmd_range);

   ASSERT_EQ(dbl_val_1, 1111);
   ASSERT_EQ(dbl_val_2, 33);
}

TEST(at_test, test_18) {

   unsigned char i_buffer[] = "222";

   range_t i_range = get_range(i_buffer);

   int i;

   range_convert_to_int(&i_range, &i);

   ASSERT_EQ(i, 222);
}

TEST(at_test, test_17) {
   unsigned char i_buffer[] = "a222";
   range_t i_range = get_range(i_buffer);
   ASSERT_FALSE(range_all_digits(&i_range));
}


TEST(at_test, test_16) {
   unsigned char i_buffer[] = "222";
   range_t i_range = get_range(i_buffer);
   ASSERT_TRUE(range_all_digits(&i_range));
}

TEST(at_test, test_15) {
   unsigned char i_buffer[] = "222";
   range_t i_range = get_range(i_buffer);
   range_t result;
   ASSERT_FALSE(at_get_in_quota_value(&i_range, &result));
}

TEST(at_test, test_14) {
   unsigned char i_buffer[] = "\"222\"";
   range_t i_range = get_range(i_buffer);

   range_t result;
   ASSERT_TRUE(at_get_in_quota_value(&i_range, &result));
   ASSERT_TRUE(range_equals(&result, "222"));
}

TEST(at_test, test_13) {

   unsigned char i_buffer[] = "222,ab";
   range_t i_range = get_range(i_buffer);

   range_t r;
   iterator_t it = at_get_parameter(i_range.begin, i_range.end, &r);

   ASSERT_TRUE(range_equals(&r, "222"));

   it = at_get_parameter(it, i_range.end, &r);

   ASSERT_TRUE(range_equals(&r, "ab"));
}


TEST(at_test, test_12) {
   unsigned char i_buffer[] = "222,ab";
   range_t i_range = get_range(i_buffer);

   range_t r;

   iterator_t it = at_get_parameter(i_range.begin, i_range.end, &r);

   ASSERT_TRUE(range_equals(&r, "222"));
}



TEST(at_test, test_11) {

   unsigned char i_buffer[] = "222";
   range_t i_range = get_range(i_buffer);
   range_t r;

   iterator_t it = at_get_parameter(i_range.begin, i_range.end, &r);

   ASSERT_TRUE(range_equals(&r, "222"));
}


TEST(at_test, test_10) {

   at_context_t *context;
   at_context_init(&context, output_function);

   at_command_add(context, "+cpin", AT_ASSIGNMENT_COMMAND, at_set_cpin_command);

   unsigned char cmd_buffer[] = "AT+CPIN=\"1111\"\r";
   range_t cmd_range = get_range(cmd_buffer);

   at_process_input(context, &cmd_range);

   ASSERT_EQ(pin, 1111);
}

TEST(at_test, test_09) {
   unsigned char i_buffer[] = "$cpin=\"1111\"";
   range_t i_range = get_range(i_buffer);
   range_t result = at_get_tag(&i_range);
   ASSERT_TRUE( range_is_empty(&result));
}

TEST(at_test, test_08) {
   unsigned char i_buffer[] = "&cpin=\"1111\"";
   range_t i_range = get_range(i_buffer);
   range_t result = at_get_tag(&i_range);
   ASSERT_TRUE(range_equals(&result, "&cpin"));
}

TEST(at_test, test_07) {
   unsigned char i_buffer[] = "+cpin=\"1111\"";
   range_t i_range = get_range(i_buffer);
   range_t result = at_get_tag(&i_range);
   ASSERT_TRUE(range_equals( &result, "+cpin"));
}

TEST(at_test, test_06) {
   unsigned char i_buffer[] = "+cpin?";
   range_t i_range = get_range(i_buffer);
   range_t result = at_get_tag(&i_range);
   ASSERT_TRUE(range_equals(&result, "+cpin"));
}

TEST(at_test, test_05) {
   unsigned char i_buffer[] = "AT";
   range_t i_range = get_range(i_buffer);
   range_t result = at_get_tag(&i_range);
   ASSERT_TRUE(range_equals(&result, "AT"));
}


TEST(at_test, test_04) {
   unsigned char i_buffer[] = "AT";
   range_t i_range = get_range(i_buffer);
   range_t o_range;

   range_init(&o_range);

   ASSERT_TRUE(get_at_command(&i_range, &o_range));

   ASSERT_TRUE(range_equals(&o_range, ""));
}

TEST(at_test, test_03) {

   unsigned char i_buffer[] = "AT1";
   range_t i_range = get_range(i_buffer);

   range_t o_range;

   range_init(&o_range);

   ASSERT_TRUE(get_at_command(&i_range, &o_range));

   ASSERT_TRUE(range_equals(&o_range, "1"));
}


TEST(at_test, test_02) {
   unsigned char output_buffer[] = "AT";
   range_t o_range = get_range(output_buffer);
}


void at_command(at_function_result *r, at_function_context_t *){
   r->detailed = "OK";
   r->code = 0;
   r->result = true;
}

void at_cpin_status_command(at_function_result  *r, at_function_context_t *fctx){

   at_append_line(fctx->context,"+CPIN: READY");

   r->code = 0;
   r->result = true;
   r->detailed = "OK";

}

TEST(at_test, test_01) {

   at_context_t *context;
   at_context_init(&context,output_function);

   at_command_add(context, "", AT_STANDALONE_COMMAND, at_command);
   at_command_add(context, "+cpin", AT_STANDALONE_COMMAND, at_command);
   at_command_add(context, "+cpin", AT_STATUS_COMMAND, at_cpin_status_command);
   at_command_add(context, "+cpin", AT_ASSIGNMENT_COMMAND, at_command);

   unsigned char cmd_buffer[] = "AT\r";
   range_t cmd_range = get_range(cmd_buffer);

   at_process_input(context, &cmd_range);

   at_context_free(context);
}
