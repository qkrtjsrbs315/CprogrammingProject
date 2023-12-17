# CprogrammingProject

#setting <br/>
1. first clone this project<br/>
   git clone https://github.com/qkrtjsrbs315/CprogrammingProject.git

2. second download every package <br/>
   sudo apt-get install libcurl4-openssl-dev    <!--curl package download--> <br/>
   sudo apt-get install libjansson-dev  <!--jansson package download--> <br/>
   sudo apt-get install libopencv-dev <!--opencv package download--> <br/>

<br/>
<br/>
or <br/>
chmod +x install_package.sh
<br/>
./install_package.sh

<br/>

#command <br/>
g++ -o \<output file name\> \<input cpp file name\> -lopencv_core -lopencv_highgui -lcurl -ljansson -std=c++11
