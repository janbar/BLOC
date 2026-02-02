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

#if (defined(_WIN32) || defined(_WIN64) || defined(_M_ARM64))
#define __WINDOWS__
#endif

#include "spawn_process.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PERROR(a) fprintf(stderr, a)
#define PERRORF(a, ...) fprintf(stderr, a, __VA_ARGS__)

#if !defined(__WINDOWS__)
/*****************************************************************************/
/*                                   UNIX                                    */
/*****************************************************************************/
#include <unistd.h>
#include <spawn.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define PIPE_RD 0
#define PIPE_WR 1
#define TICK_MS 100

int spawn_callback(int out_RD, int in_WR,
                   void* cb_handle,
                   spawn_send_callback cb_send,
                   spawn_recv_callback cb_recv);

int spawn_process(char* argv[], char* envv[],
                  void* cb_handle,
                  spawn_send_callback cb_send,
                  spawn_recv_callback cb_recv,
                  int* exit_code)
{
  posix_spawn_file_actions_t action;
  pid_t pid;
  int ret;
  int stdin_pipe[2];
  int stdout_pipe[2];

  if(pipe(stdout_pipe) != 0)
  {
    PERROR("pipe stdout failed!\n");
    return (-1);
  }

  if (pipe(stdin_pipe) != 0)
  {
    PERROR("pipe stdin failed!\n");
    return (-1);
  }

  // define actions to configure file io
  posix_spawn_file_actions_init(&action);
  posix_spawn_file_actions_addclose(&action, stdin_pipe[PIPE_WR]);
  posix_spawn_file_actions_addclose(&action, stdout_pipe[PIPE_RD]);

  posix_spawn_file_actions_adddup2(&action, stdin_pipe[PIPE_RD], STDIN_FILENO);
  posix_spawn_file_actions_adddup2(&action, stdout_pipe[PIPE_WR], STDOUT_FILENO);

  posix_spawn_file_actions_addclose(&action, stdin_pipe[PIPE_RD]);
  posix_spawn_file_actions_addclose(&action, stdout_pipe[PIPE_WR]);

  if (posix_spawnp(&pid, argv[0], &action, NULL, argv, envv) != 0)
  {
    PERRORF("spawn process failed (%d)!\n", errno);
    return (-1);
  }

  // close unused
  close(stdin_pipe[PIPE_RD]);
  close(stdout_pipe[PIPE_WR]);

  // pipes will be closed on return
  ret = spawn_callback(stdout_pipe[PIPE_RD], stdin_pipe[PIPE_WR],
                       cb_handle, cb_send, cb_recv);

  if (ret == SPAWN_SUCCESS)
  {
    waitpid(pid, exit_code, 0);
  }
  else
  {
    if (ret == SPAWN_TIMEOUT)
      PERROR("process timed out!\n");
    else if (ret == SPAWN_STOPPED)
      PERROR("process stopped!\n");

    if (kill(pid, SIGABRT) == 0)
      PERROR("process aborted!\n");
    else
      PERRORF("process abort failed (%d)!\n", errno);
    *exit_code = (-1);
  }

  posix_spawn_file_actions_destroy(&action);

  return ret;
}

int spawn_callback(int out_RD, int in_WR,
                   void* cb_handle,
                   spawn_send_callback cb_send,
                   spawn_recv_callback cb_recv)
{
  struct pollfd pfds[2];
  int n;
  int ret;
  int in_rcv = 0;
  char in_buf;

  pfds[0].fd = out_RD;
  pfds[0].events = POLLIN;
  pfds[1].fd = in_WR;
  pfds[1].events = POLLOUT;

  n = 2;

  while ((ret = poll(pfds, n, TICK_MS)) >= 0)
  {
    if (ret == 0)
      continue;
    else if ((pfds[0].revents & (POLLERR | POLLNVAL)))
      ret = SPAWN_ERROR;
    else if ((pfds[0].revents & POLLHUP))
    {
      // flush output and finish
      if (cb_send)
      {
        char c;
        while (read(pfds[0].fd, &c, 1) > 0)
          cb_send(cb_handle, &c, 1);
      }
      ret = SPAWN_SUCCESS;
    }
    else if ((pfds[0].revents & POLLIN))
    {
      char out[512];
      int rl = (int)read(pfds[0].fd, out, sizeof(out));
      if (rl == 0)
        ret = SPAWN_SUCCESS;
      else if (!cb_send)
        continue;
      else
      {
        int rs = cb_send(cb_handle, out, rl);
        if (rs > 0)
          continue;
        else if (rs == 0)
          ret = SPAWN_STOPPED; // stop requested
        else
          ret = SPAWN_ERROR; // abort requested
      }
    }
    else if ((pfds[1].revents & (POLLERR | POLLNVAL)))
      ret = SPAWN_ERROR;
    else if ((pfds[1].revents & POLLHUP))
      ret = SPAWN_ERROR;
    else if ((pfds[1].revents & POLLOUT))
    {
      if (in_rcv || (cb_recv && cb_recv(cb_handle, &in_buf) > 0))
        in_rcv = (write(pfds[1].fd, &in_buf, 1) == 1 ? 0 : 1);
      else
      {
        // stop polling
        n = 1;
        // flush input
        if (close(pfds[1].fd))
        {
          ret = SPAWN_ERROR;
          break;
        }
      }
      continue;
    }

    break;
  }

  if (n > 1)
    close(in_WR);
  close(out_RD);
  return ret;
}

#else
/*****************************************************************************/
/*                                 WINDOWS                                   */
/*****************************************************************************/
#include <Windows.h>

#define TICK_MS 10

int spawn_callback(HANDLE out_rd, HANDLE in_wr,
                   void* cb_handle,
                   spawn_send_callback cb_send,
                   spawn_recv_callback cb_recv);

int spawn_process(char* argv[], char* envv[],
                  void* cb_handle,
                  spawn_send_callback cb_send,
                  spawn_recv_callback cb_recv,
                  int* exit_code)
{
  HANDLE childStd_OUT_Rd = NULL;
  HANDLE childStd_OUT_Wr = NULL;
  HANDLE childStd_IN_Rd = NULL;
  HANDLE childStd_IN_Wr = NULL;
  SECURITY_ATTRIBUTES sa;
  PROCESS_INFORMATION procInfo;
  STARTUPINFO startInfo;
  char* cmdLine;
  char* envBlck;
  char* const* ptr;
  int len;
  int pos;
  int ret;

  // Set the child command line
  ptr = argv;
  len = 0;
  while (*ptr)
    len += strlen(*ptr++) + 1;

  cmdLine = (char*)malloc(len + 1);
  if (!cmdLine)
  {
    PERROR("allocate memory failed!\n");
    return SPAWN_ERROR;
  }

  pos = 0;
  while (*argv)
  {
    int l = strlen(*argv);
    memcpy(cmdLine + pos, *argv, l);
    pos += l;
    cmdLine[pos++] = ' ';
    cmdLine[pos] = '\0';
    argv += 1;
  }

  // Set the child environment block
  ptr = envv;
  len = 0;
  while (*ptr)
    len += strlen(*ptr++) + 1;

  envBlck = (char*)malloc(len + 1);
  if (!envBlck)
  {
    PERROR("allocate memory failed!\n");
    // free handles
    free(cmdLine);
    return SPAWN_ERROR;
  }

  pos = 0;
  while (*envv)
  {
    int l = strlen(*envv);
    memcpy(envBlck + pos, *envv, l);
    pos += l;
    envBlck[pos++] = '\0';
    envBlck[pos] = '\0';
    envv += 1;
  }

  // Set the bInheritHandle flag so pipe handles are inherited
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;

  // Create a pipe for the child process's STDOUT
  if (!CreatePipe(&childStd_OUT_Rd, &childStd_OUT_Wr, &sa, 0))
  {
    PERROR("CreatePipe stdout failed!\n");
    // free handles
    free(cmdLine);
    free(envBlck);
    return SPAWN_ERROR;
  }

  // Ensure the read handle to the pipe for STDOUT is not inherited
  if (!SetHandleInformation(childStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
  {
    PERROR("SetHandleInformation stdout failed!\n");
    // free handles
    CloseHandle(childStd_OUT_Rd);
    CloseHandle(childStd_OUT_Wr);
    free(cmdLine);
    free(envBlck);
    return SPAWN_ERROR;
  }

  // Create a pipe for the child process's STDIN
  if (!CreatePipe(&childStd_IN_Rd, &childStd_IN_Wr, &sa, 0))
  {
    PERROR("CreatePipe stdin failed!\n");
    // free handles
    CloseHandle(childStd_OUT_Rd);
    CloseHandle(childStd_OUT_Wr);
    free(cmdLine);
    free(envBlck);
    return SPAWN_ERROR;
  }

  // Ensure the write handle to the pipe for STDIN is not inherited.
  if (!SetHandleInformation(childStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
  {
    PERROR("SetHandleInformation stdin failed!\n");
    // free handles
    CloseHandle(childStd_IN_Rd);
    CloseHandle(childStd_IN_Wr);
    CloseHandle(childStd_OUT_Rd);
    CloseHandle(childStd_OUT_Wr);
    free(cmdLine);
    free(envBlck);
    return SPAWN_ERROR;
  }

  // Set up members of the PROCESS_INFORMATION structure
  ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));

  // Set up members of the STARTUPINFO structure
  ZeroMemory(&startInfo, sizeof(STARTUPINFO));
  startInfo.cb = sizeof(STARTUPINFO);
  startInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  startInfo.hStdOutput = childStd_OUT_Wr;
  startInfo.hStdInput = childStd_IN_Rd;
  startInfo.dwFlags |= STARTF_USESTDHANDLES;

  // Create the child process.
  if (!CreateProcess(NULL,
      cmdLine,       // command line
      NULL,          // process security attributes
      NULL,          // primary thread security attributes
      TRUE,          // handles are inherited
      0,             // creation flags
      envBlck,       // use parent's environment
      NULL,          // use parent's current directory
      &startInfo,    // STARTUPINFO pointer
      &procInfo))    // receives PROCESS_INFORMATION
  {
    PERROR("CreateProcess failed!\n");
    // free handles
    CloseHandle(childStd_IN_Rd);
    CloseHandle(childStd_IN_Wr);
    CloseHandle(childStd_OUT_Rd);
    CloseHandle(childStd_OUT_Wr);
    free(envBlck);
    free(cmdLine);
    return SPAWN_ERROR;
  }

  // it should be closed now
  CloseHandle(childStd_OUT_Wr);
  CloseHandle(childStd_IN_Rd);

  // pipes will be closed on return
  ret = spawn_callback(childStd_OUT_Rd, childStd_IN_Wr,
                       cb_handle, cb_send, cb_recv);

  if (ret == SPAWN_SUCCESS)
  {
    DWORD result;
    WaitForSingleObject(procInfo.hProcess, TICK_MS);
    GetExitCodeProcess(procInfo.hProcess, &result);
    *exit_code = result;
  }
  else
  {
    if (ret == SPAWN_TIMEOUT)
      PERROR("process timed out!\n");
    else if (ret == SPAWN_STOPPED)
      PERROR("process stopped!\n");

    if (TerminateProcess(procInfo.hProcess, 1))
      PERROR("process aborted!\n");
    else
      PERROR("process abort failed!\n");
    *exit_code = (-1);
  }

  // free handles
  CloseHandle(procInfo.hProcess);
  CloseHandle(procInfo.hThread);
  free(envBlck);
  free(cmdLine);

  return ret;
}

int spawn_callback(HANDLE out_rd, HANDLE in_wr,
                   void* cb_handle,
                   spawn_send_callback cb_send,
                   spawn_recv_callback cb_recv)
{
  int ret;
  BOOL in_data = TRUE;
  BOOL in_rcv = FALSE;
  char in_buf;

  ret = SPAWN_STOPPED;
  for (;;)
  {
    // Write to the pipe that is the standard input for a child process.
    // Data is written to the pipe's buffers, so it is not necessary to
    // wait until the child process is running before writing data.
    while (in_data)
    {
      DWORD wc;
      if (!in_rcv)
      {
        in_data = (cb_recv && cb_recv(cb_handle, &in_buf) > 0);
        if (!in_data)
        {
          if (CloseHandle(in_wr))
            break;
          PERROR("close pipe failed!\n");
          CloseHandle(out_rd);
          return SPAWN_ERROR;
        }
      }
      if (WriteFile(in_wr, &in_buf, 1, &wc, NULL) && wc == 1)
      {
        in_rcv = FALSE;
        continue;
      }
      in_rcv = TRUE; // try writing again
      break; // pipe buffer is full
    }

    // Read nowait avaialable data from the pipe that is standard output
    // for a child process.
    {
      DWORD rc;
      DWORD rl;
      char out[512];

      if (!PeekNamedPipe(out_rd, NULL, 0, NULL, &rc, NULL))
      {
        DWORD err = GetLastError();
        switch (err)
        {
        case ERROR_BROKEN_PIPE:
        case ERROR_HANDLE_EOF:
          ret = SPAWN_SUCCESS;
          break;
        default:
          PERRORF("peek pipe failed (%d)!\n", err);
          ret = SPAWN_ERROR;
        }
        break;
      }

      // has available data to read ?
      if (rc <= 0)
      {
        Sleep(TICK_MS);
        continue;
      }

      // read pipe
      rl = (rc > sizeof(out) ? sizeof(out) : rc);
      if (ReadFile(out_rd, out, rl, &rc, NULL))
      {
        if (rc > 0 && cb_send)
        {
          int rs = cb_send(cb_handle, out, rc);
          if (rs > 0)
            continue;
          else if (rs == 0)
            ret = SPAWN_STOPPED; // stop requested
          else
            ret = SPAWN_ERROR; // abort requested
          break;
        }
      }
      else
      {
        DWORD err = GetLastError();
        PERRORF("read pipe failed (%d)\n", err);
        ret = SPAWN_ERROR;
        break;
      }
    }
  }

  if (in_data)
    CloseHandle(in_wr);
  CloseHandle(out_rd);
  return ret;
}
#endif
