/*
 *      Copyright (C) 2024 Jean-Luc Barriere
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "winstub.h"
#include <windows.h>

static breaker_t * g_winbrk = nullptr;

BOOL WINAPI ConsoleHandler(DWORD ctrl)
{
  switch (ctrl)
  {
    // Handle the CTRL-C signal
  case CTRL_C_EVENT:
    sig_handler(SIGINT, g_winbrk);
    return TRUE;
  default:
    // pass signal on to the next handler
    return FALSE;
  }
}

bool init_ctrl_handler(breaker_t * brk)
{
  if (g_winbrk == nullptr)
  {
    g_winbrk = brk;
    return (bool) SetConsoleCtrlHandler(ConsoleHandler, TRUE);
  }
  return false;
}

extern void stop_ctrl_handler()
{
  if (g_winbrk != nullptr)
    SetConsoleCtrlHandler(ConsoleHandler, FALSE);
  g_winbrk = nullptr;
}
