#include"logic_core.h"

using namespace std;

void img_data_preparation_process_handler::slicing_process_handler()
{
    int no_of_physical_threads=sysconf(_SC_NPROCESSORS_ONLN);
    vector<image_package_class*> ipc_vec;
    //ipc_vec.push_back(obj);
    //creation of objs for threads
    obj->enter_training_critical_variables(no_of_sq_areas_need_to_be_checked_for_avg_color,color_sensitivity,color_sensitivity2,min_size_of_obj);
    for(int a=0;a<no_of_physical_threads-1;a++)
    {
        image_package_class* new_obj=new image_package_class();
        new_obj->enter_training_critical_variables(no_of_sq_areas_need_to_be_checked_for_avg_color,color_sensitivity,color_sensitivity2,min_size_of_obj);
        ipc_vec.push_back(new_obj);
    }
    obj->split_package_data(&ipc_vec);
    ipc_vec.push_back(obj);
    cout<<"\nipc_vec size="<<ipc_vec.size();
    for(int a=0;a<ipc_vec.size();a++)
    {   cout<<"\nno of img in vec="<<ipc_vec.at(a)->return_no_of_images();}
    //threading process
    cout<<"\ndata preparation thread running...";
    vector<pthread_t> img_data_preparation_thread(ipc_vec.size());
    vector<int> errors(ipc_vec.size());
        //running the threads
    for(int a=0;a<img_data_preparation_thread.size();a++)
    {   errors[a]=pthread_create(&img_data_preparation_thread[a],NULL,(THREADFUNCPTR)&image_package_class::start_data_preparation_process,ipc_vec.at(a));}
    for(int a=0;a<img_data_preparation_thread.size();a++)
    {
        if(errors[a])
        {   cout<<"\nThread "<<a<<" creation failed : "<<strerror(errors[a]);}
    }
    for(int a=0;a<img_data_preparation_thread.size();a++)
    {   errors[a]=pthread_join(img_data_preparation_thread[a],NULL);}
    //data combining process
    ipc_vec.pop_back();
    obj->combine_package_data(&ipc_vec);
    ipc_vec.clear();
    cout<<"\nfinised a label....";
}

void logic_core::fix_first_read_dir_name(string &str)
{
   bool fixed=false;
   for(int a=0;a<str.length();a++)
   {
      if(str.at(a)=='/' && str.at(a+1)=='.' && fixed==false)
	  {
	     str.erase(0,a);
	     fixed=true;
      }
   }
}

bool logic_core::read_image_data()
{
    image_package_class *dataset_table_obj_address;
    ifstream stream1("image_data/dataset_table.csv",ios::in);
    string line;
    stream1>>line;
    cout<<"\nline1="<<line;
    stream1>>line;
    cout<<"\nline2="<<line;
    while(stream1)
    {
        string line2;
        stream1>>line2;
        line=line2;
        if(stream1.eof())
        {   break;}
        char line_arr[line.length()],ch[2];
        for(int a=0;a<line.length();a++)
	    {   line_arr[a]='\0';}	

        int word_index=0;
        if(stream1.eof())
        {   break;}	
        
        int id_temp;
        string label_temp,dir_path_temp;
        label_temp.clear();
        dir_path_temp.clear();
        for(int a=0;a<line.length();a++)
        {    
            if(line.at(a)==',')
            { 
                word_index++;
                if(word_index==1)
                {   id_temp=atoi(line_arr);}
                else if(word_index==2)
                {   label_temp.assign(line_arr);}
                else if(word_index==3) 
                {   dir_path_temp.assign(line_arr);}
                for(int b=0;b<line.length();b++)
                {   line_arr[b]='\0';}
                continue;
            }
            ch[0]=line.at(a);
            ch[1]='\0';
            strcat(line_arr,ch);
        }
        //cout<<"\ndir_vec="<<dir_path_temp.c_str()<<" name="<<label_temp.c_str()<<" line="<<line.c_str();
        dataset_table_obj_address=new image_package_class(id_temp,label_temp,dir_path_temp);
        image_package_vec.push_back(dataset_table_obj_address);  
    }
    
    struct dirent *de;  // Pointer for directory entry
    // opendir() returns a pointer of DIR type.
    DIR *dr;
    vector<DIR*> dir_vec;char arr[5]={'\0'};
    for(int a=0;a<image_package_vec.size();a++)
    {
        dr=opendir(image_package_vec[a]->return_dir_path().c_str());
        dir_vec.push_back(dr);
    } 
    for(int a=0;a<dir_vec.size();a++)
    {
        if(dir_vec[a]==NULL)
        {
            cout<<"\nCould not open the directory";
            false;
        }
    }
    for(int a=0;a<image_package_vec.size();a++)
    {
        //cout<<"\n\nlabel dir= "<<image_package_vec[a].dir_path;//display data
        int b=0;
        string label_data_path=image_package_vec[a]->return_dir_path();
        label_data_path.erase(label_data_path.begin()+label_data_path.length()-1);//removing .
        while((de = readdir(dir_vec[a]))!=NULL)
	    {
	        if(b>1)
            {	
                string image_path;
                image_path.clear();
                image_path.append(label_data_path);
                image_path.append(de->d_name);
                if(b-2==0)
                {   fix_first_read_dir_name(image_path);}
                if(strcasestr(de->d_name,".jpg") || strcasestr(de->d_name,".jpeg") || strcasestr(de->d_name,".png"))//filters the file format
                {   image_package_vec[a]->enter_image_metadata(de->d_name,image_path);}
                b++;
            }
            else if(b<=1)
	        {   b++;}  
        }
	    closedir(dir_vec[a]);    	
    }
}

void logic_core::train_segment(vector<core_class> &core_vec)
{
    cout<<"\n\nsize= "<<image_package_vec.size();    
    //creating the threads and preparation of the process handler for preparation of fata of each label
    vector<pthread_t> img_data_preparation_thread(image_package_vec.size());
    vector<int> errors(image_package_vec.size());
    img_data_preparation_process_handler_vec.clear();
    for(int a=0;a<image_package_vec.size();a++)
    {
        img_data_preparation_process_handler* obj1=new img_data_preparation_process_handler();
        obj1->color_sensitivity=8;//12,8,10 for the color mapper
        obj1->color_sensitivity2=40;//40,25,10//for un strict combination
        obj1->no_of_sq_areas_need_to_be_checked_for_avg_color=5;//value for testing
        obj1->min_size_of_obj=40;//40
        obj1->obj=image_package_vec[a];
        img_data_preparation_process_handler_vec.push_back(obj1);
    }
    //running the threads
    /*
    for(int a=0;a<img_data_preparation_thread.size();a++)
    {   errors[a]=pthread_create(&img_data_preparation_thread[a],NULL,(THREADFUNCPTR)&img_data_preparation_process_handler::slicing_process_handler,img_data_preparation_process_handler_vec[a]);}
    for(int a=0;a<img_data_preparation_thread.size();a++)
    {
        if(errors[a])
        {   cout<<"\nThread "<<a<<" creation failed : "<<strerror(errors[a]);}
    }
    for(int a=0;a<img_data_preparation_thread.size();a++)
    {   errors[a]=pthread_join(img_data_preparation_thread[a],NULL);}
    */
    for(int a=0;a<img_data_preparation_process_handler_vec.size();a++)
    {
        img_data_preparation_process_handler_vec[a]->slicing_process_handler();
    }
    //img_data_preparation_process_handler_vec[0]->slicing_process_handler();   
}

void logic_core::set_logic_core_no(int no)
{
    if(locked==false)
    {   logic_core_no=no;}
    else
    {   cout<<"\nERROR failed to set logic_core_no as it is already locked";}
}

string logic_core::return_core_name(int index)
{   return available_names[index];}

int logic_core::return_no_of_nn_cores_needed()
{   return no_of_nn_cores_needed;}

logic_core::logic_core()
{
    available_names.push_back("RED");
    available_names.push_back("GREEN");
    available_names.push_back("BLUE");
    available_names.push_back("CANNY_EDGE");
}