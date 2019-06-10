#ifndef LOGIC_CORE_H_
#define LOGIC_CORE_H_

#include<iostream>
#include<fstream>
#include<string.h>
#include<vector>
#include<dirent.h>

#include"core_class.h"
#include"data_package_class.h"
//#include"core_class.h"

using namespace cv;
using namespace std;

class img_data_preparation_process_handler 
{
    public:
    float color_sensitivity;
    float color_sensitivity2;
    float no_of_sq_areas_need_to_be_checked_for_avg_color;
    int min_size_of_obj;
    //int slice_row,slice_col;
    image_package_class* obj;
    void slicing_process_handler();
};

class logic_core
{
    private:
    int data_division=1;
    int logic_core_aim=1,logic_core_no;//locig_core_aim = 1 is for img classification, logic_core_no =for indetyfing multiple cores working on the a same aim 
    unsigned int no_of_nn_cores_needed=4;
    vector<string> available_names; 
    bool locked=false;
    
    vector<image_package_class*> image_package_vec;
    vector<img_data_preparation_process_handler*> img_data_preparation_process_handler_vec;
    
    void fix_first_read_dir_name(string &str);//called from read_image_data function        
    public:
    //training functions
    void train_segment(vector<core_class> &core_vec);
    //image_meta data reader
    bool read_image_data();
    //logic core metadata loadeing functions
    void set_logic_core_no(int no);
    //return functions
    string return_core_name(int index);
    int return_no_of_nn_cores_needed();
    //constructors
    logic_core();
};
#endif