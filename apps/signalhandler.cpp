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

#include "signalhandler.h"

#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

SignalHandler::cb_t SignalHandler::_cb = { .fct = nullptr, .hdl = nullptr };

bool SignalHandler::catchSignal(int signal)
{
  struct sigaction act;
  memset(&act, '\0', sizeof(struct sigaction));
  act.sa_sigaction = &handler;
  act.sa_flags |= SA_SIGINFO;
  sigemptyset(&act.sa_mask);
  return (sigaction(signal, &act, 0) == 0);
}

void SignalHandler::omitSignal(int signal)
{
  struct sigaction act;
  memset(&act, '\0', sizeof(struct sigaction));
  act.sa_handler = SIG_DFL;
  sigemptyset(&act.sa_mask);
  sigaction(signal, &act, 0);
}

void SignalHandler::handler(int signal, siginfo_t * info, void * data)
{
  (void)info;
  (void)data;
  if (_cb.fct != nullptr)
    _cb.fct(signal, _cb.hdl);
}
