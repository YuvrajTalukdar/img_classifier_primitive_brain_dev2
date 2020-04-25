#ifndef LOGIC_CORE_H_
#define LOGIC_CORE_H_

#include<iostream>
#include<fstream>
#include<string.h>
#include<vector>
#include<thread>

#include"image_package_class.h"

using namespace cv;
using namespace std;

class logic_core
{
    private:
    int data_division=1;
    int logic_core_aim=1,logic_core_no;//locig_core_aim = 1 is for img classification, logic_core_no =for indetyfing multiple cores working on the a same aim 
    unsigned int no_of_nn_cores_needed=4;
    vector<string> available_names; 
    bool locked=false;
    vector<image_package_class*> image_package_vec;
    
    public:
    //logic core metadata loadeing functions
    void set_logic_core_no(int no);
    //return functions
    string return_core_name(int index);
    int return_no_of_nn_cores_needed();
    //constructors
    logic_core();
};
#endif