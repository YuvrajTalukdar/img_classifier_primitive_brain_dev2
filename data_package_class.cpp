#include"data_package_class.h"

void image_package_class::data_arranger()
{

}

void image_package_class::border_info_extractor()
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
    float distance=0;
    int no_of_cells_to_check;
    if(obj_elements_vec_ptr->size()>=5)
    {   no_of_cells_to_check=5;}
    else
    {   no_of_cells_to_check=obj_elements_vec_ptr->size();}
    for(int a=obj_elements_vec_ptr->size()-1;a>=obj_elements_vec_ptr->size()-no_of_cells_to_check;--a)
    {
        if(a<0)
        {   break;}
        distance+=sqrt(pow((obj_elements_vec_ptr->at(a)->avg_red-new_element->avg_red),2)+pow((obj_elements_vec_ptr->at(a)->avg_green-new_element->avg_green),2)+pow((obj_elements_vec_ptr->at(a)->avg_blue-new_element->avg_blue),2));
    }
    //float distance=sqrt((origin_element->avg_red-new_element->avg_red)*(origin_element->avg_red-new_element->avg_red)+(origin_element->avg_green-new_element->avg_green)*(origin_element->avg_green-new_element->avg_green)+(origin_element->avg_blue-new_element->avg_blue)*(origin_element->avg_blue-new_element->avg_blue));
    return distance/no_of_cells_to_check;
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
    float upper_limit=938400,lower_limit=100000;//100000
    if(orig_image_temp->rows*orig_image_temp->cols>=upper_limit)
    {
        slice_row_size=5;//5
        slice_col_size=5;//5
    }
    else if(orig_image_temp->rows*orig_image_temp->cols<=lower_limit)
    {
        slice_row_size=1;
        slice_col_size=1;
    }
    else
    {
        float difference=upper_limit-lower_limit;
        float percentage=((orig_image_temp->rows*orig_image_temp->cols)/(difference))*2.9;
        slice_col_size=percentage+2;
        slice_row_size=percentage+2;
    }
    //cout<<"\nrow="<<slice_row_size<<" col="<<slice_col_size;
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
            obj_elements_vec_ptr=&obj_elements_vec;//for the color distance calculator
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
}

void image_package_class::plot_obj_maps(vector<vector<image_map_element*>>* obj_vec_for_one_img,vector<vector<image_map_element*>>* image_map1,string orig_img_path)//for testing create_color_maps() function//ok tested
{
    cout<<"\nno_of_objects detected= "<<obj_vec_for_one_img->size();
    Mat *orig_image_temp1=new Mat();
    *orig_image_temp1=imread(orig_img_path);
    Mat obj_map_plotted;
    obj_map_plotted.create(orig_image_temp1->size(),orig_image_temp1->type());
    obj_map_plotted = Scalar::all(0);
    srand(time(NULL));//"initialize random seed:" as reported in the docs
    slice_col_size=(orig_image_temp1->rows*orig_image_temp1->cols)/(image_map1->size()*image_map1->at(0).size());
    slice_col_size=sqrt(slice_col_size);
    slice_row_size=slice_col_size;
    no_of_slices_row_wise=(orig_image_temp1->rows)/slice_row_size;
    no_of_slices_col_wise=(orig_image_temp1->cols)/slice_col_size;
    
    int total_no_of_elements_calc=0;
    for(int a=0;a<obj_vec_for_one_img->size();a++)
    {
        int rand_red=rand()%255+0,rand_green=rand()%255+0,rand_blue=rand()%255+0;
        //if(obj_vec_for_one_img->at(a).size()>=min_size_of_obj)//looks like this checker is not required
        //{
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
        //}
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

void image_package_class::find_obj_border_elements(vector<image_map_element*> *obj,vector<image_map_element*> *border_element_vec)//need testing 
{
    int new_col_index,new_row_index;
    int delta_co_ordinates[8][2]={{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    //detect the first bordel cell by brut force
    image_map_element *border_element;
    int row_index_zero=obj->at(0)->row_index,column_index_zero=obj->at(0)->col_index,obj_id=obj->at(0)->obj_id;
    int current_row=row_index_zero,current_col=column_index_zero;
    while(current_row<image_map.size())
    {
        if(image_map.at(current_row).at(current_col)->obj_id==obj_id)
        {   border_element=image_map.at(current_row).at(current_col);}
        current_row--;
    }
    border_element_vec->push_back(border_element);
    /* 
    image_map_element *border_element;
    border_element_vec->clear();
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
                {   border_element_found=true;}
            }
        }
        if(border_element_found==true)
        {   border_element=obj->at(a);break;}
    }
    border_element_vec->push_back(border_element);
    */
    //detect the  rest of the border elements
    int a=0;bool var=false;
    point1:
    bool new_element_added=true;
    while(new_element_added==true)
    {
        int index_of_first_element_added=-1;
        new_element_added=false;
        for(a;a<border_element_vec->size();a++)
        {
            for(int b=0;b<8;b++)
            {
                new_col_index=border_element_vec->at(a)->col_index+delta_co_ordinates[b][1];
                new_row_index=border_element_vec->at(a)->row_index+delta_co_ordinates[b][0];
                if(new_col_index>=0 && new_row_index>=0 && 
                new_row_index<image_map.size() && new_col_index<image_map.at(new_row_index).size() && 
                check_if_element_is_border_element(image_map.at(new_row_index).at(new_col_index))==true &&
                image_map.at(new_row_index).at(new_col_index)->obj_id==border_element_vec->at(a)->obj_id)
                {
                    bool found=false;
                    for(int c=0;c<border_element_vec->size();c++)
                    {
                        if(border_element_vec->at(c)->row_index==image_map.at(new_row_index).at(new_col_index)->row_index &&
                        border_element_vec->at(c)->col_index==image_map.at(new_row_index).at(new_col_index)->col_index)
                        {   found=true;break;}
                    }
                    if(found==false)
                    {   
                        border_element_vec->push_back(image_map.at(new_row_index).at(new_col_index));new_element_added=true;
                        if(index_of_first_element_added!=-1)
                        {   index_of_first_element_added=a;}
                    }
                }
            }
        }
        if(new_element_added==true)
        {   a=index_of_first_element_added;}
    }
}

void image_package_class::find_neighbouring_objs(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_all_objs,vector<vector<image_map_element*>> *results,vector<image_map_element*> *border_element_vec)//ok tested
{
    int new_col_index,new_row_index;
    int delta_co_ordinates[8][2]={{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    //find_obj_border_elements(obj,border_element_vec);
    /*could have used the above function rather than writing 
      the whole code again, but due to compiler bug the above 
      function is not working when called from here. */
    //detect the first bordel cell by brut force
    /* 
    //this below algorithm is the correct version of the further below algorithm but results of the one 
    image_map_element *border_element;
    int row_index_zero=obj->at(0)->row_index,column_index_zero=obj->at(0)->col_index,obj_id=obj->at(0)->obj_id;
    int current_row=row_index_zero,current_col=column_index_zero;
    while(current_row<image_map.size())
    {
        if(image_map.at(current_row).at(current_col)->obj_id==obj_id)
        {   border_element=image_map.at(current_row).at(current_col);}
        current_row--;
    }
    border_element_vec->push_back(border_element);
    */
    image_map_element *border_element;
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
                {   border_element_found=true;}
            }
        }
        if(border_element_found==true)
        {   border_element=obj->at(a);break;}
    }
    border_element_vec->push_back(border_element);
    
    //detect the  rest of the border elements
    int a=0;
    point1:
    int index_of_first_element_added=-1;
    bool new_element_added=false;
    for(a;a<border_element_vec->size();a++)
    {
        for(int b=0;b<8;b++)
        {
            new_col_index=border_element_vec->at(a)->col_index+delta_co_ordinates[b][1];
            new_row_index=border_element_vec->at(a)->row_index+delta_co_ordinates[b][0];
            if(new_col_index>=0 && new_row_index>=0 && 
               new_row_index<image_map.size() && new_col_index<image_map.at(new_row_index).size() && 
               check_if_element_is_border_element(image_map.at(new_row_index).at(new_col_index))==true &&
               image_map.at(new_row_index).at(new_col_index)->obj_id==border_element_vec->at(a)->obj_id)
            {
                bool found=false;
                for(int c=0;c<border_element_vec->size();c++)
                {
                    if(border_element_vec->at(c)->row_index==image_map.at(new_row_index).at(new_col_index)->row_index &&
                       border_element_vec->at(c)->col_index==image_map.at(new_row_index).at(new_col_index)->col_index)
                    {   found=true;break;}
                }
                if(found==false)
                {   
                    border_element_vec->push_back(image_map.at(new_row_index).at(new_col_index));new_element_added=true;
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
    
    //detection of the neighbouring objects
    for(int a=0;a<border_element_vec->size();a++)
    {
        for(int b=0;b<8;b++)
        {
            new_col_index=border_element_vec->at(a)->col_index+delta_co_ordinates[b][1];
            new_row_index=border_element_vec->at(a)->row_index+delta_co_ordinates[b][0];
            if(new_col_index>=0 && new_row_index>=0 &&
               new_row_index<image_map.size() && new_col_index<image_map.at(new_row_index).size() &&
               border_element_vec->at(a)->obj_id!=image_map.at(new_row_index).at(new_col_index)->obj_id
               )
            {
                bool found=false;
                for(int c=0;c<results->size();c++)
                {
                    if(image_map.at(new_row_index).at(new_col_index)->obj_id==results->at(c).at(0)->obj_id)
                    {   found=true;break;}
                }
                if(found==false)
                {
                    for(int c=0;c<list_of_all_objs->size();c++)
                    {
                        if(list_of_all_objs->at(c).at(0)->obj_id==image_map.at(new_row_index).at(new_col_index)->obj_id)
                        {   results->push_back(list_of_all_objs->at(c));break;}
                    }
                }
            }
        }
    }
}

int image_package_class::find_neighbouring_obj_avg_color_of_closest_area(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_neighbouring_objs,vector<image_map_element*> *border_element_vec)//ok tested
{
    int delta_co_ordinates[8][2]={{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    struct data_structure
    {
        int object_id;
        int obj_element_row_index,obj_element_col_index;
        image_map_element* neighbouring_obj_element;
        vector<image_map_element*> current_obj_common_border_element;
    };
    vector<vector<float>> id_and_closeness_vec;
    vector<data_structure> data_structure_vec;
    vector<image_map_element*> neighbour_obj_elements;//for temporary storage 
    vector<image_map_element*> border_elements;//for temporary storage
    for(int a=0;a<list_of_neighbouring_objs->size();a++)
    {  
        border_elements.clear();
        neighbour_obj_elements.clear();
        border_elements.clear();
        data_structure_vec.clear();
        //collect all the possible data
        for(int b=0;b<border_element_vec->size();b++)
        {
            for(int c=0;c<8;c++)
            {
                int new_col_index=border_element_vec->at(b)->col_index+delta_co_ordinates[c][1];
                int new_row_index=border_element_vec->at(b)->row_index+delta_co_ordinates[c][0];
                if(new_col_index>=0 && new_row_index>=0 &&
                   new_row_index<image_map.size() && new_col_index<image_map.at(new_row_index).size() &&
                   image_map.at(new_row_index).at(new_col_index)->obj_id==list_of_neighbouring_objs->at(a).at(0)->obj_id
                )
                {
                    border_elements.push_back(border_element_vec->at(b));
                    neighbour_obj_elements.push_back(image_map.at(new_row_index).at(new_col_index));
                }
            }
        }
        //standardize the data
        for(int b=0;b<neighbour_obj_elements.size();b++)
        {
            bool found=false;
            for(int c=0;c<data_structure_vec.size();c++)
            {
                if(data_structure_vec.at(c).object_id==neighbour_obj_elements.at(b)->obj_id)
                {
                    data_structure_vec.at(c).current_obj_common_border_element.push_back(border_elements.at(b));
                    found=true;
                }
            }
            if(found==false)
            {
                data_structure data_structure_obj;
                data_structure_obj.object_id=neighbour_obj_elements.at(b)->obj_id;
                data_structure_obj.obj_element_col_index=neighbour_obj_elements.at(b)->col_index;
                data_structure_obj.obj_element_row_index=neighbour_obj_elements.at(b)->row_index;
                data_structure_obj.neighbouring_obj_element=neighbour_obj_elements.at(b);
                data_structure_obj.current_obj_common_border_element.push_back(border_elements.at(b));
                data_structure_vec.push_back(data_structure_obj);
            }
        }
        //distance_calculation section
        vector<float> id_and_closeness;//id,closeness
        id_and_closeness.push_back(data_structure_vec.at(0).object_id);
        float closeness=0;
        int total=0;
        for(int b=0;b<data_structure_vec.size();b++)
        {
            for(int c=0;c<data_structure_vec.at(b).current_obj_common_border_element.size();c++)
            {
                float red=image_map.at(data_structure_vec.at(b).obj_element_row_index).at(data_structure_vec.at(b).obj_element_col_index)->avg_red-data_structure_vec.at(b).current_obj_common_border_element.at(c)->avg_red;
                float green=image_map.at(data_structure_vec.at(b).obj_element_row_index).at(data_structure_vec.at(b).obj_element_col_index)->avg_green-data_structure_vec.at(b).current_obj_common_border_element.at(c)->avg_green;
                float blue=image_map.at(data_structure_vec.at(b).obj_element_row_index).at(data_structure_vec.at(b).obj_element_col_index)->avg_blue-data_structure_vec.at(b).current_obj_common_border_element.at(c)->avg_blue;
                closeness+=sqrt(pow((red),2)+pow((green),2)+pow((blue),2));
                total++;
            }
        }
        id_and_closeness.push_back((closeness)/((float)(total)));
        id_and_closeness_vec.push_back(id_and_closeness);
    }
    struct sortingclass2 
    {
        bool operator() (vector<float> vec1,vector<float> vec2) 
        { return (vec1.at(1)<vec2.at(1));}
    }sorting_helper2;
    sort(id_and_closeness_vec.begin(),id_and_closeness_vec.end(),sorting_helper2);
    /*cout<<"\nresults1=\n";
    for(int a=0;a<id_and_closeness_vec.size();a++)
    {
        cout<<"\nelement_id="<<id_and_closeness_vec.at(a).at(0)<<" closeness="<<id_and_closeness_vec.at(a).at(1);
    }*/
    return id_and_closeness_vec.at(0).at(0);
}

void image_package_class::similar_obj_combination_process_strict()//ok tested
{
    vector<vector<image_map_element*>> obj_vec_temp;
    obj_vec_temp=obj_vec;
    sort(obj_vec_temp.begin(),obj_vec_temp.end(),sorting_helper1);
    vector<vector<image_map_element*>> neighbours_found;
    vector<image_map_element*> border_element_vec;
    for(int a=0;a<obj_vec_temp.size();a++)
    {
        if(obj_vec_temp.at(a).size()<min_size_of_obj)
        {
            neighbours_found.clear();
            border_element_vec.clear();
            //find the neighbouring obj and the border elements of te current obj
            find_neighbouring_objs(&obj_vec_temp.at(a),&obj_vec_temp,&neighbours_found,&border_element_vec);
            //find the id og the obj which is the cloesest in terms of color to the current obj
            int id_for_obj_to_combine_with=find_neighbouring_obj_avg_color_of_closest_area(&obj_vec_temp.at(a),&neighbours_found,&border_element_vec);
            //add the obj to its required neighbour
            int index_of_the_obj_to_combine_with;
            for(int b=0;b<obj_vec_temp.size();b++)
            {
                if(obj_vec_temp.at(b).at(0)->obj_id==id_for_obj_to_combine_with)
                {   index_of_the_obj_to_combine_with=b;break;}
            }
            for(int b=0;b<obj_vec_temp.at(a).size();b++)
            {
                obj_vec_temp.at(a).at(b)->obj_id=id_for_obj_to_combine_with;
                obj_vec_temp.at(index_of_the_obj_to_combine_with).push_back(obj_vec_temp.at(a).at(b));
            }
            //remove the obj
            obj_vec_temp.erase(obj_vec_temp.begin()+a);
            //handle the loop as size change is going on 
            a--;
        }
    }
    obj_vec=obj_vec_temp;
}

void image_package_class::obj_info_gatherer(int a,vector<obj_info> *obj_info_vec,vector<vector<image_map_element*>> *list_of_neighbouring_objs)//ok tested
{
    //find its neighbours
    list_of_neighbouring_objs->clear();
    vector<image_map_element*> border_element_vec;
    border_element_vec.clear();
    find_neighbouring_objs(&obj_vec.at(a),&obj_vec,list_of_neighbouring_objs,&border_element_vec);
    //calc the color distance
    for(int b=0;b<list_of_neighbouring_objs->size();b++)
    {
        for(int c=0;c<obj_info_vec->size();c++)
        {
            if(obj_info_vec->at(c).obj_id==list_of_neighbouring_objs->at(b).at(0)->obj_id &&
                obj_info_vec->at(c).select_status==false)
            {
                int n_red=obj_info_vec->at(c).avg_red;
                int n_green=obj_info_vec->at(c).avg_green;
                int n_blue=obj_info_vec->at(c).avg_blue;
                float color_distance_temp=(sqrt(pow((n_red-obj_info_vec->at(a).avg_red),2)+pow((n_green-obj_info_vec->at(a).avg_green),2)+pow((n_blue-obj_info_vec->at(a).avg_blue),2)));
                if(color_distance_temp<=color_sensitivity2)
                {   obj_info_vec->at(a).color_distance_of_each_obj.push_back(color_distance_temp);}
                else
                {
                    list_of_neighbouring_objs->erase(list_of_neighbouring_objs->begin()+b);
                    b--;
                }
                break;
            }
            else if(obj_info_vec->at(c).obj_id==list_of_neighbouring_objs->at(b).at(0)->obj_id &&
                    obj_info_vec->at(c).select_status==true)
            {
                list_of_neighbouring_objs->erase(list_of_neighbouring_objs->begin()+b);
                b--;
                break;
            }
        }
    }
}

void image_package_class::similar_obj_combinarion_process_un_strict()//ok tested
{
    //data gathering and storage
    vector<obj_info> obj_info_vec;
    for(int a=0;a<obj_vec.size();a++)
    {
        obj_info obj1;
        //calc avg rgb values for each obj
        obj1.obj_id=obj_vec.at(a).at(0)->obj_id;
        int red=0,green=0,blue=0;
        int b=0;
        for(b=0;b<obj_vec.at(a).size();b++)
        {
            red+=obj_vec.at(a).at(b)->avg_red;
            green+=obj_vec.at(a).at(b)->avg_green;
            blue+=obj_vec.at(a).at(b)->avg_blue;
        }
        obj1.avg_green=green/b;
        obj1.avg_red=red/b;
        obj1.avg_blue=blue/b;
        obj_info_vec.push_back(obj1);
    }
   
    //combination process based on the smallest distance and color sensitivity value
    //algorithm is similar to the color mapper
    vector<vector<image_map_element*>> new_obj_vec;//this  is the new obj vec, which will replace the old one
    new_obj_vec.clear();
    int obj_index=0;
    for(int a=0;a<obj_info_vec.size();a++)
    {
        if(obj_info_vec.at(a).select_status==false)
        {
            vector<vector<image_map_element*>> list_of_neighbouring_objs;//for holding the current neighbouring obj list
            vector<vector<image_map_element*>> list_of_neighbouring_objs_buffer;//for holding the extra neighboring obj list
            list_of_neighbouring_objs_buffer.clear();
            vector<vector<image_map_element*>> list_of_obj_to_be_combined;//for holding the list of objects to be combined into one single object
            list_of_obj_to_be_combined.clear();
            //for the first obj
            obj_info_vec.at(a).select_status=true;           
            obj_info_vec.at(a).obj_id=obj_index;
            for(int b=0;b<obj_vec.at(a).size();b++)
            {   obj_vec.at(a).at(b)->obj_id=obj_index;}
            list_of_obj_to_be_combined.push_back(obj_vec.at(a));
            //for the rest of the objs
            obj_info_gatherer(a,&obj_info_vec,&list_of_neighbouring_objs);
            point1:
            if(list_of_neighbouring_objs_buffer.size()!=0)
            {
                int index1;
                for(int b=0;b<obj_info_vec.size();b++)
                {
                    if(obj_info_vec.at(b).obj_id==list_of_neighbouring_objs_buffer.at(0).at(0)->obj_id && obj_info_vec.at(b).select_status==false)
                    {
                        index1=b;
                        break;
                    }
                }
                obj_info_vec.at(index1).select_status=true;
                obj_info_vec.at(index1).obj_id=obj_index;
                for(int b=0;b<obj_vec.at(index1).size();b++)
                {   obj_vec.at(index1).at(b)->obj_id=obj_index;}
                list_of_obj_to_be_combined.push_back(obj_vec.at(index1));
                list_of_neighbouring_objs.clear();
                obj_info_gatherer(index1,&obj_info_vec,&list_of_neighbouring_objs);
            }
            while(list_of_neighbouring_objs.size()>0)
            {
                int index_of_new_obj;
                for(int b=0;b<obj_info_vec.size();b++)//for checking if the first neighbouring obj is selected or not. 
                {
                    if(obj_info_vec.at(b).obj_id==list_of_neighbouring_objs.at(0).at(0)->obj_id && obj_info_vec.at(b).select_status==false)
                    {
                        obj_info_vec.at(b).obj_id=obj_index;
                        obj_info_vec.at(b).select_status=true;
                        index_of_new_obj=b;
                        break;
                    }
                }
                for(int b=0;b<list_of_neighbouring_objs.at(0).size();b++)
                {   list_of_neighbouring_objs.at(0).at(b)->obj_id=obj_index;}
                list_of_obj_to_be_combined.push_back(list_of_neighbouring_objs.at(0));
                //before clearing, other list_of_neighbouring_objs need to be handled
                if(list_of_neighbouring_objs.size()>1)
                {
                    int b=list_of_neighbouring_objs.size()-1;
                    for(b;b>=1;b--)//2
                    {   list_of_neighbouring_objs_buffer.push_back(list_of_neighbouring_objs.at(b));}
                }
                list_of_neighbouring_objs.clear(); 
                obj_info_gatherer(index_of_new_obj,&obj_info_vec,&list_of_neighbouring_objs);
            }
            //remove already included objs
            for(int b=0;b<list_of_neighbouring_objs_buffer.size();b++)
            {
                for(int c=0;c<obj_info_vec.size();c++)
                {
                    if(obj_info_vec.at(c).obj_id==list_of_neighbouring_objs_buffer.at(b).at(0)->obj_id && obj_info_vec.at(c).select_status==true)
                    {
                        list_of_neighbouring_objs_buffer.erase(list_of_neighbouring_objs_buffer.begin()+b);
                        b--;
                        break;
                    }
                }
            }
            if(list_of_neighbouring_objs_buffer.size()!=0)
            {   goto point1;}
            obj_index++;
            //actual combination part
            vector<image_map_element*> new_obj;
            for(int b=0;b<list_of_obj_to_be_combined.size();b++)
            {
                for(int c=0;c<list_of_obj_to_be_combined.at(b).size();c++)
                {   new_obj.push_back(list_of_obj_to_be_combined.at(b).at(c));}
            }
            new_obj_vec.push_back(new_obj);
        }
    }
    obj_vec=new_obj_vec;
}

void image_package_class::contour_data_plotter()//ok tested
{
    vector<contour_map_element> contour_map_element_vec(orig_image_temp->cols);
    contour_data_map.clear();
    for(int b=0;b<orig_image_temp->rows;b++)
    {   contour_data_map.push_back(contour_map_element_vec);}
    for(int a=0;a<orig_img_contours.size();a++)
    {
        //plotting of each contours 
        Mat contour_mat(orig_image_temp->rows,orig_image_temp->cols,CV_8UC3,Scalar(0,0,0));
        drawContours(contour_mat,orig_img_contours,a,Scalar(255,255,255),1);
        //finding the difference after and before plotting of the contours
        int max_row=0,max_col=0,min_row=orig_img_contours.at(a).at(0).y,min_col=orig_img_contours.at(a).at(0).x;
        for(int b=0;b<orig_img_contours.at(a).size();b++)
        {
            if(max_row<orig_img_contours.at(a).at(b).y)
            {   max_row=orig_img_contours.at(a).at(b).y;}
            if(max_col<orig_img_contours.at(a).at(b).x)
            {   max_col=orig_img_contours.at(a).at(b).x;}
            if(min_row>orig_img_contours.at(a).at(b).y)
            {   min_row=orig_img_contours.at(a).at(b).y;}
            if(min_col>orig_img_contours.at(a).at(b).x)
            {   min_col=orig_img_contours.at(a).at(b).x;}
        }
        Mat ROI(contour_mat,Rect(min_col,min_row,(max_col-min_col+1),(max_row-min_row+1)));
        for(int b=0;b<ROI.rows;b++)
        {
            for(int c=0;c<ROI.cols;c++)
            {
                if(((int)ROI.at<Vec3b>(b,c)[0])==255 /*&& ((int)ROI.at<Vec3b>(b,c)[1])==255 && ((int)ROI.at<Vec3b>(b,c)[2])==255*/)
                {
                    contour_data_map.at(b+min_row).at(c+min_col).contour_status=true;
                    contour_data_map.at(b+min_row).at(c+min_col).contour_id=a+1;
                }
            }
        }
        ROI.release();
        contour_mat.release();
    }
    total_no_of_contours=orig_img_contours.size();
    orig_img_contours.clear();
    orig_img_heirachy.clear();
    /*
    Mat test(orig_image_temp->rows,orig_image_temp->cols,CV_8UC3,Scalar(0,0,0));
    cout<<"\nrow="<<contour_data_map->size()<<" col="<<contour_data_map->at(0).size()<<" n_row="<<test.rows<<" n_col="<<test.cols;
    int ghj;cin>>ghj;
    for(int a=0;a<contour_data_map->size();a++)
    {
        for(int b=0;b<contour_data_map->at(a).size();b++)
        {
            if(contour_data_map->at(a).at(b).contour_status==true)
            {
                test.at<Vec3b>(a,b)[0]=255;
                test.at<Vec3b>(a,b)[1]=255;
                test.at<Vec3b>(a,b)[2]=255;
            }
        }
    }
    imshow("test",test);
    waitKey(0);
    int gh;cin>>gh;
    test.release();*/
}

void image_package_class::check_contour_status_under_an_element(vector<vector<int>>* contour_statistics,int obj_index,int row_index,int col_index,bool *partially_conflict_status)//ok tested
{
    int orig_row_index=image_map.at(row_index).at(col_index)->row_index*slice_row_size;
    int orig_col_index=image_map.at(row_index).at(col_index)->col_index*slice_col_size;
    int no_of_contour_elements_present=0;
    int srs=slice_row_size,rs=contour_data_map.size(),cs=contour_data_map[0].size();
    int previous_contour_id=0;
    for(int a=0;a<slice_row_size;a++)
    {
        for(int b=0;b<slice_col_size;b++)
        {
            if(contour_data_map.at(a+orig_row_index).at(b+orig_col_index).contour_status==true)
            {
                image_map.at(row_index).at(col_index)->id_of_contours_present_under_the_slice.push_back(contour_data_map.at(a+orig_row_index).at(b+orig_col_index).contour_id);
                vector<int> co_ordinates(2);
                co_ordinates.clear();
                co_ordinates.push_back(orig_row_index+a);
                co_ordinates.push_back(orig_col_index+b);
                image_map.at(row_index).at(col_index)->co_ordinates_of_contour_elements.push_back(co_ordinates);
                if(previous_contour_id==0 || previous_contour_id!=contour_data_map.at(a+orig_row_index).at(b+orig_col_index).contour_id)
                {   
                    no_of_contour_elements_present++;
                    previous_contour_id=contour_data_map.at(a+orig_row_index).at(b+orig_col_index).contour_id;
                }
                bool found=false;
                for(int c=0;c<contour_statistics->size();c++)
                {
                    if(contour_data_map.at(a+orig_row_index).at(b+orig_col_index).contour_id==contour_statistics->at(c).at(0))
                    {   contour_statistics->at(c).at(1)++;found=true;}
                }
                if(found==false)
                {
                    vector<int> contour_info(2);
                    contour_info.at(0)=contour_data_map.at(a+orig_row_index).at(b+orig_col_index).contour_id;
                    contour_info.at(1)=1;
                    contour_statistics->push_back(contour_info);
                }
            }
        }
    }
    if(no_of_contour_elements_present==1)
    {   (*partially_conflict_status)=true;}//this means partially conflict
    else
    {   (*partially_conflict_status)=false;}//this means fully conflict 
}

void image_package_class::border_differentiator()//ok tested
{
    //non conflict border filter
    vector<image_map_element*> non_conflict_border_elements;
    vector<image_map_element*> conflict_border_elements;
    vector<vector<int>> contour_info_vec;//holds contour id data and the no of times the elements the contour shares with the border 
    for(int a=0;a<obj_vec.size();a++)
    {
        non_conflict_border_elements.clear();
        conflict_border_elements.clear();
        contour_info_vec.clear();
        for(int b=0;b<border_element_vec.at(a).size();b++)
        {
            bool partially_conflict_status;
            check_contour_status_under_an_element(&contour_info_vec,a,border_element_vec.at(a).at(b)->row_index,border_element_vec.at(a).at(b)->col_index,&partially_conflict_status);
            if(partially_conflict_status==true)
            {   non_conflict_border_elements.push_back(border_element_vec.at(a).at(b));}
            else
            {   conflict_border_elements.push_back(border_element_vec.at(a).at(b));}               
        }
        non_conflict_border_elements_vec.push_back(non_conflict_border_elements);
        conflict_border_elements_vec.push_back(conflict_border_elements);
        contour_info_vec_vec.push_back(contour_info_vec);
        /*
        cout<<"\non_conflict_border_elements:-"<<non_conflict_border_elements.size();
        cout<<"\nconflict_border_elements:-"<<conflict_border_elements.size();
        cout<<"\ncontour_info_vec:-"<<contour_info_vec.size();
        cout<<"\nborder_element_size="<<border_element_vec.at(a).size();
        int ghj;cin>>ghj;
        for(int b=0;b<contour_info_vec.size();b++)
        {
            cout<<"\ncontour_id="<<contour_info_vec.at(b).at(0)<<" no="<<contour_info_vec.at(b).at(1);
        }
        int gh;cin>>gh;
        */
        //under construction

    }
}

void image_package_class::border_plotter(vector<vector<image_map_element*>>* border)//this function is for testing purpose only.
{
    Mat test(orig_image_temp->rows,orig_image_temp->cols,CV_8UC3,Scalar(0,0,0));
    vector<int> r_g_b(3);
    vector<vector<int>> rgb_vec;
    for(int a=0;a<total_no_of_contours;a++)
    {
        int rand_red=rand()%255+0,rand_green=rand()%255+0,rand_blue=rand()%255+0;    
        r_g_b[0]=rand_red;
        r_g_b[1]=rand_green;
        r_g_b[2]=rand_blue;
        rgb_vec.push_back(r_g_b);
    }
    for(int a=0;a<border_element_vec.size();a++)
    {
        
        //cout<<"\nobj="<<border_element_vec.size()<<" non_con="<<border->size();
        //int ghj;cin>>ghj;
        //Mat test(orig_image_temp->rows,orig_image_temp->cols,CV_8UC3,Scalar(0,0,0));
        for(int b=0;b<border->at(a).size();b++)
        {
            int orig_row_index=border->at(a).at(b)->row_index*slice_row_size,orig_col_index=border->at(a).at(b)->col_index*slice_col_size;
            bool stop=false;
            int id;
            for(int c=0;c<slice_row_size;c++)
            {
                for(int d=0;d<slice_col_size;d++)
                {
                    if(contour_data_map.at(c+orig_row_index).at(d+orig_col_index).contour_status==true)
                    {
                        id=contour_data_map.at(c+orig_row_index).at(d+orig_col_index).contour_id;
                        stop=true;
                        break;
                    }
                }
                if(stop==true)
                {   break;}
            }

            int red2=rgb_vec[id-1][0],green2=rgb_vec[id-1][1],blue2=rgb_vec[id-1][2];
            for(int c=0;c<slice_row_size;c++)
            {
                for(int d=0;d<slice_col_size;d++)
                {
                    test.at<Vec3b>((c+orig_row_index),(d+orig_col_index))[0]=blue2;
                    test.at<Vec3b>((c+orig_row_index),(d+orig_col_index))[1]=green2;
                    test.at<Vec3b>((c+orig_row_index),(d+orig_col_index))[2]=red2;
                }
            }
        }
    
    }
    imshow("test",test);
    waitKey(0);
    //int gh;cin>>gh;
    //test.release();   
}

void image_package_class::prominient_border_finder()//ok tested
{
    vector<vector<int>> contour_counter_for_obj;
    for(int a=0;a<obj_vec.size();a++)//for each obj
    {
        contour_counter_for_obj.clear();
        //contour statistics collection for each obj
        for(int b=0;b<non_conflict_border_elements_vec.at(a).size();b++)//for each slice
        {
            for(int c=0;c<non_conflict_border_elements_vec.at(a).at(b)->id_of_contours_present_under_the_slice.size();c++)//for id vectors present under the slice
            {
                bool found=false;
                for(int d=0;d<contour_counter_for_obj.size();d++)// for contour_counter_for_obj 
                {
                    if(non_conflict_border_elements_vec.at(a).at(b)->id_of_contours_present_under_the_slice.at(c)==contour_counter_for_obj.at(d).at(0))
                    {
                        found=true;
                        contour_counter_for_obj.at(d).at(1)++;
                        break;
                    }
                }
                if(found==false)
                {
                    vector<int> contour_counter_element(2);
                    contour_counter_element.at(0)=non_conflict_border_elements_vec.at(a).at(b)->id_of_contours_present_under_the_slice.at(c);
                    contour_counter_element.at(1)=1;
                    contour_counter_for_obj.push_back(contour_counter_element);
                }
            }
        }

        for(int b=0;b<conflict_border_elements_vec.at(a).size();b++)
        {
            for(int c=0;c<conflict_border_elements_vec.at(a).at(b)->id_of_contours_present_under_the_slice.size();c++)//for id vectors present under the slice
            {
                bool found=false;
                for(int d=0;d<contour_counter_for_obj.size();d++)// for contour_counter_for_obj 
                {
                    if(conflict_border_elements_vec.at(a).at(b)->id_of_contours_present_under_the_slice.at(c)==contour_counter_for_obj.at(d).at(0))
                    {
                        found=true;
                        contour_counter_for_obj.at(d).at(1)++;
                        break;
                    }
                }
                if(found==false)
                {
                    vector<int> contour_counter_element(2);
                    contour_counter_element.at(0)=conflict_border_elements_vec.at(a).at(b)->id_of_contours_present_under_the_slice.at(c);
                    contour_counter_element.at(1)=1;
                    contour_counter_for_obj.push_back(contour_counter_element);
                }
            }
        }
        //prominient contour finder
        for(int b=0;b<contour_counter_for_obj.size();b++)
        {
            for(int c=0;c<contour_info_vec_vec.at(a).size();c++)
            {
                if(contour_counter_for_obj.at(b).at(0)==contour_info_vec_vec.at(a).at(c).at(0))
                {
                    bool found=false;
                    for(int d=0;d<prominient_contour_id.size();d++)
                    {
                        if(prominient_contour_id.at(d)==contour_counter_for_obj.at(b).at(0))
                        {   found=true;break;}
                    }
                    if(found==false && (((float)contour_counter_for_obj.at(b).at(1)))/((float)contour_info_vec_vec.at(a).at(c).at(1))>0.50)
                    {   prominient_contour_id.push_back(contour_counter_for_obj.at(b).at(0));}
                    break;
                }
            }
        }
    }
    //shifting of conflicting border elements to non conflicting border elements
    for(int a=0;a<obj_vec.size();a++)
    {
        //cout<<"\nc_data_before="<<conflict_border_elements_vec.at(a).size()<<" nc_data_before="<<non_conflict_border_elements_vec.at(a).size();;
        for(int b=conflict_border_elements_vec.at(a).size()-1;b>=0;b--)
        {
            for(int c=0;c<conflict_border_elements_vec.at(a).at(b)->id_of_contours_present_under_the_slice.size();c++)
            {
                bool found=false;
                for(int d=0;d<prominient_contour_id.size();d++)
                {
                    if(conflict_border_elements_vec.at(a).at(b)->id_of_contours_present_under_the_slice.at(c)==prominient_contour_id.at(d))
                    {
                        non_conflict_border_elements_vec.at(a).push_back(conflict_border_elements_vec.at(a).at(b));
                        conflict_border_elements_vec.at(a).erase(conflict_border_elements_vec.at(a).begin()+b);
                        found=true;
                        break;
                    }
                }
                if(found==true)
                {   break;}
            }
        }
        //cout<<"  c_data_after="<<conflict_border_elements_vec.at(a).size()<<" nc_data_after="<<non_conflict_border_elements_vec.at(a).size();
        //int gh;cin>>gh;
    }
}

void image_package_class::border_finder()//under construction
{
    contour_data_plotter();
    border_differentiator();
    //border_plotter(&non_conflict_border_elements_vec);
    prominient_border_finder();
    //border_plotter(&non_conflict_border_elements_vec);
    //int gh;cin>>gh;
}

void image_package_class::data_preparation_for_border_finder()//ok tested
{   
    //border elements collection and buffer area creator
    vector<image_map_element*> border_elements;
    obj_buffer_area.clear();
    border_element_vec.clear();
    for(int a=0;a<obj_vec.size();a++)
    {
        border_elements.clear();
        find_obj_border_elements(&obj_vec.at(a),&border_elements);
        //buffer_area_finder(&border_elements);
        border_element_vec.push_back(border_elements);
    }
    //canny edge detector
    Mat* canny_edge_temp;
    canny_edge_temp=new Mat(orig_image_temp->cols,orig_image_temp->rows,CV_8UC3,Scalar(0,0,0));
    Mat orig_img_gray;
    cvtColor(*orig_image_temp,orig_img_gray,CV_BGR2GRAY);
    blur(orig_img_gray,*canny_edge_temp,Size(kernel_size,kernel_size));
    Canny(*canny_edge_temp,*canny_edge_temp,lowThreshold,lowThreshold*ratio,kernel_size);
    orig_img_contours.clear();
    orig_img_heirachy.clear();
    cv::findContours(*canny_edge_temp,orig_img_contours,orig_img_heirachy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE,Point(0,0));
    orig_img_gray.release();
    canny_edge_temp->release();
    //cout<<"\n\n\n\nobj_vec="<<obj_vec.size()<<" buffer="<<obj_buffer_area.size();int gh;cin>>gh;
}

void image_package_class::start_data_preparation_process()
{
    clean_up_prepared_data_obj();
    for(unsigned int a=0;a<image_paths.size();a++)
    {
        orig_image_temp=new Mat();
        *orig_image_temp=imread(image_paths[a]);
        //data preparation step 1
        create_color_maps();
        similar_obj_combination_process_strict();//no data leakage till here
        similar_obj_combinarion_process_un_strict();//leakage fixed
        //data preparation step 2
        data_preparation_for_border_finder();
        border_finder();
        //data preparation step 3
        border_info_extractor();
        //data preparation step 4
        data_arranger();
        orig_image_temp->release();

        save_current_img_data();
        //cout<<"\ndone!!!!";
        //int gh;cin>>gh;
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

void image_package_class::enter_training_critical_variables(int no_of_sq_areas_need_to_be_checked_for_avg_color1,float color_sensi,float color_sensi2,int min_size_of_obj1)
{
    min_size_of_obj=min_size_of_obj1;
    color_sensitiviy=color_sensi;
    color_sensitivity2=color_sensi2;
    no_of_sq_areas_need_to_be_checked_for_avg_color=no_of_sq_areas_need_to_be_checked_for_avg_color1;
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
    {   plot_obj_maps(&prepared_data_obj.obj_vec_for_each_img.at(a),&prepared_data_obj.image_map_vec.at(a),image_paths.at(a));}
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