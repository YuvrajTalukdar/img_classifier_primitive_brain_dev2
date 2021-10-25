/*
Data package class
*/
#ifndef DATA_PACKAGE_CLASS_H_
#define DATA_PACKAGE_CLASS_H_

#include<vector>
#include<iostream>
#include<fstream>
#include<string>
//for dir related functions
#include<dirent.h>
//for random generator required for the plot_obj_maps function but looks like without these .h files the program is working.
#include<stdlib.h>     /* srand, rand */
#include<time.h>       /* time */
#include<chrono>
//for opencv----------------------------------------
#include<opencv2/imgproc/imgproc.hpp>//for canny
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>

using namespace cv;
using namespace std;
using namespace std::chrono;

class image_package_class
{
    private:
    //package locking mechanism
    bool lock_enabled=false;
    //critial variables used in data preparation process
    float color_sensitiviy;
    float color_sensitivity2;
    float percentage_of_close_pixels;//used with color_distance2 function
    int slice_row_size,slice_col_size,min_size_of_obj=0;//min_size_fo_obj used for testing using plot_obj_maps function.
    //prepared data structures and objs
    struct image_map_element
    {
        unsigned int row_index,col_index;
        int obj_id=-1;
        bool select_status=false;
        bool non_conflict_border_element=false;
        uchar avg_red,avg_green,avg_blue;
        uchar avg_border_all,avg_border_low_res;//avg_border_all contains sobel result and avg_border_low_res.
    };
    //these are data vector for ONLY one image.
    Mat sobel_plus_edge,low_res_edge,high_res_edge;//result from modified_sobel is saved here. get_color_sensitivity function needs them;//required by modified_sobel get_color_sensitivity functions
    vector<vector<image_map_element*>> image_map;//the main img map
    vector<vector<image_map_element*>> obj_vec;//stores the elements making an object area.
    vector<vector<image_map_element*>> border_element_vec;//border elements of each obj without the buffer area
    
    struct prepared_data
    {
        vector<vector<vector<image_map_element*>>> image_map_vec;
        vector<vector<vector<image_map_element*>>> obj_vec_for_each_img;     
    }prepared_data_obj;
    //temporary variables
    Mat* orig_image_temp;
    int no_of_slices_row_wise,no_of_slices_col_wise;//required by plot_obj_maps and create_color_maps
    //meta data
    int label_id;
    //int slice_rows=2,slice_cols=2;
    string label_str;
        //location information
        string dir_path;//label;
        vector<string> image_file_name;
        vector<string> image_paths;    

    public:
    bool annote_mode=false;
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
    //modified sobel functions
    struct pixel
    {
        int edge_id;
        int value;
        int x,y;
        bool select_status=false;
    };
    void remove_non_free_elements_MODIFIED_SOBEL(vector<vector<pixel>> &sobel_binary_map,vector<vector<int>>* result);//ok tested
    void search_for_neighbour_MODIFIED_SOBEL(pixel &current_pixel,vector<vector<pixel>> &sobel_binary_map,vector<vector<int>> &result);//ok tested
    void bin_color_mapper(Mat* plot,int min_size_of_edge,vector<vector<pixel>> &sobel_binary_map,vector<vector<pixel*>> &edge_vec);
    void plot_combiner(vector<Mat*>& input_plot_vec,Mat* new_plot);
    void variable_thershold_sobel(Mat &img1,Mat &sobel_result,vector<vector<pixel>> &sobel_binary_map,vector<vector<pixel*>> &edge_vec,int min_edge_size);
    void modified_sobel_process_handler();
        //data preparation step 1 
    struct obj_info
    {   
        int avg_red,avg_green,avg_blue,obj_id;
        float avg_hue;
        vector<float> color_distance_of_each_obj;
        int index_of_the_smallest_distance,obj_id_with_smallest_distance;
        float smallest_distance;
        bool select_status=false;
    };
    
    //similar obj combination process un-strict
    void obj_info_gatherer(int a,vector<obj_info> *obj_info_vec,vector<vector<image_map_element*>> *list_of_neighbouring_objs);//ok tested
    void similar_obj_combinarion_process_un_strict();//ok tested
    vector<vector<image_map_element*>>* list_of_obj_to_be_combined_ptr;
    
    //similar object combination process strict
    int find_neighbouring_obj_avg_color_of_closest_area(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_neighbouring_objs,vector<image_map_element*> *border_element_vec);//ok tested
    bool check_if_element_is_border_element(image_map_element* element);//ok tested
    void find_obj_border_elements(vector<image_map_element*> *obj,vector<image_map_element*> *border_element_vec);//need testing
    void find_neighbouring_objs(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_all_objs,vector<vector<image_map_element*>> *results,vector<image_map_element*> *border_elements_vec,int neighbouring_obj_search_mode);//ok tested
    /*struct sortingclass1 
    {
        bool operator() (vector<image_map_element*> vec1,vector<image_map_element*> vec2) 
        { return (vec1.size()<vec2.size());}
    }sorting_helper1;*/ 
    int binary_search_for_strict(vector<vector<image_map_element*>>* obj_vec_temp,int search_id);
    void similar_obj_combination_process_strict();//ok testing
    //geans problem solution functions
    struct cell_stat
    {
        bool horizontal_check=false,vertical_check=false;
        int row_index,col_index;//this are the index for new_cell in img_map.
        int x=0,y=0;//this co-ordinates are for new_cell in cell_stat_map only and not for img_map. y=row, x=col.
        bool select_status=false;//rquired by the color mapper for getting the no of sets.
        int set_id=-1;
    };
    struct conflicting_cell
    {
        int row_index,col_index;//this are the index for new_cell in img_map.
        int x,y;//this co-ordinates are for new_cell in cell_stat_map only and not for img_map. y=row, x=col.
        int orig_row_index,orig_col_index;//these are for orig element index of whose neighbours are being found.
        vector<vector<cell_stat>> visible_cell_set_wise;
    };
    bool border_conflict_status(int orig_img_map_row_index,int orig_img_map_col_index,int img_map_row_index,int img_map_col_index,vector<conflicting_cell> &potential_conflicting_cell_vec);
    
    //color map creation function.
    int avg_color_in_slice(Mat &slice,char color);//color maper function//ok tested  
    vector<image_map_element*> *obj_elements_vec_ptr;//required for the color distance calculator
    float hue_distance_calc(float hi,float h2);
    bool color_distance(image_map_element* origin_element,image_map_element* new_element);//color maper function//ok tested
    void search_for_neighbour(image_map_element* element,vector<vector<int>>* result,vector<conflicting_cell> &potential_conflicting_cell_vec);//color maper function//ok tested
    void remove_non_free_elements(vector<vector<int>>* result);//color maper function//ok tested
    void create_color_maps();//color maper function//ok tested
    float get_color_sensitivity(image_map_element* origin_element,image_map_element* new_element);
    
    //testing functions and variables
    static void onMouse(int evt,int x,int y,int flags,void* param);
    void second_stage_analyzer(Mat plot,int slice_size);
    void plot_obj_maps(vector<vector<image_map_element*>>* obj_vec_for_one_img,vector<vector<image_map_element*>>* image_map1,string orig_img_path);//for testing create_color_maps() function
    bool perform_time_analysis=false;
    int64_t find_neighbouring_objs_timer=0,find_neighbouring_obj_avg_color_of_closest_area_timer=0;
    int64_t first=0,second=0,third=0;
    
    //data preparation process handler function
    void start_data_preparation_process();
    void clean_up_prepared_data_obj();
    void save_current_img_data();
    void clean_image_package_class_entierly(bool clean_prepared_data);
    
    //meta data functions
    void enter_image_metadata(int id_,string label_,string dir_path_);
    void enter_image_metadata(int start_index,vector<string> *img_file_name,vector<string> *img_paths);
    void remove_image_metadata(int start_index);
    
    //setting up of training critical variables
    void enter_training_critical_variables(float color_sensi,float colot_sensi,int min_size_of_obj1);
    
    //threading functions
    void split_package_data(vector<image_package_class*> *ipc_vec);//ok tested
    void combine_package_data(vector<image_package_class*> *ipc_vec);//ok tested
    
    //constructor
    image_package_class(int id_,string label_,string dir_path_);
    image_package_class();

    //cleanup function
    void clean_up_all_the_data_addresses();

    //Annotate image functions
    static void next_image_button(int state, void* userdata);
    static void select_opject(int evt,int x,int y,int flags,void* param);
    set<int> selected_obj_index;
    struct click_data
    {
        bool m_btn_down=false;
        vector<Point> points;
    };
    void colour_obj(Mat& obj_map_plotted,vector<image_map_element*>& obj,bool use_rand_color);
    static void annote_object(int state, void* data);
};
#endif