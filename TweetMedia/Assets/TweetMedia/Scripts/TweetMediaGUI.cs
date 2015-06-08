using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.UI;

public class TweetMediaGUI : MonoBehaviour
{
    public bool m_begin_auth_on_start = true;

    public TweetMedia m_tweet_media;
    public GameObject m_ui_auth;
    public GameObject m_ui_tweet;
    public UnityEngine.UI.Text m_text_status;
    public UnityEngine.UI.InputField m_input_pin;
    public UnityEngine.UI.InputField m_input_message;
    public UnityEngine.UI.Button m_button_tweet;


    string status_text
    {
        get { return m_text_status != null ? m_text_status.text : ""; }
        set { if (m_text_status != null) { m_text_status.text = value; } }
    }

    public virtual void OnChangeAuthState(TweetMedia.AuthStateCode code)
    {
        switch(code)
        {
            case TweetMedia.AuthStateCode.VerifyingCredentialsBegin:
                status_text = "verifying credentials: begin";
                break;
            case TweetMedia.AuthStateCode.VerifyingCredentialsSucceeded:
                status_text = "verifying credentials: succeeded";
                m_ui_tweet.SetActive(true);
                break;
            case TweetMedia.AuthStateCode.VerifyingCredentialsFailed:
                status_text = "verifying credentials: failed\n" + m_tweet_media.error_message;
                break;

            case TweetMedia.AuthStateCode.RequestAuthURLBegin:
                status_text = "request auth URL: begin";
                break;
            case TweetMedia.AuthStateCode.RequestAuthURLSucceeded:
                status_text = "request auth URL: succeeded";
                m_ui_auth.SetActive(true);
                break;
            case TweetMedia.AuthStateCode.RequestAuthURLFailed:
                status_text = "request auth URL: failed\n" + m_tweet_media.error_message;
                break;

            case TweetMedia.AuthStateCode.EnterPinBegin:
                status_text = "enter pin: begin";
                break;
            case TweetMedia.AuthStateCode.EnterPinSucceeded:
                status_text = "enter pin: succeeded";
                m_ui_auth.SetActive(false);
                m_ui_tweet.SetActive(true);
                break;
            case TweetMedia.AuthStateCode.EnterPinFailed:
                status_text = "enter pin: failed\n" + m_tweet_media.error_message;
                break;
        }
    }


    public virtual void Start()
    {
        m_tweet_media.on_change_auth_state = OnChangeAuthState;
        m_tweet_media.on_change_tweet_state = OnChangeTweetState;
        if (m_begin_auth_on_start)
        {
            BeginAuthorization();
        }
    }

    public virtual void BeginAuthorization()
    {
        m_tweet_media.BeginAuthorize();
    }

    public virtual void OpenAuthURL()
    {
        Application.OpenURL(m_tweet_media.auth_url);
    }

    public virtual void EnterPin()
    {
        m_tweet_media.pin = m_input_pin.text;
    }


    public virtual void OnChangeTweetState(TweetMedia.TweetStateCode code)
    {
        switch (code)
        {
            case TweetMedia.TweetStateCode.Begin:
                status_text = "tweet: in progress";
                m_input_message.interactable = false;
                m_button_tweet.interactable = false;
                break;
            case TweetMedia.TweetStateCode.Succeeded:
                m_input_message.text = "";
                status_text = "tweet: succeeded";
                m_input_message.interactable = true;
                m_button_tweet.interactable = true;
                break;
            case TweetMedia.TweetStateCode.Failed:
                status_text = "tweet: failed\n" + m_tweet_media.error_message;
                m_input_message.interactable = true;
                m_button_tweet.interactable = true;
                break;
        }
    }

    public virtual void BeginTweet()
    {
        m_tweet_media.BeginTweet(m_input_message.text);
    }
}
