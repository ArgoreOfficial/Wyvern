#pragma once
#include <stdio.h>
// std studio -david 2023

#define LOG_RESET printf("\033[0m")
#define LOG_WHITE printf("\033[0;37m")
#define LOG_YELLOW printf("\033[0;33m")
#define LOG_RED printf("\033[0;31m")
#define LOG_CYAN printf("\033[0;36m")

#define LOG_MESSAGE(str) LOG_WHITE; printf("    "); printf(str); LOG_RESET
#define LOG_WARNING(str) LOG_YELLOW; printf("[!] "); printf(str); LOG_RESET
#define LOG_ERROR(str) LOG_RED; printf("[X] "); printf(str); LOG_RESET
#define LOG_WYVERN_MESSAGE(str) LOG_CYAN; printf("[*] "); printf(str); LOG_RESET
