Import('env')

env.Replace(UPLOADHEXCMD="~/scripts/uploadToArduino.sh $SOURCES 192.168.178.74")
