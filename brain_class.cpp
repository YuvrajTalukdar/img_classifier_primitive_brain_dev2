/*describes brain_class.h*/

#include"brain_class.h"

bool brain_class::check_availability_of_core_filename()
{
    bool consistancy=false;
    vector<bool> availabilty_vec;
    for(int a=0;a<brain_table.size();a++)
    {
        if(brain_table[a].core_save_file_name.length()>0)
        {   availabilty_vec.push_back(true);}
        else
        {   availabilty_vec.push_back(false);}
    }
    int count_true=0,count_false=0;
    for(int a=0;a<availabilty_vec.size();a++)
    {   
        if(availabilty_vec[a]==true)
        {   count_true++;}
        else
        {   count_false++;}
    }
    if(count_true==availabilty_vec.size() || count_false==availabilty_vec.size())
    {
        if(count_false>count_true)
        {   return false;}
        else
        {   return true;}
    }
    else
    {
        cout<<"\nERROR!!! bstate file do not have all required core file names. Operation mode switching to complete training mode."<<endl;
        return false;
    }
}

void brain_class::restore_brain_state()
{
    cout<<"Restoring brain state...\n";
    //filteration of the brain table data
    bool core_save_file_available=check_availability_of_core_filename();
    //core_save_file_available if true than retrain the model or make predictions using the model(train/predict)
    //else train from scratch mode as no core files mentioned in the bstate file (train only)
    if(core_save_file_available==true)
    {   cout<<"Creating segments and cores and loading data into the cores...";}
    else 
    {   cout<<"Creating segments and cores, core save files name not present in bstate file, switching to complete training mode.";}
    //bstate file has core names to load. 
    for(int a=0;a<brain_table.size();a++)
    {
        segment_class *brain_segment_obj=new segment_class();
        bool found=false;
        for(int b=0;b<brain_segment.size();b++)
        {
            if(brain_segment[b].return_segment_aim()==brain_table[a].segment_aim && brain_segment[b].return_segment_no()==brain_table[a].segment_no)
            {   found=true;}
        }
        if(found==false)
        {   
            loaded_brain_architecture.segment_aim.push_back(brain_table[a].segment_aim);
            loaded_brain_architecture.segment_no.push_back(brain_table[a].segment_no);   
            brain_segment_obj->set_id(brain_table[a].segment_aim,brain_table[a].segment_no);
            brain_segment_obj->set_train_test_predict_mode(loaded_brain_architecture.train_test_predict_mode,loaded_brain_architecture.core_files_available);
            brain_segment.push_back(*brain_segment_obj);
        }
        free(brain_segment_obj);
    }
    //send core info to brain_segments_architecture
    for(int a=0;a<brain_segment.size();a++)
    {
        for(int b=0;b<brain_table.size();b++)
        {
            if(brain_segment[a].return_segment_aim()==brain_table[b].segment_aim && brain_segment[a].return_segment_no()==brain_table[b].segment_no)
            {   
                if(core_save_file_available==true)
                {   brain_segment[a].add_core_info_into_segment(brain_table[b].core_save_file_name,brain_table[b].core_no,brain_table[b].core_aim);}
                else
                {   brain_segment[a].add_core_info_into_segment(brain_table[b].core_no,brain_table[b].core_aim);}
            }
        }
    }
    //create cores into segments and load weight matrices form core files also send the ids into the cores
    for(int a=0;a<brain_segment.size();a++)
    {   
        if(a==0)//enable the datafile reader in the segment
        {   brain_segment[a].enable_input_file_reader_segment();}
        if(brain_segment[a].initialize_the_cores()==false)
        {   cout<<"\ncore files not available so new core created...";}
        else
        {   cout<<"\ncore files loaded successfully";}
    }
}

int brain_class::brain_state_file_chooser(vector<brain_state_file_identifier> brain_state_file_identifier_vec)
{
    int option;
    if(brain_state_file_identifier_vec.size()==0)
    {   
        cout<<"ERROR no brain state file found!!!!!";
        option=0;
    }
    else 
    {
        cout<<"List of available brain state files:\n\n";
        for(int a=0;a<brain_state_file_identifier_vec.size();a++)
        {
            cout<<a+1<<".  "<<"file_name= "<<brain_state_file_identifier_vec[a].file_name<<", bstate_save_id= "<<brain_state_file_identifier_vec[a].bstate_save_id<<", brain_aim= "
            <<brain_state_file_identifier_vec[a].brain_aim<<", brain_no= "<<brain_state_file_identifier_vec[a].brain_no<<", brain_state_aim= "
            <<brain_state_file_identifier_vec[a].brain_state_aim<<", date= "<<brain_state_file_identifier_vec[a].day
            <<"/"<<brain_state_file_identifier_vec[a].month<<"/"<<brain_state_file_identifier_vec[a].year<<", time= "
            <<brain_state_file_identifier_vec[a].hr<<":"<<brain_state_file_identifier_vec[a].min<<":"<<brain_state_file_identifier_vec[a].sec<<"\n\n\n";
        }
        cout<<"select 0 to quit, option= ";
        cin>>option;
    }
    return option-1;
}

bool brain_class::load_brain_architectue_map_file_if_found(bool &close_status)
{
    //get the file names with dates with comment(info or the task the architecture can perform)
    struct dirent *de;  // Pointer for directory entry
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir(".");
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        cout<<"Could not open current directory";
        return false;
    }
    char filename_sub_str[]="bstate";//full form brain state
    vector<string> network_save_file_list;
    network_save_file_list.clear();
    bool file_found=false;
    while ((de = readdir(dr)) != NULL)
    {
        if(strcasestr(de->d_name,filename_sub_str) && strcasestr(de->d_name,".csv"))
        {   network_save_file_list.push_back(de->d_name); file_found=true;}
    }
    closedir(dr);    
    vector<ifstream> input_bstate_file_streams(network_save_file_list.size());
    for(int a=0;a<network_save_file_list.size();a++)
    {   input_bstate_file_streams[a].open(network_save_file_list[a],ios::in);}
    //int input_bstate_file_stream_index;//holds the index of the file to be read.
    vector<brain_state_file_identifier> brain_state_file_identifier_vec;
    for(int a=0;a<input_bstate_file_streams.size();a++)
    {
        while(input_bstate_file_streams[a])
        {
            string line;
            input_bstate_file_streams[a]>>line;
            if(input_bstate_file_streams[a].eof())
            {   break;}
            char line_arr[line.length()];
            strcpy(line_arr,line.c_str());
            brain_state_file_identifier_default_obj.file_name=network_save_file_list[a];
            if(strcasestr(line_arr,"BASIC_BSTATE_FILE_INFO"))
            {
                input_bstate_file_streams[a]>>line;//for the category line
                line.clear();//we dont need this
                input_bstate_file_streams[a]>>line;//for the actual brain_state_file_obj
                char arr_temp[line.length()],ch[2];
                int word_index=0;
                for(int b=0;b<line.length();b++)
                {
                    if(line.at(b)==',')
                    {
                        word_index++;
                        if(word_index==2)
                        {   brain_state_file_identifier_default_obj.bstate_save_id.assign(arr_temp);}
                        else if(word_index==3)
                        {   brain_state_file_identifier_default_obj.brain_aim=atoi(arr_temp);}
                        else if(word_index==4)
                        {   brain_state_file_identifier_default_obj.brain_no=atoi(arr_temp);}
                        else if(word_index==5)
                        {   brain_state_file_identifier_default_obj.year=atoi(arr_temp);}
                        else if(word_index==6)
                        {   brain_state_file_identifier_default_obj.month=atoi(arr_temp);}
                        else if(word_index==7)
                        {   brain_state_file_identifier_default_obj.day=atoi(arr_temp);}
                        else if(word_index==8)
                        {   brain_state_file_identifier_default_obj.hr=atoi(arr_temp);}
                        else if(word_index==9)
                        {   brain_state_file_identifier_default_obj.min=atoi(arr_temp);}
                        else if(word_index==10)
                        {   brain_state_file_identifier_default_obj.sec=atoi(arr_temp);}
                        for(int c=0;c<line.length();c++)
                        {   arr_temp[c]='\0';}
                        continue;

                    }
                    ch[0]=line.at(b);
                    ch[1]='\0';
                    strcat(arr_temp,ch);
                }
                line.clear();
            }
            else if(strcasestr(line_arr,"BRAIN_STATE_AIM"))
            {
                input_bstate_file_streams[a]>>line;//for the actual brain_state_file_obj
                char arr_temp[line.length()],ch[2];
                int word_index=0;
                for(int c=0;c<line.length();c++)
                {   arr_temp[c]='\0';}
                for(int b=0;b<line.length();b++)
                {
                    if(line.at(b)==',')
                    {
                        word_index++;
                        if(word_index==2)
                        {   brain_state_file_identifier_default_obj.brain_state_aim.assign(arr_temp);}
                        for(int c=0;c<line.length();c++)
                        {   arr_temp[c]='\0';}
                        continue;
                    }
                    ch[0]=line.at(b);
                    ch[1]='\0';
                    strcat(arr_temp,ch);
                }
                line.clear();   
            }
        }
        brain_state_file_identifier_vec.push_back(brain_state_file_identifier_default_obj);
    }
    point1:
    int input_bstate_file_stream_index=brain_state_file_chooser(brain_state_file_identifier_vec);
    int vec_size=brain_state_file_identifier_vec.size();
    if(input_bstate_file_stream_index<-1 || input_bstate_file_stream_index>=vec_size)
    {   
        system("clear");
        cout<<"Invalid option!!\n";
        goto point1;
    }
    else if(input_bstate_file_stream_index==-1)
    {   
        cout<<"Brain shuting down ...";
        close_status=true;
        file_found=false;
    }
    
    //closing the file streams
    for(int a=0;a<input_bstate_file_streams.size();a++)
    {   input_bstate_file_streams[a].close();}
    
    if(file_found==true)
    {
        ifstream target_bstate_file(brain_state_file_identifier_vec[input_bstate_file_stream_index].file_name,ios::in);
        
        while(target_bstate_file)
        {
            string line;
            target_bstate_file>>line;
            if(target_bstate_file.eof())
            {   break;}
            char line_arr[line.length()];
            strcpy(line_arr,line.c_str());
            if(strcasestr(line_arr,"BASIC_BSTATE_FILE_INFO"))
            {
                target_bstate_file>>line;
                target_bstate_file>>line;
                char arr_temp[line.length()],ch[2];
                int word_index=0;
                for(int c=0;c<line.length();c++)
                {   arr_temp[c]='\0';}
                for(int a=0;a<line.length();a++)
                {
                    if(line.at(a)==',')
                    {
                        word_index++;
                        if(word_index==3)
                        {   loaded_brain_architecture.brain_aim=atoi(arr_temp);}
                        else if(word_index==4)
                        {   loaded_brain_architecture.brain_no=atoi(arr_temp);}
                        for(int c=0;c<line.length();c++)
                        {   arr_temp[c]='\0';}
                        continue;
                    }
                    ch[0]=line.at(a);
                    ch[1]='\0';
                    strcat(arr_temp,ch);
                }
                line.clear();
            }
            if(strcasestr(line_arr,"BASIC_BRAIN_STRUCTURE"))
            {
                target_bstate_file>>line;
                target_bstate_file>>line;
                char arr_temp[line.length()],ch[2];
                int word_index=0;
                for(int c=0;c<line.length();c++)
                {   arr_temp[c]='\0';}
                for(int a=0;a<line.length();a++)
                {
                    if(line.at(a)==',')
                    {
                        word_index++;
                        if(word_index==2)
                        {   loaded_brain_architecture.total_no_of_segments=atoi(arr_temp);}
                        else if(word_index==3)
                        {   loaded_brain_architecture.total_no_of_nn_cores=atoi(arr_temp);}
                        else if(word_index==4)
                        {   loaded_brain_architecture.total_no_of_logic_cores=atoi(arr_temp);}
                        for(int c=0;c<line.length();c++)
                        {   arr_temp[c]='\0';}
                        continue;
                    }
                    ch[0]=line.at(a);
                    ch[1]='\0';
                    strcat(arr_temp,ch);
                }
                line.clear();
            }
            if(strcasestr(line_arr,"BRAIN_TABLE"))
            {
                target_bstate_file>>line;
                //target_bstate_file>>line;
                if(target_bstate_file.eof())
                {   continue;}
                char arr_temp[line.length()],ch[2];
                int word_index=0;
                for(int c=0;c<line.length();c++)
                {   arr_temp[c]='\0';}
                //BRAIN_TABLE reader
                brain_table_record brain_table_record_obj;
                while(target_bstate_file)
                {
                    target_bstate_file>>line;
                    if(target_bstate_file.eof())
                    {   break;}
                    word_index=0;
                    for(int a=0;a<line.length();a++)
                    {
                        if(line.at(a)==',')
                        {
                            word_index++;
                            if(word_index==1)
                            {   brain_table_record_obj.id_primary_key=atoi(arr_temp);}
                            else if(word_index==2)
                            {   brain_table_record_obj.segment_aim=atoi(arr_temp);}
                            else if(word_index==3)
                            {   brain_table_record_obj.segment_no=atoi(arr_temp);}
                            else if(word_index==4)
                            {   brain_table_record_obj.core_aim=atoi(arr_temp);}
                            else if(word_index==5)
                            {   brain_table_record_obj.core_no=atoi(arr_temp);}
                            else if(word_index==6)
                            {   
                                brain_table_record_obj.core_save_file_name=arr_temp;
                                if(strcmp(arr_temp,"NULL")==0)
                                {   
                                    loaded_brain_architecture.train_test_predict_mode=0;
                                    loaded_brain_architecture.core_files_available=false;
                                }
                                else
                                {   loaded_brain_architecture.core_files_available=true;}
                            }
                            for(int c=0;c<line.length();c++)
                            {   arr_temp[c]='\0';}
                            continue;
                        }
                        ch[0]=line.at(a);
                        ch[1]='\0';
                        strcat(arr_temp,ch);
                    }
                    brain_table.push_back(brain_table_record_obj);
                    brain_table_record_obj.core_save_file_name="";
                    line.clear();
                }
            }
        }

        target_bstate_file.close();
        
        /*
        UNDER CONSTRUCTION
        load the architecture in the default architecture obj
        the default architecture must also hold the core to core connections and placement location of the cores in the segments.
        non nn cores info are not included in the architecture file but if connections need to be made between non nn cores than its included.
        */
    }
    return file_found;
}

void brain_class::brain_process_handler()
{
    //inter segment connections may be present here in future versions
    //start segemnt using threads
    vector<pthread_t> threadIds(brain_segment.size());
    vector<int> errors(brain_segment.size());
    for(int a=0;a<brain_segment.size();a++)
    {   errors[a]=pthread_create(&threadIds[a],NULL,(THREADFUNCPTR) &segment_class::segment_process_handler,&brain_segment[a]);}
    for(int a=0;a<errors.size();a++)
    {   if(errors[a])
        {   cout << "\nThread "<<a<< " creation failed : " << strerror(errors[a]);}
    }
    for(int a=0;a<brain_segment.size();a++)
    {  errors[a]=pthread_join(threadIds[a],NULL);}
}

void brain_class::initialize_brain()
{
    cout<<"Initializing brain...\n";
    bool files_found,close_status=false;
    files_found=load_brain_architectue_map_file_if_found(close_status);
    if(files_found==true)
    {   
        cout<<"\n\n\nBrain structure:-\n";
        cout<<"no_of_segments= "<<loaded_brain_architecture.total_no_of_segments<<endl;
        cout<<"no_of_nn_cores= "<<loaded_brain_architecture.total_no_of_nn_cores<<endl;
        cout<<"no_of_logic_cores= "<<loaded_brain_architecture.total_no_of_logic_cores<<endl;
        cout<<"brain_aim= "<<loaded_brain_architecture.brain_aim<<endl;
        cout<<"brain no= "<<loaded_brain_architecture.brain_no<<endl;
        cout<<"\nBRAIN_TABLE:- \n";
        for(int a=0;a<brain_table.size();a++)
        {   cout<<"primary_key= "<<brain_table[a].id_primary_key<<", segment_aim= "<<brain_table[a].segment_aim<<", segment_no= "<<brain_table[a].segment_no<<", core_aim= "<<brain_table[a].core_aim<<", core_no= "<<brain_table[a].core_no<<", core_file_name= "<<brain_table[a].core_save_file_name<<endl;}
        if(close_status==false && brain_table.size()>0)
        {  
            restore_brain_state();
            cout<<"\n\nAfter translating the BRAIN_TABLE:- ";
            for(int a=0;a<brain_segment.size();a++)
            {
                cout<<"\n\nsegment aim= "<<brain_segment[a].return_segment_aim()<<" segment no= "<<brain_segment[a].return_segment_no();
                for(int b=0;b<brain_segment[a].return_no_of_cores();b++)
                {   cout<<"\n   core_no= "<<brain_segment[a].return_core_no(b)<<" core_aim= "<<brain_segment[a].return_core_aim(b)<<" no_of_core_file_names= "<<brain_segment[a].return_no_of_corefilenames();}
            }
            brain_process_handler();
        }
        else if(brain_table.size()==0)
        {   cout<<"ERROR!!! brain table not found   shuting down...";}
    }
    else if(close_status==false)
    {   cout<<"ERROR: brain architecture file not found!!!!       shuting down...\n";}
}