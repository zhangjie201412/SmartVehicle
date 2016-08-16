#ifndef __SHELL_OBD_H__
#define __SHELL_OBD_H__

#include "shell.h"

int shell_cmd_help(shell_cmd_args *args);
int shell_cmd_eon(shell_cmd_args *args);
int shell_cmd_eoff(shell_cmd_args *args);
int shell_send_can(shell_cmd_args *args);
int shell_filter_can(shell_cmd_args *args);
int shell_get_addr(shell_cmd_args *args);
int shell_set_addr(shell_cmd_args *args);

#endif
