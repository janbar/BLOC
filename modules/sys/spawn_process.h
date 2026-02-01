/*
 *      Copyright (C) 2026 Jean-Luc Barriere
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

#ifndef SPAWN_PROCESS_H_
#define SPAWN_PROCESS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define SPAWN_ERROR   (-1)
#define SPAWN_TIMEOUT 0
#define SPAWN_SUCCESS 1
#define SPAWN_STOPPED 2

typedef int (*spawn_send_callback)(void* handle, const char* data, int len);
typedef int (*spawn_recv_callback)(void* handle, char* c);

int spawn_process(char* argv[], char* envv[],
                  void* cb_handle,
                  spawn_send_callback cb_send,
                  spawn_recv_callback cb_recv,
                  int* exit_code);

#ifdef __cplusplus
}
#endif

#endif /* SPAWN_PROCESS_H_ */

