#pragma once

//i'm building the server and the client in the same project
#define BUILD_SERVER	//build the server
//#define BUILD_CLIENT	//build the client

#if defined(BUILD_SERVER)
#undef BUILD_CLIENT
#else
#undef BUILD_SERVER
#endif