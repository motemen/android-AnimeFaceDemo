AnimeFaceDemo
=============

![Screenshot](http://cdn-ak.f.st-hatena.com/images/fotolife/m/motemen/20110502/20110502025333.png)

WHAT IS THIS
------------

アニメ顔検出ライブラリ [Imager::AnimeFace](http://anime.udp.jp/imager-animeface.html) を Android から使ってみたものです。最適化などを行っていないため検出に大変時間がかかります (数分)。

HOW TO MAKE
-----------

	# download and extract nvxs source (http://www.udp.jp/software/nvxs-1.0.2.tar.gz)
	% export NVXS_ROOT=...
	% (cd $NVXS_ROOT/CLAPACK/F2CLIBS/libf2c/; make arith.h sysdep1.h signal1.h)
	% ndk-build
	# then compile android app
