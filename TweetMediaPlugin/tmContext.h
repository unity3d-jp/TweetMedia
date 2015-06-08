#ifndef tmContext_h
#define tmContext_h

#include <deque>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

typedef std::shared_ptr<std::istream> tmIStreamPtr;

struct tmMediaData
{
    twitCurlTypes::eTwitCurlMediaType type;
    tmIStreamPtr stream;
    std::string media_id;
};
typedef std::shared_ptr<tmMediaData> tmMediaDataPtr;
typedef std::vector<tmMediaDataPtr> tmMediaCont;
typedef std::shared_ptr<tmMediaCont> tmMediaContPtr;


struct tmTweetData
{
    tmEStatusCode code;
    tmMediaContPtr media;
    twitStatus tweet;
    std::string error_message;

    tmTweetData() : code(tmEStatusCode_Unknown) {}
};
typedef std::vector<tmTweetData> tmTweetDataCont;


class tmContext
{
public:
    tmContext();
    ~tmContext();

    bool loadCredentials(const char *path);
    bool saveCredentials(const char *path);
    void setConsumerKeyAndSecret(const char *consumer_key, const char *consumer_secret);
    void setAccessToken(const char *token, const char *token_secret);

    tmAuthState     verifyCredentials();
    void            verifyCredentialsAsync();
    tmAuthState     getVerifyCredentialsState();

    tmAuthState     requestAuthURL(const char *consumer_key, const char *consumer_secret);
    void            requestAuthURLAsync(const char *consumer_key, const char *consumer_secret);
    tmAuthState     getRequestAuthURLState();

    tmAuthState     enterPin(const char *pin);
    void            enterPinAsync(const char *pin);
    tmAuthState     getEnterPinState();

    bool            addMedia(const void *data, int data_size, twitCurlTypes::eTwitCurlMediaType mtype);
    bool            addMediaFile(const char *path);
    void            clearMedia();

    int             tweet(const char *message);
    int             tweetAsync(const char *message);
    tmTweetState    getTweetStatus(int thandle);
    void            eraseTweetCache(int thandle);
    tmTweetData*    getTweetData(int thandle);

private:
    bool getErrorMessage(std::string &dst, bool error_if_response_is_not_json = false);
    int pushTweet(const char *message);
    void tweetImpl(tmTweetData &tw);
    void enqueueTask(const std::function<void()> &f);
    void processTasks();

private:
    twitCurl m_twitter;

    tmEStatusCode m_auth_code;
    std::string m_auth_url;
    std::string m_auth_error;

    tmMediaContPtr m_media;
    tmTweetDataCont m_tweets;

    std::thread m_send_thread;
    std::mutex m_queue_mutex;
    std::condition_variable m_condition;
    std::deque<std::function<void()>> m_tasks;
    bool m_stop;
};

#endif // tmContext_h
