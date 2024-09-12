#pragma once
#define MAX_COUNT_STATUS 100
#define PASSWORD_OK 101
#define PASSWORD_INVALID 102

//additional password requirement type ID, 'C' = character, 'N' = number, 'S' = symbol
#define REQ_STARTS_C 500 // require starts with
#define REQ_STARTS_N 501
#define REQ_STARTS_S 502
#define REQ_END_C 503
#define REQ_END_N 504
#define REQ_END_S 505
#define NO_C 506 //not allowed
#define NO_N 507
#define NO_S 508
//for additional requirements such as 'starts with "123"' and 'ends with "abc"' and 'the fifth char is a "b"' another function is required
//#define PREFIX_TYPE 600
//#define POSTFIX_TYPE 601
#define INDEX_CHAR_TYPE 602
#define UPPERCASE_PERMUTATOR 1100
#define LEET_PERMUTATOR 1101
#define PARADIGM_PERMUTATOR 1102
#define KEYWORDCOMBO_PERMUTATOR 1103
#define BASEWORDCUTTER_PERMUTATOR 1104
#define PREFIX_REQUIREMENT 1105
#define SUFFIX_REQUIREMENT 1106
#define INDEX_REQUIREMENT 1107