#pragma once
#ifndef ALLHEADERS_H
#define ALLHEADERS_H

#ifdef _WIN32

#include <process.h>
#include <winsock.h>

#include <Windows.h>
#include <thread>
#include <mutex>
#include<fstream>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <stack>
#endif