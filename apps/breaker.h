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
#ifndef BREAKER_H
#define BREAKER_H

#include <signal.h>
#include <blocc/context.h>

typedef struct { bool state; bloc::Context * ctx; } breaker_t;

static void sig_handler(int signal, void * handle)
{
  if (signal == SIGINT)
  {
    breaker_t * brk = static_cast<breaker_t*>(handle);
    /* if breaker has been set then exit now */
    if (brk->state)
      exit(signal);
    /* set breaker and stop running program */
    brk->state = true;
    brk->ctx->returnCondition(true);
  }
}

#endif /* BREAKER_H */
