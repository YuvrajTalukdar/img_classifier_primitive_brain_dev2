#ifndef BRAIN_CLASS_H_
#define BRAIN_CLASS_H_

#include<stdexcept>
#include<cstdlib>
#include<dirent.h> //for directory
#include<fstream>
#include<string.h>

#include"segment_class.h"

using namespace std;

class brain_class{
    
    private:
    //brain baisc structure handling data structures.
    struct brain_state_file_identifier//acts as a storage buffer for the bstate file loader
    {
        int brain_aim,brain_no,day,month,year,hr,min,sec;
        string bstate_save_id,file_name,brain_state_aim;
    }brain_state_file_identifier_default_obj;
    struct brain_table_record//holds the segment and core info from the bstate file
    {   
        int id_primary_key,segment_aim,segment_no,core_aim,core_no;
        string core_save_file_name;
    };
    vector<brain_table_record> brain_table;
    struct brain_architecture
    {
        //this will hols the map of the entire brain including its connections
        int brain_aim,brain_no,brain_id;
        /*brain_id will be make with the help of constant info
          like no of cores and segments. need further analysis as these numbers may change in future*/
        int total_no_of_segments,total_no_of_nn_cores,total_no_of_logic_cores;
        vector<int> segment_aim;
        vector<int> segment_no;
        int train_test_predict_mode=0;//train=0,test=1,predict=2, but during construction 0 means train and test
        bool core_files_available=false;
    }loaded_brain_architecture;
    vector<segment_class> brain_segment;
    int train_test_predict_annote_code=-1;
    bool test_predict_available=false;
    //brain baisc structure handling functions.
    void restore_brain_state();
    int brain_state_file_chooser(vector<brain_state_file_identifier> brain_state_file_identifier_vec);
    bool load_brain_architectue_map_file_if_found(bool &close_status);
    bool check_availability_of_core_filename();
    //data classes handler
    void parallel_ipc_handler();
    
    void brain_process_handler();

    public:
    
    void initialize_brain();
};

#endif