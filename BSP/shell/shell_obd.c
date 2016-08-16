#include "shell_obd.h"
#include "includes.h"
#include "globals.h"


shell_cmds obd_shell_cmds = {
     5,
     {
          {
               "help",
               " Print available shell commands",
               shell_cmd_help,
          },
          /*
          {
               "eon",
               "  Turn echo on",
               shell_cmd_eon,
          },
          {
               "eoff",
               " Turn echo off",
               shell_cmd_eoff,
          },
          */
          {
              "filter",
              " filter can id  ",
              shell_filter_can,
          },
          {
              "send",
              " Send can data ",
              shell_send_can,
          },
          {
              "getaddr",
              " Get immo addr ",
              shell_get_addr,
          },
          {
              "setaddr",
              " Set immo addr ",
              shell_set_addr,
          },
     },
};

int do_echo = 1;

int shell_cmd_help(shell_cmd_args *args)
{
     int i;

     shell_ok();

     shell_println("Vehicle can tool:");

     for(i = 0; i < obd_shell_cmds.count; i++) {
          shell_print("- ");
          shell_print((char *)obd_shell_cmds.cmds[i].cmd);
          shell_print("  ");
          shell_println((char *)obd_shell_cmds.cmds[i].desc);

          // XBee seams not to be fast enough when it gets send a lot of data at once ...
          //delay(1000);
     }

     shell_eod();

     return 0;
}

int shell_cmd_eon(shell_cmd_args *args)
{
     shell_ok();

     shell_println("echo on");

     do_echo = 1;

     shell_eod();

     return 0;
}

int shell_cmd_eoff(shell_cmd_args *args)
{
     shell_ok();

     shell_println("echo off");

     do_echo = 0;

     shell_eod();

     return 0;
}

void usage_filter_can(void)
{
    shell_println("\tusage: filter [canid1] [canid2] ...");
}

int shell_filter_can(shell_cmd_args *args)
{
    shell_ok();

    return 0;
}

void usage_send_can(void)
{
    shell_println("\tusage: send [canid] xx xx xx xx ...");
}

int shell_send_can(shell_cmd_args *args)
{
    shell_ok();

    return 0;
}

int shell_get_addr(shell_cmd_args *args)
{
    printf("ADDR: 0x%02x\r\n", get_immo_addr());
    shell_ok();

    return 0;
}

char hexToValue(char hex)
{
    char mask;
    switch ((int)hex) {
      case (int)'0':
        mask = 0;
        break;
      case (int)'1':
        mask = 1;
        break;
      case (int)'2':
        mask = 2;
        break;
      case (int)'3':
        mask = 3;
        break;
      case (int)'4':
        mask = 4;
        break;
      case (int)'5':
        mask = 5;
        break;
      case (int)'6': 
        mask = 6;
        break;
      case (int)'7': 
        mask = 7;
        break;
      case (int)'8': 
        mask = 8;
        break;
      case (int)'9': 
        mask = 9;
        break;
      case (int)'a': 
      case (int)'A': 
        mask = 10;
        break;
      case (int)'b': 
      case (int)'B': 
        mask = 11;
        break;
      case (int)'c': 
      case (int)'C': 
        mask = 12;
        break;
      case (int)'d': 
      case (int)'D': 
        mask = 13;
        break;
      case (int)'e': 
      case (int)'E': 
        mask = 14;
        break;
      case (int)'f': 
      case (int)'F': 
        mask = 15;
        break;
      default:
        mask = -1;
        break;
    }

    return mask;
}

int shell_set_addr(shell_cmd_args *args)
{
    uint8_t addr = 0x00;
    if(args->count == 0) {
        printf("Usage: setaddr [xx]\r\n");
        shell_eod();
        return 1;
    }

    addr = 16 * hexToValue(args->args[0].val[0]);
    addr += hexToValue(args->args[0].val[1]);
    printf("set addr: 0x%02x\r\n", addr);
    set_immo_addr(addr);
    shell_ok();

    return 0;
}

void shell_prompt(void)
{
    shell_print("$> ");
}

int shell_process(char *cmd_line)
{
    return shell_process_cmds(&obd_shell_cmds, cmd_line);
}

void USART2_IRQHandler(void)
{
    uint8_t data;
    static char cmd_line[SHELL_MAX_CMD_LINE];
    static int i = 0;

    if(USART_GetITStatus(USART2, USART_IT_RXNE)
            != RESET) {
        data = USART_ReceiveData(USART2);

        if(do_echo) {
            printf("%c", data);
        }

        if(data == '\r') {
            shell_println("");
            if(i > 0) {
                cmd_line[i] = 0;

                if(shell_process(cmd_line) 
                        != SHELL_PROCESS_OK) {
                    //shell_error();
                    //shell_eod();
                }
            }

            i = 0;

            shell_prompt();
        } else if(data != '\n') {
            cmd_line[i ++] = data;
        }

        if(i > SHELL_MAX_CMD_LINE - 1)
            i = 0;
    }

    if(USART_GetITStatus(USART2, USART_IT_TXE)
            != RESET) {
        USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    }
}
