#!/bin/sh
TESTYPE=$1

#downscale downscale 8Mpx "3280 2464"
if [ $TESTYPE = "8MPto8MP" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPto8MP_3280x2464_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 3280 -oh 2464"
elif [ $TESTYPE = "8MPto5MP" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPto5MP_2592x1944_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 2592 -oh 1944"
elif [ $TESTYPE = "8MPtoQXGA" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoQXGA_2048x1536_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 2048 -oh 1536"
elif [ $TESTYPE = "8MPtoUXGA" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoUXGA_1600x1200_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 1600 -oh 1200"
elif [ $TESTYPE = "8MPtoSXGA" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoSXGA_1280x960_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 1280 -oh 960"
elif [ $TESTYPE = "8MPtoXGA" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoXGA_1024x768_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 1024 -oh 768"
elif [ $TESTYPE = "8MPtoSVGA" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoSVGA_816x618_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 800 -oh 600"
elif [ $TESTYPE = "8MPtoVGA" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoVGA_816x618_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 640 -oh 480"
elif [ $TESTYPE = "8MPtoQVGA" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoQVGA_816x618_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 320 -oh 240"
elif [ $TESTYPE = "8MPtoQCIF" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoQCIF_816x618_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 176 -oh 144"
elif [ $TESTYPE = "8MPtoQQVGA" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoQQVGA_816x618_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 160 -oh 120"
elif [ $TESTYPE = "8MPtoSQCIF" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoSQCIF_816x618_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 128 -oh 96"
elif [ $TESTYPE = "8MPtoQQCIF" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_8MPtoQQCIF_816x618_UYVY.yuv"
  INSIZE="-iw 3280 -ih 2464"
  OUTSIZE="-ow 88 -oh 72"
#downscale  5 Mpix "2592 1944"
elif [ $TESTYPE = "5MPto5MP" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPto5MP_2592x1944_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 2592 -oh 1944"
elif [ $TESTYPE = "5MPtoQXGA" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoQXGA_2048x1536_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 2048 -oh 1536"
elif [ $TESTYPE = "5MPtoUXGA" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoUXGA_1600x1200_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 1600 -oh 1200"
elif [ $TESTYPE = "5MPtoSXGA" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoSXGA_1280x960_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 1280 -oh 960"
elif [ $TESTYPE = "5MPtoXGA" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoXGA_1024x768_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 1024 -oh 768"
elif [ $TESTYPE = "5MPtoSVGA" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoSVGA_800x600_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 800 -oh 600"
elif [ $TESTYPE = "5MPtoVGA" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoVGA_640x488_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 640 -oh 480"
elif [ $TESTYPE = "5MPtoQVGA" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoQVGA_648x488_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 320 -oh 240"
elif [ $TESTYPE = "5MPtoQCIF" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoQCIF_648x488_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 176 -oh 144"
elif [ $TESTYPE = "5MPtoQQVGA" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoQQVGA_648x488_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 160 -oh 120"
elif [ $TESTYPE = "5MPtoSQCIF" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoSQCIF_648x488_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 128 -oh 96"
elif [ $TESTYPE = "5MPtoQQCIF" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPtoQQCIF_648x488_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 88 -oh 72"
#downscale XGA "1024 768"
elif [ $TESTYPE = "XGAtoXGA" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoXGA_1024x768_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 1024 -oh 768"
elif [ $TESTYPE = "XGAtoSVGA" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoSVGA_800x600_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 800 -oh 600"
elif [ $TESTYPE = "XGAtoVGA" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoVGA_640x480_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 640 -oh 480"
elif [ $TESTYPE = "XGAtoQVGA" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoQVGA_320x240_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 320 -oh 240"
elif [ $TESTYPE = "XGAtoQCIF" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoQCIF_256x194_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 176 -oh 144"
elif [ $TESTYPE = "XGAtoQQVGA" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoQQVGA_256x194_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 160 -oh 120"
elif [ $TESTYPE = "XGAtoSQCIF" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoSQCIF_256x194_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 128 -oh 96"
elif [ $TESTYPE = "XGAtoQQCIF" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoQQCIF_256x194_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 88 -oh 72"

#downscale VGA "640 480"
elif [ $TESTYPE = "VGAtoVGA" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoVGA_640x480_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 640 -oh 480"
elif [ $TESTYPE = "VGAtoQVGA" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoQVGA_320x240_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 320 -oh 240"
elif [ $TESTYPE = "VGAtoQCIF" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoQCIF_176x144_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 176 -oh 144"
elif [ $TESTYPE = "VGAtoQQVGA" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoQQVGA_160x120_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 160 -oh 120"
elif [ $TESTYPE = "VGAtoSQCIF" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoSQCIF_160x122_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 128 -oh 96"
elif [ $TESTYPE = "VGAtoQQCIF" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoQQCIF_160x122_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 88 -oh 72"

#upscale VGA "640 480"
elif [ $TESTYPE = "VGAto8MP" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAto8MP_2368x1760_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 3280 -oh 2464"
elif [ $TESTYPE = "VGAto5MP" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAto5MP_2368x1760_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 2592 -oh 1944"
elif [ $TESTYPE = "VGAtoQXGA" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoQXGA_2048x1536_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 2048 -oh 1536"
elif [ $TESTYPE = "VGAtoUXGA" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoUXGA_1600x1200_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 1600 -oh 1200"
elif [ $TESTYPE = "VGAtoSXGA" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoSXGA_1280x960_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 1280 -oh 960"
elif [ $TESTYPE = "VGAtoXGA" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoXGA_1024x768_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 1024 -oh 768"
elif [ $TESTYPE = "VGAtoSVGA" ]; then
  FIN="${TESTSCRIPT}/VGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_VGAtoSVGA_800x600_UYVY.yuv"
  INSIZE="-iw 640  -ih 480"
  OUTSIZE="-ow 800 -oh 600"

#upscale XGA "1024 768"
elif [ $TESTYPE = "XGAto8MP" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAto8MP_3280x2464_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 3280 -oh 2464"
elif [ $TESTYPE = "XGAto5MP" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAto5MP_2592x1944_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 2592 -oh 1944"
elif [ $TESTYPE = "XGAtoQXGA" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoQXGA_2048x1536_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 2048 -oh 1536"
elif [ $TESTYPE = "XGAtoUXGA" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoUXGA_1600x1200_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 1600 -oh 1200"
elif [ $TESTYPE = "XGAtoSXGA" ]; then
  FIN="${TESTSCRIPT}/XGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_XGAtoSXGA_1280x960_UYVY.yuv"
  INSIZE="-iw 1024 -ih 768"
  OUTSIZE="-ow 1280 -oh 960"
#upscale 5MP
elif [ $TESTYPE = "5MPto8MP" ]; then
  FIN="${TESTSCRIPT}/5MP.raw"
  FNAME="${TMPBASE}/p2rhq_out_5MPto8MP_3280x2464_UYVY.yuv"
  INSIZE="-iw 2592 -ih 1944"
  OUTSIZE="-ow 3280 -oh 2464"
elif [ $TESTYPE = "UXGAtoVGA" ]; then
  FIN="${TESTSCRIPT}/UXGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_UXGAtoVGA_640x480_UYVY.yuv"
  INSIZE="-iw 1600 -ih 1200"
  OUTSIZE="-ow 640 -oh 480"
elif [ $TESTYPE = "SVGAtoQVGA" ]; then
  FIN="${TESTSCRIPT}/SVGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_SVGAtoQVGA_320x240_UYVY.yuv"
  INSIZE="-iw 800 -ih 600"
  OUTSIZE="-ow 320 -oh 240"
elif [ $TESTYPE = "QQVGAtoSQCIF" ]; then
  FIN="${TESTSCRIPT}/QQVGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_QQVGAtoSQCIF_128x96_UYVY.yuv"
  INSIZE="-iw 160 -ih 120"
  OUTSIZE="-ow 128 -oh 96"
elif [ $TESTYPE = "SXGAto8MP" ]; then
  FIN="${TESTSCRIPT}/SXGA.raw"
  FNAME="${TMPBASE}/p2rhq_out_SQCIFtoCIF_3280x2464_UYVY.yuv"
  INSIZE="-iw 1280 -ih 960"
  OUTSIZE="-ow 3280 -oh 2464"
elif [ $TESTYPE = "SQCIFtoCIF" ]; then
  FIN="${TESTSCRIPT}/SQCIF.raw"
  FNAME="${TMPBASE}/p2rhq_out_SQCIFtoCIF_352x288_UYVY.yuv"
  INSIZE="-iw 128 -ih 96"
  OUTSIZE="-ow 352 -oh 288"
fi

${TESTBIN}/p2rhq -ifile $FIN -ofile $FNAME $INSIZE $OUTSIZE
RESULT=$?
echo "Test returned $RESULT"
sleep 1
chmod 744 $FNAME

if [ $? -eq 1 ]; then
  #color output - The outpu file wasn't created
  echo -en "Failed to chmod the $FNAME \n"
  echo -en "\033[0;31m FAIL \033[0m \n"
  exit 1
fi

#initialize ERR var with PASS
ERR=0

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Was previewer-resizer capable of changing $FIN \
  with $INSIZE size to $OUTSIZE size into $FNAME file?";echo ""
  $WAIT_ANSWER
  ERR=$?
fi
if [ $ERR -eq 1 ]; then
  #color output
  echo -en "\033[0;31m FAIL \033[0m \n"
  #echo "FAIL"
  exit 1
else
  echo -en "\033[0;32m PASS \033[0m \n"
  #echo "PASS"
  exit 0
fi

