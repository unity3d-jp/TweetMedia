#ifndef TweetMedia_h
#define TweetMedia_h

#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include "libtwitcurl/twitcurl.h"

//#pragma comment(lib, "libeay32.lib")
//#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "normaliz.lib")
#pragma comment(lib, "twitcurl.lib")


#ifdef _WIN32
#define tmWindows
#endif // _WIN32

#define tmCLinkage extern "C"
#ifdef tmWindows
    #define tmExport __declspec(dllexport)
#else // tmWindows
    #define tmExport
#endif // tmWindows


class tmContext;

enum tmEStatusCode
{
    tmEStatusCode_Unknown,
    tmEStatusCode_InProgress,
    tmEStatusCode_Failed,
    tmEStatusCode_Succeeded,
};


struct tmAuthState
{
    tmEStatusCode code;
    const char *error_message;
    const char *auth_url;
};


struct tmTweetState
{
    tmEStatusCode code;
    const char *error_message;
};


tmCLinkage tmExport tmContext*      tmCreateContext();
tmCLinkage tmExport void            tmDestroyContext(tmContext *ctx);

tmCLinkage tmExport bool            tmLoadCredentials(tmContext *ctx, const char *path);
tmCLinkage tmExport bool            tmSaveCredentials(tmContext *ctx, const char *path);

tmCLinkage tmExport tmAuthState     tmVerifyCredentials(tmContext *ctx);
tmCLinkage tmExport void            tmVerifyCredentialsAsync(tmContext *ctx);
tmCLinkage tmExport tmAuthState     tmGetVerifyCredentialsState(tmContext *ctx);

tmCLinkage tmExport tmAuthState     tmRequestAuthURL(tmContext *ctx, const char *consumer_key, const char *consumer_secret);
tmCLinkage tmExport void            tmRequestAuthURLAsync(tmContext *ctx, const char *consumer_key, const char *consumer_secret);
tmCLinkage tmExport tmAuthState     tmGetRequestAuthURLState(tmContext *ctx);

tmCLinkage tmExport tmAuthState     tmEnterPin(tmContext *ctx, const char *pin);
tmCLinkage tmExport void            tmEnterPinAsync(tmContext *ctx, const char *pin);
tmCLinkage tmExport tmAuthState     tmGetEnterPinState(tmContext *ctx);

tmCLinkage tmExport bool            tmAddMedia(tmContext *ctx, const void *data, int data_size, twitCurlTypes::eTwitCurlMediaType mtype);
tmCLinkage tmExport bool            tmAddMediaFile(tmContext *ctx, const char *path);
tmCLinkage tmExport void            tmClearMedia(tmContext *ctx);

tmCLinkage tmExport int             tmTweet(tmContext *ctx, const char *message);
tmCLinkage tmExport int             tmTweetAsync(tmContext *ctx, const char *message);
tmCLinkage tmExport tmTweetState    tmGetTweetState(tmContext *ctx, int thandle);
tmCLinkage tmExport void            tmReleaseTweetCache(tmContext *ctx, int thandle);


tmExport bool                                tmFileToString(std::string &o_buf, const char *path);
tmExport twitCurlTypes::eTwitCurlMediaType   tmGetMediaTypeByFilename(const char *path);

#endif // TweetMedia_h
