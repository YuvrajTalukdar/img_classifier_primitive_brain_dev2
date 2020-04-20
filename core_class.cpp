#include"core_class.h"

using namespace std;


//modified_simplex_solver::make_solution_feasible
bool modified_simplex_solver::make_solution_feasible::termination_condition_checker(simplex_table* st)
{
    bool status=true;
    for(int a=0;a<st->r_id.size();a++)
    {
        if(st->r_id[a].slack==true)
        {
            if(st->slack_var[a][st->r_id[a].id-st->basic_var[a].size()]<0 && st->rhs[a]>=0)
            {
                status=false;
                break;
            }
        }
        if(status==false)
        {   break;}
    }
    if(status==true)
    {
        //cout<<"\n\ntraining ended check table";
    }
    return status;
}

void modified_simplex_solver::make_solution_feasible::display_st(simplex_table st)
{
    fstream file1("simplex_table.csv",ios::out);
    file1<<",";
    for(int a=0;a<st.c_id.size();a++)
    {
        if(st.c_id[a].basic==true)
        {
            file1<<"c"<<st.c_id[a].id<<",";
        }
        else if(st.c_id[a].slack==true)
        {
            file1<<"s"<<st.c_id[a].id<<",";
        }
        else if(st.c_id[a].z==true)
        {
            file1<<"z"<<",";
        }
        else if(st.c_id[a].rhs==true)
        {
            file1<<"rhs"<<",";
        }
        else if(st.c_id[a].theta==true)
        {
            file1<<"theta,";
        }
    }
    file1<<"\n";
    for(int a=0;a<st.r_id.size();a++)
    {
        if(st.r_id[a].basic==true)
        {
            file1<<"c"<<st.r_id[a].id<<",";
        }
        else if(st.r_id[a].slack==true)
        {
            file1<<"s"<<st.r_id[a].id<<",";
        }
        else if(st.r_id[a].z==true)
        {
            file1<<"z"<<",";
        }
        else if(st.r_id[a].rhs==true)
        {
            file1<<"rhs"<<",";
        }


        for(int c=0;c<st.basic_var[a].size();c++)
        {
            file1<<st.basic_var[a][c]<<",";
        }
        for(int c=0;c<st.slack_var[a].size();c++)
        {
            file1<<st.slack_var[a][c]<<",";
        }
        file1<<st.z_col[a]<<","<<st.rhs[a]<<","<<st.theta[a]<<",";

        file1<<"\n";
    }
    file1<<"z,";
    for(int a=0;a<st.z_row.size();a++)
    {
        file1<<st.z_row[a]<<",";
    }

    file1.close();
}

long double modified_simplex_solver::make_solution_feasible::round_to_zero(long double input)
{
    if(input<0.001 && input>-0.001)
    {   return input;}//0;}
    else
    {   return input;}
}

void modified_simplex_solver::print_message()
{
    pthread_mutex_lock(&lock_1);
    cout<<message;
    pthread_mutex_unlock(&lock_1);
}

void modified_simplex_solver::make_solution_feasible::print_message()
{
    pthread_mutex_lock(&lock_1);
    cout<<message;
    pthread_mutex_unlock(&lock_1);
}

void modified_simplex_solver::make_solution_feasible::simplex_table_modifier(int p_row_index,int p_col_index,simplex_table* st)
{
    /*if(fg==1)
    {
        display_st(*st);
        int gh;cin>>gh;
    }*/
    //row id changer
    st->r_id[p_row_index].basic=st->c_id[p_col_index].basic;
    st->r_id[p_row_index].id=st->c_id[p_col_index].id;
    st->r_id[p_row_index].rhs=st->c_id[p_col_index].rhs;
    st->r_id[p_row_index].slack=st->c_id[p_col_index].slack;
    st->r_id[p_row_index].theta=st->c_id[p_col_index].theta;
    //st->r_id[p_row_index].z=st->r_id[p_col_index].z; //logically this line is fucked up dont know how it was working earlier

    float pe;bool checker=false;
    //pivot element extractor
    if(p_col_index<st->basic_var[0].size())
    {
        pe=st->basic_var[p_row_index][p_col_index];
        if(display_iterations==true)
        {  
            message.clear();
            message="\n\ncheck table";
            print_message();
        }
    }
    else
    {
        int slack_p_col=p_col_index-st->basic_var[0].size();
        pe=st->slack_var[p_row_index][slack_p_col];
        if(display_iterations==true)
        {
            message.clear();
            message="pe= "+to_string(pe)+"\n\n\ncheck table";
            print_message();
        }
        checker=true;
    }

    //pivot row modifier
    for(int a=0;a<st->basic_var[p_row_index].size();a++)
    {   st->basic_var[p_row_index][a]=st->basic_var[p_row_index][a]/pe;}

    for(int a=0;a<st->slack_var[p_row_index].size();a++)
    {   st->slack_var[p_row_index][a]=st->slack_var[p_row_index][a]/pe;}

    st->z_col[p_row_index]=st->z_col[p_row_index]/pe;
    double rhs_temp1=st->rhs[p_row_index],pe_temp1=pe;
    st->rhs[p_row_index]=rhs_temp1/pe_temp1;

    //rest of the row - z_row modifier
    bool basic_point=true;
    int p_col_index_temp;
    if(p_col_index>=st->basic_var[0].size())
    {
        p_col_index_temp=p_col_index-st->basic_var[0].size();
        basic_point=false;
    }

    long double multiplying_element;
    for(int a=0;a<st->basic_var.size();a++)
    {
        //multiplying element finder
        bool p_row=false;
        if(basic_point==true)
        {
            if(a==p_row_index)
            {   continue;}
            multiplying_element=st->basic_var[a][p_col_index];
        }
        else
        {
            if(a==p_row_index)
            {   continue;}
            multiplying_element=st->slack_var[a][p_col_index_temp];
        }
        //basic_var row modification instruction
        for(int b=0;b<st->basic_var[a].size();b++)
        {
            st->basic_var[a][b]=(st->basic_var[a][b]-multiplying_element*st->basic_var[p_row_index][b]);
        }
        //slack_var modification instruction
        for(int b=0;b<st->slack_var[a].size();b++)
        {
            st->slack_var[a][b]=(st->slack_var[a][b]-multiplying_element*st->slack_var[p_row_index][b]);
        }

        //z col modification
        st->z_col[a]=(st->z_col[a]-multiplying_element*st->z_col[p_row_index]);
        //rhs col modification instruction
        st->rhs[a]=st->rhs[a]-multiplying_element*st->rhs[p_row_index];
    }

    //z row modifier
    if(display_iterations==true)
    {  
        message.clear();
        message="\np_col_index= "+to_string(p_col_index)+" multiplying element= "+to_string(multiplying_element);
        print_message();
    }
    multiplying_element=st->z_row[p_col_index];
    //vector<float> basic_plus_slack_plus_z_plus_rhs_temp;
    //basic_plus_slack_plus_z_plus_rhs_temp.clear();
    buffer_obj.basic_plus_slack_plus_z_plus_rhs_temp.clear();
    buffer_obj.basic_plus_slack_plus_z_plus_rhs_temp.resize(st->basic_var[p_row_index].size());//bug found memory allocation problem
    for(int b=0;b<st->basic_var[p_row_index].size();b++)
    {   
        float var1=st->basic_var[p_row_index][b];
        int s1=buffer_obj.basic_plus_slack_plus_z_plus_rhs_temp.size();
        buffer_obj.basic_plus_slack_plus_z_plus_rhs_temp[b]=var1;
        //basic_plus_slack_plus_z_plus_rhs_temp.push_back(var1);
    }//canny data crash point
    for(int b=0;b<st->slack_var[p_row_index].size();b++)
    {   buffer_obj.basic_plus_slack_plus_z_plus_rhs_temp.push_back(st->slack_var[p_row_index][b]);}// bug found long double to float conversion
    buffer_obj.basic_plus_slack_plus_z_plus_rhs_temp.push_back(st->z_col[p_row_index]);
    buffer_obj.basic_plus_slack_plus_z_plus_rhs_temp.push_back(st->rhs[p_row_index]);

    for(int b=0;b<st->z_row.size();b++)
    {
        //cout<<"\nz_row= "<<st->z_row[b]<<" basic_s_z_r_t= "<<basic_plus_slack_plus_z_plus_rhs_temp[b];
        st->z_row[b]=(st->z_row[b]-multiplying_element*buffer_obj.basic_plus_slack_plus_z_plus_rhs_temp[b]);
    }
    //display_st(*st);
}

void modified_simplex_solver::make_solution_feasible::conflicting_data_finder(simplex_table* st)
{
    conflict_id.conflict_id_present=true;
    conflict_id.id.clear();
    for(int a=0;a<st->r_id.size();a++)
    {
        if(st->r_id[a].slack==true)
        {
            if(st->slack_var[a][st->r_id[a].id-st->basic_var[a].size()]<0 && st->rhs[a]>0) //a bug may be present here.
            {
                conflict_id.id.push_back(a);
            }
        }
    }
    //module working tested problem present in the conflictig data detection
}

void modified_simplex_solver::make_solution_feasible::pivot_element_finder(simplex_table* st)
{
    bool conflicting_data_found=false,bad_p_row_index_status=false;
    cyclic_bug_present_var=false;
    buffer_obj.p_col_index.clear();
    buffer_obj.p_row_index.clear();
    do
    {
        iteration++;
        if(display_iterations==true)
        {
            message.clear();
            message="\niteration= "+to_string(iteration)+"r_id size= "+to_string(st->r_id.size());
            print_message();
        }
        //pivot column finder
        int p_col_index,p_row_index;
        int row_with_negative_slack;
            //row havaing negative slack finder
        bool p_col_found=false;
        int row_skip=1;
        bool row_with_negative_slack_found=false;
        for(int a=0;a<st->r_id.size();a++)
        {
            if(st->r_id[a].slack==true)
            {
                if(st->slack_var[a][st->r_id[a].id-st->basic_var[a].size()]<0 && st->rhs[a]>0) //a bug may be present here.
                {
                    row_with_negative_slack=a;
                    row_with_negative_slack_found=true;
                    break;
                }
            }
        }

        if(row_with_negative_slack_found==true)
        {
            if(display_iterations==true)
            {   
                message.clear();
                message="\nrow_with_negative_slack= "+to_string(row_with_negative_slack);
                print_message();
            }
                //pivot column finding process
                    //search in basic var vector

            for(int a=0;a<st->basic_var[row_with_negative_slack].size();a++)
            {
                if(st->basic_var[row_with_negative_slack][a]>0)
                {
                    p_col_index=a;
                    p_col_found=true;
                    break;
                }
            }
                    //search in slack var vector
            if(p_col_found==false)
            {
                for(int a=0;a<st->slack_var[row_with_negative_slack].size();a++)
                {
                    if(st->slack_var[row_with_negative_slack][a]>0)
                    {
                        p_col_index=a+st->basic_var[row_with_negative_slack].size();
                        p_col_found=true;
                        break;
                    }
                }
            }

            if(p_col_found==false)
            {
                conflicting_data_finder(st);
                conflicting_data_found=true;
            }
            else
            {
                conflict_id.id.clear();
                conflict_id.conflict_id_present=false;
                st->theta.clear();
                for(int a=0;a<st->r_id.size();a++)
                {
                    if(p_col_index<st->basic_var[a].size())
                    {
                        if(st->basic_var[a][p_col_index]==0)
                        {
                            long double var1=0;
                            st->theta.push_back(var1);
                        }
                        else
                        {
                            long double rhs=st->rhs[a],basic_var=st->basic_var[a][p_col_index];//if long double is not used than precision problem may occur which may result in looping which even bland's rule fails to solve
                            long double calc=rhs/basic_var;
                            st->theta.push_back(calc);
                        }
                    }
                    else
                    {
                        int temp_col_index=p_col_index-st->basic_var[a].size();
                        if(st->slack_var[a][temp_col_index]==0)
                        {
                            st->theta.push_back(0);
                        }
                        else
                        {
                            long double rhs=st->rhs[a],slack_var=st->slack_var[a][temp_col_index];
                            long double calc=rhs/slack_var;
                            st->theta.push_back(calc);
                        }
                    }
                }
                //pivot row finder
                //vector<long double> sorted_theta;
                buffer_obj.sorted_theta.clear();
                buffer_obj.sorted_theta.insert(buffer_obj.sorted_theta.begin(),st->theta.begin(),st->theta.end());
                //sorted_theta=st->theta;
                sort(buffer_obj.sorted_theta.begin(),buffer_obj.sorted_theta.end());
                long double smallest_positive_theta;
                for(int a=0;a<buffer_obj.sorted_theta.size();a++)
                {
                    if(buffer_obj.sorted_theta[a]>0)
                    {   smallest_positive_theta=buffer_obj.sorted_theta[a];
                        break;
                    }
                }
                bool bad_p_row_index=true;
                for(int a=0;a<st->theta.size();a++)
                {
                    if(st->theta[a]==smallest_positive_theta)
                    {
                        p_row_index=a;bad_p_row_index=false;
                        break;
                    }
                }
                if(display_iterations==true)
                { 
                    message.clear();
                    message="\nsmallest positive theta= "+to_string(smallest_positive_theta)+"\npivot row= "+to_string(p_row_index)+" pivot column= "+to_string(p_col_index);
                    print_message();
                }
                if(bad_p_row_index==false)
                {   
                    if(add_index_data(p_col_index,p_row_index)==false)
                    {   simplex_table_modifier(p_row_index,p_col_index,st);}
                    else
                    {   
                        cyclic_bug_present_var=true;
                        break;
                    }
                }
                else if(bad_p_row_index==true)
                {
                    //bad_p_row_index handler
                    bad_p_row_index_status=true;
                }        
            }
        }
        else
        {   break;}
    }
    while(termination_condition_checker(st)==false && conflicting_data_found==false && bad_p_row_index_status==false);
}

bool modified_simplex_solver::make_solution_feasible::add_index_data(int p_col,int p_row)
{
    if(buffer_obj.p_col_index.size()<2 && buffer_obj.p_row_index.size()<2)
    {
        buffer_obj.p_col_index.push_back(p_col);
        buffer_obj.p_row_index.push_back(p_row);
        return false;
    }
    else
    {
        bool status=false;
        for(int a=0;a<buffer_obj.p_row_index.size();a++)
        {
            if(buffer_obj.p_row_index[a]==p_row && buffer_obj.p_col_index[a]==p_col)
            {   status=true;}
        }
        if(status==true)
        {   return status;}
        else
        {
            vector<int> p_row_temp;
            vector<int> p_col_temp;
            for(int a=1;a<buffer_obj.p_col_index.size();a++)
            {
                p_row_temp.push_back(buffer_obj.p_row_index[a]);
                p_col_temp.push_back(buffer_obj.p_col_index[a]);
            }
            p_row_temp.push_back(p_row);
            p_col_temp.push_back(p_col);
            buffer_obj.p_col_index.clear();
            buffer_obj.p_row_index.clear();
            buffer_obj.p_col_index=p_col_temp;
            buffer_obj.p_row_index=p_row_temp;
            return false;
        }   
    }
}

bool modified_simplex_solver::make_solution_feasible::cyclic_bug_present()
{   return cyclic_bug_present_var;}

conflicting_data_id modified_simplex_solver::make_solution_feasible::return_conflict_id_pack()
{   return conflict_id;}

void modified_simplex_solver::make_solution_feasible::start(simplex_table* st)
{   pivot_element_finder(st);}

//modified_simplex_solver
void modified_simplex_solver::display_st(simplex_table st)
{
    fstream file1("simplex_table.csv",ios::out);
    file1<<",";
    for(int a=0;a<st.c_id.size();a++)
    {
        if(st.c_id[a].basic==true)
        {
            file1<<"c"<<st.c_id[a].id<<",";
        }
        else if(st.c_id[a].slack==true)
        {
            file1<<"s"<<st.c_id[a].id<<",";
        }
        else if(st.c_id[a].z==true)
        {
            file1<<"z"<<",";
        }
        else if(st.c_id[a].rhs==true)
        {
            file1<<"rhs"<<",";
        }
        else if(st.c_id[a].theta==true)
        {
            file1<<"theta,";
        }
    }
    file1<<"\n";
    for(int a=0;a<st.r_id.size();a++)
    {
        if(st.r_id[a].basic==true)
        {
            file1<<"c"<<st.r_id[a].id<<",";
        }
        else if(st.r_id[a].slack==true)
        {
            file1<<"s"<<st.r_id[a].id<<",";
        }
        else if(st.r_id[a].z==true)
        {
            file1<<"z"<<",";
        }
        else if(st.r_id[a].rhs==true)
        {
            file1<<"rhs"<<",";
        }


        for(int c=0;c<st.basic_var[a].size();c++)
        {
            file1<<st.basic_var[a][c]<<",";
        }
        for(int c=0;c<st.slack_var[a].size();c++)
        {
            file1<<st.slack_var[a][c]<<",";
        }
        file1<<st.z_col[a]<<","<<st.rhs[a]<<","<<st.theta[a]<<",";

        file1<<"\n";
    }
    file1<<"z,";
    for(int a=0;a<st.z_row.size();a++)
    {
        file1<<st.z_row[a]<<",";
    }

    file1.close();
}

bool modified_simplex_solver::check_for_corrupt_cdp(converted_data_pack* cdp)
{
    try{
        int firing_crash_size=cdp->firing_data.size();
        int not_firing_data=cdp->not_firing_data.size();
        int firing_crash=cdp->firing_data.at(0).size();
        int not_firing_crash=cdp->not_firing_data.at(0).size();
        return false;
    }
    catch(exception &e)
    {   return true;}
}

bool modified_simplex_solver::start_solver(converted_data_pack* cdp)
{
    if(display_iterations==true)
    {   
        message.clear();
        message="\n\ncdp->firing_data.size()= "+to_string(cdp->firing_data.size());
        print_message();
    }
    id temp_id;
    simplex_table* st=new simplex_table();
    st->c_id.clear();
    bool corrupt_cdp=check_for_corrupt_cdp(cdp);
    if(corrupt_cdp==true)
    {
        cout<<"got it!!!!";
        cdp->corupt_pack=true;
        return true;
    }
    else if(corrupt_cdp==false)
    {   
        for(int a=0;a<cdp->firing_data[0].size()*2;a++)
        {
            temp_id.basic=true;
            temp_id.slack=false;
            temp_id.z=false;
            temp_id.rhs=false;
            temp_id.theta=false;
            temp_id.id=a;
            st->c_id.push_back(temp_id);
        }
        int slack_id=cdp->firing_data[0].size()*2;
        for(int a=cdp->firing_data[0].size();a<(cdp->firing_data[0].size()+cdp->firing_data.size()+cdp->not_firing_data.size());a++)
        {
            temp_id.slack=true;
            temp_id.basic=false;
            temp_id.z=false;
            temp_id.theta=false;
            temp_id.rhs=false;
            temp_id.id=slack_id;
            st->c_id.push_back(temp_id);
            slack_id++;
        }

        temp_id.slack=false;
        temp_id.basic=false;
        temp_id.rhs=false;
        temp_id.z=true;
        temp_id.theta=false;
        temp_id.id=slack_id;
        st->c_id.push_back(temp_id);
        slack_id++;

        temp_id.slack=false;
        temp_id.basic=false;
        temp_id.rhs=true;
        temp_id.z=false;
        temp_id.theta=false;
        temp_id.id=slack_id;
        st->c_id.push_back(temp_id);

        temp_id.slack=false;
        temp_id.basic=false;
        temp_id.rhs=false;
        temp_id.z=false;
        temp_id.theta=true;
        temp_id.id=slack_id;
        st->c_id.push_back(temp_id);

        st->r_id.clear();
        for(int a=0;a<st->c_id.size();a++)
        {
            if(st->c_id[a].slack==true)
            {
                st->r_id.push_back(st->c_id[a]);
            }
        }

        st->slack_var.clear();
        st->basic_var.clear();
        vector<float> temp;
        int x;
        for(int a=0;a<cdp->firing_data.size();a++)
        {
            temp.clear();
            //entering basic variable data
            for(int b=0;b<cdp->firing_data[a].size();b++)
            {   temp.push_back(cdp->firing_data[a][b]);
                temp.push_back(cdp->firing_data[a][b]-2*cdp->firing_data[a][b]);
            }
            st->basic_var.push_back(temp);
            temp.clear();
            //entering slack var data
            for(int b=0;b<st->r_id.size();b++)
            {
                if(b==a)
                {   temp.push_back(-1);x=b;}
                else
                {   temp.push_back(0);}
            }
            //temp.push_back(0);//for z
            //temp.push_back(cdp->lower_firing_constrain_rhs);
            st->slack_var.push_back(temp);

            st->z_col.push_back(0);
            st->rhs.push_back(cdp->lower_firing_constrain_rhs); //modification needs to be done here
        }
        for(int a=0;a<cdp->not_firing_data.size();a++)
        {
            temp.clear();
            //entering basic variable data
            for(int b=0;b<cdp->not_firing_data[a].size();b++)
            {   temp.push_back(cdp->not_firing_data[a][b]);
                temp.push_back(cdp->not_firing_data[a][b]-2*cdp->not_firing_data[a][b]);
            }
            st->basic_var.push_back(temp);
            temp.clear();
            //entering slack var data
            for(int b=0;b<st->r_id.size();b++)
            {
                if((b)==(a+x+1))
                {   temp.push_back(1);}
                else
                {   temp.push_back(0);}
            }
            //temp.push_back(0);//for z
            //temp.push_back(cdp->upper_not_firing_constrain_rhs);
            st->slack_var.push_back(temp);

            st->z_col.push_back(0);
            st->rhs.push_back(cdp->upper_not_firing_constrain_rhs); //modification needs to be done here
        }

        st->z_row.clear();
        //entering z row data
        for(int a=0;a<cdp->objective_function_coefficients.size();a++)
        {
            st->z_row.push_back(cdp->objective_function_coefficients[a]);
            st->z_row.push_back(cdp->objective_function_coefficients[a]-2*cdp->objective_function_coefficients[a]);
        }
        for(int a=0;a<st->r_id.size();a++)
        {   st->z_row.push_back(0);}
        st->z_row.push_back(1);// co_orodianted (z,z)
        st->z_row.push_back(0);//(z,rhs)
        //display_st();
        //cout<<"data size1= "<<cdp->firing_data.size()<<endl;
        //needs modification
        feasible_solution_calculator.start(st);
        //cout<<"data size2= "<<cdp->firing_data.size()<<endl;
        conflicting_data_id conflict_id;
        conflict_id=feasible_solution_calculator.return_conflict_id_pack();

        if(conflict_id.conflict_id_present==true)
        {
            conflicting_data.firing_data.clear();
            conflicting_data.not_firing_data.clear();
            conflicting_data.objective_function_coefficients.clear();
            conflicting_data.weight_matrix.clear();
            if(display_iterations==true)
            {
                message.clear();
                message="\nconflicting databefore erasing:\n firing data size= "+to_string(cdp->firing_data.size())+"\n";
                print_message();
            }
            if(conflict_id.id.size()==cdp->firing_data.size() && cdp->firing_data.size()==1)
            {
                cout<<"\nbingo!!";
                cdp->corupt_pack=true;
            }
            else if(conflict_id.id.size()==cdp->firing_data.size())//for handling 0:0 bug
            {   
                int firing_data_limit=cdp->firing_data.size()/2;
                int not_firing_data_limit=cdp->not_firing_data.size()/2;
                converted_data_pack cdp_temp;
                for(int a=cdp->firing_data.size()-1;a>=firing_data_limit;a--)//saving the 2nd half firing_data in the conflicting_data
                {
                    conflicting_data.firing_data.push_back(cdp->firing_data[a]);
                    cdp->firing_data.erase(cdp->firing_data.begin()+a);
                }
                for(int a=cdp->not_firing_data.size()-1;a>=not_firing_data_limit;a--)
                {
                    conflicting_data.not_firing_data.push_back(cdp->not_firing_data[a]);
                    cdp->not_firing_data.erase(cdp->not_firing_data.begin()+a);
                }
                conflicting_data.firing_label=cdp->firing_label;
                conflicting_data.firing_neuron_index=cdp->firing_neuron_index;
                if(check_for_corrupt_cdp(cdp)==false)
                start_solver(cdp);
                else
                cout<<"\ncheck2";
            }
            else
            {
                int count1=0;
                for(int a=conflict_id.id.size()-1;a>=0;a--)
                {
                    //save the conflicting data in a obj of converted data pack.
                    conflicting_data.firing_data.push_back(cdp->firing_data[conflict_id.id[a]]); //copying the conflicting data
                    if(display_iterations==true)
                    {   
                        message.clear();
                        message="erasing id= "+to_string(conflict_id.id[a])+",";
                        print_message();
                    }
                    cdp->firing_data.erase(cdp->firing_data.begin()+conflict_id.id[a]); //erasing the conflicting data
                    if(display_iterations==true)
                    {   
                        message.clear();
                        message="firing data size= "+to_string(cdp->firing_data.size())+"\n";
                        print_message();
                    }
                    count1++;
                }
                //ratio maintainer
                int current_not_firing_data_size=cdp->not_firing_data.size();
                int current_firing_data_size=cdp->firing_data.size();//                                   .get_ratio(cdp->firing_neuron_index)                        .get_ratio(cdp->firing_neuron_index) //get_ratio function is not required in this version.
                float no_of_notfiring_data_to_be_removed_float=(current_not_firing_data_size*ratio_locker1.get_default_ratio()-current_firing_data_size)/ratio_locker1.get_default_ratio();
                int no_of_notfiring_data_to_be_removed=no_of_notfiring_data_to_be_removed_float;
                    //not firing data are veing removed from cdp and added to conflicting_data
                if(display_iterations==true)//left_out
                {
                    cout<<"\nno of not_firing_data to be removed= "<<no_of_notfiring_data_to_be_removed<<endl;
                    cout<<"current not_firing_data size= "<<cdp->not_firing_data.size()<<endl;
                    cout<<"firing_data_size= "<<cdp->firing_data.size()<<endl;
                    cout<<"no of firing data removed= "<<count1<<endl;
                    cout<<"default_ratio= "<<ratio_locker1.get_default_ratio()<<endl;
                    cout<<"cdp->firing_neuron_index= "<<cdp->firing_neuron_index<<endl;
                }
                for(int a=current_not_firing_data_size-1;a>=(current_not_firing_data_size-no_of_notfiring_data_to_be_removed);a--)
                {
                    conflicting_data.not_firing_data.push_back(cdp->not_firing_data[a]);
                    cdp->not_firing_data.erase(cdp->not_firing_data.begin()+a);
                }
                conflicting_data.firing_neuron_index=cdp->firing_neuron_index; //this line is important 
                conflicting_data.firing_label=cdp->firing_label; //this line is important
                if(display_iterations==true)
                {
                    cout<<"\nafter erasing\n";
                    cout<<"firing_size= "<<cdp->firing_data.size()<<" not_firing_size= "<<cdp->not_firing_data.size()<<endl;
                }
                if(check_for_corrupt_cdp(cdp)==false)
                start_solver(cdp); //calling start_solver function with non conflicting data.
                else
                cout<<"\ncheck1";
            }
        }
        else
        {
            cdp->weight_matrix.clear();
            vector<double> weight_matrix_raw;
            weight_matrix_raw.clear();
            //push the raw solution in the converted data pack.
            for(int a=0;a<cdp->objective_function_coefficients.size()*2;a++)
            {
                bool found=false;
                for(int b=0;b<st->r_id.size();b++)
                {
                    if(st->r_id[b].id==a && st->r_id[b].basic==true)
                    {
                        //cdp->weight_matrix.push_back(st.rhs[b]*st.basic_var[b][a]);
                        weight_matrix_raw.push_back(st->rhs[b]*st->basic_var[b][a]);
                        found=true;
                        break;
                    }
                }
                if(found==false)
                {
                    //cdp->weight_matrix.push_back(0);
                    weight_matrix_raw.push_back(0);
                }
            }
            //pushing the calculated solution in the weight matrix
            for(int a=0;a<cdp->objective_function_coefficients.size()*2;a+=2)
            {
                cdp->weight_matrix.push_back(weight_matrix_raw[a]-weight_matrix_raw[a+1]);
            }
            //as cdp is an address of the original cdp obj is already present in the previous function.
        }
        return false;
    }
    
    free(st);
}

bool modified_simplex_solver::cyclic_bug_present()
{   return feasible_solution_calculator.cyclic_bug_present();}


//simplex_solver_data_preparation_class
void simplex_solver_data_preparation_class::cdp_viewer(converted_data_pack* cdp)
{
    cout<<"firing neuron index= "<<cdp->firing_neuron_index<<"  label= "<<cdp->firing_label<<endl;
    cout<<"objective function coefficient= "<<endl;
    for(int a=0;a<cdp->objective_function_coefficients.size();a++)
    {   cout<<cdp->objective_function_coefficients[a]<<",";}
    cout<<"\n\nfiring data:-"<<endl<<endl;
    for(int a=0;a<cdp->firing_data.size()-100;a++)
    {
        //cout<<"a="<<a<<" size= "<<cdp->firing_data[a].size()<<endl;
        for(int b=0;b<cdp->firing_data[a].size();b++)
        {   cout<<cdp->firing_data[a][b]<<",";}
        cout<<endl;
    }
    cout<<"\n\nnot_firing data:-\n\n";
    for(int a=0;a<cdp->not_firing_data.size()-100;a++)
    {
        for(int b=0;b<cdp->not_firing_data[a].size();b++)
        {   cout<<cdp->not_firing_data[a][b]<<",";}
        cout<<endl;
    }
}

void simplex_solver_data_preparation_class::cdp_saver_in_mathematical_format(converted_data_pack* cdp)
{
    fstream file1("question.txt",ios::out);

    for(int a=0;a<cdp->objective_function_coefficients.size();a++)
    {
        file1<<cdp->objective_function_coefficients[a]<<"c"<<a+1<<"+";
        cout<<cdp->objective_function_coefficients[a]<<",";
    }
    file1<<"\n\n\n";
    for(int a=0;a<cdp->firing_data.size();a++)
    {
        file1<<"\n";
        for(int b=0;b<cdp->firing_data[a].size();b++)
        {
            file1<<cdp->firing_data[a][b]<<"c"<<b+1;
            if(b==cdp->firing_data[a].size()-1)
            {   continue;}
            file1<<"+";
        }
        file1<<">40";
    }
    file1<<"\n\n";
    file1.close();
    ofstream file2("question2.txt",ios::out);
    file2<<"\n\n";
    cout<<" size= "<<cdp->not_firing_data.size();
    for(int a=0;a<cdp->not_firing_data.size();a++)
    {
        file2<<"\n";
        for(int b=0;b<cdp->not_firing_data[a].size();b++)
        {
            file2<<cdp->not_firing_data[a][b]<<"c"<<b+1;
            //cout<<cdp->not_firing_data[a][b]<<"c"<<b+1<<endl;
            if(b==cdp->not_firing_data[a].size()-1)
            {   continue;}
            file2<<"+";
        }
        //cout<<a<<" ";
        file2<<"<10";
    }
}

bool simplex_solver_data_preparation_class::cyclic_bug_present()
{   return lpp_solver1.cyclic_bug_present();}
/*void simplex_solver_preparation_class::cdp_saver(converted_data_pack* cdp)
{
    ofstream file2("question2.txt",ios::out);
    file2<<"\n\n";
    cout<<" size= "<<cdp->not_firing_data.size();
    for(int a=0;a<cdp->not_firing_data.size();a++)
    {
        file2<<"\n";
        for(int b=0;b<cdp->not_firing_data[a].size();b++)
        {
            file2<<cdp->not_firing_data[a][b]<<"c"<<b+1;
            //cout<<cdp->not_firing_data[a][b]<<"c"<<b+1<<endl;
            if(b==cdp->not_firing_data[a].size()-1)
            {   continue;}
            file2<<"+";
        }
        //cout<<a<<" ";
        file2<<"<10";
    }
    file2<<"testing hello";
}*/

void simplex_solver_data_preparation_class::cdp_spliter(vector<converted_data_pack*> &cdps,int index)
{
    //rooms
    converted_data_pack *cdp_temp1=new converted_data_pack();
    converted_data_pack *cdp_temp2=new converted_data_pack();
    //spliting process
    int a=0,b=0;
        //first half
    for(a;a<cdps[index]->firing_data.size()/2;a++)
    {   cdp_temp1->firing_data.push_back(cdps[index]->firing_data[a]);}
    for(b;b<cdps[index]->not_firing_data.size()/2;b++)
    {   cdp_temp1->not_firing_data.push_back(cdps[index]->not_firing_data[b]);}
    cdp_temp1->corupt_pack=cdps[index]->corupt_pack;
    cdp_temp1->firing_label=cdps[index]->firing_label;
    cdp_temp1->firing_neuron_index=cdps[index]->firing_neuron_index;
    cdp_temp1->lower_firing_constrain_rhs=cdps[index]->lower_firing_constrain_rhs;
    cdp_temp1->objective_function_coefficients=cdps[index]->objective_function_coefficients;
    cdp_temp1->upper_not_firing_constrain_rhs=cdps[index]->upper_not_firing_constrain_rhs;
    cdp_temp1->weight_matrix=cdps[index]->weight_matrix;
        //second half
    for(a=cdps[index]->firing_data.size()/2;a<cdps[index]->firing_data.size();a++)
    {   cdp_temp2->firing_data.push_back(cdps[index]->firing_data[a]);}
    for(b=cdps[index]->not_firing_data.size()/2;b<cdps[index]->not_firing_data.size();b++)
    {   cdp_temp2->not_firing_data.push_back(cdps[index]->not_firing_data[b]);}
    cdp_temp2->corupt_pack=cdps[index]->corupt_pack;
    cdp_temp2->firing_label=cdps[index]->firing_label;
    cdp_temp2->firing_neuron_index=cdps[index]->firing_neuron_index;
    cdp_temp2->lower_firing_constrain_rhs=cdps[index]->lower_firing_constrain_rhs;
    cdp_temp2->objective_function_coefficients=cdps[index]->objective_function_coefficients;
    cdp_temp2->upper_not_firing_constrain_rhs=cdps[index]->upper_not_firing_constrain_rhs;
    cdp_temp2->weight_matrix=cdps[index]->weight_matrix;
    //add data to cdp vec
    //cout<<"\n\norig:";
    //cout<<"\nfiring_data= "<<cdps[index]->firing_data.size();
    //cout<<"\nnot fiirng data= "<<cdps[index]->not_firing_data.size();
    converted_data_pack* temp;
    temp=cdps[index];
    cdps[index]=cdp_temp1;
    temp->firing_data.clear();
    temp->not_firing_data.clear();
    temp->objective_function_coefficients.clear();
    temp->weight_matrix.clear();
    //cout<<"\n\ncdp_temp1:";
    //cout<<"\nfiring_data= "<<cdps[index]->firing_data.size();
    //cout<<"\nnot firing data= "<<cdps[index]->not_firing_data.size();
    cdps.push_back(cdp_temp2);
    //cout<<"\n\ncdp_temp2:";
    //cout<<"\nfiring_data= "<<cdps[cdps.size()-1]->firing_data.size();
    //cout<<"\nnot firing data= "<<cdps[cdps.size()-1]->not_firing_data.size();
    //cout<<"\n\ncheck!!!";
    //int fg;cin>>fg;
}

void simplex_solver_data_preparation_class::check_path_quality()
{
    //cout<<"\n\nf_data_labels= "<<network->f_data_vec_for_elimination_algorithm.size();
    //cout<<"\ndata size= "<<network->f_data_vec_for_elimination_algorithm[0].data.size();
    vector<bool> label_fire_status;//name is confusing, i know
    vector<bool> pass_status(network->f_data_vec_for_elimination_algorithm.size());
    float pass_ratio=1.9;//2
    for(int a=0;a<network->f_data_vec_for_elimination_algorithm.size();a++)
    {
        if(network->path.at(network->path.size()-1).output_neuron_id==a)
        {   label_fire_status.push_back(true);}
        else
        {   label_fire_status.push_back(false);}
    }
    for(int a=0;a<network->f_data_vec_for_elimination_algorithm.size();a++)
    {
        int correct=0;
        for(int b=0;b<network->f_data_vec_for_elimination_algorithm[a].data.size();b++)
        {
            float summation_temp=0;
            for(int c=0;c<network->path.at(network->path.size()-1).weight_matrix.size();c++)
            {   summation_temp+=network->path.at(network->path.size()-1).weight_matrix[c]*network->f_data_vec_for_elimination_algorithm[a].data[b][c];}
            //summation_temp=((atan(summation_temp)*180/3.1415)/90)*100;
            if(label_fire_status[a]==true)
            {
                if(summation_temp>0)
                {   correct++;}
            }
            else if(label_fire_status[a]==false)
            {
                if(summation_temp<=0)
                {   correct++;}
            }
        }
        if(correct>=((float)network->f_data_vec_for_elimination_algorithm[a].data.size())/pass_ratio)
        {   pass_status[a]=true;}
        else
        {   pass_status[a]=false;}
    }
    for(int a=0;a<pass_status.size();a++)
    {
        if(pass_status[a]==false)
        {
            network->path.pop_back();
            break;
        }
    }
    //cout<<"\n\n\nno_of_paths= "<<network->path.size();
    //int gh;cin>>gh;
}

void simplex_solver_data_preparation_class::lp_solver()
{
    //the main conflicting_data_outer cleaner
    conflicting_data_buffer_outer.conflicting_data_buffer_vector.clear();
    int f_size=cdp[0]->firing_data.size();//memory_optimization7
    int nf_size=cdp[0]->not_firing_data.size();//memory_optimization7
    for(int a=0;a<cdp.size();a++)
    {
        shared_block_data_obj.no_of_c_datapacks_completed++;
        point1:
        //cdp_spliter(cdp,a);
        lpp_solver1.start_solver(cdp[a]);
        if(lpp_solver1.cyclic_bug_present()==true)
        {
            message.clear();
            message="\ncyclic bug detected...fixing...";
            print_message();
            cdp_spliter(cdp,a);
            goto point1;
        }
        if(display_iterations==true)
        {
            cout<<"\nfiring size= "<<cdp[a]->firing_data.size()<<"\nnot firing size= "<<cdp[a]->not_firing_data.size()<<"\n";
            cout<<"weight matrix: ";
            for(int b=0;b<cdp[a]->weight_matrix.size();b++)
            {
                cout<<cdp[a]->weight_matrix[b]<<",";
            }
            cout<<endl;
        }
        //adding the conflicting data to the conflicting data buffer
        //getting the conflicting data from the lpp_solver1 and adding them to the conflicting_data_buffer.
        conflicting_data_buffer.firing_data.insert(conflicting_data_buffer.firing_data.end(),lpp_solver1.conflicting_data.firing_data.begin(),lpp_solver1.conflicting_data.firing_data.end());
        conflicting_data_buffer.not_firing_data.insert(conflicting_data_buffer.not_firing_data.end(),lpp_solver1.conflicting_data.not_firing_data.begin(),lpp_solver1.conflicting_data.not_firing_data.end());
        conflicting_data_buffer.firing_neuron_index=lpp_solver1.conflicting_data.firing_neuron_index; //this line is important //the data here is directily brought from the cdp
        conflicting_data_buffer.firing_label=lpp_solver1.conflicting_data.firing_label; //this line is important //the data here is directily brought from the cdp
        //claner of conflicting_data_buffer_outer.conflicting_data_buffer_vector
        //erasing the conflicting data from the lpp_solver1
        lpp_solver1.conflicting_data.firing_data.clear();
        lpp_solver1.conflicting_data.not_firing_data.clear();
        lpp_solver1.conflicting_data.objective_function_coefficients.clear();
        lpp_solver1.conflicting_data.weight_matrix.clear();

        //lp_optimizer may be added here.
        //handling each of the weight matrix is required.

        // 1. create new path
        //   1a. provide the output neuron id. (training for which output neuron or which label. First neuron = label 2 second neuron = label 4)
        //   2b. push the weight matrix
        if(cdp[a]->corupt_pack==false)
        {   
            pthread_mutex_lock(&lock);
            network->create_new_path(cdp[a]->weight_matrix,cdp[a]->firing_neuron_index);
            //THE GREATE ELIMINATION ALGORITHM
            if(network->return_no_of_paths()>0)
            {   check_path_quality();}
            pthread_mutex_unlock(&lock);
            //new paths getting created
        }
        //enter the label of the conflicting data //I think this process is already complete...... 
        
        //pushing the conflicting_data_buffer of each label to the conflicting_data_buffer_vector.
        if(conflicting_data_buffer.firing_data.size()>0 && conflicting_data_buffer.not_firing_data.size()>0)
        {   conflicting_data_buffer_outer.conflicting_data_buffer_vector.push_back(conflicting_data_buffer);}
        
        //cleanup
        conflicting_data_buffer.weight_matrix.clear();
        conflicting_data_buffer.objective_function_coefficients.clear();
        conflicting_data_buffer.firing_data.clear();
        conflicting_data_buffer.not_firing_data.clear();
    }
    // 2. conflicting datas must be handled here.

    if(conflicting_data_buffer_outer.conflicting_data_buffer_vector.size()>0)
    {
        //objective function coefficient calculation
        int d=0;
        for(int a=0;a<conflicting_data_buffer_outer.conflicting_data_buffer_vector.size();a++)
        {
            conflicting_data_buffer_outer.conflicting_data_buffer_vector[a].objective_function_coefficients.resize(data_structure->no_of_elements_in_each_record);
            for(int b=0;b<data_structure->no_of_elements_in_each_record;b++)
            {
                for(d=0;d<conflicting_data_buffer_outer.conflicting_data_buffer_vector[a].firing_data.size();d++){
                    conflicting_data_buffer_outer.conflicting_data_buffer_vector[a].objective_function_coefficients[b]+=conflicting_data_buffer_outer.conflicting_data_buffer_vector[a].firing_data[d][b];
                }
                conflicting_data_buffer_outer.conflicting_data_buffer_vector[a].objective_function_coefficients[b]=conflicting_data_buffer_outer.conflicting_data_buffer_vector[a].objective_function_coefficients[b]/d;
            }
        }
        for(int a=0;a<conflicting_data_buffer_outer.conflicting_data_buffer_vector.size();a++)
        {
            if(conflicting_data_buffer_outer.conflicting_data_buffer_vector[a].firing_data.size()==0)
            {
                conflicting_data_buffer_outer.conflicting_data_buffer_vector.erase(conflicting_data_buffer_outer.conflicting_data_buffer_vector.begin()+a);
            }
        }
        message.clear();
        message="\nsolving for the conflicting data.....\n";
        print_message();
        cdp.clear();//memory_optimization7
        vector<converted_data_pack> cdp_temp;
        cdp_temp=conflicting_data_buffer_outer.conflicting_data_buffer_vector;
        for(int a=0;a<cdp_temp.size();a++)
        {   cdp.push_back(&cdp_temp[a]);}
        //cdp=conflicting_data_buffer_outer.conflicting_data_buffer_vector;
        lp_solver();
    }
    else
    {   
        message.clear();
        message="\nfinished training the entire package.\n";
        print_message();
    }
}

void simplex_solver_data_preparation_class::print_message()
{
    pthread_mutex_lock(&lock_1);
    cout<<message;
    pthread_mutex_unlock(&lock_1);
}

simplex_solver_data_preparation_class::simplex_solver_data_preparation_class(vector<converted_data_pack> &cdps,datapack_structure_defination* ds,ann* network1)
{
    network=network1;
    data_structure=ds;
    for(int a=0;a<cdps.size();a++)//memory_optimization7
    {   cdp.push_back(&cdps[a]);}
    //cdp=cdps;
}


//core_class
bool core_class::check_if_datapack_has_valid_labels(nn_core_data_package_class* data_pack)
{
    if(data_pack->data.size()>data_pack->labels.size())
    {   return false;}
    else
    {   return true;}
}

void core_class::save_network()
{
    time_t theTime = time(NULL);
    struct tm *aTime = localtime(&theTime);
    int day = aTime->tm_mday;
    string day_str= to_string(day);
    if(day<10)
    {   day_str="0"+day_str;}
    int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    string month_str=to_string(month);
    if(month<10)
    {   month_str="0"+month_str;}
    int year = aTime->tm_year + 1900; // Year is # years since 1900
    string year_str=to_string(year);
    time_t now = time(0);
    // Convert now to tm struct for local timezone
    tm* localtm = localtime(&now);
    int hr,min,sec;
    hr=localtm->tm_hour;
    min=localtm->tm_min;
    sec=localtm->tm_sec;
    string hr_str=to_string(hr),min_str=to_string(min),sec_str=to_string(sec);
    if(hr<10)
    {   hr_str="0"+hr_str;}
    if(min<10)
    {   min_str="0"+min_str;}
    if(sec<10)
    {   sec_str="0"+sec_str;}
    string net_savefile_id="";//core_aim,core_no,day,month,year
    string core_aim_str=to_string(core_aim);
    string core_no_str=to_string(core_no);
    net_savefile_id=core_aim_str+core_no_str+year_str+month_str+day_str+hr_str+min_str+sec_str;
    string net_savefile_name="network-"+net_savefile_id+".csv";
    ofstream file1(net_savefile_name,ios::out);
    file1<<"FILE_NAME:,\n";
    file1<<"name=,"<<net_savefile_name<<",\n";
    file1<<"BASIC_SAVEFILE_INFO:,\n";
    file1<<",network_save_id,core_aim,core_no,year,month,day,hour,minute,sec,\n";
    file1<<"network_save_id,"<<net_savefile_id<<","<<core_aim<<","<<core_no<<","<<year_str<<","<<month_str<<","<<day_str<<","<<hr_str<<","<<min_str<<","<<sec_str<<",\n";
    file1<<"BASIC_NETWORK_INFO:,";
    file1<<"net_info_category,no_of_input_neuron,no_of_output_neuron,\n";
    file1<<",net_info_category,"<<network1.input_neuron_size()<<","<<network1.output_neuron_size()<<",\n";
    file1<<"DATA_LABEL_AND_OUTPUT_NEURON_INDEX:,\n";
    file1<<"[data_label~output_neuron_index],";
    for(int a=0;a<ds.elements.size();a++)
    {
        file1<<ds.elements[a]<<"~"<<a<<",";
    }
    file1<<"\n";
    file1<<"PATH_INFO:,\n";
    file1<<"no_of_path=,"<<network1.return_no_of_paths()<<",\n";
    file1<<"path_id,";
    for(int a=0;a<network1.path[0].weight_matrix.size();a++)
    {
        int weight_index=a;
        string weight_index_str=to_string(weight_index);
        weight_index_str="w"+weight_index_str;
        file1<<weight_index_str<<",";
    }
    file1<<"input_neuron_id,output_neuron_id\n";
    for(int a=0;a<network1.path.size();a++)
    {
        file1<<network1.path[a].path_id<<",";
        for(int b=0;b<network1.path[a].weight_matrix.size();b++)
        {
            file1<<network1.path[a].weight_matrix[b]<<",";
        }
        file1<<"[|";
        for(int b=0;b<network1.path[a].input_neuron_id.size();b++)
        {
            file1<<network1.path[a].input_neuron_id[b]<<"|";
        }
        file1<<"],";
        file1<<network1.path[a].output_neuron_id<<",\n";
    }
    file1.close();
}

bool core_class::load_network_if_available(int core_aim,int core_no,bool file_name_received,string file_name)
{
    string file_name_local;
    bool file_found=false;
    int weight_count=0;//weight_count
    int no_of_input_neuron,no_of_output_neuron;
    vector<float> elements;
    if(file_name_received==false)
    {        
        //int core_aim=0, core_no=0;
        struct dirent *de;  // Pointer for directory entry
        // opendir() returns a pointer of DIR type. 
        DIR *dr = opendir(".");
        if (dr == NULL)  // opendir returns NULL if couldn't open directory
        {
            message.clear();
            message="Could not open current directory";
            print_message();
            return false;
        }
        char filename_sub_str[]="network";
        vector<string> network_save_file_list;
        network_save_file_list.clear();
        while ((de = readdir(dr)) != NULL)
        {
            if(strcasestr(de->d_name,filename_sub_str))
            {   network_save_file_list.push_back(de->d_name);}
        }
        closedir(dr);    
        //string required_save_filename;
        vector<ifstream> input_file_streams(network_save_file_list.size());
        for(int a=0;a<network_save_file_list.size();a++)
        {   input_file_streams[a].open(network_save_file_list[a],ios::in);}
        int input_file_stream_index;
        for(int a=0;a<input_file_streams.size();a++)
        {
            int core_aim_read=7,core_no_read=5;
            while(input_file_streams[a])
            {
                string line;
                input_file_streams[a]>>line;
                if(input_file_streams[a].eof())
                {   break;}
                char line_arr[line.length()];
                bool core_no_read_collected=false,core_aim_read_collected=false;
                strcpy(line_arr,line.c_str());
                if(strcasestr(line_arr,"BASIC_SAVEFILE_INFO:"))
                {
                    input_file_streams[a]>>line;
                    int core_aim_index,core_no_index,word_index=0;
                    bool core_no_found=false,core_aim_found=false;
                    char ch[2]={'\0'},ch_arr[20]={'\0'};
                    for(int b=0;b<line.length();b++)
                    {
                        if(line.at(b)==',')
                        {
                            //float val = atof(num_char);  
                            word_index++;
                            if(strcasestr(ch_arr,"core_no"))
                            {
                                //cout<<"core_no= "<<word_index<<endl;
                                core_no_index=word_index;
                                core_no_found=true;
                            }
                            if(strcasestr(ch_arr,"core_aim"))
                            {
                                //cout<<"core_aim= "<<word_index;
                                core_aim_index=word_index;
                                core_aim_found=true;
                            }
                            if(core_aim_found==true && core_no_found==true)
                            {   break;}
                            for(int b=0;b<20;b++){
                                ch_arr[b]='\0';
                            }
                            continue;
                        }
                        ch[0]=line.at(b);
                        ch[1]='\0';
                        strcat(ch_arr,ch);
                    }
                    if(core_aim_found==true && core_no_found==true)
                    {
                        input_file_streams[a]>>line;
                        word_index=0;
                        for(int b=0;b<line.length();b++)
                        {
                            if(line.at(b)==',')
                            {
                                //float val = atof(num_char);  
                                word_index++;
                                if(word_index==core_aim_index)
                                {   
                                    core_aim_read=atoi(ch_arr);
                                    core_aim_read_collected==true;//bug may be present
                                    //cout<<"core_aim= "<<core_aim_read<<endl;
                                }
                                if(word_index==core_no_index)
                                {   
                                    core_no_read=atoi(ch_arr);
                                    core_no_read_collected==true;//bug may be  present
                                    //cout<<"core_no= "<<core_no_read<<endl;
                                }
                                for(int b=0;b<20;b++){
                                    ch_arr[b]='\0';
                                }
                                continue;
                            }
                            ch[0]=line.at(b);
                            ch[1]='\0';
                            strcat(ch_arr,ch);
                        }
                    }
                }
                if(core_no_read_collected==true && core_aim_read_collected==true)//what the fuck is this????? keep an eye on this for bugs
                {   break;}
            }
            //cout<<"core_aim_read="<<core_aim_read<<" core_aim="<<core_aim<<endl;
            //cout<<"core_no_read="<<core_no_read<<" core_no="<<core_aim<<endl;
            if(core_aim==core_aim_read && core_no==core_no_read)
            {   
                input_file_stream_index=a;
                file_found=true;
                break;
            }
        }
        for(int a=0;a<input_file_streams.size();a++)
        {   input_file_streams[a].close();}
        if(file_found==true)
        {   file_name_local=network_save_file_list[input_file_stream_index];}
    }
    if(file_found==true || file_name_received==true)
    {
        if(file_name_received==true)
        {   file_name_local=file_name;}
        ifstream input_net_savefile(file_name_local,ios::in);
        bool path_info_found=false;int p=0;
        while(input_net_savefile)
        {
            string line;
            input_net_savefile>>line;
            if(input_net_savefile.eof())
            {   break;}
            char line_arr[line.length()];
            strcpy(line_arr,line.c_str());
            int no_of_paths;
            if(path_info_found==true)
            {
                p++;//reading the paths data.
                char arr_temp[line.length()],ch[2];
                int output_neuron_id;
                vector<float> weight_matrix;
                weight_matrix.clear();
                int index=0;
                for(int a=0;a<line.length();a++)
                {
                    if(line.at(a)==',')
                    {
                        index++;
                        if(index>1 && (index-1)<=weight_count )
                        {
                            weight_matrix.push_back(atof(arr_temp));
                        }
                        if(index==(3+weight_count))
                        {
                            output_neuron_id=atof(arr_temp);
                        }
                        for(int b=0;b<line.length();b++){
                            arr_temp[b]='\0';
                        }
                        continue;
                    }
                    ch[0]=line.at(a);
                    ch[1]='\0';
                    strcat(arr_temp,ch);
                }
                //point for entering the data in the network
                /*for(int a=0;a<weight_matrix.size();a++)
                {
                    cout<<weight_matrix[a]<<",";
                }
                cout<<" output_neuron= "<<output_neuron_id;
                cout<<endl;*/
                network1.create_new_path(weight_matrix,output_neuron_id);
            }
            if(strcasestr(line_arr,"BASIC_NETWORK_INFO"))//needed to be added in the core_class function
            {
                input_net_savefile>>line;
                char arr_temp[line.length()],ch[2];
                int word_index=0;
                for(int a=0;a<line.length();a++)
                {
                    if(line.at(a)==',')
                    {
                        word_index++;
                        if(word_index==3)
                        {
                            no_of_input_neuron=atoi(arr_temp);//cout<<"arr_temp= "<<arr_temp;
                        }
                        else if(word_index==4)
                        {
                            no_of_output_neuron=atoi(arr_temp);
                        }
                        for(int b=0;b<line.length();b++)
                        {   arr_temp[b]='\0';}
                        continue;
                    }
                    ch[0]=line.at(a);
                    ch[1]='\0';
                    strcat(arr_temp,ch);
                }
            }
            if(strcasestr(line_arr,"DATA_LABEL_AND_OUTPUT_NEURON_INDEX:"))
            {
                input_net_savefile>>line;
                char arr_temp[line.length()],ch[2];
                int word_index=0;
                for(int a=0;a<line.length();a++)
                {
                    if(line.at(a)==',')
                    {
                        word_index++;
                        if(word_index>1)
                        {
                            bool arr_temp_cleaner=false;
                            for(int b=0;b<strlen(arr_temp);b++)
                            {
                                if(arr_temp[b]=='~')
                                {   arr_temp_cleaner=true;}
                                if(arr_temp_cleaner==true)
                                {   arr_temp[b]='\0';}
                            }
                            elements.push_back(atof(arr_temp));
                        }
                        for(int b=0;b<line.length();b++)
                        {   arr_temp[b]='\0';}
                        continue;
                    }
                    ch[0]=line.at(a);
                    ch[1]='\0';
                    strcat(arr_temp,ch);
                }
                network1.set_elements_vector(elements);
            }
            if(strcasestr(line_arr,"PATH_INFO:"))
            {
                input_net_savefile>>line;
                char arr_temp[line.length()],ch[2];
                int word_index=0;
                for(int a=0;a<line.length();a++)
                {
                    if(line.at(a)==',')
                    {
                        word_index++;
                        if(word_index==2)
                        {   
                            no_of_paths=atoi(arr_temp);
                        }
                        for(int b=0;b<line.length();b++){
                            arr_temp[b]='\0';
                        }
                        continue;
                    }
                    ch[0]=line.at(a);
                    ch[1]='\0';
                    strcat(arr_temp,ch);
                }
                //cout<<"no_of_paths= "<<no_of_paths;
                input_net_savefile>>line;
                char arr_temp1[line.length()],ch1[2];
                for(int b=0;b<line.length();b++)
                {
                    arr_temp1[b]='\0';
                }
                for(int a=0;a<line.length();a++)
                {
                    if(line.at(a)==',')
                    {
                        //cout<<"arr_temp1= "<<arr_temp1<<endl;
                        if(strcasestr(arr_temp1,"w"))
                        {
                            weight_count++;
                        }
                        for(int b=0;b<line.length();b++){
                            arr_temp1[b]='\0';
                        }
                        continue;
                    }
                    ch1[0]=line.at(a);
                    ch1[1]='\0';
                    strcat(arr_temp1,ch1);
                }
                path_info_found=true;
                //cout<<"weight_count= "<<weight_count;
                //int g;cin>>g;
            }
        }
        //the below lines does the work of network_structure_modifier() function
        network1.set_no_of_input_neuron(no_of_input_neuron);
        network1.set_no_of_output_neuron(no_of_output_neuron);
        network_analyzer();//initialization of ns
        //cout<<"p="<<p;
        return true;
    }
    else
    {   return false;}
}

void core_class::network_analyzer()
{
    ns.no_of_input_neuron=network1.input_neuron_size();
    ns.no_of_output_neuron=network1.output_neuron_size();
    ns.elements=network1.elements;
    ns.mean_buffer_size=network1.input_neurons.size();//what the fuck is this????
}

void core_class::datapack_analyzer(nn_core_data_package_class* data_pack)
{
    if(data_pack->analyze_status==false)
    {
        ds.no_of_elements_in_each_record=data_pack->data[0].size();
        vector<int> labels;
        bool found=false;
        for(int a=0;a<data_pack->data.size();a++)
        {
            found=false;
            for(int b=0;b<labels.size();b++)
            {
                if(labels[b]==data_pack->labels[a])
                {
                    found=true;
                    break;
                }
            }
            if(found==false)
            {
                labels.push_back(data_pack->labels[a]);
            }
        }
        ds.no_of_labels=labels.size();
        for(int a=0;a<labels.size();a++)
        {
            ds.elements.push_back(labels[a]);
        }
        data_pack->analyze_status=true;
    }
}

void core_class::network_structure_modifier()
{
    try{
        if(ns.no_of_input_neuron>ds.no_of_elements_in_each_record)
        {
            throw("network has more neuron than required by the data");//needs working here. UNDER CONSTRUCTION.
            save_network();
        }
        else
        {
            network1.set_no_of_input_neuron(ds.no_of_elements_in_each_record);
            network1.set_no_of_output_neuron(ds.no_of_labels);
        }
    }
    catch(string s)
    {   cout<<s<<endl;}
}

void core_class::shuffler(nn_core_filtered_data* f_data)
{
    vector<shuffling_data> shuffling_data_temp_vector;
    shuffling_data shuffling_data_temp;
    shuffling_data_temp_vector.clear();
    //pushing the data the the shuffling vector
    for(int a=0;a<f_data->data.size();a++)
    {
        shuffling_data_temp.temp_data=f_data->data[a];
        shuffling_data_temp.temp_label=f_data->label;
        shuffling_data_temp_vector.push_back(shuffling_data_temp);
    }
    //shuffeling the data
    random_shuffle(shuffling_data_temp_vector.begin(),shuffling_data_temp_vector.end());
    //pushing the data in the f_data
    f_data->data.clear();
    for(int a=0;a<shuffling_data_temp_vector.size();a++)
    {
        f_data->data.push_back(shuffling_data_temp_vector[a].temp_data);
        f_data->label=shuffling_data_temp_vector[a].temp_label;
    }
}

void core_class::f_data_viewer(string str,vector<nn_core_filtered_data> f_data)
{
    message.clear();
    message="\n"+str+"\n";
    print_message();
    for(int a=0;a<f_data.size();a++)
    {
        message.clear();
        message="\nlabel= "+to_string(f_data[a].label)+" data_size= "+to_string(f_data[a].data.size());
        print_message();
    }
}

void core_class::filter(nn_core_data_package_class* data_pack,int train_test_predict)
{
    nn_core_filtered_data f_data;
    vector<nn_core_filtered_data> f_data_vector;
    for(int a=0;a<ds.elements.size();a++)
    {
        f_data.data.clear();
        for(int b=0;b<data_pack->labels.size();b++)
        {
            if(ds.elements[a]==data_pack->labels[b])
            {
                f_data.data.push_back(data_pack->data[b]);
                f_data.label=data_pack->labels[b];
                f_data.str=data_pack->label_str;
            }
        }
        f_data_vector.push_back(f_data);
    }
    //calling the shuffler
    for(int a=0;a<f_data_vector.size();a++)
    {
        shuffler(&f_data_vector[a]);//has same size as that is of no of element
    }
    //copying the data to f_train_data and f_test_data
    for(int a=0;a<f_data_vector.size();a++)
    {
        nn_core_filtered_data f_data_temp;
        //copying data to f_train_data
        int limit=0;
        if(train_test_predict==0)//for training
        {   limit=f_data_vector[a].data.size();}
        else if(train_test_predict==1)//for testing
        {   limit=f_data_vector[a].data.size()/data_division;}
        //for(int b=0;b<f_data_vector[a].data.size()/data_division;b++)
        for(int b=0;b<limit;b++)
        {
            f_data_temp.data.push_back(f_data_vector[a].data[b]);
        }
        f_data_temp.label=f_data_vector[a].label;
        f_data_temp.str=f_data_vector[a].str;
        f_train_data.push_back(f_data_temp);
        f_data_temp.data.clear();
        //copying data to f_test_data
        //memory_optimization1
        /*int begin=0;
        if(train_test_predict==0)//for training 
        {   begin=f_data_vector[a].data.size();}
        else if(train_test_predict==1)//for testing
        {   begin=f_data_vector[a].data.size()/data_division;}
        //for(int b=f_data_vector[a].data.size()/data_division;b<f_data_vector[a].data.size();b++)
        for(int b=begin;b<f_data_vector[a].data.size();b++)
        {
            f_data_temp.data.push_back(f_data_vector[a].data[b]);
        }
        f_data_temp.label=f_data_vector[a].label;
        f_test_data.push_back(f_data_temp); 
        f_data_temp.data.clear();
        */
    }
    //copying the data to the data_pack s
    for(int a=0;a<f_data_vector.size();a++)
    {
        //copying data to testing data
        int begin=0;
        if(train_test_predict==0)//for training 
        {   begin=f_data_vector[a].data.size();}
        else if(train_test_predict==1)//for testing
        {   begin=f_data_vector[a].data.size()/data_division;}
        //for(int b=f_data_vector[a].data.size()/data_division;b<f_data_vector[a].data.size();b++)
        for(int b=begin;b<f_data_vector[a].data.size();b++)
        {
            vector<float> temp;
            for(int c=0;c<f_data_vector[a].data[b].size();c++)
            {
                temp.push_back(f_data_vector[a].data[b][c]);
            }
            test_data.data.push_back(temp);
            temp.clear();
            test_data.label_str=f_data_vector[a].str;
            test_data.labels.push_back(f_data_vector[a].label);
        }
    }
    f_data_vector.clear();//if memory_optimization1 is removed and testing_for_each_label_is_enabled this must be removed
}

void core_class::big_c_datapack_handler(vector<converted_data_pack> &cdp)//passing the vector by reference //this function might be a temporary offer //this is for preventing 0:0 bug
{
    int limit=50;//50
    converted_data_pack cdp_temp1,cdp_temp2;
    vector<converted_data_pack> cdp_vect_temp;
    for(int a=cdp.size()-1;a>=0;a--)
    {
        if(cdp[a].firing_data.size()>limit)
        {
            cdp_temp1=cdp[a];
            cdp.erase(cdp.begin()+a);
            int begin=0,end=0;
            bool end_reached=false;
            while(end_reached==false)
            {
                cdp_temp2.firing_data.clear();
                cdp_temp2.not_firing_data.clear();
                cdp_temp2.objective_function_coefficients.clear();
                begin=end;
                end=begin+limit;
                if(end>=cdp_temp1.firing_data.size())
                {   end=cdp_temp1.firing_data.size();end_reached=true;}
                cdp_temp2.firing_data.insert(cdp_temp2.firing_data.end(),cdp_temp1.firing_data.begin()+begin,cdp_temp1.firing_data.begin()+end);
                if(end_reached==true)
                {   end=cdp_temp1.not_firing_data.size();}
                else if(end>=cdp_temp1.not_firing_data.size())
                {
                    int end2=cdp_temp1.firing_data.size();
                    cdp_temp2.firing_data.insert(cdp_temp2.firing_data.end(),cdp_temp1.firing_data.begin()+end,cdp_temp1.firing_data.begin()+end2);
                    end=cdp_temp1.not_firing_data.size();
                    end_reached=true;
                }
                cdp_temp2.not_firing_data.insert(cdp_temp2.not_firing_data.end(),cdp_temp1.not_firing_data.begin()+begin,cdp_temp1.not_firing_data.begin()+end);
                cdp_temp2.firing_label=cdp_temp1.firing_label;
                cdp_temp2.firing_neuron_index=cdp_temp1.firing_neuron_index;
                cdp_temp2.objective_function_coefficients=cdp_temp1.objective_function_coefficients;
                cdp_vect_temp.push_back(cdp_temp2);
            }
            cdp_temp1.firing_data.clear();
            cdp_temp1.not_firing_data.clear();
            cdp_temp1.objective_function_coefficients.clear();
        }
    }
    cdp.insert(cdp.end(),cdp_vect_temp.begin(),cdp_vect_temp.end());
    message.clear();
    message="\n\ncdp size after erasing in big data handler = "+to_string(cdp.size());
    print_message();
    message.clear();
    message=", cdp_vect_temp size= "+to_string(cdp_vect_temp.size());
    print_message();
    cdp_vect_temp.clear();
    
    for(int a=0;a<cdp.size();a++)
    {
        int difference=cdp[a].firing_data.size()-cdp[a].not_firing_data.size();
        if(abs(difference)>10)
        {
            cdp_temp1.firing_data.clear();
            cdp_temp1.not_firing_data.clear();
            cdp_temp1.objective_function_coefficients.clear();
            cdp_temp2.firing_data.clear();
            cdp_temp2.not_firing_data.clear();
            cdp_temp2.objective_function_coefficients.clear();
            cdp_temp1=cdp[a];
            cdp.erase(cdp.begin()+a);
            while(abs(difference)>10)
            {
                if(difference<0)
                {
                    limit=cdp_temp1.firing_data.size();
                    cdp_temp2.firing_data=cdp_temp1.firing_data;
                    cdp_temp2.not_firing_data.insert(cdp_temp2.not_firing_data.end(),cdp_temp1.not_firing_data.begin()+abs(difference),cdp_temp1.not_firing_data.end());
                    cdp_temp2.firing_label=cdp_temp1.firing_label;
                    cdp_temp2.firing_neuron_index=cdp_temp1.firing_neuron_index;
                    cdp_temp2.objective_function_coefficients=cdp_temp1.objective_function_coefficients;
                    cdp_temp1.not_firing_data.erase(cdp_temp1.not_firing_data.begin()+abs(difference),cdp_temp1.not_firing_data.end());
                    cdp_vect_temp.push_back(cdp_temp2);
                }
                else if(difference>0)
                {
                    limit=cdp_temp1.not_firing_data.size();
                    cdp_temp2.not_firing_data=cdp_temp1.not_firing_data;
                    cdp_temp2.firing_data.insert(cdp_temp2.firing_data.end(),cdp_temp1.firing_data.begin()+abs(difference),cdp_temp1.firing_data.end());
                    cdp_temp2.firing_label=cdp_temp1.firing_label;
                    cdp_temp2.firing_neuron_index=cdp_temp1.firing_neuron_index;
                    cdp_temp2.objective_function_coefficients=cdp_temp1.objective_function_coefficients;
                    cdp_temp1.firing_data.erase(cdp_temp1.firing_data.begin()+abs(difference),cdp_temp1.firing_data.end());
                    cdp_vect_temp.push_back(cdp_temp2);
                }
                difference=cdp_temp1.firing_data.size()-cdp_temp1.not_firing_data.size();
                cdp_temp2.firing_data.clear();
                cdp_temp2.not_firing_data.clear();
                cdp_temp2.objective_function_coefficients.clear();
            }
            cdp.push_back(cdp_temp1);
        }
    }
    cdp.insert(cdp.end(),cdp_vect_temp.begin(),cdp_vect_temp.end());
    message.clear();
    message="\ncdp size after stabilizing extreme ratios = "+to_string(cdp.size());
    print_message();
    message.clear();
    message=", cdp_vect_temp size= "+to_string(cdp_vect_temp.size());
    print_message();
}

void core_class::simplex_solver_data_entry_point(vector<nn_core_filtered_data> f_data_pack,datapack_structure_defination* ds)
{
    vector<converted_data_pack> c_datapacks;
    converted_data_pack c_datapack;
    //ratio maintance and packing data in c_datapacks.
    int sum_total_training_data=0;
    for(int a=0;a<f_data_pack.size();a++)
    {   sum_total_training_data=sum_total_training_data+f_data_pack[a].data.size();}
    message.clear();
    message="\nsize of training data set= "+to_string(sum_total_training_data)+"\n";
    print_message();
    c_datapacks.clear(); //for asured cleaniness
    for(int a=0;a<f_data_pack.size();a++)
    {
        message.clear();
        message="packing data for label= "+to_string(f_data_pack[a].label)+"\n";
        print_message();
        //determining the c_data_pack critical info
        int sum_total_not_firing_data=sum_total_training_data-f_data_pack[a].data.size();
        int no_of_c_data_packs_needed=0,no_of_not_firing_data_in_each_pack=0,no_of_firing_data_in_each_pack=0;
        int additional_firing_data_in_the_last_datapack=0,additional_not_firing_data_in_the_last_datapack=0;
        if(sum_total_not_firing_data>=f_data_pack[a].data.size())//for not firing data > firing data
        {
            while(sum_total_not_firing_data>=f_data_pack[a].data.size())
            {
                sum_total_not_firing_data=sum_total_not_firing_data-f_data_pack[a].data.size();//cout<<"check2";
                no_of_c_data_packs_needed++;
            }
            int rem1=sum_total_not_firing_data;
            no_of_firing_data_in_each_pack=f_data_pack[a].data.size();
            no_of_not_firing_data_in_each_pack=f_data_pack[a].data.size()+rem1/no_of_c_data_packs_needed;
            additional_not_firing_data_in_the_last_datapack=rem1%no_of_c_data_packs_needed;
        }
        else if(sum_total_not_firing_data<f_data_pack[a].data.size()) //for firing data more than not firing data
        {
            int sum_total_firing_data=f_data_pack[a].data.size();
            while(sum_total_firing_data>=sum_total_not_firing_data)
            {
                sum_total_firing_data=sum_total_firing_data-sum_total_not_firing_data;//cout<<"check3";
                no_of_c_data_packs_needed++;
            }
            int rem1=sum_total_firing_data;
            no_of_not_firing_data_in_each_pack=sum_total_not_firing_data;
            no_of_firing_data_in_each_pack=sum_total_not_firing_data+rem1/no_of_c_data_packs_needed;
            additional_firing_data_in_the_last_datapack=rem1%no_of_c_data_packs_needed;
        }
        
        //packaging the data
        if(no_of_firing_data_in_each_pack==f_data_pack[a].data.size())//this means firing data < not firing data
        {
            int no_of_packages_created=0;
            int initial_value=0,final_value=0;
            vector<vector<float>> not_firing_data_temp;
            not_firing_data_temp.clear();
            //copying all the not firing data in not_firing_data_temp
            for(int b=0;b<f_data_pack.size();b++)
            {
                if(b!=a)
                {
                    for(int c=0;c<f_data_pack[b].data.size();c++)
                    {   not_firing_data_temp.push_back(f_data_pack[b].data[c]);}
                }
            }
            while(no_of_packages_created!=no_of_c_data_packs_needed)
            {
                //clearing the buffers
                c_datapack.firing_data.clear();
                c_datapack.not_firing_data.clear();
                c_datapack.objective_function_coefficients.clear();
                c_datapack.weight_matrix.clear();
                //packing the firing data
                for(int b=0;b<f_data_pack[a].data.size();b++)
                {   c_datapack.firing_data.push_back(f_data_pack[a].data[b]);}
                //packing not firing data
                initial_value=final_value;
                final_value=final_value+no_of_not_firing_data_in_each_pack;
                if(no_of_packages_created==no_of_c_data_packs_needed-1)//for the last package
                {   final_value=final_value+additional_not_firing_data_in_the_last_datapack;}
                //cout<<"\nnot_firing_data_temp size= "<<not_firing_data_temp.size();
                //cout<<"\nfinal_value= "<<final_value<<" initial_value= "<<initial_value;
                //cout<<"\nadditional_not_firing_data_in_the_last_datapack= "<<additional_not_firing_data_in_the_last_datapack;
                for(int b=initial_value;b<final_value;b++)
                {
                    c_datapack.not_firing_data.push_back(not_firing_data_temp[b]);
                }
                //setting up the label and output neuron index
                c_datapack.firing_label=f_data_pack[a].label;
                c_datapack.firing_neuron_index=a;
                //setting up the objective function coefficient 
                for(int b=0;b<c_datapack.firing_data[0].size();b++)
                {
                    float summation=0;
                    for(int c=0;c<c_datapack.firing_data.size();c++)
                    {   summation=summation+c_datapack.firing_data[c][b];}
                    c_datapack.objective_function_coefficients.push_back(summation);
                }
                //pushing the c_datapack in c_datapacks vector
                c_datapacks.push_back(c_datapack);
                no_of_packages_created++;
            }
        }
        else if(no_of_not_firing_data_in_each_pack==sum_total_not_firing_data)//this means firing data > not firing data
        {
            int no_of_packages_created=0;
            int initial_value=0,final_value=0;
            while(no_of_packages_created!=no_of_c_data_packs_needed)
            {
                //clearing the buffers
                c_datapack.firing_data.clear();
                c_datapack.not_firing_data.clear();
                c_datapack.objective_function_coefficients.clear();
                c_datapack.weight_matrix.clear();
                //packing the firing data
                initial_value=final_value;
                final_value=final_value+no_of_firing_data_in_each_pack;
                if(no_of_packages_created==no_of_c_data_packs_needed-1)
                {   final_value=final_value+additional_firing_data_in_the_last_datapack;}
                for(int b=initial_value;b<final_value;b++)
                {
                    c_datapack.firing_data.push_back(f_data_pack[a].data[b]);
                }
                //packing the not firing data
                for(int b=0;b<f_data_pack.size();b++)
                {
                    if(b!=a)
                    {
                        for(int c=0;c<f_data_pack[b].data.size();c++)
                        {   c_datapack.not_firing_data.push_back(f_data_pack[b].data[c]);}
                    }
                }
                //setting up the label and output neuron index
                c_datapack.firing_label=f_data_pack[a].label;
                c_datapack.firing_neuron_index=a;
                //setting up the objective function coefficient
                for(int b=0;b<c_datapack.firing_data[0].size();b++)
                {
                    float summation=0;
                    for(int c=0;c<c_datapack.firing_data.size();c++)
                    {   summation=summation+c_datapack.firing_data[c][b];}
                    c_datapack.objective_function_coefficients.push_back(summation);
                }
                //pushing the c_datapack in c_datapacks vector
                c_datapacks.push_back(c_datapack);
                no_of_packages_created++;
            }
        }
    }
    f_data_pack.clear();//memory_optimization3
    message.clear();
    message="finished packaging data in c_datapacks.";
    print_message();
    message.clear();
    message="\ntotal no of c_data_packs= "+to_string(c_datapacks.size());
    print_message();
    big_c_datapack_handler(c_datapacks);//for handling c_datapack with huge data which may create full conlflict senarios.
    message.clear();
    message="\ntotal no of c_data_packs after big c_datapacks handling= "+to_string(c_datapacks.size());
    print_message();
    //this is the place for parallelization process.
    //int required_no_of_threads;//=sysconf(_SC_NPROCESSORS_ONLN);//no of threads
        //c_datapacks division
    //required_no_of_threads=2;//15
    //set_the_no_of_threads_required(c_datapacks.size());
    required_no_of_threads=15;//15
    vector<vector<converted_data_pack>> c_datapacks_vector;
    message.clear();
    message="\narranging c_datapacks for "+to_string(required_no_of_threads)+" threads..........";
    print_message();
    c_data_packs_division_for_multi_threading(c_datapacks_vector,c_datapacks,required_no_of_threads);
    point1:
    if(required_no_of_threads!=c_datapacks_vector.size())
    {
        message.clear();
        message="\n\nERROR!!! failed to set a valid number of threads. Try setting the value to "+to_string(c_datapacks_vector.size());
        print_message();
        set_the_no_of_threads_required(0,true);//here 0 is just a random value to make this function call work
        goto point1;
    }
    //memory_optimization4 : turn  vector<simplex_solver_data_preparation_class> to  vector<simplex_solver_data_preparation_class*>       
    vector<simplex_solver_data_preparation_class*> lpp_solver_vec;
    for(int a=0;a<c_datapacks_vector.size();a++)
    {
        simplex_solver_data_preparation_class *lpp_solver=new simplex_solver_data_preparation_class(c_datapacks_vector[a],ds,&network1);//initializing the obj of the class   
        lpp_solver_vec.push_back(lpp_solver);
    }
    vector<pthread_t> threadIds(required_no_of_threads);//thread declaration
    pthread_t progress_display_thread;
    vector<int> error(required_no_of_threads);
    message.clear();
    message=" lpp_solver_vec size="+to_string(lpp_solver_vec.size());
    print_message();
    //cin.ignore(1024, '\n');
    message.clear();
    message="\n\nPress enter to continue...";
    print_message();
    //cin.get();//pause
    time_t begin=time(0);
    //lpp solvers will start now.........
    for(int a=0;a<required_no_of_threads;a++)
    {   //memory_optimization4 : turn &lpp_solver_vec[a] to lpp_solver_vec[a]
        error[a]=pthread_create(&threadIds[a], NULL, (THREADFUNCPTR) &simplex_solver_data_preparation_class::lp_solver,lpp_solver_vec[a]);//thread creator
    }
    int progress_bar_error;
    if(pds==true)
    {   progress_bar_error=pthread_create(&progress_display_thread,NULL,(THREADFUNCPTR) &core_class::display_training_progress,NULL);}
    for(int a=0;a<error.size();a++)
    {
        if(error[a])
        {   cout << "\nThread "<<a<< " creation failed : " << strerror(error[a]);}
    }
    if(pds==true)
    {
        if(progress_bar_error)
        {   cout<<"\nprogress bar thread creation failed!";}
    }
    for(int a=0;a<required_no_of_threads;a++)
    {   error[a]=pthread_join(threadIds[a],NULL);}
    //memory_optimization6 : next three lines
    for(int a=0;a<lpp_solver_vec.size();a++)
    {   free(lpp_solver_vec[a]);}
    lpp_solver_vec.clear();
    threadIds.clear();
    error.clear();
    if(pds==true)
    {   progress_bar_error=pthread_join(progress_display_thread,NULL);}
    time_t end=time(0);
    message.clear();
    message="\ntime taken for training: "+to_string(end-begin);
    print_message();
}

void core_class::display_training_progress()
{
    while(shared_block_data_obj.no_of_c_datapacks_completed<shared_block_data_obj.total_c_datapacks)
    {
        float x=shared_block_data_obj.no_of_c_datapacks_completed,y=shared_block_data_obj.total_c_datapacks;
        struct winsize w;
        ioctl(0,TIOCGWINSZ,&w);
        float percentage=(x/y)*100;
        system("clear");    
        cout<<"\nprogress: ";
        float hl=w.ws_col/2;
        float ratio=100/hl;
        float pl=percentage*hl/100;
        for(int a=0;a<hl;a++)
        {
            if(a<pl)
            {   cout<<"#";}
            else
            {   cout<<".";}
        }
        cout<<"  "<<percentage<<"%";
        cout<<"  "<<shared_block_data_obj.no_of_c_datapacks_completed<<" out of "<<shared_block_data_obj.total_c_datapacks<<" c_datapacks complete"<<endl<<endl;
        sleep(1);
    }
}

int core_class::size_of_c_datapacks_vector(vector<converted_data_pack> &c_datapacks)
{
    int sum=0;
    for(int a=0;a<c_datapacks.size();a++)
    {   sum+=c_datapacks[a].firing_data.size();}
    return sum;
}

void core_class::c_data_packs_division_for_multi_threading(vector<vector<converted_data_pack>> &c_datapacks_vector,vector<converted_data_pack> &c_datapacks,int no_of_threads)
{
    int total_data=0;
    for(int a=0;a<c_datapacks.size();a++)
    {   total_data+=c_datapacks[a].firing_data.size();}
    int data_for_each_thread=total_data/no_of_threads;
    vector<converted_data_pack> c_datapacks_new;
    c_datapacks_new.clear();
    c_datapacks_vector.clear();
    message.clear();
    message="\nno of c_datapacks= "+to_string(c_datapacks.size());
    print_message();
    for(int a=0;a<no_of_threads;a++)
    {
        int b=c_datapacks.size()-1;
        if(b<0)
        {   continue;}
        while(size_of_c_datapacks_vector(c_datapacks_new)<data_for_each_thread)
        {                
            c_datapacks_new.push_back(c_datapacks[b]);
            c_datapacks.erase(c_datapacks.begin()+b);
            b--;
            if(b<0)
            {   break;}
        }
        c_datapacks_vector.push_back(c_datapacks_new);
        c_datapacks_new.clear();
    }
    int sum=0;
    for(int a=0;a<c_datapacks_vector.size();a++)
    {   sum+=c_datapacks_vector[a].size();
        message.clear();
        message="\na= "+to_string(a)+" size= "+to_string(c_datapacks_vector[a].size());
        print_message();
    }
    message.clear();
    message="\nno of c_datapacks in c_datapacks_vector= "+to_string(sum);
    print_message();
    shared_block_data_obj.total_c_datapacks=sum;
    message.clear();
    message="\nc_data_packs size= "+to_string(c_datapacks.size());
    print_message();
}

bool core_class::ds_ns_have_same_labels()
{
    if(ds.elements.size()<=ns.elements.size())
    {   
        int count=0;
        for(int a=0;a<ds.elements.size();a++)
        {
            for(int b=0;b<ns.elements.size();b++)
            {
                if(ds.elements[a]==ns.elements[b])
                {   count++;}
            }
        }
        //cout<<"count= "<<count<<" ds= "<<ds.elements.size()<<"ns= "<<ns.elements.size()<<endl;
        if(count==ds.elements.size() && count==ns.elements.size())
        {   return true;}
        else
        {   return false;}//means current ds is different from previous ds which was used to train the network
    }
    else 
    {   return false;}//means current ds is different from previous ds which was used to train the network
}

void core_class::train(nn_core_data_package_class* data_pack,bool network_avail_status,int train_test_predict)
{
    datapack_analyzer(data_pack);//function checked!
    //filtering data according to labels
    filter(data_pack,train_test_predict);//f_train_data pack gets created here.
    //memory_optimization5 : the next two lines are added
    vector<nn_core_filtered_data> f_data_for_elimination_algorithm;
    nn_core_filtered_data f_data_temp;
    int limit_for_elimination_algorithm=50;
    for(int a=0;a<f_train_data.size();a++)
    {
        int size_of_data_for_each_label=f_train_data[a].data.size()/limit_for_elimination_algorithm;
        for(int b=0;b<f_train_data[a].data.size();b+=size_of_data_for_each_label)
        {
            f_data_temp.data.push_back(f_train_data[a].data[b]);
            f_data_temp.label=f_train_data[a].label;
            f_data_temp.str=f_train_data[a].str;
        }
        /*
        for(int b=0;b<f_train_data[a].data.size()-(f_train_data[a].data.size()-limit_for_elimination_algorithm);b++)
        {
            f_data_temp.data.push_back(f_train_data[a].data[b]);
            f_data_temp.label=f_train_data[a].label;
            f_data_temp.str=f_train_data[a].str;
        }*/
        f_data_for_elimination_algorithm.push_back(f_data_temp);
    }
    network1.set_data_for_elimination_algorithm(f_data_for_elimination_algorithm);

    data_pack->data.clear();
    data_pack->labels.clear();
    if(network_avail_status==true)
    {
        if(ds_ns_have_same_labels()==true)//training by batching
        {
            if(train_test_predict==0)//only training
            {
                message.clear();
                message="\nnetwork save file found. So training by batching without testing.";
                print_message();
                simplex_solver_data_entry_point(f_train_data,&ds);
                save_network();
                message.clear();
                message="\nnetwork saved";
                print_message();
            }
            else if(train_test_predict==1)//training with testing 
            {
                message.clear();
                message="\nnetwork save file found. So training by batching with testing.";
                print_message();
                simplex_solver_data_entry_point(f_train_data,&ds);
                save_network();
                message.clear();
                message="\nnetwork saved";
                print_message();
                testing_for_each_label();
                //test();
            }
        }
        else
        {
                //1. new variable is added to the data. Or adding new label.
                cout<<"ERROR!!!  The currently loaded network file is not associated with the currently loaded data \nfrom file you have selected as input source.";
        }
    }
    else if(network_avail_status==false)
    {
        if(train_test_predict==0)//only training
        {
            message.clear();
            message="\nnetwork save file not found. Brand new training without testing.";
            print_message();
            network_structure_modifier();
            simplex_solver_data_entry_point(f_train_data,&ds);
            save_network();
            message.clear();
            message="\nnetwork saved";
            print_message();
        }
        else if(train_test_predict==1)//training with testing 
        {
            message.clear();
            message="\nnetwork save file not found. Brand new training with testing.";
            print_message();
            network_structure_modifier();
            simplex_solver_data_entry_point(f_train_data,&ds);
            save_network();
            message.clear();
            message="\nnetwork saved";;
            print_message();
            //test();
            testing_for_each_label();
        }
    }
}

void core_class::only_testing(nn_core_data_package_class* data_pack,int train_test_predict)
{
    datapack_analyzer(data_pack);//function checked!
    //filtering data according to labels
    filter(data_pack,train_test_predict);//f_train_data pack gets created here.
    //memory_optimization5 : the next two lines are added
    data_pack->data.clear();
    data_pack->labels.clear();
    cout<<"\n\n\nONLY TESTING MODE:";
    testing_for_each_label();
}

void core_class::test()
{
    cout<<"\nTESTING PHASE"<<endl;
    // format is yes and then no naswers.
    ofstream file1("weight_matrix.csv",ios::out);
    file1<<"path id,output_neuron_id,weight matrix: ,";
    for(int b=0;b<network1.path[0].input_neuron_id.size();b++)
    {   file1<<network1.path[0].input_neuron_id[b]<<":";}
    file1<<"\n";
    for(int a=0;a<network1.path.size();a++)
    {
        file1<<network1.path[a].path_id<<",";
        file1<<network1.path[a].output_neuron_id<<":,";
        for(int b=0;b<network1.path[a].weight_matrix.size();b++)
        {   file1<<network1.path[a].weight_matrix[b]<<";";}
        file1<<"\n";
    }
    file1.close();
    cout<<"weight matrix saved\n";
    //entering data to network.
    for(int a=0;a<test_data.data.size();a++)
    {
        network1.enter_data_in_the_network(test_data.data[a]);
        network1.enter_label_neuron_to_be_fired(test_data.labels[a],ds.elements);
        network1.propagate();    
    }
    cout<<"accuracy= "<<network1.return_accuracy()<<endl;
}
//memory_optimization1
void core_class::testing_for_each_label()
{
    cout<<"wrights="<<network1.return_no_of_paths();
    nn_core_filtered_data f_data;
    vector<nn_core_filtered_data> f_test_data;
    for(int a=0;a<ds.elements.size();a++)
    {
        f_data.data.clear();
        for(int b=0;b<test_data.labels.size();b++)
        {
            if(ds.elements[a]==test_data.labels[b])
            {
                f_data.data.push_back(test_data.data[b]);
                f_data.label=test_data.labels[b];
                f_data.str=test_data.label_str;
            }
        }
        f_test_data.push_back(f_data);
    }
    ofstream *file1;
    cout<<"\nTESTING FOR EACH LABELS: \n\n";
    for(int a=0;a<f_test_data.size();a++)
    {
        //cout<<"\ntesting for data of label "<<f_test_data[a].label<<": str_label= "<<f_test_data[a].str;
        string str="data";
        str.append(to_string(a));
        file1=new ofstream(str,ios::out);
        network1.reset_statistics();
        for(int b=0;b<f_test_data[a].data.size();b++)
        {
            for(int c=0;c<f_test_data[a].data[b].size();c++)
            {   *file1<<f_test_data[a].data[b][c]<<";";}
            *file1<<"\n";
            network1.enter_data_in_the_network(f_test_data[a].data[b]);
            network1.enter_label_neuron_to_be_fired(f_test_data[a].label,ds.elements);
            network1.propagate();
        }
        *file1<<f_test_data[a].label<<",";
        file1->close();
        cout<<"accuracy= "<<network1.return_accuracy()<<endl;
    }
}

void core_class::predict_progress_bar()
{
    while(shared_block_data_obj.predict_progress_bar_numerator<shared_block_data_obj.predict_progress_bar_denominator)
    {
        float x=shared_block_data_obj.predict_progress_bar_numerator,y=shared_block_data_obj.predict_progress_bar_denominator;
        if(y!=0)
        {
            struct winsize w;
            ioctl(0,TIOCGWINSZ,&w);
            float percentage=(x/y)*100;   
            system("clear");
            cout<<"\nprogress: ";
            float hl=w.ws_col/2;
            float ratio=100/hl;
            float pl=percentage*hl/100;
            for(int a=0;a<hl;a++)
            {
                if(a<pl)
                {   cout<<"#";}
                else
                {   cout<<".";}
            }
            cout<<"  "<<percentage<<"%";
            cout<<"  "<<shared_block_data_obj.predict_progress_bar_numerator<<" out of "<<shared_block_data_obj.predict_progress_bar_denominator<<" predictions complete"<<endl<<endl;
            sleep(1);
        }
    }
}

void core_class::predict(nn_core_data_package_class* data_pack)
{
    cout<<"\nprediction going on......";
    ofstream out_stream("prediction_result.csv",ios::out);
    out_stream<<"data,label,\n";
    network1.predict_mode=true;
    float label;
    shared_block_data_obj.predict_progress_bar_denominator=data_pack->data.size();
        
    pthread_t predict_progress_bar_thread;
    int predict_progress_bar_error;
    if(pds==true)
    {   predict_progress_bar_error=pthread_create(&predict_progress_bar_thread,NULL,(THREADFUNCPTR) &core_class::predict_progress_bar,NULL);}
        
    for(int a=0;a<data_pack->data.size();a++)
    {
        for(int b=0;b<data_pack->data[a].size();b++)
        {   out_stream<<data_pack->data[a][b]<<",";}
        network1.enter_data_in_the_network(data_pack->data[a]);
        network1.reset_all_output_neurons();
        //network1.enter_label_neuron_to_be_fired(data_pack->labels[a],network1.elements);
        network1.propagate();
        int fired_output_neuron_index=0;
        for(int b=0;b<network1.output_neurons.size();b++)
        {
            bool status=network1.output_neurons[b].return_fire_status();
            if(status==true)
            {   fired_output_neuron_index=b;}
        }
        label=network1.return_label_for_firing_neuron_index(fired_output_neuron_index);
        out_stream<<":"<<label<<",\n";
        shared_block_data_obj.predict_progress_bar_numerator++;
    }
    if(pds==true)
    {   predict_progress_bar_error=pthread_join(predict_progress_bar_thread,NULL);}
    //cout<<"accuracy= "<<network1.return_accuracy()<<endl;
    out_stream.close();
    cout<<"\nPrediction complete, check the file prediction_result.csv\n";
}

void core_class::make_prediction_on_user_entered_data()
{
    system("clear");
    char continue1='y';
    vector<float> data_vector;
    float label;
    while(continue1=='y'||continue1=='Y')
    {
        cout<<"\nEnter the "<<network1.input_neuron_size()<<" digit data: \n";
        for(int a=0;a<network1.input_neuron_size();a++)
        {   
            float data;
            cout<<"a"<<a<<"= ";
            cin>>data;
            data_vector.push_back(data);
        }

        network1.enter_data_in_the_network(data_vector);
        network1.reset_all_output_neurons();
        //network1.enter_label_neuron_to_be_fired(data_pack->labels[a],network1.elements);
        network1.propagate();
        int fired_output_neuron_index=0;
        for(int b=0;b<network1.output_neurons.size();b++)
        {
            bool status=network1.output_neurons[b].return_fire_status();
            if(status==true)
            {   fired_output_neuron_index=b;}
        }
        label=network1.return_label_for_firing_neuron_index(fired_output_neuron_index);
        cout<<"\nResult = "<<label<<endl;

        data_vector.clear();
        point1:
        cout<<"\nDo you want to make prediction on another data? (y,n)";
        cin>>continue1;
        if(continue1!='N'&&continue1!='n'&&continue1!='y'&&continue1!='Y')
        {
            cout<<"\nWrong option!!!!";
            goto point1;
        }
        //a menu should not have been in here.
    }
}

void core_class::set_the_no_of_threads_required(int no_of_c_datapacks_after_big_datapack_handling/*=0*/,bool tried_before/*=false*/)//default parameters are defined in .h file only
{
    int no_of_physical_threads_in_cpu=sysconf(_SC_NPROCESSORS_ONLN);
    int prefered_no_of_threads=no_of_physical_threads_in_cpu+11;//for good speed in i5 7200
    char option;
    point1:
    if(tried_before==false)
    {
        cout<<"\n\nDo you want to manually set the no of threads? (y,n): ";
        cin>>option;
    }
        
    if(tried_before==false && (option=='n' || option=='N'))
    {
        if(no_of_c_datapacks_after_big_datapack_handling>prefered_no_of_threads)
        {   required_no_of_threads=prefered_no_of_threads;}
        else
        {   required_no_of_threads=2;}
    }
    else if(option=='y' || option=='Y' || tried_before==true)
    {
        cout<<"\nSet the no of threads= ";
        cin>>required_no_of_threads;
    }
    else
    {
        cout<<"\nWrong option!!";
        goto point1;
    }
}

void core_class::print_message()
{
    pthread_mutex_lock(&lock_1);
    cout<<message;
    //cout<<"\ncore_no= "<<core_no<<"core_name="<<core_name<<" started...";
    pthread_mutex_unlock(&lock_1);
}

void core_class::start_core()//train_test_predict=1//train_test_predic is required for extra assurance
{
    message.clear();
    message="\ncore_no= "+to_string(core_no)+" core_name="+core_name+" datapack_size= "+to_string((int)data_pack->data.size())+" started...";
    print_message();
    if(train_test_predict==1)
    {   data_division=data_division1;}
    if(check_if_datapack_has_valid_labels(data_pack)==true && train_test_predict<2)//true
    {
        bool network_avail_status=load_network_if_available(0,0,false,"");
        train(data_pack,network_avail_status,train_test_predict);
    }
    else if(train_test_predict==3)//making prediction on used entered individual data.
    {
        bool network_load_status=load_network_if_available(0,0,true,network_save_file_name);
        if(network_load_status==true)
        {   
            message.clear();
            message="Network successfully loaded";
            print_message();
            make_prediction_on_user_entered_data();
        }
        else
        {   
            message.clear();
            message="ERROR!!! failed to load network from the network file.";
            print_message();
        }
    }
    else
    {
        if(load_network_if_available(0,0,true,network_save_file_name)==true && train_test_predict==2)//network avail,vaild label not avail,predic mode
        {   
            //predict(data_pack);
            only_testing(data_pack,train_test_predict);
            //testing_for_each_label(data_pack);
        }
        else
        {   
            message.clear();
            message="Invalid option";
            print_message();
        }//ERROR!!
    }
}

void core_class::add_data(nn_core_data_package_class* data_pack1,int train_test_predict1,float data_division2,string network_save_file_name1)
{
    data_pack=data_pack1;
    train_test_predict=train_test_predict1;
    data_division1=data_division2;
    network_save_file_name=network_save_file_name1;
    cout<<"\ndata_pack= "<<data_pack->data.size()<<" train_test_predict="<<train_test_predict<<" data_division="<<data_division1<<" save_file_name="<<network_save_file_name;
}

core_class::core_class(int core_aim1,int core_no1,int parent_segment_aim1,int parent_segment_no1,string core_name1)
{
    if(id_lock==false)
    {
        core_aim=core_aim1;
        core_no=core_no;
        parent_segment_aim=parent_segment_aim1;
        parent_segment_no=parent_segment_no1;
        core_name=core_name1;
        id_lock=true;
    }
    else
    {  
        message.clear();
        message="Failed to set core number and core aim as id_lock=true";
        print_message();
    }
}