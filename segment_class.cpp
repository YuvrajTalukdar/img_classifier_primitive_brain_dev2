/*describes segment_class.h*/

#include"segment_class.h"

using namespace std;

void segment_class::set_train_test_predict_mode(int train_test_predict,bool core_save_file_available)
{   
    segment_architecture_obj.train_test_predict=train_test_predict;
    segment_architecture_obj.core_save_file_available=core_save_file_available;
}

void segment_class::set_id(int aim,int no)
{
    if(id_lock==false)
    {
        segment_no=no;
        segment_aim=aim;
        logic_core_obj.set_logic_core_no(no);
    }
    else
    {   cout<<"ERROR!!! Failed to set id because id_lock enabled.";}
}

int segment_class::return_segment_aim()
{   return segment_aim;}

int segment_class::return_segment_no()
{   return segment_no;}

string segment_class::return_core_file_file_name(int index)
{   return segment_architecture_obj.core_file_name[index];}

int segment_class::return_core_aim(int index)
{   return segment_architecture_obj.core_aim[index];}

int segment_class::return_core_no(int index)
{   return segment_architecture_obj.core_no[index];}

int segment_class::return_no_of_cores()
{   return segment_architecture_obj.no_of_cores;}

int segment_class::return_no_of_corefilenames()
{   return segment_architecture_obj.core_file_name.size();}

void segment_class::add_core_info_into_segment(string core_network_file_name,int core_no,int core_aim)//if core_network_save_file available
{
    segment_architecture_obj.core_aim.push_back(core_aim);
    segment_architecture_obj.core_no.push_back(core_no);
    segment_architecture_obj.core_file_name.push_back(core_network_file_name);
    segment_architecture_obj.no_of_cores++;
}

void segment_class::add_core_info_into_segment(int core_no,int core_aim)//if core_network_save_file not_availble
{   
    segment_architecture_obj.core_aim.push_back(core_aim);
    segment_architecture_obj.core_no.push_back(core_no);
    segment_architecture_obj.no_of_cores++;
}

bool segment_class::initialize_the_cores()
{
    if(segment_architecture_obj.no_of_cores>0)
    {
        //loading weight matrix from core files
        if(segment_architecture_obj.core_file_name.size()>0 && segment_architecture_obj.core_save_file_available==true)
        {   
            //1. create the cores set the id using core_class constructor
            //2. set core_save_file_name in core_class_obj
            //3. load the data into the core_class_obj
                //under construction
                
                //datapack + train_test_predict + save_file_name+ data_division needed
            
            return true;
        }
        else
        {   
            //creation of brand new cores
            for(int a=0;a<logic_core_obj.return_no_of_nn_cores_needed();a++)
            {
                if(logic_core_obj.return_no_of_nn_cores_needed()==segment_architecture_obj.core_no.size())
                {
                    core_class *core_add= new core_class(segment_architecture_obj.core_aim[a],
                                                         segment_architecture_obj.core_no[a],
                                                         segment_aim,
                                                         segment_no,
                                                         logic_core_obj.return_core_name(a));
                    cores.push_back(*core_add);
                }
            }
            return false;
            //construction completed
        }

        //under construction
    }
    else
    {   cout<<"\n\nERROR!!! no core information loaded in segment no "<<segment_no;return false;}
}

void segment_class::enable_input_file_reader_segment()
{
    if(segment_architecture_obj.input_file_reader_segment==false)
    {   segment_architecture_obj.input_file_reader_segment=true;}
}

bool segment_class::return_input_file_reader_status()
{   return segment_architecture_obj.input_file_reader_segment;}

void segment_class::segment_process_handler()
{      
    cores.clear();
    //under construction
}