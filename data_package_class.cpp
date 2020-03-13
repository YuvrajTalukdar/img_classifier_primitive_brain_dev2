#include"data_package_class.h"

void image_package_class::data_arranger()
{

}

void image_package_class::border_info_extractor()
{

}

int image_package_class::avg_color_in_slice(Mat* slice,char color)//color maper function//ok tested
{
    //Mat hsv;
    //cvtColor(*slice,hsv,COLOR_BGR2HSV);

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
            else if(color=='e'||color=='E')
            {   avg+=(int)slice->at<uchar>(a,b);}

            //else if(color=='h'||color=='H')
            //{   avg+=(int)hsv.at<Vec3b>(a,b)[0];}
        }
    }
    
    //hsv.release();

    avg=avg/(slice->rows*slice->cols);
    return avg;
}

float image_package_class::hue_distance_calc(float h1,float h2)
{
    //direct calc
    float difference1=abs(h1-h2);
    //reverse calc
    float difference2;
    //left calc is same as the values h1 and h2
    //right calc
    float right_calc1=abs(180.0-h1);
    float right_calc2=abs(180.0-h2);
    float h1_new,h2_new;
    if(right_calc1<h1)
    {   h1_new=right_calc1;}
    else
    {   h1_new=h1;}
    if(right_calc2<h2)
    {   h2_new=right_calc2;}
    else
    {   h2_new=h2;}
    difference2=abs(h1_new+h2_new);
    if(difference2<difference1)
    {   difference1=difference2;}
    return difference1;
}

bool image_package_class::color_distance(image_map_element* origin_element,image_map_element* new_element)//color maper function//ok tested
{
    float distance=0,variable_color_sensitiviy_sum=0;
    int no_of_cells_to_check=1;
    if(obj_elements_vec_ptr->size()>=5)//5
    {   no_of_cells_to_check=5;}//5
    else
    {   no_of_cells_to_check=obj_elements_vec_ptr->size();}
    int count1=0;
    for(int a=obj_elements_vec_ptr->size()-1;a>=obj_elements_vec_ptr->size()-no_of_cells_to_check;--a)
    {
        if(a<0)
        {   break;}
        Mat hsv1,hsv2,conversion_mat1=Mat(2,2,CV_8UC3),conversion_mat2=Mat(2,2,CV_8UC3);
        for(int b=0;b<2;b++)
        {
            for(int c=0;c<2;c++)
            {
                conversion_mat1.at<Vec3b>(b,c)[2]=obj_elements_vec_ptr->at(a)->avg_red;
                conversion_mat1.at<Vec3b>(b,c)[1]=obj_elements_vec_ptr->at(a)->avg_green;
                conversion_mat1.at<Vec3b>(b,c)[0]=obj_elements_vec_ptr->at(a)->avg_blue;
                
                conversion_mat2.at<Vec3b>(b,c)[2]=new_element->avg_red;
                conversion_mat2.at<Vec3b>(b,c)[1]=new_element->avg_green;
                conversion_mat2.at<Vec3b>(b,c)[0]=new_element->avg_blue;    
            }
        }
        cvtColor(conversion_mat1,hsv1,CV_BGR2HSV);
        cvtColor(conversion_mat2,hsv2,CV_BGR2HSV);
        conversion_mat1.release();
        conversion_mat2.release();
        //       +
        //distance=sqrt(pow((obj_elements_vec_ptr->at(a)->avg_red-new_element->avg_red),2)+pow((obj_elements_vec_ptr->at(a)->avg_green-new_element->avg_green),2)+pow((obj_elements_vec_ptr->at(a)->avg_blue-new_element->avg_blue),2)+pow((((int)hsv1.at<Vec3b>(0,0)[0])-((int)hsv2.at<Vec3b>(0,0)[0])),2));
        float hue_distance=hue_distance_calc(((int)hsv1.at<Vec3b>(0,0)[0]),((int)hsv2.at<Vec3b>(0,0)[0]));
        //hue_distance=hue_distance*5;
        hue_distance=(hue_distance/180.0)*255.0;
        //float value_distance=abs(((int)hsv1.at<Vec3b>(0,0)[2])-((int)hsv2.at<Vec3b>(0,0)[2]));
        //if(hue_distance<=25)
        //{   hue_distance=0;}
        float red_dis=obj_elements_vec_ptr->at(a)->avg_red-new_element->avg_red,green_dis=obj_elements_vec_ptr->at(a)->avg_green-new_element->avg_green,blue_dis=obj_elements_vec_ptr->at(a)->avg_blue-new_element->avg_blue;
        //rgb dampners based on hue, this is to prevent the total failuure in the apple and orange pic but the performance in other imgs get reduced.
        /*if(hue_distance<3)
        {   
            red_dis=red_dis*(3.0/4.0);
            green_dis=green_dis*(3.0/4.0);
            blue_dis=blue_dis*(3.0/4.0);
        }*/
        //hue value stabilizer (makes hue value to 0 when value is high and satturation is near 0) //for sloving the new hue problem particular in the japanese building picture
        float s_avg1=(int)hsv1.at<Vec3b>(0,0)[1],s_avg2=(int)hsv2.at<Vec3b>(0,0)[1];
        float v_avg1=(int)hsv1.at<Vec3b>(0,0)[2],v_avg2=(int)hsv2.at<Vec3b>(0,0)[2];
        if(s_avg1<20 && s_avg2<20 && v_avg1>230 && v_avg2>230)
        {   hue_distance=0;}
        float distance1=sqrt(pow((red_dis),2)+pow((green_dis),2)+pow((blue_dis),2)+pow((hue_distance),2));
        //float distance1=sqrt(pow((obj_elements_vec_ptr->at(a)->avg_red-new_element->avg_red),2)+pow((obj_elements_vec_ptr->at(a)->avg_green-new_element->avg_green),2)+pow((obj_elements_vec_ptr->at(a)->avg_blue-new_element->avg_blue),2)+pow((hue_distance),2)+pow(value_distance,2));
        distance+=distance1;
        variable_color_sensitiviy_sum+=get_color_sensitivity(obj_elements_vec_ptr->at(a),new_element);
        //distance+=sqrt(pow((obj_elements_vec_ptr->at(a)->avg_red-new_element->avg_red),2)+pow((obj_elements_vec_ptr->at(a)->avg_green-new_element->avg_green),2)+pow((obj_elements_vec_ptr->at(a)->avg_blue-new_element->avg_blue),2));//non patched
        /*if(distance1>color_sensitiviy)
        {   count1++;}
        if(count1>no_of_cells_to_check/2)
        {   break;}*/
        hsv1.release();
        hsv2.release();
        
        //distance+=obj_elements_vec_ptr->at(a)->avg_hue-new_element->avg_hue;
    }
    //float distance=sqrt((origin_element->avg_red-new_element->avg_red)*(origin_element->avg_red-new_element->avg_red)+(origin_element->avg_green-new_element->avg_green)*(origin_element->avg_green-new_element->avg_green)+(origin_element->avg_blue-new_element->avg_blue)*(origin_element->avg_blue-new_element->avg_blue));
    
    //get_color_sensitivity(origin_element,new_element);
    //if((distance/no_of_cells_to_check)>(variable_color_sensitiviy_sum/no_of_cells_to_check))
    if(distance/no_of_cells_to_check>get_color_sensitivity(origin_element,new_element) /*|| count1>no_of_cells_to_check/2*/)
    //if(distance/no_of_cells_to_check>color_sensitiviy) 
    {   return false;}
    else
    {   return true;}
}

float image_package_class::get_color_sensitivity(image_map_element* origin_element,image_map_element* new_element)
{
    //double orig=(int)sobel_plus_edge.at<uchar>(origin_element->row_index,origin_element->col_index);
    //double new_ele=(int)sobel_plus_edge.at<uchar>(new_element->row_index,new_element->col_index);
    //double x=abs(orig-new_ele);
    double x=abs(origin_element->avg_border_all-new_element->avg_border_all);
    //double y=-0.04081632653*x+10.40816327;
    //double y=-0.0408*x+10.408;
    //double y=-0.22*x+10;
    float y=-0.18*x+8;
    //float y=-0.11*x+5; 
    //cout<<"\n\nx="<<x<<" y="<<y;
    if(y>0)
    {   return y;}
    else if(y<=0)
    {   return 0;}
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
            if(image_map.at(new_row_index).at(new_col_index)->select_status==false && color_distance(element,image_map.at(new_row_index).at(new_col_index))==true)//<=color_sensitiviy
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
    no_of_slices_row_wise=(orig_image_temp->rows)/slice_row_size;//loss of pixel in the end position, happens here.
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
            Mat ROI2(sobel_plus_edge,Rect(startx,starty,slice_row_size,slice_col_size));
            element->avg_border_all=avg_color_in_slice(&ROI2,'e');
            //element->avg_hue=avg_color_in_slice(slice,'h');

            slice->release();
            delete slice;
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

/*void image_package_class::onMouse(int evt,int x,int y,int flags,void* param,void* userdata)
{

}*/
void image_package_class::onMouse(int evt,int x,int y,int flags,void* param)
{
    if(evt==CV_EVENT_LBUTTONDOWN)
    {
        vector<Point>* ptPtr= (std::vector<cv::Point>*)param;
        ptPtr->push_back(Point(x,y));
    }
}

void image_package_class::second_stage_analyzer(Mat plot,int slice_size)
{
    vector<Point> points;
    namedWindow("Output Window",WINDOW_NORMAL);
    resizeWindow("Output Window",1920,1080);
    setMouseCallback("Output Window",onMouse, (void*)&points);
    int x,y;
    int r_old=0,g_old=0,b_old=0,h_old=0;
    while(1)
    {
        imshow("Output Window",plot);
        if(points.size()>2)
        {
            for(auto it = points.begin();it!=points.end();it++)
            {
                cout<<"x and y coordinates are given below"<<endl;
                cout<<(*it).x<<'\t'<<(*it).y;
                int x_index=(*it).x/slice_size,y_index=(*it).y/slice_size;
                int obj_index,id=image_map.at(y_index).at(x_index)->obj_id;
                
                for(int a=0;a<obj_vec.size();a++)
                {
                    if(obj_vec.at(a).at(0)->obj_id==id)
                    {
                        obj_index=a;
                        break;
                    }
                }
                int avg_r=0,avg_g=0,avg_b=0;
                int a;
                for(a=0;a<obj_vec.at(obj_index).size();a++)
                {
                    avg_r+=obj_vec.at(obj_index).at(a)->avg_red;
                    avg_g+=obj_vec.at(obj_index).at(a)->avg_green;
                    avg_b+=obj_vec.at(obj_index).at(a)->avg_blue;
                }
                avg_r=avg_r/a;
                avg_g=avg_g/a;
                avg_b=avg_b/a;
                cout<<" red="<<avg_r;
                cout<<" green="<<avg_g;
                cout<<" blue="<<avg_b;
                Mat hsv,conversion_mat=Mat(2,2,CV_8UC3);
                for(int a=0;a<2;a++)
                {
                    for(int b=0;b<2;b++)
                    {
                        conversion_mat.at<Vec3b>(a,b)[2]=avg_r;
                        conversion_mat.at<Vec3b>(a,b)[1]=avg_g;
                        conversion_mat.at<Vec3b>(a,b)[0]=avg_b;
                    }
                }
                cvtColor(conversion_mat,hsv,CV_BGR2HSV);
                conversion_mat.release();
                float avg_h=(int)hsv.at<Vec3b>(0,0)[0];
                //avg_h=(avg_h/180.0)*255.0;//for only the unstrict layer
                hsv.release();
                float h_distance=hue_distance_calc(h_old,avg_h);
                cout<<" hue="<<avg_h<<" h_dis="<<h_distance;
                float distance=sqrt(pow((avg_r-r_old),2)+pow((avg_g-g_old),2)+pow((avg_b-b_old),2)+pow((h_distance),2));
                r_old=avg_r;
                g_old=avg_g;
                b_old=avg_b;
                h_old=avg_h;
                cout<<" distance="<<distance<<endl;
            }
            if(points.size()>15)
            {   points.erase(points.begin(),points.begin()+13);}
        }
        waitKey(10);
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
    //second_stage_analyzer(obj_map_plotted,slice_row_size);//for testing
    imwrite(img_name.c_str(),obj_map_plotted);
    //imshow("obj_map",*orig_image_temp1);
    //waitKey(0);
    orig_image_temp1->release();
    delete orig_image_temp1;
    cout<<"\ncols="<<obj_map_plotted.cols<<" rows="<<obj_map_plotted.rows;
    //int gh;cin>>gh;
    //imshow("obj_map",obj_map_plotted);
    //waitKey(0);
    obj_map_plotted.release();
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

void image_package_class::find_neighbouring_objs(vector<image_map_element*> *obj,vector<vector<image_map_element*>> *list_of_all_objs,vector<vector<image_map_element*>> *results,vector<image_map_element*> *border_element_vec,int neighbouring_obj_search_mode)//ok tested
{
    auto start1 = high_resolution_clock::now(); 
    int new_col_index,new_row_index;
    int delta_co_ordinates[8][2]={{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    //find_obj_border_elements(obj,border_element_vec);
    /*could have used the above function rather than writing 
      the whole code again, but due to compiler bug the above 
      function is not working when called from here. */
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
    auto stop1 = high_resolution_clock::now(); 
    auto duration1 = duration_cast<microseconds>(stop1 - start1); 
    first+=duration1.count();

    //detect the  rest of the border elements
    auto start2 = high_resolution_clock::now(); 
    int a=0;
    point1:
    int index_of_first_element_added=-1;
    bool new_element_added=false;
    for(;a<border_element_vec->size();a++)
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
    auto stop2 = high_resolution_clock::now(); 
    auto duration2 = duration_cast<microseconds>(stop2 - start2); 
    second+=duration2.count();
    //detection of the neighbouring objects
    auto start3 = high_resolution_clock::now(); 
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
                    //linear searcher
                    if(neighbouring_obj_search_mode==2)
                    {
                        for(int c=0;c<list_of_all_objs->size();c++)
                        {
                            if(list_of_all_objs->at(c).at(0)->obj_id==image_map.at(new_row_index).at(new_col_index)->obj_id)
                            {   results->push_back(list_of_all_objs->at(c));break;}
                        }
                    } 
                    //binary_searcher
                    else if(neighbouring_obj_search_mode==1)
                    {
                        int required_index=binary_search_for_strict(0,list_of_all_objs->size()-1,list_of_all_objs,image_map.at(new_row_index).at(new_col_index)->obj_id);
                        if(required_index!=-1)
                        results->push_back(list_of_all_objs->at(required_index));
                    }
                }
            }
        }
    }
    auto stop3 = high_resolution_clock::now();
    auto duration3 = duration_cast<microseconds>(stop3 - start3); 
    third+=duration3.count();
}

int image_package_class::binary_search_for_strict(int start_index,int end_index,vector<vector<image_map_element*>>* obj_vec_temp,int search_id)
{
    int current_index=(end_index-start_index)/2+start_index;
    if(obj_vec_temp->at(current_index).at(0)->obj_id==search_id)
    {   return current_index;}
    else if(obj_vec_temp->at(current_index).at(0)->obj_id>search_id)
    {   binary_search_for_strict(start_index,current_index-1,obj_vec_temp,search_id);}
    else if(obj_vec_temp->at(current_index).at(0)->obj_id<search_id)
    {   binary_search_for_strict(current_index+1,end_index,obj_vec_temp,search_id);}
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
    vector<image_map_element*> neighbour_obj_elements;//for temporary storage. Its for the neighbouring obj border elements sharing the border with current obj.
    vector<image_map_element*> border_elements;//for temporary storage. Its for storing the current obj border elements sharing border with the neighbouring obj.
    for(int a=0;a<list_of_neighbouring_objs->size();a++)
    {  
        border_elements.clear();
        neighbour_obj_elements.clear();
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
    //sort(obj_vec_temp.begin(),obj_vec_temp.end(),sorting_helper1);
    vector<vector<image_map_element*>> neighbours_found;
    vector<image_map_element*> border_element_vec;
    for(int a=0;a<obj_vec_temp.size();a++)
    {
        if(obj_vec_temp.at(a).size()<min_size_of_obj)
        {
            neighbours_found.clear();
            border_element_vec.clear();
            //find the neighbouring obj and the border elements of te current obj
            auto start1 = high_resolution_clock::now(); 
            find_neighbouring_objs(&obj_vec_temp.at(a),&obj_vec_temp,&neighbours_found,&border_element_vec,1);
            auto stop1= high_resolution_clock::now(); 
            auto duration1 = duration_cast<microseconds>(stop1 - start1); 
            find_neighbouring_objs_timer+=duration1.count();
            //find the id og the obj which is the cloesest in terms of color to the current obj
            auto start2 = high_resolution_clock::now(); 
            int id_for_obj_to_combine_with=find_neighbouring_obj_avg_color_of_closest_area(&obj_vec_temp.at(a),&neighbours_found,&border_element_vec);
            auto stop2 = high_resolution_clock::now();
            auto duration2 = duration_cast<microseconds>(stop2 - start2); 
            find_neighbouring_obj_avg_color_of_closest_area_timer+=duration2.count();
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
    find_neighbouring_objs(&obj_vec.at(a),&obj_vec,list_of_neighbouring_objs,&border_element_vec,2);// 2 is neighbouring_obj_search_mode in linear mode
    //calc the color distance
    
    int no_of_objs_to_be_checked=10;//new patch //10
    if(no_of_objs_to_be_checked>list_of_obj_to_be_combined_ptr->size())
    {   no_of_objs_to_be_checked=list_of_obj_to_be_combined_ptr->size();}

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
                bool all_is_well=true;
                //float color_distance_temp=(sqrt(pow((n_red-obj_info_vec->at(a).avg_red),2)+pow((n_green-obj_info_vec->at(a).avg_green),2)+pow((n_blue-obj_info_vec->at(a).avg_blue),2)));
                float n_hue=obj_info_vec->at(c).avg_hue;
                float color_distance_temp=sqrt(pow((n_red-obj_info_vec->at(a).avg_red),2)+pow((n_green-obj_info_vec->at(a).avg_green),2)+pow((n_blue-obj_info_vec->at(a).avg_blue),2)+pow((n_hue-obj_info_vec->at(a).avg_hue),2));
                if(color_distance_temp>color_sensitivity2)
                {   all_is_well=false;}
                //if(abs(n_red-obj_info_vec->at(a).avg_red)<=color_sensitivity2 && abs(n_green-obj_info_vec->at(a).avg_green)<=color_sensitivity2 && abs(n_blue-obj_info_vec->at(a).avg_blue)<=color_sensitivity2)
                //new patch start
                int d;
                for(d=list_of_obj_to_be_combined_ptr->size()-1;d>list_of_obj_to_be_combined_ptr->size()-no_of_objs_to_be_checked;d--)
                {
                    int index1;
                    for(int e=0;e<obj_info_vec->size();e++)
                    {
                        if(obj_info_vec->at(e).obj_id==list_of_obj_to_be_combined_ptr->at(d).at(0)->obj_id)
                        {   index1=e;break;}
                    }
                    n_red=obj_info_vec->at(index1).avg_red;
                    n_green=obj_info_vec->at(index1).avg_green;
                    n_blue=obj_info_vec->at(index1).avg_blue;
                    //                 +
                    //color_distance_temp=sqrt(pow((n_red-obj_info_vec->at(c).avg_red),2)+pow((n_green-obj_info_vec->at(c).avg_green),2)+pow((n_blue-obj_info_vec->at(c).avg_blue),2));
                    n_hue=obj_info_vec->at(index1).avg_hue;
                    color_distance_temp=sqrt(pow((n_red-obj_info_vec->at(c).avg_red),2)+pow((n_green-obj_info_vec->at(c).avg_green),2)+pow((n_blue-obj_info_vec->at(c).avg_blue),2)+pow((n_hue-obj_info_vec->at(c).avg_hue),2));
                    if(color_distance_temp>color_sensitivity2)
                    {   all_is_well=false;break;}
                }
                //color_distance_temp=color_distance_temp/(d+1);
                //new patch end
                //cout<<"\na="<<a<<" b="<<b<<" c="<<c;
                //if(color_distance_temp<=color_sensitivity2)
                if(all_is_well==true)
                {   //cout<<"  combined";
                    obj_info_vec->at(a).color_distance_of_each_obj.push_back(color_distance_temp);}
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
        float hue=0;
        int b=0;
        for(b=0;b<obj_vec.at(a).size();b++)
        {
            red+=obj_vec.at(a).at(b)->avg_red;
            green+=obj_vec.at(a).at(b)->avg_green;
            blue+=obj_vec.at(a).at(b)->avg_blue;
            
            Mat conversion_mat=Mat(2,2,CV_8UC3);
            for(int c=0;c<2;c++)
            {
                for(int d=0;d<2;d++)
                {
                    conversion_mat.at<Vec3b>(c,d)[2]=obj_vec.at(a).at(b)->avg_red;
                    conversion_mat.at<Vec3b>(c,d)[1]=obj_vec.at(a).at(b)->avg_green;
                    conversion_mat.at<Vec3b>(c,d)[0]=obj_vec.at(a).at(b)->avg_blue;
                }
            }
            Mat hsv;
            cvtColor(conversion_mat,hsv,CV_BGR2HSV);
            float hue_c=(int)hsv.at<Vec3b>(0,0)[0];
            hue_c=(hue_c/180.0)*255.0;
            conversion_mat.release();
            hue+=hue_c;
        }
        obj1.avg_green=green/b;
        obj1.avg_red=red/b;
        obj1.avg_blue=blue/b;
        
        obj1.avg_hue=hue/b;
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
            list_of_obj_to_be_combined_ptr=&list_of_obj_to_be_combined;
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

void image_package_class::remove_non_free_elements_MODIFIED_SOBEL(vector<vector<pixel>> &sobel_binary_map,vector<vector<int>>* result)
{
    vector<bool> free_status(result->size());
    for(int a=0;a<result->size();a++)
    {
        if(sobel_binary_map.at(result->at(a).at(0)).at(result->at(a).at(1)).select_status==true)
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

void image_package_class::search_for_neighbour_MODIFIED_SOBEL(pixel &current_pixel,vector<vector<pixel>> &sobel_binary_map,vector<vector<int>> &result)
{
    int new_col_index,new_row_index;
    vector<int> co_ordinates(2);
    int delta_co_ordinates[8][2]={{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    for(int a=0;a<8;a++)
    {
        new_col_index=current_pixel.x+delta_co_ordinates[a][1];
        new_row_index=current_pixel.y+delta_co_ordinates[a][0];
        if(new_col_index>=0 && new_row_index>=0 && 
           new_row_index<sobel_binary_map.size() && 
           new_col_index<sobel_binary_map.at(new_row_index).size() &&
           sobel_binary_map.at(new_row_index).at(new_col_index).select_status==false &&
           sobel_binary_map.at(new_row_index).at(new_col_index).value==255
           )
        {
            co_ordinates.at(0)=new_row_index;
            co_ordinates.at(1)=new_col_index;
            result.push_back(co_ordinates);
        }
    }
}

void image_package_class::bin_color_mapper(Mat* plot,int min_size_of_edge,vector<vector<pixel>> &sobel_binary_map,vector<vector<pixel*>> &edge_vec)
{
    //color mapper
    //creating the sobel_binary_map. Here the plotted img is cleaned up by first creating objs using color mapper and than eliminating small objs.
    sobel_binary_map.clear();
    for(int a=0;a<plot->rows;a++)
    {
        vector<pixel> pixel_row;
        pixel_row.clear();
        for(int b=0;b<plot->cols;b++)
        {
            pixel pixel_obj;
            pixel_obj.x=b;
            pixel_obj.y=a;
            pixel_obj.value=(int)plot->at<uchar>(a,b);
            pixel_row.push_back(pixel_obj);
        }
        sobel_binary_map.push_back(pixel_row);
    }
    //color mapping work starts here
    edge_vec.clear();
    int edge_id=0;
    for(int a=0;a<sobel_binary_map.size();a++)//rows
    {
        for(int b=0;b<sobel_binary_map.at(a).size();b++)//cols
        {
            if(sobel_binary_map.at(a).at(b).select_status==true || sobel_binary_map.at(a).at(b).value==0)
            {   continue;}
            vector<vector<int>> result_buffer;
            result_buffer.clear();
            vector<vector<int>> result;
            result.clear();
            vector<pixel*> edge_pixel_vec;
            edge_pixel_vec.clear();
            // something for the color distance bla bla.....
            vector<pixel> pixel_vec;
            sobel_binary_map.at(a).at(b).edge_id=edge_id;
            edge_pixel_vec.push_back(&sobel_binary_map.at(a).at(b));
            sobel_binary_map.at(a).at(b).select_status=true;
            search_for_neighbour_MODIFIED_SOBEL(sobel_binary_map.at(a).at(b),sobel_binary_map,result);
            point1:
            if(result_buffer.size()!=0)
            {
                sobel_binary_map.at(result_buffer.at(0).at(0)).at(result_buffer.at(0).at(1)).edge_id=edge_id;
                edge_pixel_vec.push_back(&sobel_binary_map.at(result_buffer.at(0).at(0)).at(result_buffer.at(0).at(1)));
                sobel_binary_map.at(result_buffer.at(0).at(0)).at(result_buffer.at(0).at(1)).select_status=true;
                result.clear();
                search_for_neighbour_MODIFIED_SOBEL(sobel_binary_map.at(result_buffer.at(0).at(0)).at(result_buffer.at(0).at(1)),sobel_binary_map,result);
            }
            while(result.size()>0)
            {
                sobel_binary_map.at(result.at(0).at(0)).at(result.at(0).at(1)).edge_id=edge_id;
                edge_pixel_vec.push_back(&sobel_binary_map.at(result.at(0).at(0)).at(result.at(0).at(1)));
                sobel_binary_map.at(result.at(0).at(0)).at(result.at(0).at(1)).select_status=true;
                int row_index=result.at(0).at(0),col_index=result.at(0).at(1);
                if(result.size()>1)
                {
                    for(int c=1;c<result.size();c++)
                    {   result_buffer.push_back(result.at(c));}
                }
                result.clear();
                search_for_neighbour_MODIFIED_SOBEL(sobel_binary_map.at(row_index).at(col_index),sobel_binary_map,result);
            }
            remove_non_free_elements_MODIFIED_SOBEL(sobel_binary_map,&result_buffer);
            if(result_buffer.size()!=0)
            {   goto point1;}
            edge_id++;
            edge_vec.push_back(edge_pixel_vec);
        }
    }
    //cleaning up small edges
    for(int a=edge_vec.size()-1;a>=0;a--)
    {
        if(edge_vec.at(a).size()<min_size_of_edge)//10
        {
            edge_vec.erase(edge_vec.begin()+a);
        }
    }
}

void image_package_class::plot_combiner(vector<Mat*>& input_plot_vec,Mat* new_plot)
{
    for(int a=0;a<new_plot->rows;a++)
    {
        for(int b=0;b<new_plot->cols;b++)
        {
            for(int c=0;c<input_plot_vec.size();c++)
            {   
                if(((int)input_plot_vec[c]->at<uchar>(a,b))==255)
                {   
                    new_plot->at<uchar>(a,b)=255;
                    break; 
                }
            }
        }
    }
    //cleaning things up
    for(int a=0;a<input_plot_vec.size();a++)
    {
        input_plot_vec[a]->release();
        delete input_plot_vec[a];
    }
    input_plot_vec.clear();
}

void image_package_class::variable_thershold_sobel(Mat &img1,Mat &sobel_result,vector<vector<pixel>> &sobel_binary_map,vector<vector<pixel*>> &edge_vec,int min_edge_size)
{
    Mat* sobel;
    vector<Mat*> sobel_vec;
    vector<Mat*> img_vec;
    img_vec.push_back(&img1);
    Mat hsv;
    cvtColor(img1,hsv,CV_BGR2HSV);
    vector<Mat> hsv_channels;
    split(hsv,hsv_channels);
    Mat s=hsv_channels[1];  
    img_vec.push_back(&s);
    Mat v=hsv_channels[2]; 
    img_vec.push_back(&v);

    //sobel
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    for(int a=0;a<img_vec.size();a++)
    {
        Mat src_gray;
        sobel=new Mat();
        GaussianBlur(*img_vec[a],*img_vec[a], Size(3,3), 0, 0, BORDER_DEFAULT );
        if(a==0)
        {   cvtColor(*img_vec[a],src_gray, COLOR_BGR2GRAY );}
        else
        {   src_gray=img_vec[a]->clone();}
        Mat grad_x, grad_y;
        Mat abs_grad_x, abs_grad_y;
        Scharr(src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
        Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );//remove these 2 for best result alita
        Scharr(src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
        Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );//3
        convertScaleAbs( grad_x, abs_grad_x );
        convertScaleAbs( grad_y, abs_grad_y );
        addWeighted( abs_grad_x, 1, abs_grad_y, 1, 0,*sobel);//0.5
        sobel_vec.push_back(sobel);
        src_gray.release();
        //delete src_gray;
        grad_x.release();
        grad_y.release();
        abs_grad_x.release();
        abs_grad_y.release();
        img_vec[a]->release();
    }
    img_vec.clear();
    //string str1="sobel_result_"+src_img;
    //imwrite(str1,*sobel_vec[0]);
    sobel_result=sobel_vec[0]->clone();
    struct sortingclass 
    {
        bool operator() (pixel obj1,pixel obj2) 
        { return obj1.value>obj2.value;}
    }sorting_helper;
    int slice_row_size=40,slice_col_size=40;//40
    vector<Mat*> plot_vec;
    for(int x=0;x<sobel_vec.size();x++)
    {
        vector<pixel> pixels;
        Mat* plot=new Mat(sobel_vec[x]->size(), CV_8U,Scalar(0));
        for(int a=0;a<sobel_vec[x]->rows;a+=slice_row_size)
        {
            for(int b=0;b<sobel_vec[x]->cols;b+=slice_col_size)
            {
                Mat ROI;
                int new_slice_col_size=slice_col_size,new_slice_row_size=slice_row_size;
                if(b+slice_col_size>=sobel_vec[x]->cols || a+slice_row_size>=sobel_vec[x]->rows)
                {   //break;
                    if(b+slice_col_size>=sobel_vec[x]->cols)
                    {   new_slice_col_size=sobel_vec[x]->cols-b-1;}
                    if(a+slice_row_size>=sobel_vec[x]->rows)
                    {   new_slice_row_size=sobel_vec[x]->rows-a-1;}
                    Mat ROI2(*sobel_vec[x],Rect(b,a,new_slice_col_size,new_slice_row_size));                   
                    ROI=ROI2.clone();
                    ROI2.release();
                }
                else
                {   
                    Mat ROI2(*sobel_vec[x],Rect(b,a,slice_col_size,slice_row_size));
                    ROI=ROI2.clone();
                    ROI2.release();
                }
                for(int c=0;c<new_slice_row_size;c++)
                {
                    for(int d=0;d<new_slice_col_size;d++)
                    {   
                        pixel new_pixel;
                        new_pixel.x=c;
                        new_pixel.y=d;
                        new_pixel.value=(int)ROI.at<uchar>(c,d);
                        pixels.push_back(new_pixel);
                    }
                }
                sort(pixels.begin(),pixels.end(),sorting_helper);//sorts in descending order
                int point=0,last=0,count=0;
                for(int c=0;c<new_slice_col_size*new_slice_row_size;c++)
                {
                    if(last==pixels[c].value)
                    {   count++;}
                    else
                    {
                        last=pixels[c].value;
                        count=0;
                    }
                    if(count>10 && pixels[c].value<240)//if more than 10 pixels same selete the thershold point
                    {   
                        point=c;
                        break;
                    }
                }
                for(int c=0;c<point;c++)
                {   plot->at<uchar>(a+pixels[c].x,b+pixels[c].y)=255;}
                pixels.clear();
            }
        }
        sobel_vec[x]->release();
        delete sobel_vec[x];
        plot_vec.push_back(plot);
    }
    sobel_vec.clear();
    Mat* new_plot1=new Mat(plot_vec[0]->size(),CV_8U,Scalar(0));

    plot_combiner(plot_vec,new_plot1);
    bin_color_mapper(new_plot1,min_edge_size,sobel_binary_map,edge_vec);
    new_plot1->release();
    delete new_plot1;
}

void image_package_class::modified_sobel_process_handler()
{
    short int decrease_time=slice_row_size;//6
    Mat small_src;
    resize(*orig_image_temp,small_src,Size(orig_image_temp->cols/decrease_time,orig_image_temp->rows/decrease_time));
    vector<vector<pixel>> sobel_binary_map;
    vector<vector<pixel*>> edge_vec;
    //Mat small_out(small_src.size(),CV_8U,Scalar(0));//only for testing pusposes
    Mat sobel_result;
    variable_thershold_sobel(small_src,sobel_result,sobel_binary_map,edge_vec,30);//small_src will automatically get released in this function.
    //sobel_result.release();//sobel result not required for the small_src.
    //small src edge plotting process
    Mat small_src_edge_plot(orig_image_temp->size(),CV_8U,Scalar(0));
    for(int a=0;a<edge_vec.size();a++)
    {
        for(int b=0;b<edge_vec.at(a).size();b++)
        {
            int orig_row_index=edge_vec.at(a).at(b)->y*decrease_time,orig_col_index=edge_vec.at(a).at(b)->x*decrease_time;
            for(int c=orig_row_index;c<orig_row_index+decrease_time;c++)
            {
                for(int d=orig_col_index;d<orig_col_index+decrease_time;d++)
                {   small_src_edge_plot.at<uchar>(c,d)=255;}
            }
        }
    }
    sobel_binary_map.clear();//this is for the unpurified bin map
    edge_vec.clear();
    Mat src_img=orig_image_temp->clone();
    variable_thershold_sobel(src_img,sobel_result,sobel_binary_map,edge_vec,30);
    Mat src_edge_plot(orig_image_temp->size(),CV_8U,Scalar(0));
    for(int a=0;a<edge_vec.size();a++)
    {
        for(int b=0;b<edge_vec.at(a).size();b++)
        {
            src_edge_plot.at<uchar>(edge_vec.at(a).at(b)->y,edge_vec.at(a).at(b)->x)=255;
            small_src_edge_plot.at<uchar>(edge_vec.at(a).at(b)->y,edge_vec.at(a).at(b)->x)=255;
        }
    }
    sobel_binary_map.clear();
    edge_vec.clear();
    
    //imshow("small",src_edge_plot);
    //waitKey(0);
    //int gh;cin>>gh;
    Mat temp(sobel_result.size(),CV_8U,Scalar(0));
    for(int a=0;a<small_src_edge_plot.rows;a++)
    {
        for(int b=0;b<small_src_edge_plot.cols;b++)
        {
            if(small_src_edge_plot.at<uchar>(a,b)==255)
            {   
                //temp.at<uchar>(a,b)=255;
                sobel_result.at<uchar>(a,b)=255;
            }
        }
    }
    //sobel_plus_edge=small_src_edge_plot.clone();
    //sobel_plus_edge=src_edge_plot.clone();
    sobel_plus_edge=sobel_result.clone();
    //sobel_plus_edge=temp.clone();
    //temp.release();
    small_src_edge_plot.release();
    //not used currently under construction
    src_edge_plot.release();
    sobel_result.release();
    //imwrite("sobel_plus_edge.jpg",sobel_plus_edge);
}

void image_package_class::start_data_preparation_process()
{
    clean_up_prepared_data_obj();
    for(unsigned int a=0;a<image_paths.size();a++)
    {
        cout<<"\npath="<<image_paths[a];
        auto start = high_resolution_clock::now(); 
        cout<<"\npreprocessing going on...";
        //int gh;cin>>gh;
        orig_image_temp=new Mat();
        *orig_image_temp=imread(image_paths[a]);
        vector<Mat> channels;
        cvtColor(*orig_image_temp,*orig_image_temp,CV_BGR2HSV);
        split(*orig_image_temp,channels);
        /*
        GaussianBlur(channels[1], channels[1], Size(45,45), 0);
        GaussianBlur(channels[1], channels[1], Size(45,45), 0);
        GaussianBlur(channels[1], channels[1], Size(45,45), 0);
        GaussianBlur(channels[1], channels[1], Size(45,45), 0);

        GaussianBlur(channels[1], channels[1], Size(45,45), 0);
        GaussianBlur(channels[1], channels[1], Size(45,45), 0);
        GaussianBlur(channels[1], channels[1], Size(45,45), 0);
        */
        merge(channels,*orig_image_temp);
        cvtColor(*orig_image_temp,*orig_image_temp,COLOR_HSV2BGR);
        channels[0].release();
        channels[1].release();
        channels[2].release();
        channels.clear();
        
        //data preparation step 1
        auto stop = high_resolution_clock::now(); 
        if(perform_time_analysis==true)
        { 
            auto duration1 = duration_cast<microseconds>(stop - start); 
            cout<<"\nduration= "<<duration1.count()<<" microseconds";
        }
        
        auto start2 = high_resolution_clock::now(); 
        cout<<"\ncolor mapping...";
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
        modified_sobel_process_handler();
        //cout<<"\n\nx="<<sobel.cols<<" y="<<sobel.rows;
        imwrite("modified_sobel2_result.jpg",sobel_plus_edge);
        create_color_maps();
        sobel_plus_edge.release();
        //cout<<"\nx2="<<image_map[0].size()<<" y2="<<image_map.size();
        auto stop2 = high_resolution_clock::now(); 
        if(perform_time_analysis==true)
        {  
            auto duration2 = duration_cast<microseconds>(stop2 - start2); 
            cout<<"\nduration= "<<duration2.count()<<" microseconds";
        }
        auto start3 = high_resolution_clock::now(); 
        cout<<"\nstrict process...";
        //similar_obj_combination_process_strict();//no data leakage till here
        auto stop3 = high_resolution_clock::now(); 
        if(perform_time_analysis==true)
        {
            auto duration3 = duration_cast<microseconds>(stop3 - start3); 
            cout<<"\nduration= "<<duration3.count()<<" microseconds";
            cout<<"\nfind_neighbouring_objs_timer= "<<find_neighbouring_objs_timer;
            cout<<"\nfind_neighbouring_obj_avg_color_of_closest_area_timer= "<<find_neighbouring_obj_avg_color_of_closest_area_timer;
            cout<<"\nfirst= "<<first;
            cout<<"\nsecond= "<<second;
            cout<<"\nthird= "<<third;
        }
        
        //cout<<"\ntesting...........";
        //plot_obj_maps(&obj_vec,&image_map,image_paths[a]);

        auto start4 = high_resolution_clock::now(); 
        cout<<"\nunstrict process...";
        //similar_obj_combinarion_process_un_strict();//leakage fixed
        auto stop4 = high_resolution_clock::now();
        if(perform_time_analysis==true) 
        {
            auto duration4 = duration_cast<microseconds>(stop4 - start4); 
            cout<<"\nduration= "<<duration4.count()<<" microseconds";
        }
        //data preparation step 2
        
        auto start5 = high_resolution_clock::now(); 
        cout<<"\nmodified_sobel...";
        //modified_sobel();
        auto stop5 = high_resolution_clock::now();
        if(perform_time_analysis==true)
        {
            auto duration5 = duration_cast<microseconds>(stop5 - start5);  
            cout<<"\nduration= "<<duration5.count()<<" microseconds";
        }
        
        //data preparation step 3
        //border_info_extractor();
        //data preparation step 4
        //data_arranger();
        orig_image_temp->release();
        delete orig_image_temp;
        //save_current_img_data();//this function is not used currently but will be used once the final result is arrived
        auto start6 = high_resolution_clock::now(); 
        cout<<"\nplotting the img_map...";
        plot_obj_maps(&obj_vec,&image_map,image_paths.at(a));
        auto stop6 = high_resolution_clock::now();
        if(perform_time_analysis==true)
        {
            auto duration6 = duration_cast<microseconds>(stop6 - start6);  
            cout<<"\nduration= "<<duration6.count()<<" microseconds";
        }
        clean_image_package_class_entierly(true);
    }
}

void image_package_class::clean_image_package_class_entierly(bool clean_prepared_data)
{
    border_element_vec.clear();
    obj_vec.clear();
    if(clean_prepared_data==true)
    {
        for(int a=0;a<image_map.size();a++)
        {
            for(int b=0;b<image_map.at(a).size();b++)
            {
                delete image_map[a][b];
            }
            image_map[a].clear();
        }
        prepared_data_obj.obj_vec_for_each_img.clear();
    }
    image_map.clear();

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
    //cout<<"\npath="<<img_path;
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

void image_package_class::combine_package_data(vector<image_package_class*> *ipc_vec)//this function is not used currently but will be used once the final result is arrived
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
    clean_image_package_class_entierly(true);
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