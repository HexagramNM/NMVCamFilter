NMVCamFilter
==============

![Discord内での表示](vcInDiscord.PNG)

`stdafx.h`の`TARGET_WINDOW_NAME`で指定したウィンドウ名をもつウィンドウの画面をDiscordやZoomなどに映す仮想カメラです。DirectShowを利用しております。仮想カメラを自作する際の参考になればと思い、公開しました。

ここのコードを主に参考にしております。

[mysourcefilter](https://github.com/syu5-gh/mysourcefilter)

[svcam](https://github.com/aricatamoy/svcam)

DirectShowで仮想カメラを実装する際の注意点をQiitaにも記事としてあげました。こちらもぜひご覧ください。
[DirectShowで仮想カメラを自作しよう](https://qiita.com/HexagramNM/items/2311f025f3af758c83a0)

# ビルド方法

Visual Studio 2017で開発をしておりました。

コンパイルの際にはDirectShowのライブラリが必要です。[ここ](https://github.com/ganboing/sdk71examples)からダウンロードし、`multimedia/directshow/baseclasses`にあるコードをビルドしてください。

コンパイル出来たら、「追加のインクルードフォルダ」を`multimedia/directshow/baseclasses`に、「追加のライブラリディレクトリ」を`baseclasses`のコンパイルにより生成された`Release`または`Debug`フォルダに設定してください。最後に「全般」の項目にある「構成の種類」をdllファイルにし、「リンカー」→「入力」のところにあるモジュール定義ファイルを`module.def`に設定すれば、本コードをビルドできます。

生成されたdllファイルはwindowsのSystem32もしくはSysWOW64に入っている、`regsvr32.exe`で登録することで使えるようになります。


# 参考にしたコードの著作権表示（MITライセンス）

```
The MIT License (MIT)

Copyright (c) 2013 mahorigahama

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
