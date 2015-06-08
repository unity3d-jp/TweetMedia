#include "TweetMedia.h"
#include "tmContext.h"


tmCLinkage tmExport tmContext* tmCreateContext()
{
    return new tmContext();
}

tmCLinkage tmExport void tmDestroyContext(tmContext *ctx)
{
    delete ctx;
}

tmCLinkage tmExport bool tmLoadCredentials(tmContext *ctx, const char *path)
{
    return ctx->loadCredentials(path);
}
tmCLinkage tmExport bool tmSaveCredentials(tmContext *ctx, const char *path)
{
    return ctx->saveCredentials(path);
}


tmCLinkage tmExport tmAuthState tmVerifyCredentials(tmContext *ctx)
{
    return ctx->verifyCredentials();
}
tmCLinkage tmExport void tmVerifyCredentialsAsync(tmContext *ctx)
{
    ctx->verifyCredentialsAsync();
}
tmCLinkage tmExport tmAuthState tmGetVerifyCredentialsState(tmContext *ctx)
{
    return ctx->getVerifyCredentialsState();
}

tmCLinkage tmExport tmAuthState  tmRequestAuthURL(tmContext *ctx, const char *consumer_key, const char *consumer_secret)
{
    return ctx->requestAuthURL(consumer_key, consumer_secret);
}
tmCLinkage tmExport void tmRequestAuthURLAsync(tmContext *ctx, const char *consumer_key, const char *consumer_secret)
{
    ctx->requestAuthURLAsync(consumer_key, consumer_secret);
}
tmCLinkage tmExport tmAuthState  tmGetRequestAuthURLState(tmContext *ctx)
{
    return ctx->getRequestAuthURLState();
}

tmCLinkage tmExport tmAuthState tmEnterPin(tmContext *ctx, const char *pin)
{
    return ctx->enterPin(pin);
}
tmCLinkage tmExport void tmEnterPinAsync(tmContext *ctx, const char *pin)
{
    ctx->enterPinAsync(pin);
}
tmCLinkage tmExport tmAuthState tmGetEnterPinState(tmContext *ctx)
{
    return ctx->getEnterPinState();
}


tmCLinkage tmExport bool tmAddMedia(tmContext *ctx, const void *data, int data_size, twitCurlTypes::eTwitCurlMediaType mtype)
{
    return ctx->addMedia(data, data_size, mtype);
}
tmCLinkage tmExport bool tmAddMediaFile(tmContext *ctx, const char *path)
{
    return ctx->addMediaFile(path);
}
tmCLinkage tmExport void tmClearMedia(tmContext *ctx)
{
    ctx->clearMedia();
}

tmCLinkage tmExport int tmTweet(tmContext *ctx, const char *message)
{
    return ctx->tweet(message);
}
tmCLinkage tmExport int tmTweetAsync(tmContext *ctx, const char *message)
{
    return ctx->tweetAsync(message);
}
tmCLinkage tmExport tmTweetState tmGetTweetState(tmContext *ctx, int thandle)
{
    return ctx->getTweetStatus(thandle);
}
tmCLinkage tmExport void tmReleaseTweetCache(tmContext *ctx, int thandle)
{
    ctx->eraseTweetCache(thandle);
}
