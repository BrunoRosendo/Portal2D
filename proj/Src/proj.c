#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "xpm_files.h"
#include "timer.h"
#include "sprites.h"
#include "game.h"
#include "loop.h"
#include "rtc.h"
#include "uart.h"

int main(int argc, char *argv[])
{
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  //lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

// Proj should be initialized with the path to the resurces file
int (proj_main_loop)(int argc, char *argv[]) {

  if (argc != 1){
    printf("Invalid Syntax: Use lcom_run proj \"Path to the Resources' Folder\" or \"default\"\n");
    return 1;
  }

  if (strcmp(argv[0], "default") == OK){
    if (start_loop("/home/lcom/labs/proj/Src/Resources") != OK) return 1;
  }
  
  else{
    if (start_loop(argv[0]) != OK) return 1;
  }
  
  
  if (main_loop() != OK) return 1;
  if (end_loop() != OK) return 1;

  return 0;
}
