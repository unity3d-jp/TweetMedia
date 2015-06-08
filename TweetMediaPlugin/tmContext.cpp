#include <regex>
#include <picojson/picojson.h>
#include "TweetMedia.h"
#include "tmContext.h"


tmContext::tmContext()
    : m_stop(false)
    , m_auth_code(tmEStatusCode_Unknown)
{
    m_send_thread = std::thread([this](){ processTasks(); });
}

tmContext::~tmContext()
{
    m_stop = true;
    m_condition.notify_all();
    m_send_thread.join();
}


void tmContext::enqueueTask(const std::function<void()> &f)
{
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_tasks.push_back(std::function<void()>(f));
    }
    m_condition.notify_one();
}

void tmContext::processTasks()
{
    while (!m_stop)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            while (!m_stop && m_tasks.empty()) {
                m_condition.wait(lock);
            }
            if (m_stop) { return; }

            task = m_tasks.front();
            m_tasks.pop_front();
        }
        task();
    }
}


bool tmContext::loadCredentials(const char *path)
{
    if (path == nullptr) { return false; }
    std::ifstream f(path, std::ios::binary);
    if (!f) { return false; }

    std::string tmp;
    oAuth &oa = m_twitter.getOAuth();
    f >> tmp; oa.setConsumerKey(tmp);
    f >> tmp; oa.setConsumerSecret(tmp);
    f >> tmp; oa.setOAuthTokenKey(tmp);
    f >> tmp; oa.setOAuthTokenSecret(tmp);
    return true;
}

bool tmContext::saveCredentials(const char *path)
{
    if (path == nullptr) { return false; }
    std::ofstream f(path, std::ios::binary);
    if (!f) { return false; }

    std::string tmp;
    oAuth &oa = m_twitter.getOAuth();
    oa.getConsumerKey(tmp);     f << tmp << std::endl;
    oa.getConsumerSecret(tmp);  f << tmp << std::endl;
    oa.getOAuthTokenKey(tmp);   f << tmp << std::endl;
    oa.getOAuthTokenSecret(tmp);f << tmp << std::endl;
    return true;
}


void tmContext::setConsumerKeyAndSecret(const char *consumer_key, const char *consumer_secret)
{
    m_twitter.getOAuth().setConsumerKey(consumer_key);
    m_twitter.getOAuth().setConsumerSecret(consumer_secret);
}

void tmContext::setAccessToken(const char *token, const char *token_secret)
{
    m_twitter.getOAuth().setOAuthTokenKey(token);
    m_twitter.getOAuth().setOAuthTokenSecret(token_secret);
}

bool tmContext::getErrorMessage(std::string &dst, bool error_if_response_is_not_json)
{
    dst.clear();

    picojson::value v;
    std::string tmp;
    m_twitter.getLastWebResponse(tmp);
    std::string err = picojson::parse(v, tmp);
    if (!err.empty()) {
        if (error_if_response_is_not_json) {
            dst = err;
            return true;
        }
        else {
            return false;
        }
    }
    if(v.contains("errors")) {
        try {
            picojson::value obj = v.get("errors").get<picojson::array>()[0];
            dst = obj.get("message").get<std::string>();
        }
        catch (...) {}
        return true;
    }
    return false;
}


tmAuthState tmContext::verifyCredentials()
{
    tmEStatusCode code = tmEStatusCode_Failed;
    if (m_twitter.accountVerifyCredGet()) {
        code = getErrorMessage(m_auth_error) ? tmEStatusCode_Failed : tmEStatusCode_Succeeded;
    }
    m_auth_code = code;
    return getVerifyCredentialsState();
}

void tmContext::verifyCredentialsAsync()
{
    m_auth_code = tmEStatusCode_InProgress;
    enqueueTask([this](){ verifyCredentials(); });
}

tmAuthState tmContext::getVerifyCredentialsState()
{
    tmAuthState r = { m_auth_code, m_auth_error.c_str(), nullptr};
    return r;
}



tmAuthState tmContext::requestAuthURL(const char *consumer_key, const char *consumer_secret)
{
    tmEStatusCode code = tmEStatusCode_Failed;
    m_twitter.getOAuth().setConsumerKey(consumer_key);
    m_twitter.getOAuth().setConsumerSecret(consumer_secret);
    if (m_twitter.oAuthRequestToken(m_auth_url)) {
        code = getErrorMessage(m_auth_error) ? tmEStatusCode_Failed : tmEStatusCode_Succeeded;
    }
    m_auth_code = code;;
    return getRequestAuthURLState();
}

void tmContext::requestAuthURLAsync(const char *consumer_key_, const char *consumer_secret_)
{
    m_auth_code = tmEStatusCode_InProgress;
    std::string consumer_key = consumer_key_;
    std::string consumer_secret = consumer_secret_;
    enqueueTask([=](){ requestAuthURL(consumer_key.c_str(), consumer_secret.c_str()); });
}
tmAuthState tmContext::getRequestAuthURLState()
{
    tmAuthState r = { m_auth_code, m_auth_error.c_str(), m_auth_url.c_str() };
    return r;
}



tmAuthState tmContext::enterPin(const char *pin)
{
    tmEStatusCode code = tmEStatusCode_Failed;
    m_twitter.getOAuth().setOAuthPin(pin);
    if (m_twitter.oAuthAccessToken()) {
        std::string tmp;
        m_twitter.getLastWebResponse(tmp);
        if (tmp.find("oauth_token=") != std::string::npos) {
            code = tmEStatusCode_Succeeded;
            m_auth_error.clear();
        }
        else {
            m_auth_error = tmp;
        }
    }
    m_auth_code = code;
    return getEnterPinState();
}

void tmContext::enterPinAsync(const char *pin_)
{
    m_auth_code = tmEStatusCode_InProgress;
    std::string pin = pin_;
    enqueueTask([=](){ enterPin(pin.c_str()); });
}

tmAuthState tmContext::getEnterPinState()
{
    tmAuthState r = { m_auth_code, m_auth_error.c_str(), nullptr };
    return r;
}

bool tmContext::addMedia(const void *data, int data_size, twitCurlTypes::eTwitCurlMediaType mtype)
{
    auto md = tmMediaDataPtr(new tmMediaData());
    md->type = mtype;
    md->stream.reset(
        new std::istringstream(std::move(std::string((const char*)data, data_size)), std::ios::binary));

    if (!m_media) { m_media.reset(new tmMediaCont()); }
    m_media->push_back(md);
    return true;
}

bool tmContext::addMediaFile(const char *path)
{
    twitCurlTypes::eTwitCurlMediaType mtype = tmGetMediaTypeByFilename(path);
    if (mtype == twitCurlTypes::eTwitCurlMediaUnknown) { return false; }

    auto stream = tmIStreamPtr(new std::ifstream(path, std::ios::binary));
    if (!(*stream)) { return false; }

    auto md = tmMediaDataPtr(new tmMediaData());
    md->type = mtype;
    md->stream = stream;

    if (!m_media) { m_media.reset(new tmMediaCont()); }
    m_media->push_back(md);
    return true;
}

void tmContext::clearMedia()
{
    m_media.reset();
}

int tmContext::pushTweet(const char *message)
{
    // handle 0 == invalid
    if (m_tweets.empty()) { m_tweets.push_back(tmTweetData()); }
    int ret = (int)m_tweets.size();

    m_tweets.push_back(tmTweetData());
    tmTweetData &tw = m_tweets.back();
    tw.code = tmEStatusCode_InProgress;
    tw.tweet.status = message;
    tw.media = m_media;
    m_media.reset();
    return ret;
}

void tmContext::tweetImpl(tmTweetData &tw)
{
    tmEStatusCode code = tmEStatusCode_Failed;
    if (tw.media) {
        for (auto &media : *tw.media) {
            if (!m_twitter.uploadMedia(*media->stream, media->type, media->media_id, tw.error_message)) {
                break;
            }
            if (!tw.tweet.media_ids.empty()) { tw.tweet.media_ids += ","; }
            tw.tweet.media_ids += media->media_id;
        }
    }
    if (tw.error_message.empty()) {
        if (m_twitter.statusUpdate(tw.tweet)) {
            code = getErrorMessage(tw.error_message) ? tmEStatusCode_Failed : tmEStatusCode_Succeeded;
        }
    }
    tw.media.reset();
    tw.code = code;
}

int tmContext::tweet(const char *message)
{
    int t = pushTweet(message);
    tweetImpl(m_tweets[t]);
    return t;
}

int tmContext::tweetAsync(const char *message)
{
    int t = pushTweet(message);
    enqueueTask([this, t](){ tweetImpl(m_tweets[t]); });
    return t;
}

tmTweetState tmContext::getTweetStatus(int thandle)
{
    tmTweetState r = { tmEStatusCode_Unknown, nullptr };
    if (thandle <= m_tweets.size())
    {
        auto & tw = m_tweets[thandle];
        r.code = tw.code;
        r.error_message = tw.error_message.c_str();
    }
    return r;
}

void tmContext::eraseTweetCache(int thandle)
{
    if (thandle <= m_tweets.size() && thandle > 0)
    {
        m_tweets.erase(m_tweets.begin()+thandle);
    }
}

tmTweetData* tmContext::getTweetData(int thandle)
{
    if (thandle <= m_tweets.size())
    {
        return &m_tweets[thandle];
    }
    return nullptr;
}




bool tmFileToString(std::string &o_buf, const char *path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f) { return false; }
    f.seekg(0, std::ios::end);
    o_buf.resize(f.tellg());
    f.seekg(0, std::ios::beg);
    f.read(&o_buf[0], o_buf.size());
    return true;
}

twitCurlTypes::eTwitCurlMediaType tmGetMediaTypeByFilename(const char *path)
{
    std::regex png("\\.png$", std::regex::grep | std::regex::icase);
    std::regex jpg("\\.jpg$", std::regex::grep | std::regex::icase);
    std::regex gif("\\.gif$", std::regex::grep | std::regex::icase);
    std::regex webp("\\.webp$", std::regex::grep | std::regex::icase);
    std::regex mp4("\\.mp4$", std::regex::grep | std::regex::icase);
    std::cmatch match;
    if (std::regex_search(path, match, png)) { return twitCurlTypes::eTwitCurlMediaPNG; }
    if (std::regex_search(path, match, jpg)) { return twitCurlTypes::eTwitCurlMediaJPEG; }
    if (std::regex_search(path, match, gif)) { return twitCurlTypes::eTwitCurlMediaGIF; }
    if (std::regex_search(path, match, webp)){ return twitCurlTypes::eTwitCurlMediaWEBP; }
    if (std::regex_search(path, match, mp4)) { return twitCurlTypes::eTwitCurlMediaMP4; }
    return twitCurlTypes::eTwitCurlMediaUnknown;
}

