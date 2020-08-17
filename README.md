# World4UTAU

*This branch is based on world4utau in http://utau2008.xrea.jp/mp3/engine_hikaku.html. The initial code in this repo is from https://github.com/Luk3M/world4utau which is not maintained for a long time.*

It's ported to Linux / Mac including lots of optimizations, it's compatiable with OpenUATU and can be called by C#. So if you wanna use OpenUATU on Mac/Linux, this is your *only* option as far as I see.

# Build
Currently support Ubunt and MacOS. To use windows version, please refer the original link.
Later, all platforms will be supported.

## Ubuntu
Install dependence.

`sudo apt install git gcc texinfo`

Build `fftw`. 
*fftw* is a C subroutine library for computing the discrete Fourier transform (DFT) in one or more dimensions, of arbitrary input size, and of both real and complex data (as well as of even/odd data, i.e. the discrete cosine/sine transforms or DCT/DST). [http://www.fftw.org](http://www.fftw.org)

In this repo, we include fftw-3.3.8 source code for convienent. You can download it from official site by yourself.
```
cd fftw-3.3.8
./configure
make
make install
```

Build `world4utau`.
```
cd world4utau
make build
```

## MacOS

Suppose you already have XCode tools installed for C program.

Build `fftw`. 
*fftw* is a C subroutine library for computing the discrete Fourier transform (DFT) in one or more dimensions, of arbitrary input size, and of both real and complex data (as well as of even/odd data, i.e. the discrete cosine/sine transforms or DCT/DST). [http://www.fftw.org](http://www.fftw.org)

In this repo, we include fftw-3.3.8 source code for convienent. You can download it from official site by yourself.
```
cd fftw-3.3.8
./configure
make
make install
```

Build `world4utau`.
```
cd world4utau
make build
```

# Performance
![](https://github.com/xrdavies/world4utau/blob/master/docs/valgrind.png?raw=true)

# History Information
world4utau引擎

该程序由Masamasa Morise编写，“语音分析和合成方法WORLD 0.0.4”
http://www.aspl.is.ritsumei.ac.jp/morise/world/
它是UTAU使用的语音合成引擎。
使用它而不是UTAU的resampler.exe。
fftw dll是操作所必需的。
http://www.fftw.org/install/windows.html

莫里斯先生的UTAU引擎“外力Brisampler绅士哭声（EFB-GW）”
尽管存在，但除此之外，UTAU的创建者Ameya还执行时间延长处理和界面。
它建成了。
我在未完全了解WORLD的机制的情况下进行了此操作，因此可能会损害WORLD的原始质量。

*此程序是根据GPL（通用公共许可证）许可的。
请检查copying.txt以获取许可证的内容。
下面引用自WORLD（0.0.4）的readme.txt。
>有两点需要注意。
> ・使用该程序创建的应用程序是
>必须将源代码一起分发。
> ・应用程序还必须具有GPL许可证。
>
>如果保留上述内容，则可以自由分发或出售修改后的版本。

--------
历史：  
2011.1.17 Ver.0.0.1组织了自制零件的代码。分析文件格式略有变化（自动更新）  
2011.1.16 Ver。无临时发布  


world4utauエンジン

このプログラムは森勢将雅氏の「音声分析合成方式 WORLD 0.0.4」
http://www.aspl.is.ritsumei.ac.jp/morise/world/
を用いたUTAU用音声合成エンジンです。
UTAUのresampler.exeの代わりに使ってください．
動作には，fftwのdllが必要です．
http://www.fftw.org/install/windows.html

森勢氏によるUTAU用エンジン「エターナルフォースブリサンプラー ジェントリーウィープス(EFB-GW)」も
存在していますが、これはそれとは別にUTAU作者である飴屋が時間伸縮処理やインターフェースを
作りこんだものです。
WORLDの仕組みをよく理解しないで作ってますので、WORLD本来の品質を損なっているかもしれません。

※このプログラムはGPL (General Public License)でライセンスされています．
ライセンスの内容は，copying.txtをご確認ください．
以下、WORLD(0.0.4)のreadme.txtより引用。
> 注意すべきは以下の2点です．
> ・本プログラムを利用して作ったアプリケーションは，
> 　ソースコードも一緒に配布する必要がある．
> ・そのアプリケーションもGPLライセンスにしなければならない．
> 
> 以上を守れば，修正したものを配布するのも販売するのも自由です．

--------
履歴：  
2011.1.17 Ver.0.0.1  自作部分のコードを整理。分析ファイルの形式が若干変わる（自動で更新）  
2011.1.16 Ver.なし   暫定公開  


world4utau engine

This program is written by Masamasa Morise "Speech analysis and synthesis method WORLD 0.0.4"
http://www.aspl.is.ritsumei.ac.jp/morise/world/
It is a speech synthesis engine for UTAU using.
Use it instead of UTAU's resampler.exe.
The fftw dll is required for operation.
http://www.fftw.org/install/windows.html

UTAU engine "Eternal Force Brisampler Gentry Weeps (EFB-GW)" by Mr. Morise
Although it exists, this is aside from that, Ameya, the creator of UTAU, performs time stretching processing and interface.
It was built.
I made it without understanding the mechanism of WORLD well, so it may be impairing the original quality of WORLD.

*This program is licensed under the GPL (General Public License).
Please check copying.txt for the contents of the license.
Below, quoted from readme.txt of WORLD (0.0.4).
> There are two points to note.
> ・The application created using this program is
> It is necessary to distribute the source code together.
> ・The application must also have a GPL license.
>
> If you keep the above, you are free to distribute the modified version or sell it.

--------
History:  
2011.1.17 Ver.0.0.1 Organized the code of the self-made part. Analysis file format changes slightly (automatically updated)  
2011.1.16 Ver. None Provisional release  
