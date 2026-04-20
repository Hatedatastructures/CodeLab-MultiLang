   #include <windows.h>
   #include <mmsystem.h>
   int main()
   {
       // 播放系统感叹号声音，以异步方式
       PlaySound(TEXT("SystemExclamation"), NULL, SND_ALIAS | SND_ASYNC);
       return 0;
   }