#ifndef SEGMENT_CLASS_H_
#define SEGMENT_CLASS_H_

#include<iostream>
#include<vector>

#include"logic_core.h"
//#include"core_class.h"
//#include"data_package_class.h"
using namespace std;

class segment_class{

    private:
    int segment_aim,segment_no;//these acts as id for the segment
    bool id_lock=false;
    struct segment_architecture //brain_table like structure is not needed herin segment 
    {
        int no_of_cores=0;
        vector<int> core_no;
        vector<int> core_aim;
        vector<string> core_file_name;
        bool input_file_reader_segment=false;
        int train_test_predict;
        bool core_save_file_available=false;
    }segment_architecture_obj;
    
    struct core_result_prediction_structure
    {
        //under construction
    };
    logic_core logic_core_obj;
    vector<core_class> cores;
    
    public:
    void set_train_test_predict_mode(int,bool);

    void enable_input_file_reader_segment();

    bool return_input_file_reader_status();

    void set_id(int aim,int no);
    
    int return_segment_aim();
    
    int return_segment_no();
    
    string return_core_file_file_name(int index);
    
    int return_core_no(int index);
    
    int return_core_aim(int index);
    
    void add_core_info_into_segment(string core_network_file_name,int core_no,int core_aim);
    
    void add_core_info_into_segment(int core_no,int core_aim);
    
    int return_no_of_cores();
    
    bool initialize_the_cores();
    
    int return_no_of_corefilenames();

    void segment_process_handler();//under construction
};
#endif