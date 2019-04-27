#include"data_package_class.h"

void image_package_class::data_arranger()
{

}

void image_package_class::border_info_extractor()
{

}

void image_package_class::contour_finder()
{

}

int image_package_class::avg_color_in_slice(Mat* slice,char color)//color maper function//ok tested
{
    int avg=0;
    for(int a=0;a<slice->rows;a++)
    {
        for(int b=0;b<slice->cols;b++)
        {
            if(color=='r'||color=='R')
            {   avg+=(int)slice->at<Vec3b>(a,b)[2];}
            else if(color=='g'||color=='G')
            {   avg+=(int)slice->at<Vec3b>(a,b)[1];}
            else if(color=='b'||color=='B')
            {   avg+=(int)slice->at<Vec3b>(a,b)[0];}
        }
    }
    avg=avg/(slice->rows*slice->cols);
    return avg;
}

float image_package_class::color_distance2(image_map_element* origin_element,image_map_element* new_element)//color maper function//ok tested but poor result than its counterpart
{
    int close_elements=0;
    int orig_row_index=origin_element->row_index*slice_row_size,orig_col_index=origin_element->col_index*slice_col_size;
    int new_row_index=new_element->row_index*slice_row_size,new_col_index=new_element->col_index*slice_col_size;
    Mat* orig_element_slice=new Mat();
    Mat ROI1(*orig_image_temp,Rect(orig_col_index,orig_row_index,slice_row_size,slice_col_size));
    ROI1.copyTo(*orig_element_slice);
    Mat* new_element_slice=new Mat();
    Mat ROI2(*orig_image_temp,Rect(new_col_index,new_row_index,slice_row_size,slice_col_size));
    ROI2.copyTo(*new_element_slice);
    for(int a=0;a<slice_row_size;a++)
    {
        for(int b=0;b<slice_col_size;b++)
        {
            for(int c=0;c<slice_row_size;c++)
            {
                for(int d=0;d<slice_col_size;d++)
                {
                    int red_orig=orig_element_slice->at<Vec3b>(c,d)[2],green_orig=orig_element_slice->at<Vec3b>(c,d)[1],blue_orig=orig_element_slice->at<Vec3b>(c,d)[0];
                    int red_new=new_element_slice->at<Vec3b>(a,b)[2],green_new=new_element_slice->at<Vec3b>(a,b)[1],blue_new=new_element_slice->at<Vec3b>(a,b)[0];
                    if(sqrt(pow((red_orig-red_new),2)+pow((green_orig-green_new),2)+pow((blue_orig-blue_new),2))<color_sensitiviy)
                    {   close_elements++;}
                }
            }
        }
    }
    float total=pow((slice_col_size*slice_row_size),2);
    float percentage=(((float)close_elements)/(float)total)*((float)100.0);
    return percentage;
}

float image_package_class::color_distance(image_map_element* origin_element,image_map_element* new_element)//color maper function//ok tested
{
    float distance=sqrt((origin_element->avg_red-new_element->avg_red)*(origin_element->avg_red-new_element->avg_red)+(origin_element->avg_green-new_element->avg_green)*(origin_element->avg_green-new_element->avg_green)+(origin_element->avg_blue-new_element->avg_blue)*(origin_element->avg_blue-new_element->avg_blue));
    return distance;
}

void image_package_class::search_for_neighbour(image_map_element* element,vector<vector<int>>* result)//color maper function//ok tested
{
    int col_index=element->col_index,row_index=element->row_index;
    int new_col_index,new_row_index;
    vector<int> co_ordinate(2);
    int delta_co_ordinates[8][2]={{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    for(int a=0;a<8;a++)
    {
        new_col_index=col_index+delta_co_ordinates[a][1];
        new_row_index=row_index+delta_co_ordinates[a][0];
        if(new_col_index>=0 && new_row_index>=0 && new_row_index<image_map.size() && new_col_index<image_map.at(new_row_index).size())
        {
            if(image_map.at(new_row_index).at(new_col_index)->select_status==false && color_distance(element,image_map.at(new_row_index).at(new_col_index))<=color_sensitiviy)
            //if(image_map.at(new_row_index).at(new_col_index)->select_status==false && color_distance2(element,image_map.at(new_row_index).at(new_col_index))>=percentage_of_close_pixels)
            {
                co_ordinate.at(0)=new_row_index;
                co_ordinate.at(1)=new_col_index;
                result->push_back(co_ordinate);
            }
        }
    }
}

void image_package_class::remove_non_free_elements(vector<vector<int>>* result)//color maper function//ok tested
{
    vector<bool> free_status(result->size());
    for(int a=0;a<result->size();a++)
    {
        if(image_map.at(result->at(a).at(0)).at(result->at(a).at(1))->select_status==true)
        {   free_status.at(a)=false;}
        else
        {   free_status.at(a)=true;}
    }
    for(int a=free_status.size()-1;a>=0;a--)
    {
        if(free_status.at(a)==false)
        {   result->erase(result->begin()+a);}
    }
}

void image_package_class::create_color_maps()//color maper function//ok tested
{
    no_of_slices_row_wise=(orig_image_temp->rows)/slice_row_size;
    no_of_slices_col_wise=(orig_image_temp->cols)/slice_col_size;
    //initializing the image map
    image_map.clear();
    int startx=0,starty=0;
    for(int a=0;a<no_of_slices_row_wise;a++)
    {
        vector<image_map_element*> map_row_temp;
        for(int b=0;b<no_of_slices_col_wise;b++)
        {
            image_map_element *element=new image_map_element();
            element->col_index=b;
            element->row_index=a;
            Mat* slice=new Mat();
            Mat ROI(*orig_image_temp,Rect(startx,starty,slice_row_size,slice_col_size));
            ROI.copyTo(*slice);
            element->avg_blue=avg_color_in_slice(slice,'b');
            element->avg_green=avg_color_in_slice(slice,'g');
            element->avg_red=avg_color_in_slice(slice,'r');
            //cout<<"\nstartx="<<startx<<" starty="<<starty<<", r="<<element.avg_red<<", g="<<element.avg_green<<", b="<<element.avg_blue;
            //imshow("slice",*slice);
            //waitKey(0);
            slice->release();
            startx+=slice_row_size;
            map_row_temp.push_back(element);
        }
        startx=0;
        starty+=slice_col_size;
        image_map.push_back(map_row_temp);
        map_row_temp.clear();
    }
    //object mapping process
    obj_vec.clear();
    int obj_index=0;
    for(int a=0;a<image_map.size();a++)//moving through the rows
    {
        for(int b=0;b<image_map.at(a).size();b++)//moving through the cols
        {
            if(image_map.at(a).at(b)->select_status==true)
            {   continue;}
            vector<vector<int>> result_buffer;
            result_buffer.clear();
            vector<vector<int>> result;
            result.clear();
            vector<image_map_element*> obj_elements_vec;
            obj_elements_vec.clear();
            //for entering the first element in the obj_element_vec
            image_map.at(a).at(b)->obj_id=obj_index;
            obj_elements_vec.push_back(image_map.at(a).at(b));
            image_map.at(a).at(b)->select_status=true;
            //for searching and adding the rest of the element in the obj_element_vec
            search_for_neighbour(image_map.at(a).at(b),&result);
            point1:
            if(result_buffer.size()!=0)
            {
                image_map.at(result_buffer.at(0).at(0)).at(result_buffer.at(0).at(1))->obj_id=obj_index;
                obj_elements_vec.push_back(image_map.at(result_buffer.at(0).at(0)).at(result_buffer.at(0).at(1)));
                image_map.at(result_buffer.at(0).at(0)).at(result_buffer.at(0).at(1))->select_status=true;
                result.clear();
                search_for_neighbour(image_map.at(result_buffer.at(0).at(0)).at(result_buffer.at(0).at(1)),&result);
            }
            while(result.size()>0)
            {
                image_map.at(result.at(0).at(0)).at(result.at(0).at(1))->obj_id=obj_index;
                obj_elements_vec.push_back(image_map.at(result.at(0).at(0)).at(result.at(0).at(1)));
                image_map.at(result.at(0).at(0)).at(result.at(0).at(1))->select_status=true;
                int row_index=result.at(0).at(0),col_index=result.at(0).at(1);//for storing the current element co-ordinates
                //before clearig other results needs to be handled
                if(result.size()>1)
                {
                    for(int c=1;c<result.size();c++)//adds all the results into the buffer excluding the first one
                    {   result_buffer.push_back(result.at(c));}
                }
                result.clear();
                search_for_neighbour(image_map.at(row_index).at(col_index),&result);
            }
            remove_non_free_elements(&result_buffer);
            if(result_buffer.size()!=0)
            {   goto point1;}
            obj_index++;
            obj_vec.push_back(obj_elements_vec);
        }
    }
    //cout<<"\n\nok";
}

void image_package_class::plot_obj_maps(vector<vector<image_map_element*>>* obj_vec_for_one_img,string orig_img_path)//for testing create_color_maps() function//ok tested
{
    cout<<"\nno_of_objects detected= "<<obj_vec_for_one_img->size();
    Mat *orig_image_temp1=new Mat();
    *orig_image_temp1=imread(orig_img_path);
    Mat obj_map_plotted;
    obj_map_plotted.create(orig_image_temp1->size(),orig_image_temp1->type());
    obj_map_plotted = Scalar::all(0);
    srand(time(NULL));//"initialize random seed:" as reported in the docs
    no_of_slices_row_wise=(orig_image_temp1->rows)/slice_row_size;
    no_of_slices_col_wise=(orig_image_temp1->cols)/slice_col_size;
    
    int total_no_of_elements_calc=0;
    for(int a=0;a<obj_vec_for_one_img->size();a++)
    {
        int rand_red=rand()%255+0,rand_green=rand()%255+0,rand_blue=rand()%255+0;
        if(obj_vec_for_one_img->at(a).size()>min_size_of_obj)
        {
            //cout<<"\na="<<obj_vec.at(a).size();
            total_no_of_elements_calc+=obj_vec_for_one_img->at(a).size();
            for(int b=0;b<obj_vec_for_one_img->at(a).size();b++)
            {  
                int orig_row_index=obj_vec_for_one_img->at(a).at(b)->row_index*slice_row_size,orig_col_index=obj_vec_for_one_img->at(a).at(b)->col_index*slice_col_size;
                for(int e=orig_row_index;e<orig_row_index+slice_row_size;e++)
                {
                    for(int f=orig_col_index;f<orig_col_index+slice_col_size;f++)
                    {
                        obj_map_plotted.at<Vec3b>(e,f)[0]=rand_blue;//blue
                        obj_map_plotted.at<Vec3b>(e,f)[1]=rand_green;//green
                        obj_map_plotted.at<Vec3b>(e,f)[2]=rand_red;//red
                    }
                }
            }
        }
    }
    //cout<<"\ntotal_no_of_elements="<<obj_vec_for_one_img->size()*obj_vec_for_one_img->at(0).size();
    cout<<"\ntotal_no_of_elements_calc="<<total_no_of_elements_calc;
    string img_name;
    img_name.append(orig_img_path);
    //img_name.append(to_string(color_sensitiviy));
    //img_name.push_back(',');
    //img_name.push_back(*to_string(slice_row_size).c_str());
    img_name.append(".png");
    imwrite(img_name.c_str(),obj_map_plotted);
    //imshow("obj_map",*orig_image_temp1);
    //waitKey(0);
    cout<<"\ncols="<<obj_map_plotted.cols<<" rows="<<obj_map_plotted.rows;
    //int gh;cin>>gh;
    //imshow("obj_map",obj_map_plotted);
    //waitKey(0);
    obj_map_plotted.release();
    orig_image_temp->release();
}

bool image_package_class::check_if_element_is_border_element(image_map_element* element)
{
    bool border_element=false;
    int delta_co_ordinates[8][2]={{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    int col_index=element->col_index,row_index=element->row_index;
    int new_col_index,new_row_index;
    int id=element->obj_id;
    for(int a=0;a<8;a++)
    {
        new_col_index=col_index+delta_co_ordinates[a][1];
        new_row_index=row_index+delta_co_ordinates[a][0];
        if(new_col_index>=0 && new_row_index>=0 && new_row_index<image_map.size() && new_col_index<image_map.at(new_row_index).size())
        {
            if(image_map.at(new_row_index).at(new_col_index)->obj_id!=id)
            {   border_element=true;break;}
        }
        else
        {   border_element=true;break;}
    }
    return border_element;
}

void image_package_class::find_neighbouring_objs(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_all_objs,vector<vector<image_map_element*>> *results)//need testing
{
    int new_col_index,new_row_index;
    int delta_co_ordinates[8][2]={{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    //detect the first bordel cell by brut force
    image_map_element *border_element;
    vector<image_map_element*> border_element_vec;
    for(int a=0;a<obj->size();a++)
    {
        int col_index=obj->at(a)->col_index,row_index=obj->at(a)->row_index;
        bool border_element_found=false;
        for(int b=0;b<8;b++)
        {
            new_col_index=col_index+delta_co_ordinates[b][1];
            new_row_index=row_index+delta_co_ordinates[b][0];
            if(new_col_index>=0 && new_row_index>=0 && new_row_index<image_map.size() && new_col_index<image_map.at(new_row_index).size())
            {
                if(image_map.at(new_row_index).at(new_col_index)->obj_id!=obj->at(a)->obj_id)
                {   obj->at(a)->border_directions[b]=true;border_element_found=true;}
                else
                {   obj->at(a)->border_directions[b]=false;}
            }
        }
        if(border_element_found==true)
        {   border_element=obj->at(a);break;}
    }
    border_element_vec.push_back(border_element);
    
    //detect the  rest of the border elements
    int a=0;
    point1:
    int index_of_first_element_added=-1;
    bool new_element_added=false;
    for(a;a<border_element_vec.size();a++)
    {
        for(int b=0;b<8;b++)
        {
            new_col_index=border_element_vec.at(a)->col_index+delta_co_ordinates[b][1];
            new_row_index=border_element_vec.at(a)->row_index+delta_co_ordinates[b][0];
            if(new_col_index>=0 && new_row_index>=0 && 
               new_row_index<image_map.size() && new_col_index<image_map.at(new_row_index).size() && 
               check_if_element_is_border_element(image_map.at(new_row_index).at(new_col_index))==true &&
               image_map.at(new_row_index).at(new_col_index)->obj_id==border_element_vec.at(a)->obj_id)
            {
                bool found=false;
                for(int c=0;c<border_element_vec.size();c++)
                {
                    if(border_element_vec.at(c)->row_index==image_map.at(new_row_index).at(new_col_index)->row_index &&
                       border_element_vec.at(c)->col_index==image_map.at(new_row_index).at(new_col_index)->col_index)
                    {   found=true;break;}
                }
                if(found==false)
                {   
                    border_element_vec.push_back(image_map.at(new_row_index).at(new_col_index));new_element_added=true;
                    if(index_of_first_element_added!=-1)
                    {   index_of_first_element_added=a;}
                }
            }
        }
    }
    if(new_element_added==true)
    {
        a=index_of_first_element_added;
        goto point1;
    }
    
    cout<<"\n\nresults=\n\n";
    for(int a=0;a<border_element_vec.size();a++)
    {
        cout<<"("<<border_element_vec.at(a)->row_index<<","<<border_element_vec.at(a)->col_index<<")";
    }
    cout<<"\nsize="<<border_element_vec.size();

}

void image_package_class::find_neighbouring_obj_avg_color_of_closest_area(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_neighbouring_objs,image_map_element *element)//need testing
{
    int required_no_of_sq_for_finding_avg=no_of_sq_areas_need_to_be_checked_for_avg_color;
    vector<float> distance1;
    vector<obj_distance> neighbouring_obj_cell_and_current_obj_distance_vec;
    struct sortingclass_for_neighbouring_obj_cell_and_current_obj_distance_vec
    {
        bool operator() (obj_distance obj1,obj_distance obj2) 
        { return (obj1.distance<obj2.distance);}
    }sorting_helper2;
    vector<image_map_element*> obj_info;
    for(int a=0;a<list_of_neighbouring_objs->size();a++)
    {
        if(list_of_neighbouring_objs->at(a).size()<no_of_sq_areas_need_to_be_checked_for_avg_color)//some bug may be here
        {   required_no_of_sq_for_finding_avg=list_of_neighbouring_objs->at(a).size();}
        int nearest_distance=-1;
        //this finds the nearest possible distance between a cell of the neighbouring objand the current obj. 
        neighbouring_obj_cell_and_current_obj_distance_vec.clear();
        for(int b=0;b<list_of_neighbouring_objs->at(a).size();b++)
        {
            distance1.clear();
            for(int c=0;c<obj->size();c++)
            {   distance1.push_back(sqrt(pow((list_of_neighbouring_objs->at(a).at(b)->row_index-obj->at(c)->row_index),2)+pow((list_of_neighbouring_objs->at(a).at(b)->row_index-obj->at(c)->row_index),2)));}
            sort(distance1.begin(),distance1.end());          
            obj_distance obj1;
            obj1.distance=distance1.at(0);
            obj1.obj_id1=obj->at(0)->obj_id;
            obj1.obj_id2=list_of_neighbouring_objs->at(a).at(b)->obj_id;//b can be 0 too
            obj1.red=list_of_neighbouring_objs->at(a).at(b)->avg_red;
            obj1.green=list_of_neighbouring_objs->at(a).at(b)->avg_green;
            obj1.blue=list_of_neighbouring_objs->at(a).at(b)->avg_blue;
            neighbouring_obj_cell_and_current_obj_distance_vec.push_back(obj1);
        }
        sort(neighbouring_obj_cell_and_current_obj_distance_vec.begin(),neighbouring_obj_cell_and_current_obj_distance_vec.end(),sorting_helper2);
        //calculating the avg color for required_no_of_sq_for_finding_avg of neighbouring_obj
        image_map_element *element1=new image_map_element(); 
        element1->obj_id=list_of_neighbouring_objs->at(a).at(0)->obj_id;
        for(int b=0;b<required_no_of_sq_for_finding_avg;b++)
        {
            element1->avg_blue+=neighbouring_obj_cell_and_current_obj_distance_vec.at(b).blue;
            element1->avg_green+=neighbouring_obj_cell_and_current_obj_distance_vec.at(b).green;
            element1->avg_blue+=neighbouring_obj_cell_and_current_obj_distance_vec.at(b).blue;
        }
        element1->avg_blue/required_no_of_sq_for_finding_avg;
        element1->avg_green/required_no_of_sq_for_finding_avg;
        element1->avg_red/required_no_of_sq_for_finding_avg;
        obj_info.push_back(element1);
    }
    neighbouring_obj_cell_and_current_obj_distance_vec.clear();
    //checking which obj is most appropriate to be included in the current obj
        //get the avg color of the current obj
    image_map_element *element2=new image_map_element();
    element2->obj_id=obj->at(0)->obj_id;
    for(int a=0;a<obj->size();a++)
    {
        element2->avg_red+=obj->at(a)->avg_red;
        element2->avg_green+=obj->at(a)->avg_green;
        element2->avg_blue+=obj->at(a)->avg_blue;
    }
    element2->avg_red/obj->size();
    element2->avg_green/obj->size();
    element2->avg_blue/obj->size();
    for(int a=0;a<obj_info.size();a++)
    {
        obj_distance obj1;
        obj1.obj_id1=obj->at(0)->obj_id;
        obj1.obj_id2=obj_info.at(a)->obj_id;
        obj1.distance=sqrt(pow((obj_info.at(a)->avg_red-element2->avg_red),2)+pow((obj_info.at(a)->avg_green-element2->avg_green),2)+pow((obj_info.at(a)->avg_blue-element2->avg_blue),2));
        neighbouring_obj_cell_and_current_obj_distance_vec.push_back(obj1);
    }
    sort(neighbouring_obj_cell_and_current_obj_distance_vec.begin(),neighbouring_obj_cell_and_current_obj_distance_vec.end(),sorting_helper2);
    element->obj_id=neighbouring_obj_cell_and_current_obj_distance_vec.at(0).obj_id2;
    //needs detailed testing and efficiency needs to be improved
}

void image_package_class::same_obj_combination_process()//need testing, under construction
{
    vector<vector<image_map_element*>> obj_vec_temp;
    obj_vec_temp=obj_vec;
    sort(obj_vec_temp.begin(),obj_vec_temp.end(),sorting_helper1);
    vector<vector<image_map_element*>> neighbours_found;
    /*for(int a=0;a<obj_vec_temp.size();a++)
    {
        if(obj_vec_temp.at(a).size()<min_size_of_obj)
        {
            neighbours_found.clear();
            find_neighbouring_objs(&obj_vec_temp.at(a),&obj_vec_temp,&neighbours_found);
            //calc avg color of neighbouring obj. For only a small nearby portion of the neighbouring obj.
            //image_map_element element;
            //find_neighbouring_obj_avg_color_of_closest_area(&obj_vec_temp.at(a),&neighbours_found,&element);
            //add the obj to its required neighbour
            //remove the obj

            //handle the loop as size change is going on 
        }
        cout<<"\nsize="<<obj_vec_temp.at(a).size();
    }*/
    //for testing purpose
    vector<image_map_element*> obj_for_test;
    cout<<"\nsample obj=\n\n";
    int b=0,middle=10;
    for(int a=0;a<10;a++)
    {
        if(a==0||a==1||a==2)
        {
            for(int c=0;c<10+b;c++)
            {
                cout<<"("<<image_map.at(a).at(c)->row_index<<","<<image_map.at(a).at(c)->col_index<<")";
                image_map.at(a).at(c)->obj_id=-1;
                obj_for_test.push_back(image_map.at(a).at(c));
            }
            cout<<endl;
        }
        else
        {
            cout<<"("<<image_map.at(a).at(5)->row_index<<","<<image_map.at(a).at(5)->col_index<<")";
            image_map.at(a).at(5)->obj_id=-1;
            obj_for_test.push_back(image_map.at(a).at(5));

            cout<<"("<<image_map.at(a).at(6)->row_index<<","<<image_map.at(a).at(6)->col_index<<")";
            image_map.at(a).at(6)->obj_id=-1;
            obj_for_test.push_back(image_map.at(a).at(6));

            cout<<"("<<image_map.at(a).at(7)->row_index<<","<<image_map.at(a).at(7)->col_index<<")";
            image_map.at(a).at(7)->obj_id=-1;
            obj_for_test.push_back(image_map.at(a).at(7));
            cout<<endl;
        }
    }
    find_neighbouring_objs(&obj_for_test,&obj_vec_temp,&neighbours_found);
    int gh;cin>>gh;
}

void image_package_class::start_data_preparation_process()
{
    clean_up_prepared_data_obj();
    for(unsigned int a=0;a<image_paths.size();a++)
    {
        orig_image_temp=new Mat();
        *orig_image_temp=imread(image_paths[a]);
        create_color_maps();
        same_obj_combination_process();

        contour_finder();
        border_info_extractor();
        data_arranger();
        orig_image_temp->release();

        save_current_img_data();
    }
}

void image_package_class::save_current_img_data()
{
    prepared_data_obj.image_map_vec.push_back(image_map);
    prepared_data_obj.obj_vec_for_each_img.push_back(obj_vec);
}

void image_package_class::clean_up_prepared_data_obj()
{
    prepared_data_obj.image_map_vec.clear();
    prepared_data_obj.obj_vec_for_each_img.clear();
}

void image_package_class::enter_training_critical_variables(int no_of_sq_areas_need_to_be_checked_for_avg_color1,float color_sensi,int slice_row,int slice_col,int min_size_of_obj1)
{
    min_size_of_obj=min_size_of_obj1;
    color_sensitiviy=color_sensi;
    no_of_sq_areas_need_to_be_checked_for_avg_color=no_of_sq_areas_need_to_be_checked_for_avg_color1;
    slice_row_size=slice_row;
    slice_col_size=slice_col;
}

void image_package_class::enter_image_metadata(string img_name,string img_path)
{   
    image_file_name.push_back(img_name);
    image_paths.push_back(img_path);
}

void image_package_class::enter_image_metadata(int id_,string label_,string dir_path_)
{
    if(lock_enabled==false)
    {
        label_id=id_;
        label_str=label_;
        dir_path="image_data";
        dir_path.append(dir_path_);
        dir_path.append("/.");
        lock_enabled=true;
    }
    else
    {   cout<<"object locked";}
}

void image_package_class::enter_image_metadata(int start_index,vector<string> *img_file_name,vector<string> *img_paths)
{
    cout<<"\nstart index= "<<start_index;
    image_file_name.insert(image_file_name.end(),img_file_name->begin()+start_index,img_file_name->end());
    image_paths.insert(image_paths.end(),img_paths->begin()+start_index,img_paths->end());
}

void image_package_class::remove_image_metadata(int start_index)
{
    image_file_name.erase(image_file_name.begin()+start_index,image_file_name.end());
    image_paths.erase(image_paths.begin()+start_index,image_paths.end());
}

void image_package_class::split_package_data(vector<image_package_class*> *ipc_vec)
{
    cout<<"\nspliting started...";
    if(ipc_vec->size()>return_no_of_images())
    {
        int size=ipc_vec->size();
        for(int a=0;a<(size-return_no_of_images()+1);a++)
        {   ipc_vec->pop_back();}
    }
   
    int no_of_img_per_vec=return_no_of_images()/(ipc_vec->size()+1);
    int b=image_file_name.size()-1,start_index;
    for(int a=0;a<ipc_vec->size();a++)
    {
        //load meta data
        ipc_vec->at(a)->enter_image_metadata(label_id,label_str,dir_path);
        start_index=image_file_name.size()-no_of_img_per_vec;
        //copy the required data
        ipc_vec->at(a)->enter_image_metadata(start_index,&image_file_name,&image_paths);
        //delete the required data
        remove_image_metadata(start_index);
    }
    
    cout<<"\nspliting complete";
}

void image_package_class::combine_package_data(vector<image_package_class*> *ipc_vec)
{
    cout<<"\nchecking part (temporary)";
    //combination part
    for(int a=0;a<ipc_vec->size();a++)
    {
        for(int b=0;b<ipc_vec->at(a)->return_no_of_images();b++)
        {
            //combine meata_data
            image_paths.push_back(ipc_vec->at(a)->return_img_path(b));
            image_file_name.push_back(ipc_vec->at(a)->return_img_file_name(b));
            //combine result_data    
            prepared_data_obj.image_map_vec.push_back(ipc_vec->at(a)->prepared_data_obj.image_map_vec.at(b));
            prepared_data_obj.obj_vec_for_each_img.push_back(ipc_vec->at(a)->prepared_data_obj.obj_vec_for_each_img.at(b));
        }
        ipc_vec->at(a)->clean_up_all_the_data_addresses();
        //under construction for rest of the result data
    }
    //for testing
    for(int a=0;a<prepared_data_obj.obj_vec_for_each_img.size();a++)
    {   plot_obj_maps(&prepared_data_obj.obj_vec_for_each_img.at(a),image_paths.at(a));}
    //int gh;cin>>gh;
}

void image_package_class::clean_up_all_the_data_addresses()
{
    image_paths.clear();
    image_file_name.clear();
    prepared_data_obj.obj_vec_for_each_img.clear();
    prepared_data_obj.image_map_vec.clear();
    image_map.clear();
    obj_vec.clear();
}

image_package_class::image_package_class(int id_,string label_,string dir_path_)
{
    if(lock_enabled==false)
    {
        label_id=id_;
        label_str=label_;
        dir_path="image_data";
        dir_path.append(dir_path_);
        dir_path.append("/.");
        lock_enabled=true;
    }
    else
    {   cout<<"object locked";}
}

image_package_class::image_package_class()
{
    //blank constructor for empty package.
}