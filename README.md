# TweetMedia

ゲーム内から Twitter へ画像やメッセージを投稿する Unity 用プラグインです。  
Twitter インテグレーションは他にもいくつかありますが、TweetMedia は画像と動画 (アニメ gif、mp4) の投稿まで対応しているのが特徴です。
ゲーム内からメッセージとスクリーンショットを投稿することを目的としたもので、現状それ以外の機能はありません。  
Vista 以降の Windows (32bit & 64bit) で動作します。ソースコードレベルでは CURL と C++11 が使える環境であればコンパイルできて動作するはずですが、未確認です。  
  
[FrameCapturer](https://github.com/unity3d-jp/FrameCapturer) がこれを用いてゲーム画面のキャプチャから Twitter への投稿までを実装しています。具体的な使い方の例になると思われます。  

### 使い方
1. [このパッケージ](https://github.com/unity3d-jp/TweetMedia/raw/master/Package/TweetMedia.unitypackage) をインポート
2. UGUI オブジェクト TweetMedia.prefab をシーン内に設定
3. TweetMedia の consumer_key, consumer_secret を設定

3 について、consumer key / secret は Twitter にアプリを登録することで発行されます。
アプリ登録の手順は簡単で、[このサイト](https://apps.twitter.com/) へ行き、"Create New App" をクリックして必要な項目を記入するだけです。

ゲームをプレイする側はゲームに Twitter にアクセスする許可を与える必要があります。  
TweetMedia.prefab を使っている場合、"Open authorize URL" ボタンを押すとブラウザが開き、認証画面に移動します。  
認証画面の "Authorize app" を押し、その後出てくる PIN を "enter PIN" の InputField にコピー＆ペーストして Send ボタンを押すことで認証が完了します。  
一度認証すればあとはその情報を保存することでこの手順は不要になります。(デフォルトで保存するようになっています)  
TweetMedia.prefab は見た目も機能も必要最小限しか備えていないため、独自に改良することをおすすめいたします。  
![auth](Screenshots/auth.png)

### 注意点
(これを書いている時点で) アニメ gif は 5 MB、mp4 は 15 MB までの容量制限があり、他にも解像度や長さ (-30s) など色々細かい制限があるようです。
