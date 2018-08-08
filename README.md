# asyncbug

A weird bug in Boost.asio.
Please build this project using cmake.
REMARK: you need to edit the CMakeLists.txt file to point to your Boost repository.
(I also had little trouble with naming conventions for the Boost libs.)
Then launch the executable.

In the FtpClient.cpp file, at line 157, you will see the weirdness.
Alternatively uncommenting a line or the other produce two different behaviors.

