
#include <gtest/gtest.h>

extern "C" {
   #include "range.h"
   #include "range_internal.h"
   #include "at.h"
   #include "at_internal.h"
}

range_t result;
int calls = 0;
void callback(range_t *data){
   result = *data;
   calls++;
}

TEST(range_tests, test23) {
   unsigned char command[] = "ttt\";\";a";
   range_t cmd = get_range(command);

   calls = 0;
   split_at_commands(&cmd, callback);

   ASSERT_TRUE(range_equals(&result, "a"));
   ASSERT_EQ(calls, 2);
}

TEST(range_tests, test22) {
   unsigned char command[] = "ttt\";\"";
   range_t cmd = get_range(command);

   calls = 0;
   split_at_commands(&cmd, callback);

   ASSERT_TRUE(range_equals(&result, "ttt\";\""));
   ASSERT_EQ(calls, 1);
}

TEST(range_tests, test21) {
   unsigned char command[] = "ttt";
   range_t cmd = get_range(command);

   calls = 0;
   split_at_commands(&cmd, callback);

   ASSERT_TRUE(range_equals(&result, "ttt"));
   ASSERT_EQ(calls, 1);
}

TEST(range_tests, test20) {
   unsigned char command[] = "ttt;a;222";
   range_t cmd = get_range(command);

   calls = 0;
   split_at_commands(&cmd, callback);

   ASSERT_TRUE(range_equals(&result, "222"));
   ASSERT_EQ(calls, 3);
}


TEST(range_tests, test19) {
   unsigned char command[] = "ttt;a";
   range_t cmd = get_range(command);

   calls = 0;
   split_at_commands(&cmd, callback);

   ASSERT_TRUE(range_equals(&result, "a"));
   ASSERT_EQ(calls, 2);
}



TEST(range_tests, test18) {

   unsigned char s1[] =  "";



   ASSERT_EQ(range_strlen(s1), 0);

   unsigned char s2[] =  "1";

   ASSERT_EQ(range_strlen(s2), 1);
}

TEST(range_tests, test17) {
   unsigned char buff[11] =  "1234a6789";
   range_t first  = range_create_cnt (buff, 9);
   ASSERT_FALSE(range_equals(&first, "1234a67890"));
}

TEST(range_tests, test16) {
   unsigned char buff[11] =  "1234a67890";
   range_t first  = range_create_cnt (buff, 10);
   ASSERT_FALSE(range_equals(&first, "1234a6789"));
}

TEST(range_tests, test15) {

   unsigned char buff[11] =  "1234a67890";
   range_t first  = range_create_cnt (buff, 10);
   ASSERT_TRUE(range_equals(&first, "1234a67890"));
}

TEST(range_tests, test14) {

   unsigned char buff[11] =  "1234a67890";
   unsigned char buff2[11] =  "1234b67890";

   range_t first  = range_create_cnt (buff, 10);
   range_t second  = range_create_cnt (buff2, 10);


   ASSERT_FALSE(range_ranges_equals(&first, &second));
   ASSERT_FALSE(range_ranges_equals(&second, &first));
}

TEST(range_tests, test13) {

   unsigned char buff[11] =  "1234a67890";


   range_t first  = range_create_cnt (buff, 10);
   range_t second  = range_create_cnt (buff, 10);

   ASSERT_TRUE(range_ranges_equals(&first, &second));
   ASSERT_TRUE(range_ranges_equals(&second, &first));
}

TEST(range_tests, test12) {
   unsigned char buff[11] =  "1234a67890";
   range_t buffer = range_create_cnt(buff, 10);
   ASSERT_FALSE(range_is_numeric(&buffer));
}

TEST(range_tests, test11) {

   unsigned char buff[11] =  "1234567890";
   range_t buffer = range_create_cnt(buff, 10);
   ASSERT_TRUE(range_is_numeric(&buffer));
}

TEST(range_tests, test10) {

   unsigned char buff[20] =  "Hello world";
   range_t buffer = range_create_cnt(buff, 11);
   range_uppercase(&buffer);

   ASSERT_EQ(*buffer.begin, 'H');
   ASSERT_EQ(*(buffer.begin + 1), 'E');
   ASSERT_EQ(*(buffer.begin + 2), 'L');
}

TEST(range_tests, test9) {

   unsigned char buff[20] =  "Hello world";
   range_t buffer = range_create_cnt(buff, 11);

   range_lowercase(&buffer);

   ASSERT_EQ(*buffer.begin, 'h');
   ASSERT_EQ(*(buffer.begin + 1), 'e');
}

TEST(range_tests, test8) {

   unsigned char buff[20] =  "Hello world";
   range_t buffer = range_create_cnt(buff, 11);

   range_t result = range_trim(&buffer);

   ASSERT_EQ(11, range_size(&result));
   ASSERT_EQ(*result.begin, 'H');
}

TEST(range_tests, test7) {

   unsigned char buff[20] =  "Hello world ";

   range_t buffer = range_create_cnt(buff, 12);

   range_t result = range_trim(&buffer);

   ASSERT_EQ(11 ,  range_size(&result));
   ASSERT_EQ(*result.begin, 'H');
}

TEST(range_tests, test6) {

   unsigned char buff[20] =  " Hello world";

   range_t buffer = range_create_cnt(buff, 12);

   range_t result = range_trim(&buffer);

   ASSERT_EQ(11, range_size(&result));
   ASSERT_EQ(*result.begin, 'H');
}

TEST(range_tests, test5) {

   unsigned char buff[20] =  " Hello world  ";
   range_t buffer = range_create_cnt(buff, 14);

   range_t result = range_trim(&buffer);

   ASSERT_EQ(11, range_size(&result));
   ASSERT_EQ(*result.begin, 'H');
}

TEST(range_tests, test4) {
   unsigned char buff[20] =  "Hello world\r";

   range_t buffer = range_create_cnt(buff, 1);

   range_t result = get_line(&buffer);
   ASSERT_TRUE(range_is_empty(&result));
}

TEST(range_tests, test3) {
   unsigned char buff[20] =  "Hello world\r";
   range_t buffer = range_create_cnt(buff, 20);
   range_t result = get_line(&buffer);
   ASSERT_FALSE( range_is_empty(&result));
}

TEST(range_tests, test2) {

   unsigned char buff[20] =  "Hello world\r\n";
   range_t buffer = range_create_cnt(buff, 20);
   range_t result = get_line(&buffer);

   ASSERT_FALSE( range_is_empty(&result));
   ASSERT_EQ(  range_size(&result), 11);
   ASSERT_EQ(*(result.begin), 'H');
}


TEST(range_tests, test1) {
   unsigned char buff[10];


   range_t r = range_create_cnt(buff, 10);
   ASSERT_EQ( range_size(&r), 10);
}



