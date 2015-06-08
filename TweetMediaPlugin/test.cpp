#include "TweetMedia.h"

#define CredentialsPath "credentials.txt"


class TestBase
{
public:
    TestBase()
    {
        m_ctx = tmCreateContext();
    }

    virtual ~TestBase()
    {
        tmDestroyContext(m_ctx);
    }

    void saveCredentials()
    {
        tmSaveCredentials(m_ctx, CredentialsPath);
    }

    void loadCredentials()
    {
        tmLoadCredentials(m_ctx, CredentialsPath);
    }

    void addMedia(std::string &bin, twitCurlTypes::eTwitCurlMediaType mtype)
    {
        tmAddMedia(m_ctx, &bin[0], bin.size(), mtype);
    }

    void addMediaFile(const char *path)
    {
        tmAddMediaFile(m_ctx, path);
    }

    virtual tmAuthState verifyCredentials() = 0;
    virtual tmAuthState requestAuthURL(const char *consumer_key, const char *consumer_secret) = 0;
    virtual tmAuthState enterPin(const char *pin) = 0;
    virtual tmTweetState tweet(const char *message) = 0;

protected:
    tmContext *m_ctx;
};


class TestSync : public TestBase
{
public:
    tmAuthState verifyCredentials() override
    {
        return tmVerifyCredentials(m_ctx);
    }

    tmAuthState requestAuthURL(const char *consumer_key, const char *consumer_secret) override
    {
        return tmRequestAuthURL(m_ctx, consumer_key, consumer_secret);
    }

    tmAuthState enterPin(const char *pin) override
    {
        return tmEnterPin(m_ctx, pin);
    }

    tmTweetState tweet(const char *message) override
    {
        int th = tmTweet(m_ctx, message);
        return tmGetTweetState(m_ctx, th);
    }
};


class TestAsync : public TestBase
{
public:
    tmAuthState verifyCredentials() override
    {
        tmAuthState r;
        tmVerifyCredentialsAsync(m_ctx);
        waitFor([&](){
            r = tmGetVerifyCredentialsState(m_ctx);
            return r.code != tmEStatusCode_InProgress;
        });
        return r;
    }

    tmAuthState requestAuthURL(const char *consumer_key, const char *consumer_secret) override
    {
        tmAuthState r;
        tmRequestAuthURLAsync(m_ctx, consumer_key, consumer_secret);
        waitFor([&](){
            r = tmGetRequestAuthURLState(m_ctx);
            return r.code != tmEStatusCode_InProgress;
        });
        return r;
    }

    tmAuthState enterPin(const char *pin) override
    {
        tmAuthState r;
        tmEnterPinAsync(m_ctx, pin);
        waitFor([&](){
            r = tmGetEnterPinState(m_ctx);
            return r.code != tmEStatusCode_InProgress;
        });
        return r;
    }

    tmTweetState tweet(const char *message) override
    {
        tmTweetState r;
        int th = tmTweetAsync(m_ctx, message);
        waitFor([&](){
            r = tmGetTweetState(m_ctx, th);
            bool ret = r.code != tmEStatusCode_InProgress;
            if (!ret) { printf("in progress...\n"); }
            return ret;
        }, 2000);
        return r;
    }

private:
    template<class F>
    void waitFor(const F& f, int millisec=100)
    {
        while (!f()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
        }
    }
};




void DoTest(TestBase &test)
{
    char buf1[512];
    char buf2[512];

    test.loadCredentials();

    bool auth = false;
    auth = test.verifyCredentials().code == tmEStatusCode_Succeeded;
    if (!auth)
    {
        printf("consumer key:\n");
        gets(buf1);
        printf("consumer secret:\n");
        gets(buf2);

        auto stat = test.requestAuthURL(buf1, buf2);
        if (stat.code != tmEStatusCode_Succeeded) {
            printf("error: %s\n", stat.error_message);
            return;
        }

        printf("authorize URL: %s\n", stat.auth_url);
        printf("pin:\n");
        gets(buf1);

        stat = test.enterPin(buf1);
        if (stat.code != tmEStatusCode_Succeeded) {
            printf("error: %s\n", stat.error_message);
            return;
        }
        test.saveCredentials();
        auth = true;
    }

    if (auth)
    {
        std::string stat;

        printf("media:\n");
        gets(buf1);
        if (isprint(buf1[0])) {
            printf("on memory? [y][n]:\n");
            gets(buf2);
            if (buf2[0]=='y') {
                std::string media;
                if (tmFileToString(media, buf1)) {
                    test.addMedia(media, tmGetMediaTypeByFilename(buf1));
                }
            }
            else {
                test.addMediaFile(buf1);
            }
        }

        //tcFileToString(stat, "stat.txt");
        printf("tweet:\n");
        gets(buf1);
        stat = buf1;

        auto st = test.tweet(stat.c_str());
        if (st.code == tmEStatusCode_Succeeded) {
            printf("succeeded.\n");
        }
        else if (st.code == tmEStatusCode_Failed) {
            printf("failed. %s\n", st.error_message);
        }
    }
}


int main(int argc, char *argv[])
{
    char buf1[512];
    printf("[s] to sync [a] to async:\n");
    gets(buf1);

    if (buf1[0]=='a') {
        printf("TestAsync\n");
        TestAsync test;
        DoTest(test);
    }
    else {
        printf("TestSync\n");
        TestSync test;
        DoTest(test);
    }

    printf("done.\n");
    gets(buf1);
}
