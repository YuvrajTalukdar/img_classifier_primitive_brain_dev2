/*
core class handles natural cores
*/

#include<iostream>
#include<vector>
#include<fstream>
#include<sys/stat.h>
#include<string>
#include<algorithm>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/ioctl.h>//for the terminal size

#include"neuron_and_ann_class.h"
//#include"data_package_class.h"

typedef void * (*THREADFUNCPTR)(void *);

using namespace std;
static pthread_mutex_t lock_1;
inline bool display_iterations=false;//iteration display switch for debugging the code
inline bool pds=true;//progress_display_system for displaying the progress bar
//int fg=2;//for the checker
struct shared_block_data//required for 
{
    int predict_progress_bar_numerator=0;//required for prediction progress bar
    int predict_progress_bar_denominator=0;//required for prediction progress bar
    int no_of_c_datapacks_completed=0;//required for training progress bar
    int total_c_datapacks;//required for training progress bar

};
inline shared_block_data shared_block_data_obj;

struct datapack_structure_defination{
    int no_of_labels;
    int no_of_elements_in_each_record;
    vector<float> elements;
};

struct converted_data_pack{
    vector<vector<float>> firing_data;
    vector<vector<float>> not_firing_data;
    vector<float> objective_function_coefficients;
    vector<float> weight_matrix;//ans stored here.
    int firing_neuron_index;
    float firing_label;
    float lower_firing_constrain_rhs=60; //60

    float upper_not_firing_constrain_rhs=10; //10

    bool corupt_pack=false;
};

struct conflicting_data_id{
            vector<int> id;
            bool conflict_id_present=false;
        };

class ratio_locker
{
    private:
    bool lock_enabled=false;
    int no_of_labels;
    vector<int> no_of_data_for_each_labels;
    vector<float> ratios;
    public:
    void set_ratios(vector<int> data_no_vec)
    {
        if(lock_enabled==false)
        {
            lock_enabled=true;
            no_of_data_for_each_labels=data_no_vec;
            no_of_labels=data_no_vec.size();
            for(int a=0;a<no_of_labels;a++)
            {
                float sum=0;
                for(int b=0;b<no_of_labels;b++)
                {
                    if(b!=a)
                    {
                        sum=sum+data_no_vec[b];
                    }
                }
                ratios.push_back(data_no_vec[a]/sum);
            }
        }
    }

    void un_lock()
    {   lock_enabled=false;}

    void lock()
    {   lock_enabled=true;}

    float get_ratio(int label_index)
    {   return ratios[label_index];}

    float get_default_ratio()
    {   return 1;}

    bool is_locked()
    {   return lock_enabled;}
};
static ratio_locker ratio_locker1;

class modified_simplex_solver{
    private:
    string message;
    struct id{
            bool slack=false,basic=false,z=false,rhs=false,theta=false;
            int id;
        };

    struct simplex_table{

        vector<id> c_id;//no_of_columns-rhs-z
        vector<id> r_id; //no_of_rows-z_row
        vector<vector<float>> basic_var; //no_of_column-slack_var-z-rhs-theta*no_of_rows-z_row
        vector<vector<float>> slack_var; //no_of_columns-basic_var-rhs-theta*no_of_rows-z_row
        vector<float> z_col;//no_of_rows-z_row
        vector<double> rhs;//no_of_rows-z_row//actual double
        vector<long double> theta;//no_of_rows-z_row//actual long double
        vector<float> z_row;//no_of_columns
    };

    class simplex_optimizer{

    }s_optimizer;
    
    class make_solution_feasible{

        private:
        bool cyclic_bug_present_var=false;
        string message;

        void print_message();

        bool termination_condition_checker(simplex_table* st);

        void display_st(simplex_table st);

        long double round_to_zero(long double input);
        struct buffer
        {
            vector<float> basic_plus_slack_plus_z_plus_rhs_temp;
            vector<long double> sorted_theta;
            vector<int> p_row_index;
            vector<int> p_col_index;
        }buffer_obj;
        
        bool add_index_data(int p_col,int p_row);

        void simplex_table_modifier(int p_row_index,int p_col_index,simplex_table* st);

        conflicting_data_id conflict_id;

        void conflicting_data_finder(simplex_table* st);

        int iteration=0;
        void pivot_element_finder(simplex_table* st);

        public:
        bool cyclic_bug_present();

        conflicting_data_id return_conflict_id_pack();

        void start(simplex_table* st);

    }feasible_solution_calculator;

    void display_st(simplex_table st);
    
    void print_message();

    public:
    static bool check_for_corrupt_cdp(converted_data_pack* cdp);
    bool start_solver(converted_data_pack* cdp);
    bool cyclic_bug_present();
    converted_data_pack conflicting_data;
};

class simplex_solver_data_preparation_class
{
    private:
    string message;
    struct conflicting_data_buffer_vector_struct
    {
        vector<converted_data_pack> conflicting_data_buffer_vector;
    }conflicting_data_buffer_outer;

    ann* network;
    datapack_structure_defination* data_structure;
    converted_data_pack conflicting_data_buffer;
    modified_simplex_solver lpp_solver1;
    vector<converted_data_pack*> cdp;//memory_optimization7 : vector<converted_data_pack> cdp turned to vector<converted_data_pack*> cdp 

    void cdp_saver_in_mathematical_format(converted_data_pack* cdp);
    
    //void cdp_saver(converted_data_pack* cdp);
    void print_message();
    vector<converted_data_pack> fucked_up_cyclic_cdp;
    void cdp_spliter(vector<converted_data_pack*> &cdps,int index);
    public:
    static void cdp_viewer(converted_data_pack* cdp);
    void check_path_quality();
    void lp_solver();
    bool cyclic_bug_present();
    simplex_solver_data_preparation_class(vector<converted_data_pack> &cdps,datapack_structure_defination* ds,ann* network1);
};
//int training_step_counter=0;

struct network_structure_defination{
    int no_of_input_neuron=0;
    int no_of_output_neuron=0;
    int mean_buffer_size=0;
    vector<float> elements;
};

class core_class{
    private:
    //core_data_for_multithreading
    data_package_class* data_pack;
    int train_test_predict;
    float data_division1;
    string network_save_file_name;

    int core_no=0,core_aim=0;//this two must be changed using a function so that proper core is loaded
    int parent_segment_aim=0,parent_segment_no=0;
    string core_name;
    string core_save_file_name="NULL";//provided if core is loaded from a core/network savefile. Not set using constructor
    bool id_lock=false;

    int required_no_of_threads;
    int predict_progress_bar_numerator=0;//for the predict progress bar
    int predict_progress_bar_denominator=0;//for the predict progress bar
    float data_division=1.5;
    ann network1;
    //simplex_solver_data_preparation_class lpp_solver;
    datapack_structure_defination ds;
    network_structure_defination ns;

    bool check_if_datapack_has_valid_labels(data_package_class* data_pack);//if a label is 0 than it is invalid

    void save_network();

    bool load_network_if_available(int core_aim=0,int core_no=0,bool file_name_received=false,string file_name="");

    void network_analyzer();

    void datapack_analyzer(data_package_class* data_pack);//checked, it fills up the datapack_structure_defination ds. 

    void network_structure_modifier();//this function may need further improvements

    struct shuffling_data{
        vector<float> temp_data;
        int temp_label;
    };

    void shuffler(filtered_data* f_data);

    void f_data_viewer(string str,vector<filtered_data> f_data);

    void filter(data_package_class* data_pack,int train_test_predict);

    void big_c_datapack_handler(vector<converted_data_pack> &cdp);//passing the vector by reference //this function might be a temporary offer //this is for preventing 0:0 bug

    void simplex_solver_data_entry_point(vector<filtered_data> f_data_pack,datapack_structure_defination* ds);

    void display_training_progress();

    int size_of_c_datapacks_vector(vector<converted_data_pack> &c_datapacks);

    void c_data_packs_division_for_multi_threading(vector<vector<converted_data_pack>> &c_datapacks_vector,vector<converted_data_pack> &c_datapacks,int no_of_threads);

    bool ds_ns_have_same_labels();

    vector<filtered_data> f_train_data;//f_test_data; //memory_optimization1
    data_package_class test_data;

    void train(data_package_class* data_pack,bool network_avail_status,int train_test_predict);//there cannot be a case of invalid network and data without labels.

    void test();//parameters not required now
    //memory_optimization1
    void testing_for_each_label(/*,int train_test_predict*/);
    
    void only_testing(data_package_class* data_pack,int train_test_predict);

    void predict_progress_bar();

    void predict(data_package_class* data_pack);

    void make_prediction_on_user_entered_data();

    void set_the_no_of_threads_required(int no_of_c_datapacks_after_big_datapack_handling=0,bool tried_before=false);
    string message;
    void print_message();

    public:

    void start_core();//train_test_predict=1//train_test_predic is required for extra assurance

    void add_data(data_package_class* data_pack,int train_test_predict,float data_division1,string network_save_file_name);
    
    string return_name()
    {   return core_name;}
    core_class(int core_aim,int core_no,int parent_segment_aim,int parent_segment_no,string core_name);
};
