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
#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include <signal.h>

class SignalHandler
{
  typedef void (*callback)(int signal, void* handle);
public:
  SignalHandler() = default;
  virtual ~SignalHandler() = default;

  bool catchSignal(int signal);
  void omitSignal(int signal);

  void setCallback(callback cb, void* handle)
  {
    _cb.fct = cb;
    _cb.hdl = handle;
  };

private:
  SignalHandler(const SignalHandler&);
  SignalHandler& operator=(const SignalHandler&);

  typedef struct { callback fct; void* hdl; } cb_t;
  static cb_t _cb;
  static void handler(int signal, siginfo_t * info, void * data);
};

#endif // SIGNALHANDLER_H
