#include <windows.h>
#include <iostream>
#include "utils/utils.h"
#include "vid/vid.h"

int main(int argc, char *argv[])
{
std::cout<<GetCurrentPlayingInfo()<<std::endl;
std::string command = "start \"\" \"" + DownloadVideo(GetCurrentPlayingInfo()) + "\"";
system(command.c_str());
system(command.c_str());
system("pause");
}