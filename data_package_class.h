/*
Data package class
*/
#ifndef DATA_PACKAGE_CLASS_H_
#define DATA_PACKAGE_CLASS_H_

#include<vector>
#include<iostream>
//for memory checking functions
#include <unistd.h>
#include <ios>
#include <fstream>
#include <string>
//for total amt of memory
#include "sys/types.h"
#include "sys/sysinfo.h"
//for random generator
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
//for opencv
#include<opencv2/imgproc/imgproc.hpp>//for canny
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>

using namespace cv;
using namespace std;

class data_package_class{  //raw input data used in the input_raw_to_prepared.cpp
    public:
    string label_str;
    vector<vector<float>> data;  
    vector<int> labels; 
    bool analyze_status=false;
    
    void set_no_of_record(int n)
    {
        data.resize(n);
        labels.resize(n);
    }

    void set_no_of_elements_in_each_record(int n)
    {
        for(int a=0;a<data.size();a++)
        {
            data[a].resize(n);
        }
    }
    
    int no_fo_records()
    {
        return data.size();
    }

    int no_of_elements_in_each_record()
    {
        return data[0].size();
    }
};

struct filtered_data //its fot the filtered data used in the core class
{
    vector<vector<float>> data;
    string str;
    int label;
};
//image data classes and structures 
struct image_pixel_data//for testing //old code
{
    vector<vector<int>> red_pix;
    vector<vector<int>> green_pix;
    vector<vector<int>> blue_pix;
};

struct image_slice_data //old code 
{
    bool canny_edge_data_available=false;
    data_package_class *value_slices=new data_package_class();
    data_package_class *satturation_slices=new data_package_class();
    data_package_class *hue_slices=new data_package_class();
    data_package_class *canny_edge_slices=new data_package_class();
};

class image_package_class
{
    private:
    //canny edge slicer operations data
        //threshold
    int lowThreshold=20;//30
    int const max_lowThreshold = 100;
    float base_line=0.05;
    //other operation data
    vector<Mat*> mat_vec;
    vector<image_pixel_data*> full_image_pixel_vec;
    image_slice_data* isd_pack=new image_slice_data();
    //above(within the class) are the old pieces of code
   



    //package locking mechanism
    bool lock_enabled=false;
    //critial variables used in data preparation process
    float color_sensitiviy;
    float percentage_of_close_pixels;//used with color_distance2 function
    int no_of_sq_areas_need_to_be_checked_for_avg_color=1;//used by the similar_obj_combining_process. To be exact by this function find_neighbouring_obj_avg_color_of_closest_area
    unsigned int slice_row_size,slice_col_size,min_size_of_obj=0;//min_size_fo_obj used for testing using plot_obj_maps function.
    //prepared data structures and objs
    struct image_map_element
    {
        unsigned int row_index,col_index;
        int obj_id=-1;
        bool select_status=false;
        unsigned int avg_red,avg_green,avg_blue;
    };
    vector<vector<image_map_element*>> image_map;//the main img map
    vector<vector<image_map_element*>> obj_vec;//stores the elements making an object area.
    struct prepared_data
    {
        vector<vector<vector<image_map_element*>>> image_map_vec;
        vector<vector<vector<image_map_element*>>> obj_vec_for_each_img;     
    }prepared_data_obj;
    //temporary variables
    Mat* orig_image_temp;
    int no_of_slices_row_wise,no_of_slices_col_wise;
    //meta data
    int label_id;
    int slice_rows=2,slice_cols=2;
    string label_str;
        //location information
        string dir_path;//label;
        vector<string> image_file_name;
        vector<string> image_paths;    

    public:
    // return functions
    string return_dir_path()
    {   return dir_path;}
    int return_no_of_images()
    {   return image_paths.size();}
    string return_img_path(int a)
    {   return image_paths.at(a);}
    string return_img_file_name(int a)
    {   return image_file_name.at(a);}
    //data preparation functions

    void data_arranger();

    void border_info_extractor();

    void contour_finder();

    //similar object combination process
    struct obj_distance
    {
        unsigned int obj_id1,obj_id2;
        int red,green,blue;
        float distance;
    };
    void find_neighbouring_obj_avg_color_of_closest_area(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_neighbouring_objs,image_map_element *element);//need testing
    bool check_if_element_is_border_element(image_map_element* element);
    void find_neighbouring_objs(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_all_objs,vector<vector<image_map_element*>> *results);//need testing
    struct sortingclass1 
    {
        bool operator() (vector<image_map_element*> vec1,vector<image_map_element*> vec2) 
        { return (vec1.size()<vec2.size());}
    }sorting_helper1; 
    void same_obj_combination_process();////need testing
    //object mapping proceess functions
    int avg_color_in_slice(Mat* slice,char color);//color maper function//ok tested  
    float color_distance2(image_map_element* origin_element,image_map_element* new_element);//new method for calculating color distance //tested poor result than its counterpart
    float color_distance(image_map_element* origin_element,image_map_element* new_element);//color maper function//ok tested
    void search_for_neighbour(image_map_element* element,vector<vector<int>>* result);//color maper function//ok tested
    void remove_non_free_elements(vector<vector<int>>* result);//color maper function//ok tested
    void create_color_maps();//color maper function//ok tested
    void plot_obj_maps(vector<vector<image_map_element*>>* obj_vec_for_one_img,string orig_img_path);//for testing create_color_maps() function
    //data preparation process handler function
    void start_data_preparation_process();
    void clean_up_prepared_data_obj();
    void save_current_img_data();
    //meta data functions
    void enter_image_metadata(string img_name,string img_path);
    void enter_image_metadata(int id_,string label_,string dir_path_);
    void enter_image_metadata(int start_index,vector<string> *img_file_name,vector<string> *img_paths);
    void remove_image_metadata(int start_index);
    //setting up of training critical variables
    void enter_training_critical_variables(int no_of_sq_areas_need_to_be_checked_for_avg_color,float color_sensi,int slice_row,int slice_col,int min_size_of_obj1);
    //threading functions
    void split_package_data(vector<image_package_class*> *ipc_vec);
    void combine_package_data(vector<image_package_class*> *ipc_vec);
    //constructor
    image_package_class(int id_,string label_,string dir_path_);
    image_package_class();
    //cleanup function
    void clean_up_all_the_data_addresses();
};
#endif