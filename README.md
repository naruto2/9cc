これは、https：//www.sigbus.info/compilerbookのリファレンス実装です。


関数とローカル変数　の直前まで出来た。

consume_ident()が無いことでハマった。
consume_ident()をgithub rui314より拝借

ステップ10の直前まで来た。

program()関数を実行すると結果がおかしくなる。

tokenize()は文字列の配列を返すべきではなかろうか?

chibicc の セミコロンで区切られた複数のステートメントを受け入れる
を参照し、同様の挙動を得ることが出来た。
2020-4-16

「return」ステートメントを追加
2020-4-17

式ステートメントの概念を追加する
1文字のローカル変数をサポート
複数文字のローカル変数をサポート
「if」ステートメントを追加
「while」ステートメントを追加
「for」ステートメント追加
追加 { ... }
LICENSEとREADME.mdを追加します
最大6つの引数を持つ関数呼び出しをサポート
tokenize.cファイルを分離
スタックフレームが16バイト境界に揃えられていることを確認してください
2020-4-19

パラメータなしの関数定義をサポート
最大6つのパラメーターをサポートする関数定義
2020-4-20

各ノードに代表的なノードを追加して、エラーメッセージを改善する
単項&と*を追加する
ASTノードに型で注釈を付け、ポインター演算を機能させる
キーワード「int」を追加し、変数の定義を必須にします
1次元配列を追加する
配列の配列を追加する
[]演算子を追加
sizeofを追加
グローバル変数を追加する # test.shの違いでハマった
コンパイル時の警告を無くした
文字タイプを追加
switch文にdefaultラベルを追加しコンパイル時の警告を無くした
2020-4-21

文字列リテラルを追加する
\a, \b, \t, \n \v, \f, \r, \e, \0を追加
GNU式ステートメントを追加
argv[1]の代わりにファイルからコードを読み取る
行コメント,ブロックコメントを追加
ブロックスコープを導入
シェルスクリプトだったテストをCで書き変える
構造体を追加
2020-4-22

構造体メンバーを正しくアライメント
ローカル変数のアライメント
2020-4-24

構造体タグをサポート
「->」演算子を追加
typedefを追加
「int」のサイズを8から4に変更します
shot及びlongを追加する
ネストされた型宣言子をサポートします(例: "int (*x)[3]")
関数宣言を追加
関数に戻りの型を追加する
void型を追加
_Bool型を追加
longのエイリアスとして long longを追加
複合型の宣言を正しく解釈する
sizeofが式だけでなくタイプ名も受け入れるようにする
大きなリテラル整数を許可する
型キャストを追加
文字リテラルを追加
列挙型を追加
2020-4-25

ファイルスコープ関数をサポート
forループがローカル変数を定義できるようにする
examples/nqueen.cを追加
カンマ演算子を追加
++および--演算子を追加する
2020-4-27

+ =、-=、* =および/ =演算子を追加します
16進数、8進数、2進数のリテラルを追加する
! オペレーター追加
~ 演算子を追加
|、＆、^演算子を追加する
&&演算子と||演算子を追加
不完全な配列型の概念を追加する
func paramコンテキストで配列からポインタへの減衰を実装する	
2020-4-28

不完全な構造体型の概念を追加する
breakステートメントを追加
continueステートメントを追加
goto文とラベル文を追加
switch case文を追加
関数が void型を返すのを許す
2020-4-29

<<,>>,<<=,>>= を追加
三項演算子 ?: を追加
定数式を追加
ローカル変数初期化子をサポート
余分な配列要素をゼロで初期化する
2020-5-7

文字列リテラル初期化子を追加
初期化子が指定されている場合、配列の長さを省略できるようにします(敗けた parse.c
をリポジトリからコピーした)
ローカル変数の構造体初期化子を処理します
2020-6-19


スカラーグローバル初期化子を追加
2020-6-20